// THPS Level Editor.cpp : main project file.

#include "stdafx.h"
#pragma unmanaged
#define DIRECTINPUT_VERSION 0x0800
//#define MEMORY_LEAK_CHECK
#ifdef MEMORY_LEAK_CHECK
#define VLD_FORCE_ENABLE
#include <vld.h>
#include <vldapi.h>
#endif
#include "THPSLevel.h"
#include "Dwmapi.h"
//#include "Debug.h"
#pragma unmanaged

using namespace THPSLevelEditor;

#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")
#pragma comment (lib, "Kernel32.lib")
#pragma comment (lib, "Psapi.lib")
#pragma comment (lib, "User32.lib")
#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "Dwmapi.lib")
#pragma unmanaged


D3DPRESENT_PARAMETERS DevParams = {0};
IDirect3DDevice9Ex* __restrict Device = NULL;
IDirect3D9Ex* __restrict d3d = NULL;
bool deviceLost = false;
DWORD MAX_PASSES = 1;
DWORD MAX_TEXCOORDS = 1;
Scene* __restrict scene = NULL;
D3DVIEWPORT9 port;
bool resetMouse = false;
DIMOUSESTATE mousestate = {0};
HWND handleWindow = NULL;
Camera* __restrict camera = NULL;
bool renderCollision = false;
BYTE selectionMode = 0;
const float bias = -1.0f;
DWORD defaultCulling = D3DCULL_NONE;
DWORD alphaRef = 8;
MaterialList* __restrict globalMaterialList = NULL;
vector <Script> Scripts;
TexFiles texFiles;
LPD3DXFONT m_font = NULL;
Mesh* __restrict arrow = NULL;
RECT rct;
char info[75] = "?";
extern DIMOUSESTATE oldMousestate;
bool rendering = false;
bool loading = false;
vector <RenderableRail> rails;
struct RenderableMatSplit
{
  Material* material;
  DWORD startIndex;
  DWORD numTris;
};

struct RenderableMesh
{
  IDirect3DVertexBuffer9 *vertices;
  IDirect3DIndexBuffer9 *indices;
  DWORD numSplits;
  RenderableMatSplit* matSplits;
  Mesh* mesh;
  DWORD numVertices;

  bool IsSelected() const
  {
    extern vector <SelectedObject> selectedObjects;
    for(DWORD i=0, numSel = selectedObjects.size(); i<numSel; i++)
    {
      if(selectedObjects[i].mesh==mesh)
        return true;
    }
    return false;
  }

  __declspec (noalias) void Render0() const//render opaque
  {
    //_asm prefetcht0 [ecx]
    register IDirect3DDevice9Ex* const __restrict Dev = Device;
    Dev->SetStreamSource(0, vertices, 0, sizeof(Vertex));
    Dev->SetIndices(indices);

    for(register DWORD i=0; i<numSplits; i++)
    {
      register const RenderableMatSplit & matSplit = matSplits[i];
      if(matSplit.material)
        matSplit.material->Submit();
      else
      {
        Dev->SetTexture(0, NULL);
        Dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        Dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
      }


      if(!IsSelected())
        Dev->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, numVertices, matSplit.startIndex, matSplit.numTris);
      else
      {
        Dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
        Dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
        Dev->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, numVertices, matSplit.startIndex, matSplit.numTris);
        Dev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
      }
    }
  }

  __declspec (noalias) void Render2()//render alpha 2nd order
  {
    //_asm prefetcht0 [ecx]
    register IDirect3DDevice9Ex* const __restrict Dev = Device;
    Dev->SetStreamSource(0, vertices, 0, sizeof(Vertex));
    Dev->SetIndices(indices);

    for(register DWORD i=0; i<numSplits; i++)
    {
      register const RenderableMatSplit & matSplit = matSplits[i];
      if(matSplit.material)
      {
        matSplit.material->Submit();

        if(IsSelected())
          Dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
        Dev->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, numVertices, matSplit.startIndex, matSplit.numTris);
      }
    }
  }
};
RenderableMesh* __restrict opaque = NULL;
RenderableMesh* __restrict trans = NULL;
DWORD numTrans=0;
DWORD numOpaque=0;
BYTE pass=0;





/*__int64 cntsPerSec = 0;
__int64 prevTimeStamp = 0;
__int64 currTimeStamp = 0;
static float secsPerCnt = 0.0f;*/
extern vector <SelectedObject> selectedObjects;
extern vector <SelectedTri> selectedTriangles;
extern vector <Event> events;
extern bool heldCntrl;
D3DMATERIAL9*       g_pMeshMaterials = NULL; // Materials for our mesh
LPDIRECT3DTEXTURE9* g_pMeshTextures = NULL; // Textures for our mesh
DWORD               g_dwNumMaterials = 0L;   // Number of mesh materials
vector <Node*> spawns;
vector <Node*> selectedNodes;


/*void ExtractResource(const HINSTANCE hInstance, WORD resourceID, LPCTSTR szFilename)
{
// Find and load the resource
HRSRC hResource = FindResource(hInstance, MAKEINTRESOURCE(resourceID), "BINARY");
HGLOBAL hFileResource = LoadResource(hInstance, hResource);

// Open and map this to a disk file
LPVOID lpFile = LockResource(hFileResource);
DWORD dwSize = SizeofResource(hInstance, hResource);            

// Open the file and filemap
HANDLE hFile = CreateFile(szFilename, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
HANDLE hFileMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, dwSize, NULL);            
LPVOID lpAddress = MapViewOfFile(hFileMap, FILE_MAP_WRITE, 0, 0, 0);            

// Write the file
CopyMemory(lpAddress, lpFile, dwSize);            

// Un-map the file and close the handles
UnmapViewOfFile(lpAddress);
CloseHandle(hFileMap);
CloseHandle(hFile);
}*/

VOID CreateConsole()
{
  int hConHandle = 0;
  HANDLE lStdHandle = 0;
  FILE *fp = 0;

  // Allocate a console
  AllocConsole();

  // redirect unbuffered STDOUT to the console
  lStdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
  hConHandle = _open_osfhandle(PtrToUlong(lStdHandle), _O_TEXT);
  fp = _fdopen(hConHandle, "w");
  *stdout = *fp;
  setvbuf(stdout, NULL, _IONBF, 0);

  // redirect unbuffered STDIN to the console
  lStdHandle = GetStdHandle(STD_INPUT_HANDLE);
  hConHandle = _open_osfhandle(PtrToUlong(lStdHandle), _O_TEXT);
  fp = _fdopen(hConHandle, "r");
  *stdin = *fp;
  setvbuf(stdin, NULL, _IONBF, 0);

  // redirect unbuffered STDERR to the console
  lStdHandle = GetStdHandle(STD_ERROR_HANDLE);
  hConHandle = _open_osfhandle(PtrToUlong(lStdHandle), _O_TEXT);
  fp = _fdopen(hConHandle, "w");
  *stderr = *fp;
  setvbuf(stderr, NULL, _IONBF, 0);
}

#pragma managed

[STAThreadAttribute]
int WINAPI  WinMain(      
  HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPSTR lpCmdLine,
  int nCmdShow)
{
  //CreateConsole();
  //ExtractResource(hInstance, 102, "boost_system-vc100-mt-1_47.dll");
  //ExtractResource(hInstance, 103, "boost_filesystem-vc100-mt-1_47.dll");
  // Enabling Windows XP visual effects before any controls are created
  Application::EnableVisualStyles();
  Application::SetCompatibleTextRenderingDefault(false); 

  // Create the main window and run it
  //Application::Idle += gcnew EventHandler(DrawFrame); 

  THPSLevel^ app = gcnew THPSLevel();
  Application::Exit();
  //Application::Run(gcnew THPSLevel());

  return 0;
}
#pragma unmanaged
void CreateCamera()
{
  if(!camera)
  {
    camera = new Camera();
    camera->lookAt(D3DXVECTOR3(0.0f,250.0f,-100.0f),D3DXVECTOR3(0.0f,250.0f,100.0f),D3DXVECTOR3(0.0f,1.0f,0.0f));
  }
  camera->setLens(D3DXToRadian(45.0f),float((float)::port.Width/(float)::port.Height),12.0f,95250.0f);
  Device->SetTransform( D3DTS_PROJECTION, &camera->proj() );
  Device->SetTransform(D3DTS_VIEW,&camera->view());
}

void SaveTextures(char* name)
{
  FILE* f = fopen(name, "wb");
  if(f)
  {
    const DWORD numImages = texFiles.numFiles;
    fwrite(&numImages, 4, 1, f);
    for(DWORD i=0; i<numImages; i++)
    {
      fwrite(texFiles.fileNames[i], strlen(texFiles.fileNames[i])+1, 1, f);
      FILE* temp = fopen(texFiles.fileNames[i], "rb");
      if(temp)
      {
        fseek(temp, 0, SEEK_END);
        const DWORD size = ftell(temp);
        BYTE* data = new BYTE[size];
        fseek(temp, 0, SEEK_SET);
        fread(data, size, 1, temp);
        fclose(temp);
        fwrite(&size, 4, 1, f);
        fwrite(data, size, 1, f);
        delete [] data;
      }
    }
    fclose(f);
  }
}

void SaveMaterials(char* name)
{
  FILE* f = fopen(name, "wb");
  if(f)
  {
    const DWORD numMaterials = scene->matList.GetNumMaterials();
    fwrite(&numMaterials, 4, 1, f);
    for(DWORD i=0; i<numMaterials; i++)
    {
      Material* material = scene->matList.GetMaterial(i);

      DWORD numTextures = material->GetNumTextures();
      fwrite(&numTextures, 4, 1, f);
      DWORD matId = material->GetMatId();
      fwrite(&matId, 4, 1, f);
      fwrite(&material->drawOrder, 4 , 1, f);
      fwrite(&material->transparent, 1, 1, f);

      for(DWORD j=0; j<numTextures; j++)
      {
        Texture* texture = material->GetTexture(j);
        fwrite(&texture->uAddress, 4, 1, f);
        fwrite(&texture->vAddress, 4, 1, f);
        fwrite(&texture->fixedAlpha, 4, 1, f);
        fwrite(&texture->blendMode, 4, 1, f);
        DWORD temp = texture->GetTexId();
        fwrite(&temp, 4, 1, f);
        /*temp = texture->GetFlags();
        fwrite(&temp, 4, 1, f);*/
      }
    }
    fclose(f);
  }
}

void SaveScripts(char* name)
{
  FILE* f = fopen(name, "wb");
  if(f)
  {
    const DWORD numNodes = scene->nodes.size();
    fwrite(&numNodes, 4, 1, f);
    for(DWORD i=0; i<numNodes; i++)
    {
      fwrite(&scene->nodes[i], 12, 4, f);

      DWORD numLinks = scene->nodes[i].Links.size();
      fwrite(&numLinks, 4, 1, f);
      fwrite(&scene->nodes[i].Links.front(), sizeof(WORD), numLinks, f);
      //fwrite(&scene->nodes[i].type, 2, 1, f);
      fwrite(&scene->nodes[i].CreatedAtStart, 1, 1, f);
      fwrite(&scene->nodes[i].AbsentInNetGames, 1, 1, f);
      fwrite(&scene->nodes[i].NetEnabled, 1, 1, f);
      fwrite(&scene->nodes[i].Permanent, 1, 1, f);
      fwrite(&scene->nodes[i].TrickObject, 1, 1, f);
    }

    const DWORD numScripts = Scripts.size();
    fwrite(&numScripts, 4, 1, f);
    if(numScripts)
      fwrite(&Scripts.front(), sizeof(Script), numScripts, f);

    const DWORD numKnownScripts = scene->KnownScripts.size();
    fwrite(&numKnownScripts, 4, 1, f);
    if(numKnownScripts)
      fwrite(&scene->KnownScripts.front(), sizeof(KnownScript), numKnownScripts, f);

    const DWORD numTriggers = scene->triggers.size();
    fwrite(&numTriggers, 4, 1, f);
    if(numTriggers)
      fwrite(&scene->triggers.front(), sizeof(Checksum), numTriggers, f);

    const DWORD numGlobals = scene->globals.size();
    fwrite(&numGlobals, 4, 1, f);
    if(numGlobals)
      fwrite(&scene->globals.front(), sizeof(Checksum), numGlobals, f);
    fclose(f);
  }
}

void SaveState(char* name)
{
  _fcloseall();
  FILE* f = fopen("C:\\lols", "wb");
  if(f)
  {
    /*static const char k[] = {0,4,3,2,1,2,3,4,65,221,5,53,3,3,3};
    fwrite(k, sizeof(k), 1, f);*/
    /* fclose(f);
    return;*/
    const static float version = 2.0f;
    if(scene)
    {
      fwrite(&version, 4, 1, f);
      fwrite(scene->GetName(),  260, 1, f);
      fwrite(&scene->GetBlendMode(), 1, 1, f);
      fwrite(&scene->cullMode, 4, 1, f);
      fwrite(&MAX_PASSES, 4, 1, f);
      fwrite(&MAX_TEXCOORDS, 4, 1, f);

      SaveTextures("C:\\text");


      const DWORD numMaterials = scene->matList.GetNumMaterials();
      fwrite(&numMaterials, 4, 1, f);
      for(DWORD i=0; i<numMaterials; i++)
      {
        Material* material = scene->matList.GetMaterial(i);

        DWORD numTextures = material->GetNumTextures();
        fwrite(&numTextures, 4, 1, f);
        DWORD matId = material->GetMatId();
        fwrite(&matId, 4, 1, f);
        fwrite(&material->drawOrder, 4 , 1, f);
        fwrite(&material->transparent, 1, 1, f);

        for(DWORD j=0; j<numTextures; j++)
        {
          Texture* texture = material->GetTexture(j);
          fwrite(&texture->uAddress, 4, 1, f);
          fwrite(&texture->vAddress, 4, 1, f);
          fwrite(&texture->fixedAlpha, 4, 1, f);
          fwrite(&texture->blendMode, 4, 1, f);
          DWORD temp = texture->GetTexId();
          fwrite(&temp, 4, 1, f);
          /*temp = texture->GetFlags();
          fwrite(&temp, 4, 1, f);*/
        }
      }

      const DWORD numMeshes = scene->GetNumMeshes();
      fwrite(&numMeshes, 4, 1, f);

      for(DWORD i=0; i<numMeshes; i++)
      {
        Mesh* mesh = scene->GetMesh(i);
        fwrite(&mesh->GetFlags(), 4, 1, f);
        fwrite(&mesh->GetName(), 4, 1, f);
        DWORD numVertices = mesh->GetNumVertices();
        fwrite(&numVertices, 4, 1, f);
        fwrite(mesh->GetVertices(), sizeof(Vertex), numVertices, f);
        Collision* collision = mesh->GetCollision();
        bool hasCollision = collision != NULL;
        fwrite(&hasCollision, 1, 1, f);
        if(hasCollision)
        {
          numVertices = collision->GetNumVertices();
          fwrite(&numVertices, 4, 1, f);
          if(numVertices)
          {
            fwrite(collision->GetVertices(), sizeof(Vertex), numVertices, f);
          }
          DWORD numFaces = collision->GetNumFaces();
          fwrite(&numFaces, 4, 1, f);
          if(numFaces)
          {
            fwrite(collision->GetFaces(), sizeof(SimpleFace), numFaces, f);
            fwrite(collision->GetFacesInfo(), sizeof(FaceInfo), numFaces, f);
          }
        }
        DWORD numSplits = mesh->GetNumSplits();
        fwrite(&numSplits, 4, 1, f);
        for(DWORD j=0; j<numSplits; j++)
        {
          MaterialSplit* matSplit = mesh->GetSplit(j);
          fwrite(&matSplit->matId, 4, 1, f);
          DWORD numIndices = matSplit->Indices.size();
          fwrite(&numIndices, 4, 1, f);
          fwrite(&matSplit->Indices.front(), sizeof(WORD), numIndices, f);
        }
      }

      bool hasSky = scene->skyDome != 0;
      fwrite(&hasSky, 1, 1, f);
      if(hasSky)
      {
        const DWORD numSkyMaterials = scene->skyDome->matList.GetNumMaterials();
        fwrite(&numSkyMaterials, 4, 1, f);
        for(DWORD i=0; i<numSkyMaterials; i++)
        {
          Material* material = scene->skyDome->matList.GetMaterial(i);

          DWORD numTextures = material->GetNumTextures();
          fwrite(&numTextures, 4, 1, f);
          DWORD matId = material->GetMatId();
          fwrite(&matId, 4, 1, f);
          fwrite(&material->drawOrder, 4 , 1, f);
          fwrite(&material->transparent, 1, 1, f);

          for(DWORD j=0; j<numTextures; j++)
          {
            Texture* texture = material->GetTexture(j);
            fwrite(&texture->uAddress, 4, 1, f);
            fwrite(&texture->vAddress, 4, 1, f);
            fwrite(&texture->fixedAlpha, 4, 1, f);
            fwrite(&texture->blendMode, 4, 1, f);
            DWORD temp = texture->GetTexId();
            fwrite(&temp, 4, 1, f);
            /*temp = texture->GetFlags();
            fwrite(&temp, 4, 1, f);*/
          }
        }

        const DWORD numSkyMeshes = scene->skyDome->GetNumMeshes();
        fwrite(&numSkyMeshes, 4, 1, f);

        for(DWORD i=0; i<numSkyMeshes; i++)
        {
          Mesh* mesh = scene->skyDome->GetMesh(i);
          fwrite(&mesh->GetFlags(), 4, 1, f);
          fwrite(&mesh->GetName(), 4, 1, f);
          DWORD numVertices = mesh->GetNumVertices();
          fwrite(&numVertices, 4, 1, f);
          fwrite(mesh->GetVertices(), sizeof(Vertex), numVertices, f);
          Collision* collision = mesh->GetCollision();
          bool hasCollision = collision != NULL;
          fwrite(&hasCollision, 1, 1, f);
          if(hasCollision)
          {
            numVertices = collision->GetNumVertices();
            fwrite(&numVertices, 4, 1, f);
            if(numVertices)
            {
              fwrite(collision->GetVertices(), sizeof(Vertex), numVertices, f);
            }
            DWORD numFaces = collision->GetNumFaces();
            fwrite(&numFaces, 4, 1, f);
            if(numFaces)
            {
              fwrite(collision->GetFaces(), sizeof(SimpleFace), numFaces, f);
              fwrite(collision->GetFacesInfo(), sizeof(FaceInfo), numFaces, f);
            }
          }
          DWORD numSplits = mesh->GetNumSplits();
          fwrite(&numSplits, 4, 1, f);
          for(DWORD j=0; j<numSplits; j++)
          {
            MaterialSplit* matSplit = mesh->GetSplit(j);
            fwrite(&matSplit->matId, 4, 1, f);
            DWORD numIndices = matSplit->Indices.size();
            fwrite(&numIndices, 4, 1, f);
            fwrite(&matSplit->Indices.front(), sizeof(WORD), numIndices, f);
          }
        }
      }
      SaveScripts("C:\\script");
    }
    if(ferror(f))
      ::MessageBox(0,"Error","Error",0);
    fflush(f);
    if(ferror(f))
      ::MessageBox(0,"Error","Error",0);
    fclose(f);
    if(ferror(f))
      ::MessageBox(0,"Error","Error",0);
    ::MessageBox(0,"done","done",0);
  }
  else
    ::MessageBox(0,"Error","file...",0);
  if(ferror(f))
    ::MessageBox(0,"Error","Error",0);
  _fcloseall();
}

void AddLinkedRails(const Node* const __restrict parent, const WORD index)
{
  Node* node = &scene->nodes[index];
  static ColouredVertex vertex = ColouredVertex(0,255,0);
  RenderableRail* rail = &rails.back();
  rail->nodes.push_back(node->Name);
  float MAX_DIST = parent->Position.x-node->Position.x;
  MAX_DIST *= MAX_DIST;
  static bool axis = false;
  if(((parent->Position.z-node->Position.z)*(parent->Position.z-node->Position.z))>MAX_DIST)
    axis = true;
  else
    axis = false;

  if(parent->Links.size())
  {
    vertex.x=node->Position.x;
    vertex.y=node->Position.y+2.0f;
    vertex.z=node->Position.z;
    if(axis)
      vertex.x-=2.0f;
    else
      vertex.z-=2.0f;

    rail->vertices.push_back(vertex);

    vertex.y -= 4.0f;
    rail->vertices.push_back(vertex);

    if(axis)
      vertex.x+=4.0f;
    else
      vertex.z+=4.0f;
    rail->vertices.push_back(vertex);

    vertex.y += 4.0f;
    rail->vertices.push_back(vertex);

    rail->Indices.push_back(rail->numIndices+1);
    rail->Indices.push_back(rail->numIndices+4);
    rail->Indices.push_back(rail->numIndices+5);
    rail->Indices.push_back(rail->numIndices+1);
    rail->Indices.push_back(rail->numIndices);
    rail->Indices.push_back(rail->numIndices+4);
    rail->Indices.push_back(rail->numIndices);
    rail->Indices.push_back(rail->numIndices+4);
    rail->Indices.push_back(rail->numIndices+3);
    rail->Indices.push_back(rail->numIndices+3);
    rail->Indices.push_back(rail->numIndices+4);
    rail->Indices.push_back(rail->numIndices+7);
    rail->Indices.push_back(rail->numIndices+3);
    rail->Indices.push_back(rail->numIndices+6);
    rail->Indices.push_back(rail->numIndices+7);
    rail->Indices.push_back(rail->numIndices+6);
    rail->Indices.push_back(rail->numIndices+3);
    rail->Indices.push_back(rail->numIndices+2);
    rail->Indices.push_back(rail->numIndices+1);
    rail->Indices.push_back(rail->numIndices+5);
    rail->Indices.push_back(rail->numIndices+6);
    rail->Indices.push_back(rail->numIndices+6);
    rail->Indices.push_back(rail->numIndices+1);
    rail->Indices.push_back(rail->numIndices+2);
    rail->numIndices+=4;
  }
  else
  {
    vertex.x=parent->Position.x;
    vertex.y=parent->Position.y+2.0f;
    vertex.z=parent->Position.z;
    if(axis)
      vertex.x-=2.0f;
    else
      vertex.z-=2.0f;

    rail->vertices.push_back(vertex);

    vertex.y -= 4.0f;
    rail->vertices.push_back(vertex);

    if(axis)
      vertex.x+=4.0f;
    else
      vertex.z+=4.0f;
    rail->vertices.push_back(vertex);

    vertex.y += 4.0f;
    rail->vertices.push_back(vertex);


    vertex.x=node->Position.x;
    vertex.y=node->Position.y+2.0f;
    vertex.z=node->Position.z;
    if(axis)
      vertex.x-=2.0f;
    else
      vertex.z-=2.0f;

    rail->vertices.push_back(vertex);

    vertex.y -= 4.0f;
    rail->vertices.push_back(vertex);

    if(axis)
      vertex.x+=4.0f;
    else
      vertex.z+=4.0f;
    rail->vertices.push_back(vertex);

    vertex.y += 4.0f;
    rail->vertices.push_back(vertex);

    rail->Indices.push_back(rail->numIndices+1);
    rail->Indices.push_back(rail->numIndices+4);
    rail->Indices.push_back(rail->numIndices+5);
    rail->Indices.push_back(rail->numIndices+1);
    rail->Indices.push_back(rail->numIndices);
    rail->Indices.push_back(rail->numIndices+4);
    rail->Indices.push_back(rail->numIndices);
    rail->Indices.push_back(rail->numIndices+4);
    rail->Indices.push_back(rail->numIndices+3);
    rail->Indices.push_back(rail->numIndices+3);
    rail->Indices.push_back(rail->numIndices+4);
    rail->Indices.push_back(rail->numIndices+7);
    rail->Indices.push_back(rail->numIndices+3);
    rail->Indices.push_back(rail->numIndices+6);
    rail->Indices.push_back(rail->numIndices+7);
    rail->Indices.push_back(rail->numIndices+6);
    rail->Indices.push_back(rail->numIndices+3);
    rail->Indices.push_back(rail->numIndices+2);
    rail->Indices.push_back(rail->numIndices+1);
    rail->Indices.push_back(rail->numIndices+5);
    rail->Indices.push_back(rail->numIndices+6);
    rail->Indices.push_back(rail->numIndices+6);
    rail->Indices.push_back(rail->numIndices+1);
    rail->Indices.push_back(rail->numIndices+2);
    rail->numIndices+=4;


  }
  for(DWORD i=0, numNodes = scene->nodes.size(); i<numNodes; i++)
  {
    /*if(scene->nodes[i].Class.checksum == 0x8E6B02AD && !scene->nodes[i].Links.empty() && scene->nodes[i].Links[0] == index && !scene->nodes[i].IsAdded())
    {
    AddLinkedRails(node, i);
    break;
    }*/
    if(scene->nodes[i].Class.checksum == 0x8E6B02AD)
    {
      for(DWORD j=0, numLinks = scene->nodes[i].Links.size(); j<numLinks; j++)
      {
        if(scene->nodes[i].Links[j]==index && !scene->nodes[i].IsAdded())
        {
          AddLinkedRails(node, i);
          return;
        }
      }
    }
  }
}

void AddRails(const Node* const __restrict parent, const WORD index)
{
  Node* node = &scene->nodes[index];
  static ColouredVertex vertex = ColouredVertex(0,255,0);
  RenderableRail* rail = &rails.back();
  rail->nodes.push_back(node->Name);
  float MAX_DIST = parent->Position.x-node->Position.x;
  MAX_DIST *= MAX_DIST;
  static bool axis = false;
  if(((parent->Position.z-node->Position.z)*(parent->Position.z-node->Position.z))>MAX_DIST)
    axis = true;
  else
    axis = false;

  if(parent->Links.size())
  {
    vertex.x=node->Position.x;
    vertex.y=node->Position.y+2.0f;
    vertex.z=node->Position.z;
    if(axis)
      vertex.x-=2.0f;
    else
      vertex.z-=2.0f;

    rail->vertices.push_back(vertex);

    vertex.y -= 4.0f;
    rail->vertices.push_back(vertex);

    if(axis)
      vertex.x+=4.0f;
    else
      vertex.z+=4.0f;
    rail->vertices.push_back(vertex);

    vertex.y += 4.0f;
    rail->vertices.push_back(vertex);

    rail->Indices.push_back(rail->numIndices+1);
    rail->Indices.push_back(rail->numIndices+4);
    rail->Indices.push_back(rail->numIndices+5);
    rail->Indices.push_back(rail->numIndices+1);
    rail->Indices.push_back(rail->numIndices);
    rail->Indices.push_back(rail->numIndices+4);
    rail->Indices.push_back(rail->numIndices);
    rail->Indices.push_back(rail->numIndices+4);
    rail->Indices.push_back(rail->numIndices+3);
    rail->Indices.push_back(rail->numIndices+3);
    rail->Indices.push_back(rail->numIndices+4);
    rail->Indices.push_back(rail->numIndices+7);
    rail->Indices.push_back(rail->numIndices+3);
    rail->Indices.push_back(rail->numIndices+6);
    rail->Indices.push_back(rail->numIndices+7);
    rail->Indices.push_back(rail->numIndices+6);
    rail->Indices.push_back(rail->numIndices+3);
    rail->Indices.push_back(rail->numIndices+2);
    rail->Indices.push_back(rail->numIndices+1);
    rail->Indices.push_back(rail->numIndices+5);
    rail->Indices.push_back(rail->numIndices+6);
    rail->Indices.push_back(rail->numIndices+6);
    rail->Indices.push_back(rail->numIndices+1);
    rail->Indices.push_back(rail->numIndices+2);
    rail->numIndices+=4;
  }
  else
  {
    vertex.x=parent->Position.x;
    vertex.y=parent->Position.y+2.0f;
    vertex.z=parent->Position.z;
    if(axis)
      vertex.x-=2.0f;
    else
      vertex.z-=2.0f;

    rail->vertices.push_back(vertex);

    vertex.y -= 4.0f;
    rail->vertices.push_back(vertex);

    if(axis)
      vertex.x+=4.0f;
    else
      vertex.z+=4.0f;
    rail->vertices.push_back(vertex);

    vertex.y += 4.0f;
    rail->vertices.push_back(vertex);


    vertex.x=node->Position.x;
    vertex.y=node->Position.y+2.0f;
    vertex.z=node->Position.z;
    if(axis)
      vertex.x-=2.0f;
    else
      vertex.z-=2.0f;

    rail->vertices.push_back(vertex);

    vertex.y -= 4.0f;
    rail->vertices.push_back(vertex);

    if(axis)
      vertex.x+=4.0f;
    else
      vertex.z+=4.0f;
    rail->vertices.push_back(vertex);

    vertex.y += 4.0f;
    rail->vertices.push_back(vertex);

    rail->Indices.push_back(rail->numIndices+1);
    rail->Indices.push_back(rail->numIndices+4);
    rail->Indices.push_back(rail->numIndices+5);
    rail->Indices.push_back(rail->numIndices+1);
    rail->Indices.push_back(rail->numIndices);
    rail->Indices.push_back(rail->numIndices+4);
    rail->Indices.push_back(rail->numIndices);
    rail->Indices.push_back(rail->numIndices+4);
    rail->Indices.push_back(rail->numIndices+3);
    rail->Indices.push_back(rail->numIndices+3);
    rail->Indices.push_back(rail->numIndices+4);
    rail->Indices.push_back(rail->numIndices+7);
    rail->Indices.push_back(rail->numIndices+3);
    rail->Indices.push_back(rail->numIndices+6);
    rail->Indices.push_back(rail->numIndices+7);
    rail->Indices.push_back(rail->numIndices+6);
    rail->Indices.push_back(rail->numIndices+3);
    rail->Indices.push_back(rail->numIndices+2);
    rail->Indices.push_back(rail->numIndices+1);
    rail->Indices.push_back(rail->numIndices+5);
    rail->Indices.push_back(rail->numIndices+6);
    rail->Indices.push_back(rail->numIndices+6);
    rail->Indices.push_back(rail->numIndices+1);
    rail->Indices.push_back(rail->numIndices+2);
    rail->numIndices+=4;


  }
  for(DWORD i=0, numNodes = scene->nodes.size(); i<numNodes; i++)
  {
    /*if(scene->nodes[i].Class.checksum == 0x8E6B02AD && !scene->nodes[i].Links.empty() && scene->nodes[i].Links[0] == index)
    {
    AddRails(node, i);
    break;
    }*/
    if(scene->nodes[i].Class.checksum == 0x8E6B02AD)
    {
      for(DWORD j=0, numLinks = scene->nodes[i].Links.size(); j<numLinks; j++)
      {
        if(scene->nodes[i].Links[j]==index)
        {
          AddRails(node, i);
          return;
        }
      }
    }
  }
}

void AddParentRail(const Node* const __restrict parent, const WORD index)
{
  Node* node = &scene->nodes[index];
  static ColouredVertex vertex = ColouredVertex(0,255,0);
  RenderableRail* rail = &rails.back();
  rail->nodes.push_back(node->Name);
  float MAX_DIST = parent->Position.x-node->Position.x;
  MAX_DIST *= MAX_DIST;
  static bool axis = false;
  if(((parent->Position.z-node->Position.z)*(parent->Position.z-node->Position.z))>MAX_DIST)
    axis = true;
  else
    axis = false;


  vertex.x=parent->Position.x;
  vertex.y=parent->Position.y+2.0f;
  vertex.z=parent->Position.z;
  if(axis)
    vertex.x-=2.0f;
  else
    vertex.z-=2.0f;

  rail->vertices.push_back(vertex);

  vertex.y -= 4.0f;
  rail->vertices.push_back(vertex);

  if(axis)
    vertex.x+=4.0f;
  else
    vertex.z+=4.0f;
  rail->vertices.push_back(vertex);

  vertex.y += 4.0f;
  rail->vertices.push_back(vertex);


  vertex.x=node->Position.x;
  vertex.y=node->Position.y+2.0f;
  vertex.z=node->Position.z;
  if(axis)
    vertex.x-=2.0f;
  else
    vertex.z-=2.0f;

  rail->vertices.push_back(vertex);

  vertex.y -= 4.0f;
  rail->vertices.push_back(vertex);

  if(axis)
    vertex.x+=4.0f;
  else
    vertex.z+=4.0f;
  rail->vertices.push_back(vertex);

  vertex.y += 4.0f;
  rail->vertices.push_back(vertex);

  rail->Indices.push_back(rail->numIndices+1);
  rail->Indices.push_back(rail->numIndices+4);
  rail->Indices.push_back(rail->numIndices+5);
  rail->Indices.push_back(rail->numIndices+1);
  rail->Indices.push_back(rail->numIndices);
  rail->Indices.push_back(rail->numIndices+4);
  rail->Indices.push_back(rail->numIndices);
  rail->Indices.push_back(rail->numIndices+4);
  rail->Indices.push_back(rail->numIndices+3);
  rail->Indices.push_back(rail->numIndices+3);
  rail->Indices.push_back(rail->numIndices+4);
  rail->Indices.push_back(rail->numIndices+7);
  rail->Indices.push_back(rail->numIndices+3);
  rail->Indices.push_back(rail->numIndices+6);
  rail->Indices.push_back(rail->numIndices+7);
  rail->Indices.push_back(rail->numIndices+6);
  rail->Indices.push_back(rail->numIndices+3);
  rail->Indices.push_back(rail->numIndices+2);
  rail->Indices.push_back(rail->numIndices+1);
  rail->Indices.push_back(rail->numIndices+5);
  rail->Indices.push_back(rail->numIndices+6);
  rail->Indices.push_back(rail->numIndices+6);
  rail->Indices.push_back(rail->numIndices+1);
  rail->Indices.push_back(rail->numIndices+2);
  rail->numIndices+=4;

  for(DWORD i=0, numNodes = scene->nodes.size(); i<numNodes; i++)
  {
    /*if(scene->nodes[i].Class.checksum == 0x8E6B02AD && !scene->nodes[i].Links.empty() && scene->nodes[i].Links[0] == index && !scene->nodes[i].IsAdded())
    {
    AddLinkedRails(node, i);
    break;
    }*/
    if(scene->nodes[i].Class.checksum == 0x8E6B02AD)
    {
      for(DWORD j=0, numLinks = scene->nodes[i].Links.size(); j<numLinks; j++)
      {
        if(scene->nodes[i].Links[j]==index && !scene->nodes[i].IsAdded())
        {
          AddLinkedRails(node, i);
          return;
        }
      }
    }
  }
}

void AddLinked(const WORD index)
{
  Node* node = &scene->nodes[index];
  for(DWORD i=0, numNodes = scene->nodes.size(); i<numNodes; i++)
  {
    /*if(scene->nodes[i].Class.checksum == 0x8E6B02AD && !scene->nodes[i].Links.empty() && scene->nodes[i].Links[0] == index && scene->nodes[i].IsAdded())
    {
    AddParentRail(node, i);
    break;
    }*/
    if(scene->nodes[i].Class.checksum == 0x8E6B02AD)
    {
      for(DWORD j=0, numLinks = scene->nodes[i].Links.size(); j<numLinks; j++)
      {
        if(scene->nodes[i].Links[j]==index && !scene->nodes[i].IsAdded())
        {
          AddParentRail(node, i);
          return;
        }
      }
    }
  }
}

void AddRails(const WORD index)
{
  Node* node = &scene->nodes[index];
  for(DWORD i=0, numNodes = scene->nodes.size(); i<numNodes; i++)
  {
    /*if(scene->nodes[i].Class.checksum == 0x8E6B02AD && !scene->nodes[i].Links.empty() && scene->nodes[i].Links.back() == index)
    {
    AddRails(node, i);
    break;
    }*/
    if(scene->nodes[i].Class.checksum == 0x8E6B02AD)
    {
      for(DWORD j=0, numLinks = scene->nodes[i].Links.size(); j<numLinks; j++)
      {
        if(scene->nodes[i].Links[j]==index)
        {
          AddRails(node, i);
          return;
        }
      }
    }
  }
}

void LoadTextures(register const BYTE const* __restrict pFile)
{
  texFiles.numFiles = *(DWORD*)pFile;
  pFile+=4;
  if(texFiles.numFiles)
  {
    texFiles.fileNames = (char**)malloc(texFiles.numFiles);

    /*const BYTE* olda = pFile;
    DWORD newSize = size-(pFile-old);
    olda = new BYTE[newSize];
    memcpy((void*)olda, pFile, newSize);
    delete [] old;
    pFile = olda;*/

    for(DWORD i=0, numFiles = texFiles.numFiles; i<numFiles; i++)
    {
      const DWORD len = strlen((char*)pFile)+1;
      texFiles.fileNames[i] = new char[len];
      memcpy(texFiles.fileNames[i], pFile, len);
      pFile+=len;
      FILE* temp = fopen(texFiles.fileNames[i], "wb");
      if(temp)
      {
        const DWORD size = *(DWORD*)pFile;
        pFile+=4;
        fwrite(pFile, size, 1, temp);
        pFile+=size;
        fclose(temp);
        _fcloseall();
      }
    }
  }
}

void LoadMaterials(register const BYTE const* __restrict pFile)
{
  const DWORD numMaterials = *(DWORD*)pFile;
  pFile+=4;
  scene->matList.Resize(numMaterials);
  for(DWORD i=0; i<numMaterials; i++)
  {
    Material* mat = scene->matList.GetMaterial(i);
    const DWORD numTextures = *(DWORD*)pFile;
    pFile+=4;
    mat->Reserve(numTextures);
    mat->SetMatId(*(DWORD*)pFile);
    pFile+=4;
    mat->drawOrder = *(float*)pFile;
    pFile+=4;
    mat->transparent = *(bool*)pFile;
    pFile++;

    for(DWORD j=0; j<numTextures; j++)
    {
      Texture texture;
      texture.uAddress = *(DWORD*)pFile;
      pFile+=4;
      texture.vAddress = *(DWORD*)pFile;
      pFile+=4;
      texture.fixedAlpha = *(DWORD*)pFile;
      pFile+=4;
      texture.blendMode = *(DWORD*)pFile;
      pFile+=4;
      texture.SetId(*(DWORD*)pFile);
      pFile+=4;
      /*texture.SetFlags(*(DWORD*)pFile);
      pFile+=4;*/
      texture.CreateTexture();
      mat->AddTexture2(&texture);
    }
  }
}

void LoadScripts(register const BYTE const* __restrict pFile)
{
  const DWORD numNodes = *(DWORD*)pFile;
  pFile+=4;
  scene->nodes.resize(numNodes);
  for(DWORD i=0; i<numNodes; i++)
  {
    std::copy((DWORD*)pFile, (DWORD*)pFile+12, (DWORD*)&scene->nodes[i]);
    pFile+=48;

    const DWORD numLinks = *(DWORD*)pFile;
    pFile+=4;
    scene->nodes[i].Links.assign((WORD*)pFile, (WORD*)pFile+numLinks);
    pFile+=numLinks*2;
    /*scene->nodes[i].type = *(WORD*)pFile;
    pFile+=2;*/
    scene->nodes[i].CreatedAtStart = *(bool*)pFile;
    pFile++;
    scene->nodes[i].AbsentInNetGames = *(bool*)pFile;
    pFile++;
    scene->nodes[i].NetEnabled = *(bool*)pFile;
    pFile++;
    scene->nodes[i].Permanent = *(bool*)pFile;
    pFile++;
    scene->nodes[i].TrickObject = *(bool*)pFile;
    pFile++;
  }

  const DWORD numScripts = *(DWORD*)pFile;
  pFile+=4;
  if(numScripts)
  {
    Scripts.reserve(numScripts);
    Scripts.assign((Script*)pFile, (Script*)pFile+numScripts);
    pFile+=numScripts*sizeof(Script);
  }

  const DWORD numKnownScripts = *(DWORD*)pFile;
  pFile+=4;
  if(numKnownScripts)
  {
    scene->KnownScripts.reserve(numKnownScripts);
    scene->KnownScripts.assign((KnownScript*)pFile, (KnownScript*)pFile+numKnownScripts);
    pFile+=numKnownScripts*sizeof(KnownScript);
  }

  const DWORD numTriggers = *(DWORD*)pFile;
  pFile+=4;
  if(numTriggers)
  {
    scene->triggers.reserve(numTriggers);
    scene->triggers.assign((Checksum*)pFile, (Checksum*)pFile+numTriggers);
    pFile+=numTriggers*sizeof(Checksum);
  }

  const DWORD numGlobals = *(DWORD*)pFile;
  pFile+=4;
  if(numGlobals)
  {
    scene->globals.reserve(numGlobals);
    scene->globals.assign((Checksum*)pFile, (Checksum*)pFile+numGlobals);
    pFile+=numGlobals*sizeof(Checksum);
  }
}

bool LoadState_unmanaged(register const BYTE const* __restrict pFile)
{
  if(pFile)
  {
    CreateCamera();

    pFile+=260;
    scene->SetBlend(*pFile);
    pFile++;
    scene->cullMode = *(DWORD*)pFile;
    pFile+=4;

    MAX_PASSES = *(DWORD*)pFile;
    pFile+=4;
    MAX_TEXCOORDS = *(DWORD*)pFile;
    const DWORD fvf = D3DFVF_XYZ|D3DFVF_DIFFUSE|MAX_TEXCOORDS;
    pFile+=4;

    const DWORD numMaterials = *(DWORD*)pFile;
    pFile+=4;
    scene->matList.Resize(numMaterials);
    for(DWORD i=0; i<numMaterials; i++)
    {
      Material* mat = scene->matList.GetMaterial(i);
      const DWORD numTextures = *(DWORD*)pFile;
      pFile+=4;
      mat->Reserve(numTextures);
      mat->SetMatId(*(DWORD*)pFile);
      pFile+=4;
      mat->drawOrder = *(float*)pFile;
      pFile+=4;
      mat->transparent = *(bool*)pFile;
      pFile++;

      for(DWORD j=0; j<numTextures; j++)
      {
        Texture texture;
        texture.uAddress = *(DWORD*)pFile;
        pFile+=4;
        texture.vAddress = *(DWORD*)pFile;
        pFile+=4;
        texture.fixedAlpha = *(DWORD*)pFile;
        pFile+=4;
        texture.blendMode = *(DWORD*)pFile;
        pFile+=4;
        texture.SetId(*(DWORD*)pFile);
        pFile+=4;
        /*texture.SetFlags(*(DWORD*)pFile);
        pFile+=4;*/
        texture.CreateTexture();
        mat->AddTexture2(&texture);
      }
    }

    /*delete [] olda;
    newSize = newSize-(pFile-olda);
    olda = pFile;
    olda = new BYTE[newSize];
    memcpy((void*)olda, pFile, newSize);
    pFile = olda;
    olda = pFile;*/

    const DWORD numMeshes = *(DWORD*)pFile;
    pFile+=4;
    scene->Resize(numMeshes);
    for(DWORD i=0; i<numMeshes; i++)
    {
      Mesh* mesh = scene->GetMesh(i);
      mesh->flags = *(DWORD*)pFile;
      pFile+=4;
      mesh->SetName(*(Checksum*)pFile);
      pFile+=4;
      DWORD numVertices = *(DWORD*)pFile;
      mesh->Reserve(numVertices);
      pFile+=4;
      if(numVertices)
      {
        mesh->AddVertices((Vertex*)pFile, numVertices);
        Device->CreateVertexBuffer(numVertices*sizeof(Vertex), D3DUSAGE_WRITEONLY, fvf, D3DPOOL_DEFAULT, &mesh->verts, NULL);

        Vertex* verts;
        mesh->verts->Lock(0,0,(void**)&verts, 0);
        memcpy(verts, mesh->GetVertices(), numVertices*sizeof(Vertex));
        mesh->verts->Unlock();

        pFile+=sizeof(Vertex)*numVertices;
      }
      bool hasCollision = *(bool*)pFile;
      pFile++;
      if(hasCollision)
      {
        mesh->InitCollision();
        Collision* collision = mesh->GetCollision();
        numVertices = *(DWORD*)pFile;
        pFile+=4;
        if(numVertices)
        {
          collision->AddVertices((Vertex*)pFile, numVertices);
          pFile+=sizeof(Vertex)*numVertices;
        }
        const DWORD numFaces = *(DWORD*)pFile;
        pFile+=4;
        if(numFaces)
        {
          collision->AddFaces((SimpleFace*)pFile, numFaces);
          pFile+=sizeof(SimpleFace)*numFaces;
          collision->AddFaces((FaceInfo*)pFile, numFaces);
          pFile+=sizeof(FaceInfo)*numFaces;
        }
      }
      const DWORD numSplits = *(DWORD*)pFile;
      pFile+=4;

      mesh->matSplits.resize(numSplits);
      if(numSplits)
      {
        char str[42];
        if(i>72)
        {
        sprintf(str, "%u", numSplits);
        ::MessageBox(0, str, "", 0);
        }
        for(DWORD j=0; j<numSplits; j++)
        {
          mesh->matSplits[i].matId = *(DWORD*)pFile;
          pFile+=4;
          const DWORD numIndices = *(DWORD*)pFile;
          pFile+=4;
          if(numIndices)
          {
            if(i>72)
            {
            sprintf(str, "%u", numIndices);
            ::MessageBox(0, str, "", 0);
            }
            mesh->matSplits[j].Indices.reserve(numIndices);
            mesh->matSplits[j].Indices.assign((WORD*)pFile, (WORD*)pFile+numIndices);
            mesh->matSplits[j].numIndices = numIndices-2;
            pFile+=numIndices*sizeof(WORD);
            Device->CreateIndexBuffer(numIndices*2, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &mesh->matSplits[j].indices, NULL);

            WORD* indices;
            mesh->matSplits[j].indices->Lock(0,0,(void**)&indices, 0);
            memcpy(indices, &mesh->matSplits[j].Indices.front(), numIndices*2);
            mesh->matSplits[j].indices->Unlock();
          }
        }
      }
    }

    bool hasSky = *(bool*)pFile;
    pFile++;
    if(hasSky)
    {
      scene->skyDome = new Scene();

      const DWORD numSkyMaterials = *(DWORD*)pFile;
      pFile+=4;
      scene->skyDome->matList.Resize(numSkyMaterials);
      for(DWORD i=0; i<numSkyMaterials; i++)
      {
        Material* mat = scene->skyDome->matList.GetMaterial(i);
        const DWORD numTextures = *(DWORD*)pFile;
        pFile+=4;
        mat->Reserve(numTextures);
        mat->SetMatId(*(DWORD*)pFile);
        pFile+=4;
        mat->drawOrder = *(float*)pFile;
        pFile+=4;
        mat->transparent = *(bool*)pFile;
        pFile++;

        for(DWORD j=0; j<numTextures; j++)
        {
          Texture texture;
          texture.uAddress = *(DWORD*)pFile;
          pFile+=4;
          texture.vAddress = *(DWORD*)pFile;
          pFile+=4;
          texture.fixedAlpha = *(DWORD*)pFile;
          pFile+=4;
          texture.blendMode = *(DWORD*)pFile;
          pFile+=4;
          texture.SetId(*(DWORD*)pFile);
          pFile+=4;
          /*texture.SetFlags(*(DWORD*)pFile);
          pFile+=4;*/
          texture.CreateTexture();
          mat->AddTexture2(&texture);
        }
      }

      const DWORD numSkyMeshes = *(DWORD*)pFile;
      pFile+=4;
      scene->skyDome->Resize(numSkyMeshes);
      for(DWORD i=0; i<numSkyMeshes; i++)
      {
        Mesh* mesh = scene->skyDome->GetMesh(i);
        mesh->flags = *(DWORD*)pFile;
        pFile+=4;
        mesh->SetName(*(Checksum*)pFile);
        pFile+=4;
        DWORD numVertices = *(DWORD*)pFile;
        mesh->Reserve(numVertices);
        pFile+=4;
        if(numVertices)
        {
          mesh->AddVertices((Vertex*)pFile, numVertices);
          Device->CreateVertexBuffer(numVertices*sizeof(Vertex), D3DUSAGE_WRITEONLY, fvf, D3DPOOL_DEFAULT, &mesh->verts, NULL);

          Vertex* verts;
          mesh->verts->Lock(0,0,(void**)&verts, 0);
          memcpy(verts, mesh->GetVertices(), numVertices*sizeof(Vertex));
          mesh->verts->Unlock();

          pFile+=sizeof(Vertex)*numVertices;
        }
        bool hasCollision = *(bool*)pFile;
        pFile++;
        if(hasCollision)
        {
          mesh->InitCollision();
          Collision* collision = mesh->GetCollision();
          numVertices = *(DWORD*)pFile;
          pFile+=4;
          if(numVertices)
          {
            collision->AddVertices((Vertex*)pFile, numVertices);
            pFile+=sizeof(Vertex)*numVertices;
          }
          const DWORD numFaces = *(DWORD*)pFile;
          pFile+=4;
          if(numFaces)
          {
            collision->AddFaces((SimpleFace*)pFile, numFaces);
            pFile+=sizeof(SimpleFace)*numFaces;
            collision->AddFaces((FaceInfo*)pFile, numFaces);
            pFile+=sizeof(FaceInfo)*numFaces;
          }
        }
        const DWORD numSplits = *(DWORD*)pFile;
        pFile+=4;
        mesh->matSplits.resize(numSplits);
        if(numSplits)
        {
          for(DWORD j=0; j<numSplits; j++)
          {
            mesh->matSplits[i].matId = *(DWORD*)pFile;
            pFile+=4;
            const DWORD numIndices = *(DWORD*)pFile;
            pFile+=4;
            if(numIndices)
            {
              mesh->matSplits[j].Indices.reserve(numIndices);
              mesh->matSplits[j].Indices.assign((WORD*)pFile, (WORD*)pFile+numIndices);
              pFile+=numIndices*sizeof(WORD);
              mesh->matSplits[j].numIndices = numIndices-2;
              Device->CreateIndexBuffer(numIndices*2, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &mesh->matSplits[j].indices, NULL);

              WORD* indices;
              mesh->matSplits[j].indices->Lock(0,0,(void**)&indices, 0);
              memcpy(indices, &mesh->matSplits[j].Indices.front(), numIndices*2);
              mesh->matSplits[j].indices->Unlock();
            }
            /*mesh->matSplits[j].Indices.resize(numIndices);
            if(numIndices)
            {
            std::copy((WORD*)pFile, (WORD*)pFile+numIndices, mesh->matSplits[j].Indices.begin());
            pFile+=numIndices*sizeof(WORD);
            Device->CreateIndexBuffer(numIndices*2, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &mesh->matSplits[j].indices, NULL);

            WORD* indices;
            mesh->matSplits[j].indices->Lock(0,0,(void**)&indices, 0);
            memcpy(indices, &mesh->matSplits[j].Indices.front(), numIndices*2);
            mesh->matSplits[j].indices->Unlock();
            }*/
          }
        }
      }
    }
    return true;
  }
  return false;
}

extern struct Th3Scene : Scene{};
#pragma managed
bool THPSLevel::LoadScene(char* path)
{
  loading = true;
  bool sort=false;
  this->sceneName = gcnew String(path);
  toolStripStatusLabel1->Text = "Loading Level: " + sceneName;
  this->Refresh();
  CreateCamera();

  DWORD i = 0;
  while(path[i])
  {
    char c = path[i];
    if(c >= 'A' && c <= 'Z') c += 32;
    path[i] = c;
    i++;
  }

  if(strstr(path, "scn.dat") || strstr(path, "skin.dat") || strstr(path, "mdl.dat"))
  {
    scene = new Th4Scene(path);
    sort=true;
  }
  else if(strstr(path, ".scn.xbx") || strstr(path, ".skin.xbx") || strstr(path, ".mdl.xbx"))
  {
    scene = new ThugScene(path);
    sort=true;
  }
  else if(strstr(path, ".psx"))
  {
    scene = new Th2Scene(path);
    /*if(scene->skyDome)
    {
    scene->skyDome->MoveObjects(scene);
    }*/
  }
  /*else if(strstr(path, ".bsp"))
  {
  scene = new Th3Scene(path,false);
  }*/
  else
    MessageBox::Show("unsupported scene");
  _fcloseall();

  if(scene)
  {
    globalMaterialList = &scene->matList;
    //scene->RemoveUnusedMaterials();//scene->OptimizeMaterials();
    //scene->CreateBuffers(Device, D3DFVF_XYZ|D3DFVF_DIFFUSE|MAX_TEXCOORDS);
    if(sort)
    {
      for(DWORD i=0, numMeshes = scene->GetNumMeshes(); i<numMeshes; i++) {Mesh* tmpMesh = scene->GetMesh(i); /*tmpMesh->Sort();*/ listBox1->Items->Add(gcnew String(tmpMesh->GetName().GetString()) + "[" + i.ToString() + "]"); properties->Add(gcnew MeshProperties(tmpMesh));  if(tmpMesh->IsTransparent() && tmpMesh->flags & MeshFlags::isVisible) numTrans++; else if(tmpMesh->flags & MeshFlags::isVisible) numOpaque++;}
    }
    else
    {
      for(DWORD i=0, numMeshes = scene->GetNumMeshes(); i<numMeshes; i++) {Mesh* tmpMesh = scene->GetMesh(i); /*tmpMesh->Sort();*/ listBox1->Items->Add(gcnew String(tmpMesh->GetName().GetString()) + "[" + i.ToString() + "]"); properties->Add(gcnew MeshProperties(tmpMesh));  if(tmpMesh->IsTransparent()&& tmpMesh->flags & MeshFlags::isVisible) numTrans++; else if(tmpMesh->flags & MeshFlags::isVisible) numOpaque++;}
    }

    opaque = new RenderableMesh[numOpaque];
    numOpaque=0;
    trans = new RenderableMesh[numTrans];
    numTrans=0;

    for(DWORD i=0, numMeshes = scene->GetNumMeshes(); i<numMeshes; i++)
    {
      Mesh* tmpMesh = scene->GetMesh(i);
      if(tmpMesh->flags & MeshFlags::isTransparent && tmpMesh->flags & MeshFlags::isVisible) 
      {
        //trans[numTrans] = tmpMesh; numTrans++;
         const DWORD numVerts = tmpMesh->GetNumVertices();

         if(numVerts != 0)
         {
           trans[numTrans].numVertices = numVerts;
           if(FAILED(Device->CreateVertexBuffer(numVerts*sizeof(Vertex), D3DUSAGE_WRITEONLY, D3DFVF_XYZ|D3DFVF_DIFFUSE|MAX_TEXCOORDS, D3DPOOL_DEFAULT, &trans[numTrans].vertices, NULL)))
             ::MessageBox(0,"failed to create vertex buffer","trans",0);


           Vertex* pVertices;
           trans[numTrans].vertices->Lock(0,0,(void**)&pVertices, 0);
           memcpy(pVertices, tmpMesh->GetVertices(),numVerts*sizeof(Vertex));
           trans[numTrans].vertices->Unlock();

          
           DWORD numIndices=0;
           trans[numTrans].numSplits = tmpMesh->GetNumSplits();
           trans[numTrans].matSplits = new RenderableMatSplit[trans[numTrans].numSplits];

           for(DWORD i=0; i<trans[numTrans].numSplits; i++)
           {
             trans[numTrans].matSplits[i].material = scene->matList.GetMaterial(tmpMesh->GetSplit(i)->matId);
             trans[numTrans].matSplits[i].numTris = tmpMesh->GetSplit(i)->numIndices;
             trans[numTrans].matSplits[i].startIndex = numIndices;
             numIndices += tmpMesh->GetSplit(i)->Indices.size(); 
           }
           Device->CreateIndexBuffer(numIndices*2, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &trans[numTrans].indices, NULL);

           WORD* indices;
           trans[numTrans].indices->Lock(0,0,(void**)&indices, 0);
           for(DWORD i=0; i<trans[numTrans].numSplits; i++)
           {
             numIndices = tmpMesh->GetSplit(i)->Indices.size();
             memcpy(indices, &tmpMesh->GetSplit(i)->Indices.front(), numIndices*2);
             indices+=numIndices;
           }
           trans[numTrans].indices->Unlock();
           numTrans++;
         }
      }
      else if(tmpMesh->flags & MeshFlags::isVisible)
      {
        //opaque[numOpaque] = tmpMesh; numOpaque++;
        const DWORD numVerts = tmpMesh->GetNumVertices();

         if(numVerts != 0)
         {
           opaque[numOpaque].numVertices = numVerts;
           if(FAILED(Device->CreateVertexBuffer(numVerts*sizeof(Vertex), D3DUSAGE_WRITEONLY, D3DFVF_XYZ|D3DFVF_DIFFUSE|MAX_TEXCOORDS, D3DPOOL_DEFAULT, &opaque[numOpaque].vertices, NULL)))
             ::MessageBox(0,"failed to create vertex buffer","",0);


           Vertex* pVertices;
           opaque[numOpaque].vertices->Lock(0,0,(void**)&pVertices, 0);
           memcpy(pVertices, tmpMesh->GetVertices(),numVerts*sizeof(Vertex));
           opaque[numOpaque].vertices->Unlock();

          
           DWORD numIndices=0;
           opaque[numOpaque].numSplits = tmpMesh->GetNumSplits();
           opaque[numOpaque].matSplits = new RenderableMatSplit[opaque[numOpaque].numSplits];

           for(DWORD i=0; i<trans[numTrans].numSplits; i++)
           {
             opaque[numOpaque].matSplits[i].material = scene->matList.GetMaterial(tmpMesh->GetSplit(i)->matId);
             opaque[numOpaque].matSplits[i].numTris = tmpMesh->GetSplit(i)->numIndices;
             opaque[numOpaque].matSplits[i].startIndex = numIndices;
             numIndices += tmpMesh->GetSplit(i)->Indices.size(); 
           }
           Device->CreateIndexBuffer(numIndices*2, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &opaque[numOpaque].indices, NULL);

           WORD* indices;
           opaque[numOpaque].indices->Lock(0,0,(void**)&indices, 0);
           for(DWORD i=0; i<opaque[numOpaque].numSplits; i++)
           {
             numIndices = tmpMesh->GetSplit(i)->Indices.size();
             memcpy(indices, &tmpMesh->GetSplit(i)->Indices.front(), numIndices*2);
             indices+=numIndices;
           }
           opaque[numOpaque].indices->Unlock();
           numOpaque++;
         }
      }
    }

    const DWORD numNodes = scene->nodes.size();
    Node* nodes = &scene->nodes.front();


    for(DWORD i=0; i<numNodes; i++)
    {
      if(nodes[i].Class.checksum == 0x1806DDF8)
      {
        spawns.push_back(&nodes[i]);
      }
      else if(nodes[i].Class.checksum == 0x8E6B02AD)
      {
        for(DWORD j=0; j<nodes[i].Links.size(); j++)
        {
          if(nodes[nodes[i].Links[j]].Class.checksum != 0x8E6B02AD)
          {
            nodes[i].Links.erase(nodes[i].Links.begin()+j);
            j--;
          }
        }
        if(!nodes[i].Links.size())
        {
          rails.push_back(RenderableRail());
          AddRails(i);
          //rails.back().numIndices /= 3;
        }
      }
    }

    for(DWORD i=0; i<numNodes; i++)
    {
      if(nodes[i].Class.checksum == 0x8E6B02AD && nodes[i].Links.size() && !nodes[i].IsAdded())
      {
        rails.push_back(RenderableRail());
        AddLinked(i);
        //rails.back().numIndices /= 3;
      }
    }


    for(DWORD i=0, numRails = rails.size(); i<numRails; i++)
    {
      const DWORD numVerts = rails[i].vertices.size();

      if(numVerts != 0)
      {
        const DWORD numIndices = rails[i].Indices.size();
        rails[i].numVerts = numVerts;
        rails[i].numIndices = numIndices;
        if(FAILED(Device->CreateVertexBuffer(numVerts*sizeof(ColouredVertex), D3DUSAGE_WRITEONLY, D3DFVF_XYZ|D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &rails[i].verts, NULL)))
          MessageBox::Show("failed to create vertex buffer");


        ColouredVertex* pVertices;
        rails[i].verts->Lock(0,0,(void**)&pVertices, 0);
        memcpy(pVertices, &rails[i].vertices.front(),numVerts*sizeof(ColouredVertex));
        rails[i].verts->Unlock();
        rails[i].vertices.clear();


        Device->CreateIndexBuffer(numIndices*2, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &rails[i].indices, NULL);


        WORD* indices;
        rails[i].indices->Lock(0,0,(void**)&indices, 0);
        memcpy(indices, &rails[i].Indices.front(), numIndices*2);
        rails[i].indices->Unlock();
        rails[i].Indices.clear();
      }
      else
        rails[i].Indices.clear();
    }

    vector<char*> numFiles;

    for(DWORD i=0; i<texFiles.numFiles; i++)
    {
      numFiles.push_back(texFiles.fileNames[i]);
    }

    free(texFiles.fileNames);
    redo:
    for(DWORD i=0; i<numFiles.size(); i++)
    {
      for(DWORD j=i+1; j<numFiles.size(); j++)
      {
        if(!stricmp(numFiles[i], numFiles[j]))
        {
          delete [] numFiles[i];
          numFiles[i]=NULL;
          numFiles.erase(numFiles.begin()+i);
          goto redo;
        }
      }
    }
    texFiles.numFiles = numFiles.size();
    texFiles.fileNames = (char**)malloc(numFiles.size()*sizeof(TexFiles));
    for(DWORD i=0; i<texFiles.numFiles; i++)
    {
      texFiles.fileNames[i] = numFiles[i];
      numFiles[i]=NULL;
    }

    numFiles.clear();

    /*redo:

    for(DWORD i=0; i<texFiles.numFiles; i++)
    {
      for(DWORD j=i+1; j<texFiles.numFiles; j++)
      {
        if(!stricmp(texFiles.fileNames[i],texFiles.fileNames[j]))
        {
          delete [] texFiles.fileNames[i];
          texFiles.numFiles--;
          memcpy(&texFiles.fileNames[i], &texFiles.fileNames[j], texFiles.numFiles*sizeof(char*));
          goto redo;
        }
      }
    }*/

    selectedObjects.reserve(scene->GetNumMeshes());
    //scene->FixZFighting();
    sprintf(info,"%d Verts %d Tris",scene->GetNumVertices(),scene->GetNumTriangles());
    Checksum("Shatter");
    Checksum("Kill");
    THPSLevel::toolStripStatusLabel1->Text = sceneName;
    loading = false;
    return true;
  }

  THPSLevel::toolStripStatusLabel1->Text = "?";

  return false;
}

void SetProgressbarMaximum(DWORD maximum)
{
  THPSLevel::toolStripProgressBar1->Maximum = maximum;
}

void SetProgressbarValue(DWORD value)
{
  if(value>THPSLevel::toolStripProgressBar1->Maximum)
    THPSLevel::toolStripProgressBar1->Value = THPSLevel::toolStripProgressBar1->Maximum;
  else
    THPSLevel::toolStripProgressBar1->Value = value;
}

void ResetProgressbar()
{
  THPSLevel::toolStripProgressBar1->Value = 0;
}

void IncreasProgressbar()
{
  if(THPSLevel::toolStripProgressBar1->Value<THPSLevel::toolStripProgressBar1->Maximum)
    THPSLevel::toolStripProgressBar1->Value++;
}

#pragma managed
bool THPSLevel::LoadState(char* name)
{
  render=false;
  _fcloseall();
  if(scene)
    UnloadScene();

  FILE* f = fopen("C:\\text","rb");
  fseek(f, 0, SEEK_END);
  DWORD size = ftell(f);
  fseek(f, 0, SEEK_SET);
  BYTE* pFile = new BYTE[size];
  fread(pFile, size, 1, f);
  fclose(f);
  LoadTextures(pFile);
  delete [] pFile;

  _fcloseall();

  /*scene = new Scene();

  f = fopen("C:\\mat","rb");
  fseek(f, 0, SEEK_END);
  size = ftell(f);
  fseek(f, 0, SEEK_SET);
  pFile = new BYTE[size];
  fread(pFile, size, 1, f);
  fclose(f);
  LoadMaterials(pFile);
  delete [] pFile;

  _fcloseall();*/

  /*f = fopen("C:\\script","rb");
  fseek(f, 0, SEEK_END);
  size = ftell(f);
  fseek(f, 0, SEEK_SET);
  pFile = new BYTE[size];
  fread(pFile, size, 1, f);
  fclose(f);
  LoadScripts(pFile);
  delete [] pFile;

  _fcloseall();*/

  f = fopen("C:\\lols","rb");
  fseek(f, 0, SEEK_END);
  size = ftell(f);
  fseek(f, 0, SEEK_SET);
  pFile = new BYTE[size];
  fread(pFile, size, 1, f);
  fclose(f);
  ::MAX_PASSES=1;
  ::MAX_TEXCOORDS=1;
  resetMouse=false;

  const static float version = 2.0f;
  float oldVer;
  oldVer = *(float*)pFile;
  pFile+=4;
  if(oldVer==version)
  {
    scene = new Scene();
    scene->SetName((char*)pFile);

    loading = true;
    this->sceneName = gcnew String((char*)pFile);
    toolStripStatusLabel1->Text = "Loading Level: " + sceneName;
    this->Refresh();

    if(LoadState_unmanaged(pFile))
    {
      pFile-=4;
      delete [] pFile;
      if(scene)
      {
        f = fopen("C:\\script","rb");
        fseek(f, 0, SEEK_END);
        size = ftell(f);
        fseek(f, 0, SEEK_SET);
        pFile = new BYTE[size];
        fread(pFile, size, 1, f);
        fclose(f);
        LoadScripts(pFile);
        delete [] pFile;

        globalMaterialList = &scene->matList;

        for(DWORD i=0, numMeshes = scene->GetNumMeshes(); i<numMeshes; i++) {Mesh* tmpMesh = scene->GetMesh(i); tmpMesh->node = scene->GetNode(tmpMesh->GetName()); /*tmpMesh->Sort();*/ listBox1->Items->Add(gcnew String(tmpMesh->GetName().GetString()) + "[" + i.ToString() + "]"); properties->Add(gcnew MeshProperties(tmpMesh));  if(tmpMesh->IsTransparent()) numTrans++; else numOpaque++;}

        /*opaque = new Mesh*[numOpaque];
        numOpaque=0;
        trans = new Mesh*[numTrans];
        numTrans=0;

        for(DWORD i=0, numMeshes = scene->GetNumMeshes(); i<numMeshes; i++)
        {
          Mesh* tmpMesh = scene->GetMesh(i);
          if(tmpMesh->flags & MeshFlags::isTransparent) 
          {
            trans[numTrans] = tmpMesh; numTrans++;
          }
          else
          {
            opaque[numOpaque] = tmpMesh; numOpaque++;
          }
        }*/

        const DWORD numNodes = scene->nodes.size();
        Node* nodes = &scene->nodes.front();


        for(DWORD i=0; i<numNodes; i++)
        {
          if(nodes[i].Class.checksum == 0x1806DDF8)
          {
            spawns.push_back(&nodes[i]);
          }
          else if(nodes[i].Class.checksum == 0x8E6B02AD)
          {
            for(DWORD j=0; j<nodes[i].Links.size(); j++)
            {
              if(nodes[nodes[i].Links[j]].Class.checksum != 0x8E6B02AD)
              {
                nodes[i].Links.erase(nodes[i].Links.begin()+j);
                j--;
              }
            }
            if(!nodes[i].Links.size())
            {
              rails.push_back(RenderableRail());
              AddRails(i);
              //rails.back().numIndices /= 3;
            }
          }
        }

        for(DWORD i=0; i<numNodes; i++)
        {
          if(nodes[i].Class.checksum == 0x8E6B02AD && nodes[i].Links.size() && !nodes[i].IsAdded())
          {
            rails.push_back(RenderableRail());
            AddLinked(i);
            //rails.back().numIndices /= 3;
          }
        }


        for(DWORD i=0, numRails = rails.size(); i<numRails; i++)
        {
          const DWORD numVerts = rails[i].vertices.size();

          if(numVerts != 0)
          {
            const DWORD numIndices = rails[i].Indices.size();
            rails[i].numVerts = numVerts;
            rails[i].numIndices = numIndices;
            if(FAILED(Device->CreateVertexBuffer(numVerts*sizeof(ColouredVertex), D3DUSAGE_WRITEONLY, D3DFVF_XYZ|D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &rails[i].verts, NULL)))
              MessageBox::Show("failed to create vertex buffer");


            ColouredVertex* pVertices;
            rails[i].verts->Lock(0,0,(void**)&pVertices, 0);
            memcpy(pVertices, &rails[i].vertices.front(),numVerts*sizeof(ColouredVertex));
            rails[i].verts->Unlock();
            rails[i].vertices.clear();


            Device->CreateIndexBuffer(numIndices*2, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &rails[i].indices, NULL);


            WORD* indices;
            rails[i].indices->Lock(0,0,(void**)&indices, 0);
            memcpy(indices, &rails[i].Indices.front(), numIndices*2);
            rails[i].indices->Unlock();
            rails[i].Indices.clear();
          }
          else
            rails[i].Indices.clear();
        }

        /*for(DWORD i=0, numFiles = texFiles.numFiles; i<numFiles; i++)
        {
          for(DWORD j=i+1; j<numFiles; j++)
          {
            if(!stricmp(texFiles.fileNames[i],texFiles.fileNames[j]))
            {
              delete [] texFiles.fileNames[i];
              texFiles.numFiles--;
              memcpy(texFiles.fileNames[i], texFiles.fileNames[j], texFiles.numFiles*sizeof(char*));
              texFiles.fileNames[numFiles]=NULL;
              i--;
              j--;
            }
          }
        }*/

        selectedObjects.reserve(scene->GetNumMeshes());
        //scene->FixZFighting();
        sprintf(info,"%d Verts %d Tris",scene->GetNumVertices(),scene->GetNumTriangles());
        Checksum("Shatter");
        Checksum("Kill");
        THPSLevel::toolStripStatusLabel1->Text = sceneName;

        ResetProgressbar();
        scene->SetSkyPos(camera->pos());
        SetDeviceStates();
        /*::defaultCulling = scene->cullMode;
        Device->SetRenderState(D3DRS_CULLMODE, scene->cullMode);

        if(scene->GetBlendMode()==1)
        {
          Device->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(128,30,30,30));
          Device->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );

          Device->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
          Device->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHA );
          Device->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
          Device->SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHA );
          Device->SetTextureStageState( 2, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
          Device->SetTextureStageState( 3, D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHA );
          Device->SetTextureStageState( 3, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
        }
        else
        {
          Device->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE2X );
          Device->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
          Device->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHA );
          Device->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
          Device->SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHA );
          Device->SetTextureStageState( 2, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
          Device->SetTextureStageState( 3, D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHA );
          Device->SetTextureStageState( 3, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
        }*/
        render = true;
        _fcloseall();
        loading = false;
        return true;
      }
    }
    else
    {
      loading = false;
      render = true;
      delete [] pFile;
      _fcloseall();
      if(scene)
        UnloadScene();
      scene=NULL;

      THPSLevel::toolStripStatusLabel1->Text = "?";

      return false;
    }
  }
  _fcloseall();
  return false;
}

LPITEMIDLIST ConvertPathToLpItemIdList(const char *pszPath)
{
  LPITEMIDLIST  pidl = NULL;
  LPSHELLFOLDER pDesktopFolder = NULL;
  OLECHAR       olePath[MAX_PATH];
  ULONG         chEaten;
  HRESULT       hr;

  if (SUCCEEDED(SHGetDesktopFolder(&pDesktopFolder)))
  {
    MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pszPath, -1, 
      olePath, MAX_PATH);
    hr = pDesktopFolder->ParseDisplayName(NULL, NULL, 
      olePath, &chEaten, &pidl, NULL);
    pDesktopFolder->Release();
    return pidl;
  }
  return NULL;
}

#pragma managed

System::Void THPSLevel::tHPS3ToolStripMenuItem1_Click(System::Object^  sender, System::EventArgs^  e) {
  if(scene)
  {

    //TCHAR path[MAX_PATH] = "";

    TCHAR path[MAX_PATH] = "";
    //TCHAR rootDir[MAX_PATH] = "";
    BROWSEINFO bi = {0};
    LPMALLOC pMalloc = NULL;
    bi.hwndOwner = (HWND)(void*)this->Handle.ToPointer();
    /*GetCurrentDirectory(
    MAX_PATH,
    rootDir
    );*/
    if (SUCCEEDED(SHGetMalloc(&pMalloc))) 
    {
      //bi.pidlRoot = ConvertPathToLpItemIdList(rootDir);
      //bi.pszDisplayName = folderName;
      bi.lpszTitle = "Choose folder";
      bi.ulFlags = BIF_NEWDIALOGSTYLE;
      //bi.lpfn = Export;

      LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

      if(pidl!=0)
      {
        SHGetPathFromIDList(pidl,path);
        DWORD pos = ((std::string)path).find_last_of("\\");
        char tmp[25]="";
        strcpy(tmp,&path[pos]);
        strcat(path,tmp);
        strcat(path,".bsp");

        toolStripStatusLabel1->Text = "Exporting Level: " + gcnew String(path);
        this->Refresh();
        scene->ExportBSP(path);
        _fcloseall();
        toolStripStatusLabel1->Text = this->sceneName;
        ResetProgressbar();
      }
      pMalloc->Free(pidl); 
      pMalloc->Release();
    }


    /*bool file;
    char theFileName[MAX_PATH];
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ZeroMemory(theFileName,sizeof(theFileName));

    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrTitle = "Save BSP";
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = "THPS3 BSP (*.BSP)\0*.bsp\0\0";
    ofn.lpstrFile = theFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY;
    ofn.lpstrInitialDir = NULL;

    if(!GetOpenFileName(&ofn))
    file=false;
    else 
    file = true;
    if(file)
    {
    DWORD i=0;
    while(theFileName[i])
    {
    char c = theFileName[i];
    if(c >= 'A' && c <= 'Z') c += 32;
    theFileName[i] = c;
    i++;
    }
    if(!strstr(theFileName,"bsp"))
    {
    strcat(theFileName,".bsp");
    }
    toolStripStatusLabel1->Text = "Exporting Level: " + gcnew String(theFileName);
    this->Refresh();
    scene->ExportBSP(theFileName);
    _fcloseall();
    toolStripStatusLabel1->Text = this->sceneName;
    ResetProgressbar();
    }*/
  }
  else
    MessageBox::Show("Load a Scene first");
}

void THPSLevel::UnloadScene()
{
  numOpaque=0;
  numTrans=0;
  delete opaque;
  delete trans;
  spawns.clear();
  rails.clear();
  listBox1->Items->Clear();
  selectedNodes.clear();
  //scene->Show(false);

  for(DWORD i=0; i<texFiles.numFiles; i++)
  {
    DeleteFile(texFiles.fileNames[i]);
    delete [] texFiles.fileNames[i];
  }

  texFiles.numFiles = 0;
  free(texFiles.fileNames);
  texFiles.fileNames = NULL;
  ReleaseTextures();
  if(scene->skyDome)
    delete scene->skyDome;
  //events
  /*selectedObjects.clear();
  events.clear();*/
  CleanupEvents();

  delete scene;
  scene=NULL;
  Scripts.clear();
  delete camera;
  camera = NULL;
  propertyGrid1->SelectedObject = 0;
  properties->Clear();
}

#pragma unmanaged

void ReleaseTextures()
{
  if(scene)
  {
    scene->matList.ReleaseTextures();
    if(scene->skyDome)
    {
      scene->skyDome->matList.ReleaseTextures();
    }

  }
}

#pragma managed

void THPSLevel::DeleteManagedObjects()
{
  /*RenderSurface->Release();
  RenderTexture->Release();*/
  m_font->Release();
  m_font = NULL;

  ReleaseTextures();
  /*if(spawnpoint)
  spawnpoint->Release();
  spawnpoint=NULL;*/
}

#pragma unmanaged

inline void CreateTextures()
{
  scene->CreateTextures();
}

#pragma managed

void THPSLevel::CreateManagedObjects()
{
  /*if( FAILED( D3DXLoadMeshFromX( "Tiger.x", D3DXMESH_SYSTEMMEM | D3DXMESH_DYNAMIC,
  Device, NULL,
  NULL/*&pD3DXMtrlBuffer*//*, NULL, NULL/*&g_dwNumMaterials*///,
  /* &spawnpoint ) ) )
  MessageBox::Show("failed to create spawn mesh");*/
  D3DXCreateFontA( Device, 25, 0, FW_NORMAL, 0, FALSE, DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &m_font );
  if(scene)
  {
    CreateTextures();
  }
}

System::Void THPSLevel::objectsToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
  toolStripDropDownButton1->Text = "Mesh";

  renderCollision = false;
  showTextures=toolStripButton1->Checked;
  toolStripButton1->Enabled=true;
  toolStripButton1->Visible=true;
  Device->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|MAX_TEXCOORDS);
  Device->SetRenderState(D3DRS_ALPHAREF, 128);
  Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
}

System::Void THPSLevel::collisionToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
  toolStripDropDownButton1->Text = "Collision";

  renderCollision = true;
  //toolStripButton1->Checked=false;
  showTextures=false;
  toolStripButton1->Enabled=false;
  toolStripButton1->Visible=false;
  for(DWORD i=0; i<MAX_PASSES; i++) Device->SetTexture(i,NULL);
  Device->SetRenderState(D3DRS_ALPHAREF, 128);
  Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
  Device->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
  Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
}


void THPSLevel::OpenLoadSceneDialog()
{
  render = false;
  if(scene)
  {
    UnloadScene();
  }
  ::MAX_PASSES=1;
  ::MAX_TEXCOORDS=1;
  ::resetMouse=false;
  bool file;
  char theFileName[MAX_PATH];
  OPENFILENAME ofn;
  ZeroMemory(&ofn, sizeof(ofn));
  ZeroMemory(theFileName,sizeof(theFileName));

  ofn.lStructSize = sizeof(ofn);
  ofn.hwndOwner = NULL;
  ofn.lpstrFilter = "All supported MDL/SKIN/SCN/PSX/BSP (*.mdl *mdl.xbx *mdl.dat *.skin *skin.xbx *skin.dat *.scn *scn.xbx *scn.dat *.psx *.bsp)\0*.mdl;*mdl.xbx;*mdl.dat;*.skin;*skin.xbx;*skin.dat;*.scn;*scn.xbx;*scn.dat;*.psx;*.bsp\0THPS4 MDL/SKIN/SCN (*.mdl *mdl.xbx *mdl.dat *.skin *skin.xbx *skin.dat *.scn *scn.xbx *scn.dat)\0*.mdl;*mdl.xbx;*mdl.dat;*.skin;*skin.xbx;*skin.dat;*.scn;*scn.xbx;*scn.dat\0THPS2 psx (*.psx)\0*.psx\0THPS3 BSP (*.BSP)\0*.bsp\0\0";
  ofn.lpstrFile = theFileName;
  ofn.nMaxFile = MAX_PATH;
  ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
  ofn.lpstrInitialDir = NULL;

  if(!GetOpenFileName(&ofn))
    file=false;
  else 
    file = true;
  if(file)
  {
    if(LoadScene(theFileName))
    {
      ResetProgressbar();
      scene->SetSkyPos(camera->pos());
      SetDeviceStates();
      /*::defaultCulling = scene->cullMode;
      Device->SetRenderState(D3DRS_CULLMODE, scene->cullMode);

      if(scene->GetBlendMode()==1)
      {
        Device->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(128,30,30,30));
        Device->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );

        Device->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
        Device->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHA );
        Device->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
        Device->SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHA );
        Device->SetTextureStageState( 2, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
        Device->SetTextureStageState( 3, D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHA );
        Device->SetTextureStageState( 3, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
      }
      else
      {
        Device->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE2X );
        Device->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
        Device->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHA );
        Device->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
        Device->SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHA );
        Device->SetTextureStageState( 2, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
        Device->SetTextureStageState( 3, D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHA );
        Device->SetTextureStageState( 3, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
      }*/
    }
  }
  else
    scene=NULL;
  render = true;
}

System::Void THPSLevel::tHPS2ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
  OpenLoadSceneDialog();
}

System::Void THPSLevel::tHPS4ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
  OpenLoadSceneDialog();
}

void ClearObjectList()
{
  THPSLevel::listBox1->ClearSelected();
}

void SelectedAlreadySelected(DWORD index)
{
  if(heldCntrl)
    THPSLevel::listBox1->SetSelected(index, false);
}

void SelectObject(DWORD index)
{
  if(!heldCntrl)
    THPSLevel::listBox1->ClearSelected();

  THPSLevel::listBox1->SetSelected(index, true);
}
#pragma unmanaged
void SelectObjectOnly(Mesh* mesh, bool multiselected)
{
  if(!multiselected)
    selectedObjects.clear();

  selectedObjects.push_back(mesh);
}
#pragma managed
System::Void THPSLevel::LostFocus(System::Object^  sender, System::EventArgs^  e) {
  render = false;
  //MessageBox::Show("Lost");
}

System::Void THPSLevel::GotFocus(System::Object^  sender, System::EventArgs^  e) {
  if(!loading && scene)
    render = true;
  // MessageBox::Show("GOt");
}

#pragma unmanaged

SelectedObject::SelectedObject(Mesh* mesh, bool e)
{
  this->mesh = mesh;
  if(e)
    events.push_back(Event(Event::eSelection, selectedObjects));
}

#pragma managed

System::Void THPSLevel::listBox1_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
  listBox2->Items->Clear();
  if(listBox1->SelectedIndex >= 0 && listBox1->SelectedIndex < scene->GetNumMeshes())
    propertyGrid1->SelectedObject = properties[listBox1->SelectedIndex];
  else
    propertyGrid1->SelectedObject = 0;
  if(selectionMode==0)
  {
    selectedObjects.clear();
    Mesh* tmpMesh = NULL;
    for ( int x = 0; x < listBox1->SelectedIndices->Count; x++ )
    {
      tmpMesh = scene->GetMesh((DWORD)listBox1->SelectedIndices[x]);
      selectedObjects.push_back(SelectedObject(tmpMesh));
    }

    /* if(tmpMesh)
    {
    for(DWORD i=0; i<tmpMesh->rails.size(); i++)
    listBox2->Items->Add("Rail" + i.ToString());
    }*/
  }

  this->panel6->Focus();
}

void THPSLevel::SetDeviceStates()
{
  rct.left = 10;
  rct.right = 1010;
  rct.top = 10;
  rct.bottom = 90;
  if(m_font)
    m_font->Release();
  D3DXCreateFontA( Device, 25, 0, FW_NORMAL, 0, FALSE, DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &m_font );
  Device->SetRenderState( D3DRS_ZENABLE, TRUE );
  Device->SetRenderState( D3DRS_LIGHTING, FALSE);
  Device->SetRenderState( D3DRS_ZENABLE,          D3DZB_TRUE );
  Device->SetRenderState( D3DRS_ZWRITEENABLE,          TRUE );
  if(scene)
    Device->SetRenderState( D3DRS_CULLMODE,         scene->cullMode );
  else
    Device->SetRenderState( D3DRS_CULLMODE,         D3DCULL_NONE );
  //Device->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );
  //Device->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
  Device->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
  Device->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
  Device->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);
  Device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
  Device->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|MAX_TEXCOORDS);
  Device->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
  Device->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
  //Device->SetRenderState(D3DRS_DITHERENABLE, TRUE);
  //Device->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);
  Device->SetRenderState(D3DRS_BLENDFACTOR, D3DXCOLOR(255,0,0,255));

  Device->SetRenderState(D3DRS_BLENDOP,D3DBLENDOP_ADD);
  Device->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC );
  Device->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC );
  Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
  Device->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC );
  Device->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC );
  Device->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
  Device->SetSamplerState( 2, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC );
  Device->SetSamplerState( 2, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC );
  Device->SetSamplerState(2, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
  Device->SetSamplerState( 3, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC );
  Device->SetSamplerState( 3, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC );
  Device->SetSamplerState(3, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
  static const float mipLodBias = -1.0;
  Device->SetSamplerState(0, D3DSAMP_MIPMAPLODBIAS, *((DWORD*)&mipLodBias));
  Device->SetSamplerState(1, D3DSAMP_MIPMAPLODBIAS, *((DWORD*)&mipLodBias));
  Device->SetSamplerState(2, D3DSAMP_MIPMAPLODBIAS, *((DWORD*)&mipLodBias));
  Device->SetSamplerState(3, D3DSAMP_MIPMAPLODBIAS, *((DWORD*)&mipLodBias));
  D3DCAPS9 caps;
  Device->GetDeviceCaps(&caps);
  Device->SetSamplerState(0, D3DSAMP_MAXANISOTROPY,  caps.MaxAnisotropy);
  Device->SetSamplerState(1, D3DSAMP_MAXANISOTROPY,  caps.MaxAnisotropy);
  Device->SetSamplerState(2, D3DSAMP_MAXANISOTROPY,  caps.MaxAnisotropy);
  Device->SetSamplerState(3, D3DSAMP_MAXANISOTROPY,  caps.MaxAnisotropy);

  Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
  //D3DTEXTUREOP::D3DTOP_BLENDFACTORALPHA
  if(scene)
  {
    if(scene->GetBlendMode()==1)
    {
      Device->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(128,30,30,30));
      Device->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );

      Device->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
      Device->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHA );
      Device->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
      Device->SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHA );
      Device->SetTextureStageState( 2, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
      Device->SetTextureStageState( 3, D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHA );
      Device->SetTextureStageState( 3, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
    }
    else
    {
      Device->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE2X );
      Device->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
      Device->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHA );
      Device->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
      Device->SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHA );
      Device->SetTextureStageState( 2, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
      Device->SetTextureStageState( 3, D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHA );
      Device->SetTextureStageState( 3, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
    }
  }
  else
  {
    Device->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
    Device->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
  }
  Device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
  Device->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
  Device->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE ); 
  Device->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
  Device->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
  Device->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
  Device->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE ); 
  Device->SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_CURRENT );
  Device->SetTextureStageState( 2, D3DTSS_COLORARG1, D3DTA_TEXTURE );
  Device->SetTextureStageState( 2, D3DTSS_COLORARG2, D3DTA_CURRENT );
  Device->SetTextureStageState( 2, D3DTSS_ALPHAARG1, D3DTA_TEXTURE ); 
  Device->SetTextureStageState( 2, D3DTSS_ALPHAARG2, D3DTA_CURRENT );
  Device->SetTextureStageState( 3, D3DTSS_COLORARG1, D3DTA_TEXTURE );
  Device->SetTextureStageState( 3, D3DTSS_COLORARG2, D3DTA_CURRENT );
  Device->SetTextureStageState( 3, D3DTSS_ALPHAARG1, D3DTA_TEXTURE ); 
  Device->SetTextureStageState( 3, D3DTSS_ALPHAARG2, D3DTA_CURRENT );

  Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);

  Device->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
  Device->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );
  Device->SetTextureStageState( 2, D3DTSS_TEXCOORDINDEX, 2 );
  Device->SetTextureStageState( 3, D3DTSS_TEXCOORDINDEX, 3 );
  Device->SetTextureStageState( 4, D3DTSS_TEXCOORDINDEX, 4 );
  Device->SetTextureStageState( 5, D3DTSS_TEXCOORDINDEX, 5 );
  Device->SetTextureStageState( 6, D3DTSS_TEXCOORDINDEX, 6 );
  Device->SetTextureStageState( 7, D3DTSS_TEXCOORDINDEX, 7 );
  Device->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
  Device->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
  Device->SetSamplerState(0,D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
  Device->SetSamplerState(1,D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
  Device->SetSamplerState( 1, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
  Device->SetSamplerState(1,D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
  Device->SetSamplerState(2,D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
  Device->SetSamplerState( 2, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
  Device->SetSamplerState(2,D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
  Device->SetSamplerState(3,D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
  Device->SetSamplerState( 3, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
  Device->SetSamplerState(3, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
  D3DXMATRIX mat;
  D3DXMatrixIdentity(&mat);
  Device->SetTransform(D3DTS_WORLD, &mat);
  Device->GetViewport(&port);
  Device->SetRenderState(D3DRS_ALPHAREF, 8);
  CreateCamera();
}

void THPSLevel::AttemptRecovery()
{
  if (Device == NULL) return;

  HRESULT hr = Device->TestCooperativeLevel();

  if(hr == D3DERR_DEVICENOTRESET)
  {
    Device->Reset(&DevParams);
    deviceLost = false;
  }
}

void THPSLevel::SafeTurnOffRendering()
{
  render = false;
  while(rendering)
    Sleep(10);
}

System::Void THPSLevel::surfaceModeToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
  toolStripSplitButton1->Text = "Face Mode";

  selectionMode = 1;
  this->tabControl1->SelectedIndex = 2;
  selectedObjects.clear();
  ClearObjectList();
}
System::Void THPSLevel::railModeToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
  toolStripSplitButton1->Text = "Node Mode";

  selectionMode = 2;
  this->tabControl1->SelectedIndex = 1;
}
System::Void THPSLevel::objectModeToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
  toolStripSplitButton1->Text = "Object Mode";

  selectionMode = 0;
  this->tabControl1->SelectedIndex = 0;
  selectedTriangles.clear();
  ClearObjectList();
}

System::Void THPSLevel::FKeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
  if(e->KeyCode == Keys::Z)
  {
    if(camera)
    {
      resetMouse = !resetMouse;
      if(resetMouse)
      {
        POINT mouse;
        DWORD middleX = port.Width/2;
        DWORD middleY = port.Height/2;
        mouse.x = middleX;
        mouse.y = middleY;
        ClientToScreen(handleWindow, &mouse);
        SetCursorPos(mouse.x,mouse.y);
      }
    }
  }
  else if(e->KeyCode == Keys::Escape)//(System::Windows::Forms::Keys)27)
    this->Close();
}

System::Void THPSLevel::toolStripButton1_Click(System::Object^  sender, System::EventArgs^  e)
{
  for(DWORD i=0; i<MAX_PASSES; i++) Device->SetTexture(i,NULL);

  showTextures = !showTextures;
  Device->SetRenderState(D3DRS_ALPHAREF, 128);
  Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
  if(showTextures)
    Device->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|MAX_TEXCOORDS);
  else
    Device->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
}

#pragma unmanaged

void DeleteSelectedNodes()
{
  DWORD numSelectedNodes = selectedNodes.size();
  if(numSelectedNodes)
  {
    WORD* deletedNodes = new WORD[numSelectedNodes];
    Node* nodes = &scene->nodes.front();
    bool repopulateSpawns=false;
    for(DWORD i=0; i<numSelectedNodes; i++)
    {
      deletedNodes[i] = ((DWORD)selectedNodes[i]-(DWORD)nodes)/sizeof(Node);
      if(selectedNodes[i]->Class.checksum == 0x1806DDF8)
        repopulateSpawns=true;
    }
    for(DWORD i=0; i<numSelectedNodes; i++)
    {
      scene->DeleteNode(deletedNodes[i]);
    }
    delete deletedNodes;
    selectedNodes.clear();
    scene->FixNodes();
    if(repopulateSpawns)
    {
      spawns.clear();
      for(DWORD i=0, numNodes = scene->nodes.size(); i<numNodes; i++)
      {
        if(nodes[i].Class.checksum == 0x1806DDF8)
        {
          spawns.push_back(&nodes[i]);
        }
      }
    }
  }
}

float
  DistanceBetweenLines( D3DXVECTOR3& origin0, D3DXVECTOR3& direction0, D3DXVECTOR3& origin1, D3DXVECTOR3& direction1)
{
  D3DXVECTOR3   u = direction0 - origin0;
  D3DXVECTOR3   v = direction1 - origin1;
  D3DXVECTOR3   w = origin0 - origin1;
  const float    a = D3DXVec3Dot(&u,&u);        // always >= 0
  const float    b = D3DXVec3Dot(&u,&v);
  const float    c = D3DXVec3Dot(&v,&v);        // always >= 0
  const float    d = D3DXVec3Dot(&u,&w);
  const float    e = D3DXVec3Dot(&v,&w);
  const float    D = a*c - b*b;       // always >= 0
  static float    sc, tc;

  // compute the line parameters of the two closest points
  if (D < 0.00000001f) {         // the lines are almost parallel
    sc = 0.0;
    tc = (b>c ? d/b : e/c);   // use the largest denominator
  }
  else {
    sc = (b*e - c*d) / D;
    tc = (a*e - b*d) / D;
  }

  // get the difference of the two closest points
  D3DXVECTOR3   dP = w + (sc * u) - (tc * v);  // = L1(sc) - L2(tc)

  return sqrtf(D3DXVec3Dot(&dP,&dP));   // return the closest distance
}

void SelectNode()
{
  if(!heldCntrl)
    selectedNodes.clear();
  const DWORD numNodes = scene->nodes.size();
  Node* nodes = &scene->nodes.front();
  D3DXMATRIX nodeRotation;
  D3DXMATRIX nodeTranslation;
  //D3DXMATRIX world = scene->Get;
  D3DXVECTOR3 rayOrigin, rayDirection, translatedOrigin, translatedDirection;
  static float distance = 0.0f;
  static float closest = 0.0f;
  bool intersected = false;
  DWORD nodeIndex = 0;
  static DWORD numLinks = 0;
  static const D3DXMATRIX* matWorld = scene->GetWorldMatrix();
  D3DXMATRIX world, invWorld;
  GetMouseRay(&rayDirection,&rayOrigin);
  POINT Mouse;
  GetCursorPos(&Mouse);
  ScreenToClient(handleWindow,&Mouse);
  D3DXVECTOR3 hitPoint;
  DWORD tmp = defaultCulling;
  defaultCulling = D3DCULL_NONE;
  //D3DXVECTOR3 hitPoint = scene->GetClosestHitPoint(&rayOrigin, &rayDirection, &Mouse);

  for(DWORD i=0; i<numNodes; i++)
  {
    if(nodes[i].Class.checksum == 0x1806DDF8)
    {
      if(showSpawns)
      {
        D3DXMatrixRotationYawPitchRoll(&nodeRotation,-nodes[i].Angles.y+D3DX_PI,nodes[i].Angles.x,nodes[i].Angles.z);
        /*D3DXMatrixScaling(&nodeTranslation,20.0f,20.0f,20.0f);
        nodeTranslation(3,0) = nodes[i]->Position.x;
        nodeTranslation(3,1) = nodes[i]->Position.y;
        nodeTranslation(3,2) = nodes[i]->Position.z;*/
        D3DXMatrixTranslation(&nodeTranslation,nodes[i].Position.x,nodes[i].Position.y,nodes[i].Position.z);
        D3DXMatrixMultiply(&world, &nodeRotation, &nodeTranslation);
        D3DXMatrixInverse(&invWorld,NULL,&world);
        D3DXVec3TransformCoord(&translatedOrigin,&rayOrigin,&invWorld);
        D3DXVec3TransformNormal(&translatedDirection,&rayDirection,&invWorld);


        if(arrow->Intersect(&translatedOrigin,&translatedDirection,&distance/*,NULL,NULL*/) && (closest>=distance || !intersected))
        {
          closest = distance;
          nodeIndex = i;
          intersected=true;
        }

      }
    }
    else if(nodes[i].Class.checksum == 0x8E6B02AD)
    {
      D3DXVECTOR3 pos, pos2;
      if(showRails)
      {
        const DWORD numLinks = nodes[i].Links.size();
        if(numLinks)
        {
          for(DWORD j=0; j<numLinks; j++)
          {
            if(nodes[nodes[i].Links[j]].Class.checksum == 0x8E6B02AD)
            {
              D3DXVec3Project(&pos, (D3DXVECTOR3*)&nodes[i], &port, &camera->proj(), &camera->view(), matWorld);
              D3DXVec3Project(&pos2, (D3DXVECTOR3*)&nodes[nodes[i].Links[j]], &port, &camera->proj(), &camera->view(), matWorld);
              if((pos.z<1.0f && pos.z>0.0f) || (pos2.z<1.0f && pos2.z>0.0f))
              {
                float tmpDist;

                float MAX_DIST = nodes[i].Position.x-nodes[nodes[i].Links[j]].Position.x;
                MAX_DIST *= MAX_DIST;
                static bool axis = false;
                if(((nodes[i].Position.z-nodes[nodes[i].Links[j]].Position.z)*(nodes[i].Position.z-nodes[nodes[i].Links[j]].Position.z))>MAX_DIST)
                  axis = true;
                else
                  axis = false;

                if(TriangleIntersection(&D3DXVECTOR3(axis ? nodes[i].Position.x+10.0f : nodes[i].Position.x, nodes[i].Position.y-10.0f, axis ? nodes[i].Position.z : nodes[i].Position.z+10.0f), &D3DXVECTOR3(axis ? nodes[i].Position.x+10.0f : nodes[i].Position.x, nodes[i].Position.y+10.0f, axis ? nodes[i].Position.z : nodes[i].Position.z+10.0f), &D3DXVECTOR3(axis ? nodes[nodes[i].Links[j]].Position.x+10.0f : nodes[nodes[i].Links[j]].Position.x, nodes[nodes[i].Links[j]].Position.y-10.0f, axis ? nodes[nodes[i].Links[j]].Position.z : nodes[nodes[i].Links[j]].Position.z+10.0f), &rayOrigin,&rayDirection, &hitPoint))
                {
                  distance = D3DXVec3Length(&(*(D3DXVECTOR3*)&nodes[i].Position-hitPoint));

                  if((tmpDist = D3DXVec3Length(&(*(D3DXVECTOR3*)&nodes[nodes[i].Links[j]].Position-hitPoint)))<distance)
                  {
                    distance = tmpDist;
                    if(distance < closest || !intersected)
                    {
                      intersected=true;
                      closest = D3DXVec3Length(&(rayOrigin-hitPoint));//closest = distance;
                      nodeIndex = nodes[i].Links[j];
                    }
                  }
                  else
                  {
                    if(distance < closest || !intersected)
                    {
                      intersected=true;
                      closest = D3DXVec3Length(&(rayOrigin-hitPoint));//closest = distance;
                      nodeIndex = i;
                    }
                  }
                }
                if(TriangleIntersection(&D3DXVECTOR3(axis ? nodes[i].Position.x+10.0f : nodes[i].Position.x, nodes[i].Position.y-10.0f, axis ? nodes[i].Position.z : nodes[i].Position.z+10.0f), &D3DXVECTOR3(axis ? nodes[nodes[i].Links[j]].Position.x+10.0f : nodes[nodes[i].Links[j]].Position.x, nodes[nodes[i].Links[j]].Position.y+10.0f, axis ? nodes[nodes[i].Links[j]].Position.z : nodes[nodes[i].Links[j]].Position.z+10.0f), &D3DXVECTOR3(axis ? nodes[nodes[i].Links[j]].Position.x+10.0f : nodes[nodes[i].Links[j]].Position.x, nodes[nodes[i].Links[j]].Position.y-10.0f, axis ? nodes[nodes[i].Links[j]].Position.z : nodes[nodes[i].Links[j]].Position.z+10.0f), &rayOrigin,&rayDirection, &hitPoint))
                {
                  distance = D3DXVec3Length(&(*(D3DXVECTOR3*)&nodes[i].Position-hitPoint));

                  if((tmpDist = D3DXVec3Length(&(*(D3DXVECTOR3*)&nodes[nodes[i].Links[j]].Position-hitPoint)))<distance)
                  {
                    distance = tmpDist;
                    if(distance < closest || !intersected)
                    {
                      intersected=true;
                      closest = D3DXVec3Length(&(rayOrigin-hitPoint));//closest = distance;
                      nodeIndex = nodes[i].Links[j];
                    }
                  }
                  else
                  {
                    if(distance < closest || !intersected)
                    {
                      intersected=true;
                      closest = D3DXVec3Length(&(rayOrigin-hitPoint));//closest = distance;
                      nodeIndex = i;
                    }
                  }
                }


                if(TriangleIntersection(&D3DXVECTOR3(axis ? nodes[i].Position.x-10.0f : nodes[i].Position.x, nodes[i].Position.y-10.0f, axis ? nodes[i].Position.z : nodes[i].Position.z-10.0f), &D3DXVECTOR3(axis ? nodes[i].Position.x-10.0f : nodes[i].Position.x, nodes[i].Position.y+10.0f, axis ? nodes[i].Position.z : nodes[i].Position.z-10.0f), &D3DXVECTOR3(axis ? nodes[nodes[i].Links[j]].Position.x-10.0f : nodes[nodes[i].Links[j]].Position.x, nodes[nodes[i].Links[j]].Position.y-10.0f, axis ? nodes[nodes[i].Links[j]].Position.z : nodes[nodes[i].Links[j]].Position.z-10.0f), &rayOrigin,&rayDirection, &hitPoint))
                {
                  distance = D3DXVec3Length(&(*(D3DXVECTOR3*)&nodes[i].Position-hitPoint));

                  if((tmpDist = D3DXVec3Length(&(*(D3DXVECTOR3*)&nodes[nodes[i].Links[j]].Position-hitPoint)))<distance)
                  {
                    distance = tmpDist;
                    if(distance < closest || !intersected)
                    {
                      intersected=true;
                      closest = D3DXVec3Length(&(rayOrigin-hitPoint));//closest = distance;
                      nodeIndex = nodes[i].Links[j];
                    }
                  }
                  else
                  {
                    if(distance < closest || !intersected)
                    {
                      intersected=true;
                      closest = D3DXVec3Length(&(rayOrigin-hitPoint));//closest = distance;
                      nodeIndex = i;
                    }
                  }
                }
                if(TriangleIntersection(&D3DXVECTOR3(axis ? nodes[i].Position.x-10.0f : nodes[i].Position.x, nodes[i].Position.y-10.0f, axis ? nodes[i].Position.z : nodes[i].Position.z-10.0f), &D3DXVECTOR3(axis ? nodes[nodes[i].Links[j]].Position.x-10.0f : nodes[nodes[i].Links[j]].Position.x, nodes[nodes[i].Links[j]].Position.y+10.0f, axis ? nodes[nodes[i].Links[j]].Position.z : nodes[nodes[i].Links[j]].Position.z-10.0f), &D3DXVECTOR3(axis ? nodes[nodes[i].Links[j]].Position.x-10.0f : nodes[nodes[i].Links[j]].Position.x, nodes[nodes[i].Links[j]].Position.y-10.0f, axis ? nodes[nodes[i].Links[j]].Position.z : nodes[nodes[i].Links[j]].Position.z-10.0f), &rayOrigin,&rayDirection, &hitPoint))
                {
                  distance = D3DXVec3Length(&(*(D3DXVECTOR3*)&nodes[i].Position-hitPoint));

                  if((tmpDist = D3DXVec3Length(&(*(D3DXVECTOR3*)&nodes[nodes[i].Links[j]].Position-hitPoint)))<distance)
                  {
                    distance = tmpDist;
                    if(distance < closest || !intersected)
                    {
                      intersected=true;
                      closest = D3DXVec3Length(&(rayOrigin-hitPoint));//closest = distance;
                      nodeIndex = nodes[i].Links[j];
                    }
                  }
                  else
                  {
                    if(distance < closest || !intersected)
                    {
                      intersected=true;
                      closest = D3DXVec3Length(&(rayOrigin-hitPoint));//closest = distance;
                      nodeIndex = i;
                    }
                  }
                }

                /*if(hitPoint==D3DXVECTOR3(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF))
                {
                const float xDist = pos.x-Mouse.x;
                const float yDist = pos.y-Mouse.y;

                if(((distance = xDist*xDist + yDist*yDist))<100.0f)
                {
                if(distance==0)
                distance = pos.z+65000.0f;
                else
                distance = (xDist*xDist + yDist*yDist)*pos.z+95000.0f;

                if(!intersected || distance <= closest)
                {
                closest=distance;
                intersected=true;
                nodeIndex = i;
                }
                }
                }
                else
                {
                distance = D3DXVec3Length(&(*(D3DXVECTOR3*)&nodes[i].Position-hitPoint));
                if(distance<100.0f && (distance<=closest || !intersected))
                {
                closest = distance;
                nodeIndex = i;
                }
                }*/

                /* const __restrict float xDist = pos.x-Mouse.x;
                const float yDist = pos.y-Mouse.y;

                if(((distance = xDist*xDist + yDist*yDist))<100.0f)
                {
                if(distance==0)
                distance = pos.z+65000.0f;
                else
                distance = (xDist*xDist + yDist*yDist)*pos.z+95000.0f;

                if(!intersected || distance <= closest)
                {
                closest=distance;
                intersected=true;
                nodeIndex = i;
                }
                }
                else if(pos.x>0.0f && pos.y>0.0f && pos.x<port.Width && pos.y<port.Height && DistanceBetweenLines(rayOrigin,rayDirection,*(D3DXVECTOR3*)&nodes[i].Position, *(D3DXVECTOR3*)&nodes[nodes[i].Links[j]].Position)<10.0f)
                {
                //closest = pos.z;
                static bool hasHit=false;
                distance = xDist*xDist + yDist*yDist;
                if(distance==0)
                distance = pos.z+95000.0f;
                else
                distance = distance*pos.z+95000.0f;
                if(distance<=closest || !intersected)
                {
                hasHit=true;
                closest=distance;
                }
                else
                hasHit=false;
                distance = xDist*xDist + yDist*yDist;
                D3DXVec3Project(&pos, (D3DXVECTOR3*)&nodes[nodes[i].Links[j]].Position, &port, &camera->proj(), &camera->view(), matWorld);
                const float xDist = pos.x-Mouse.x;
                const float yDist = pos.y-Mouse.y;
                if(hasHit && distance<(xDist*xDist + yDist*yDist))
                {
                intersected=true;
                nodeIndex = i;
                }
                else
                {
                distance = xDist*xDist + yDist*yDist;
                if(distance==0)
                distance = pos.z+95000.0f;
                else
                distance = distance*pos.z+95000.0f;
                if(distance<=closest || !intersected)
                {
                nodeIndex = nodes[i].Links[j];
                closest=distance;
                intersected=true;
                }

                }

                }*/
              }
            }
          }
        }
        else
        {
          D3DXVec3Project(&pos, (D3DXVECTOR3*)&nodes[i].Position, &port, &camera->proj(), &camera->view(), matWorld);
          if(pos.z<1.0f && pos.z>0.0f)
          {
            const float xDist = pos.x-Mouse.x;
            const float yDist = pos.y-Mouse.y;

            if(((distance = xDist*xDist + yDist*yDist))<100.0f)
            {
              if(distance==0)
                distance = pos.z+65000.0f;
              else
                distance = (xDist*xDist + yDist*yDist)*pos.z+95000.0f;

              if(!intersected || distance < closest)
              {
                closest=distance;
                intersected=true;
                nodeIndex = i;
              }
            }
          }
        }
      }
    }
  }
  defaultCulling = tmp;
  if(intersected)
  {
    if(!nodes[nodeIndex].IsSelected())
      selectedNodes.push_back(&nodes[nodeIndex]);
  }
}

__declspec(noalias) void DrawFrame()
{
  IDirect3DDevice9Ex* const __restrict Dev = Device;

  Dev->Clear(0,NULL,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,0,1.0f,0);
  Dev->BeginScene();
  if(scene)
  {
    static int tLast = 0;
    const int tNow = GetTickCount();
    Update(tNow-tLast);
    tLast = tNow;
    if(showSky)
    {
      if(scene->skyDome)
      {
        Dev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        Dev->SetRenderState(D3DRS_ZENABLE, FALSE);
        scene->SetSkyPos(camera->pos());
        Dev->SetTransform(D3DTS_WORLD, scene->GetSkyMatrix());
        /*for(DWORD i=0; i<MAX_PASSES; i++)
        Device->SetSamplerState( i, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP );//Smoothly stretch the sky width*/
        if(showTextures)
        {
          //if(enableTransparency)
          scene->skyDome->Render(Dev);
          /*else
          scene->skyDome->RenderNoTrans(Device);*/
        }
        else
          scene->skyDome->RenderShaded(Dev);
        /*for(DWORD i=0; i<MAX_PASSES; i++)
        Device->SetSamplerState( i, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP );*/
        Dev->SetTransform(D3DTS_WORLD, scene->GetWorldMatrix());
        Dev->SetRenderState(D3DRS_ZENABLE, TRUE);
      }
    }

    Dev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

    if(showTextures)
    {
      Dev->SetRenderState(D3DRS_ALPHAREF, 128);
     // Mesh** const __restrict opaqueMeshes = opaque;
      //MaterialList* const __restrict matList = globalMaterialList;
      for(DWORD i=0, numMeshes = numOpaque; i< numMeshes; i++)
      {
        opaque[i].Render0();
        //if(opaqueMeshes[i]->flags & MeshFlags::isVisible) opaqueMeshes[i]->Render0(matList);
      }

       //MaterialList* const __restrict matList = globalMaterialList;
       //scene->RenderOpaque();

      if(showSpawns/* || this->toolStripButton4->Checked || this->toolStripButton6->Checked*/)
      {
        const DWORD numSpawns = spawns.size();

        if(numSpawns != 0)
        {
          //Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVBLENDFACTOR);
          Dev->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
          Dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
          Dev->SetStreamSource(0, arrow->verts, 0, sizeof(Vertex));
          Node** const __restrict nodes = &spawns.front();
          for(DWORD i=0, numPasses = MAX_PASSES; i<numPasses; i++) Dev->SetTexture(i,NULL);

          D3DXMATRIX nodeTranslation;
          D3DXMATRIX nodeRotation;
          D3DXMATRIX world;


          for(DWORD i=0; i<numSpawns; i++)
          {
            D3DXMatrixRotationYawPitchRoll(&nodeRotation,-nodes[i]->Angles.y+D3DX_PI,nodes[i]->Angles.x,nodes[i]->Angles.z);
            D3DXMatrixTranslation(&nodeTranslation,nodes[i]->Position.x,nodes[i]->Position.y,nodes[i]->Position.z);
            D3DXMatrixMultiply(&world, &nodeRotation, &nodeTranslation);
            Dev->SetTransform(D3DTS_WORLD, &world);

            if(nodes[i]->IsSelected())
            {
              Dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
              arrow->RenderShadedWithCurrentVBuffer();
              Dev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
            }
            else
              arrow->RenderShadedWithCurrentVBuffer();
          }
          for(DWORD i=0, numSelected = selectedNodes.size(); i<numSelected; i++)
          {
            D3DXVECTOR3 xyCoords;
            D3DXVec3Project(&xyCoords, (D3DXVECTOR3*)&selectedNodes[i]->Position, &port, &camera->proj(), &camera->view(), scene->GetWorldMatrix()); 
            if(xyCoords.z>0.0f && xyCoords.z<1.0f)
            {
              rct.left   = xyCoords.x;
              rct.right  = xyCoords.x;
              rct.top    = xyCoords.y;
              rct.bottom = xyCoords.y;

              m_font->DrawTextA(NULL, "hi", -1, &rct, DT_LEFT|DT_NOCLIP ,   D3DXCOLOR(1,0,0,1));
            }
          }
          rct.left = 10;
          rct.right = 1010;
          rct.top = 10;
          rct.bottom = 90;
          Dev->SetTransform(D3DTS_WORLD, scene->GetWorldMatrix());
          if(showRails)
          {
            Dev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
            for(DWORD i=0, numRails = rails.size(); i<numRails; i++)
            {
              Dev->SetStreamSource(0, rails[i].verts, 0, sizeof(ColouredVertex));
              Dev->SetIndices(rails[i].indices);
              Dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, rails[i].numVerts, 0, rails[i].numIndices/3);
            }
            Dev->SetRenderState(D3DRS_CULLMODE, scene->cullMode);
          }
          Dev->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|MAX_TEXCOORDS);
        }
        else if(showRails)
        {
          //Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVBLENDFACTOR);
          Dev->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
          Dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
          Dev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
          for(DWORD i=0, numPasses = MAX_PASSES; i<numPasses; i++) Dev->SetTexture(i,NULL);

          for(DWORD i=0, numRails = rails.size(); i<numRails; i++)
          {
            Dev->SetStreamSource(0, rails[i].verts, 0, sizeof(ColouredVertex));
            Dev->SetIndices(rails[i].indices);
            Dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, rails[i].numVerts, 0, rails[i].numIndices/3);
          }
          Dev->SetRenderState(D3DRS_CULLMODE, scene->cullMode);
          Dev->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|MAX_TEXCOORDS);
        }
      }
      else if(showRails)
      {
        //Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVBLENDFACTOR);
        Dev->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
        Dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
        Dev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
        for(DWORD i=0, numPasses = MAX_PASSES; i<numPasses; i++) Dev->SetTexture(i,NULL);

        for(DWORD i=0, numRails = rails.size(); i<numRails; i++)
        {
          Dev->SetStreamSource(0, rails[i].verts, 0, sizeof(ColouredVertex));
          Dev->SetIndices(rails[i].indices);
          Dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, rails[i].numVerts, 0, rails[i].numIndices/3);
        }
        Dev->SetRenderState(D3DRS_CULLMODE, scene->cullMode);
        Dev->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|MAX_TEXCOORDS);
      }

      Dev->SetRenderState(D3DRS_ALPHAREF, alphaRef);
      Dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
      const DWORD numSelectedTris = selectedTriangles.size();

      if(numSelectedTris != 0)
      {
        Dev->SetRenderState(D3DRS_ZENABLE, FALSE);
        if(!((showRails && rails.size()) || (showSpawns && spawns.size())))
        {
          //Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVBLENDFACTOR);
          Dev->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
          for(DWORD i=0, numPasses = MAX_PASSES; i<numPasses; i++) Dev->SetTexture(i,NULL);
        }
        for(DWORD i=0; i<numSelectedTris; i++)
        {
          Dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
          Dev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,0,3,1,&selectedTriangles[i].a,D3DFMT_INDEX16,selectedTriangles[i].vertices,sizeof(Vertex));
        }
        Dev->SetRenderState(D3DRS_ZENABLE, TRUE);
        Dev->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|MAX_TEXCOORDS);
      }
      //Mesh** const __restrict transMeshes = trans;
      for(DWORD i=0, numMeshes = numTrans; i< numMeshes; i++)
      {
        trans[i].Render2();
        //if(transMeshes[i]->flags & MeshFlags::isVisible) transMeshes[i]->Render2(matList);
      }
      //scene->RenderTransparent();
      /*if(enableTransparency)
      scene->Render(Device);
      else
      scene->RenderNoTrans(Device);*/
    }
    else
    {
      scene->RenderShaded(Dev);

      if(showSpawns/* || this->toolStripButton4->Checked || this->toolStripButton6->Checked*/)
      {
        const DWORD numSpawns = spawns.size();

        if(numSpawns != 0)
        {
          Dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
          Dev->SetStreamSource(0, arrow->verts, 0, sizeof(Vertex));
          const Node*const*const nodes = &spawns.front();

          D3DXMATRIX nodeTranslation;
          D3DXMATRIX nodeRotation;
          D3DXMATRIX world;


          for(DWORD i=0; i<numSpawns; i++)
          {
            D3DXMatrixRotationYawPitchRoll(&nodeRotation,-nodes[i]->Angles.y+D3DX_PI,nodes[i]->Angles.x,nodes[i]->Angles.z);
            D3DXMatrixTranslation(&nodeTranslation,nodes[i]->Position.x,nodes[i]->Position.y,nodes[i]->Position.z);
            D3DXMatrixMultiply(&world, &nodeRotation, &nodeTranslation);
            Dev->SetTransform(D3DTS_WORLD, &world);

            if(nodes[i]->IsSelected())
            {
              Dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
              arrow->RenderShadedWithCurrentVBuffer();
              Dev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
            }
            else
              arrow->RenderShadedWithCurrentVBuffer();
          }
          for(DWORD i=0, numSelected = selectedNodes.size(); i<numSelected; i++)
          {
            D3DXVECTOR3 xyCoords;
            D3DXVec3Project(&xyCoords, (D3DXVECTOR3*)&selectedNodes[i]->Position, &port, &camera->proj(), &camera->view(), scene->GetWorldMatrix()); 
            if(xyCoords.z>0.0f && xyCoords.z<1.0f)
            {
              rct.left   = xyCoords.x;
              rct.right  = xyCoords.x;
              rct.top    = xyCoords.y;
              rct.bottom = xyCoords.y;

              m_font->DrawTextA(NULL, "hi", -1, &rct, DT_LEFT|DT_NOCLIP ,   D3DXCOLOR(1,0,0,1));
            }
          }
          rct.left = 10;
          rct.right = 1010;
          rct.top = 10;
          rct.bottom = 90;
          Dev->SetTransform(D3DTS_WORLD, scene->GetWorldMatrix());
          if(showRails)
          {
            Dev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
            for(DWORD i=0, numRails = rails.size(); i<numRails; i++)
            {
              Dev->SetStreamSource(0, rails[i].verts, 0, sizeof(ColouredVertex));
              Dev->SetIndices(rails[i].indices);
              Dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, rails[i].numVerts, 0, rails[i].numIndices/3);
            }
            Dev->SetRenderState(D3DRS_CULLMODE, scene->cullMode);
          }
        }
        else if(showRails)
        {
          Dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
          Dev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

          for(DWORD i=0, numRails = rails.size(); i<numRails; i++)
          {
            Dev->SetStreamSource(0, rails[i].verts, 0, sizeof(ColouredVertex));
            Dev->SetIndices(rails[i].indices);
            Dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, rails[i].numVerts, 0, rails[i].numIndices/3);
          }
          Dev->SetRenderState(D3DRS_CULLMODE, scene->cullMode);
        }
      }
      else if(showRails)
      {
        Dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
        Dev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

        for(DWORD i=0, numRails = rails.size(); i<numRails; i++)
        {
          Dev->SetStreamSource(0, rails[i].verts, 0, sizeof(ColouredVertex));
          Dev->SetIndices(rails[i].indices);
          Dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, rails[i].numVerts, 0, rails[i].numIndices/3);
        }
        Dev->SetRenderState(D3DRS_CULLMODE, scene->cullMode);
      }
      Dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
      DWORD numSelectedTris = selectedTriangles.size();

      if(numSelectedTris != 0)
      {
        Dev->SetRenderState(D3DRS_ZENABLE, FALSE);
        for(DWORD i=0; i<numSelectedTris; i++)
        {
          Dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
          Dev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,0,3,1,&selectedTriangles[i].a,D3DFMT_INDEX16,selectedTriangles[i].vertices,sizeof(Vertex));
        }
        Dev->SetRenderState(D3DRS_ZENABLE, TRUE);
      }
      Dev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    }
  }
  m_font->DrawTextA(0, info, -1, &rct, 0, D3DCOLOR_ARGB(255,0,0,255) );

  Dev->EndScene();
  Dev->Present(NULL, NULL, NULL, NULL);
}

#pragma managed
/*
void THPSLevel::DrawFrame(/*System::Object ^, System::EventArgs ^) {
/*while(THPSLevel::rendering)
Sleep(3);
//if(rendering) return;

//rendering=true;
Device->Clear(0,NULL,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,0,1.0f,0);
Device->BeginScene();
if(scene)
{
//QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);
const int tNow = Environment::TickCount;
detect_input(tNow-tLast);/*float(currTimeStamp - prevTimeStamp)*secsPerCnt);
tLast = tNow;
if(this->toolStripButton7->Checked)
{
if(scene->skyDome)
{
Device->SetRenderState(D3DRS_ZENABLE, FALSE);
Device->SetTransform(D3DTS_WORLD, scene->GetSkyMatrix());
for(DWORD i=0; i<MAX_PASSES; i++)
Device->SetSamplerState( i, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP );//Smoothly stretch the sky width
scene->SetSkyPos(camera->pos());
if(this->toolStripButton1->Checked)
{
if(this->toolStripButton2->Checked)
scene->skyDome->Render(Device);
else
scene->skyDome->RenderNoTrans(Device);
}
else
scene->skyDome->RenderShaded(Device);
for(DWORD i=0; i<MAX_PASSES; i++)
Device->SetSamplerState( i, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP );
Device->SetTransform(D3DTS_WORLD, scene->GetWorldMatrix());
Device->SetRenderState(D3DRS_ZENABLE, TRUE);
}
}

if(this->toolStripButton1->Checked)
{
if(this->toolStripButton2->Checked)
scene->Render(Device);
else
scene->RenderNoTrans(Device);
}
else
scene->RenderShaded(Device);

DWORD numSelectedTris = selectedTriangles.size();

if(numSelectedTris != 0)
{
// Device->Clear(0,NULL,D3DCLEAR_ZBUFFER,0,1.f,0.0f);
Device->SetRenderState(D3DRS_ZENABLE, FALSE);
// Device->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
for(DWORD i=0; i<MAX_PASSES; i++) Device->SetTexture(i,NULL);
for(DWORD i=0; i<numSelectedTris; i++)
{
//Device->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, *(DWORD *)&bias);
Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
Device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,0,3,1,&selectedTriangles[i].a,D3DFMT_INDEX16,selectedTriangles[i].vertices,sizeof(Vertex));
//Device->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, 0);
}
Device->SetRenderState(D3DRS_ZENABLE, TRUE);
//Device->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|MAX_TEXCOORDS);
}
Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);


if(toolStripButton3->Checked/* || this->toolStripButton4->Checked || this->toolStripButton6->Checked)
{
DWORD numSpawns = spawns.size();

if(numSpawns != 0)
{
Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
Device->SetStreamSource(0, arrow->verts, 0, sizeof(Vertex));
Node** nodes = &spawns.front();
if(numSelectedTris == 0)
{
for(DWORD i=0; i<MAX_PASSES; i++) Device->SetTexture(i,NULL);
}
static D3DXMATRIX nodeTranslation;
static D3DXMATRIX nodeRotation;
static D3DXMATRIX world;


for(DWORD i=0; i<numSpawns; i++)
{
D3DXMatrixRotationYawPitchRoll(&nodeRotation,-nodes[i]->Angles.y+D3DX_PI,nodes[i]->Angles.x,nodes[i]->Angles.z);
D3DXMatrixTranslation(&nodeTranslation,nodes[i]->Position.x,nodes[i]->Position.y,nodes[i]->Position.z);
D3DXMatrixMultiply(&world, &nodeRotation, &nodeTranslation);
Device->SetTransform(D3DTS_WORLD, &world);

if(nodes[i]->IsSelected())
{
Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
arrow->RenderShadedWithCurrentVBuffer();
Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}
else
arrow->RenderShadedWithCurrentVBuffer();
}
for(DWORD i=0, numSelected = selectedNodes.size(); i<numSelected; i++)
{
static D3DXVECTOR3 xyCoords;
D3DXVec3Project(&xyCoords, (D3DXVECTOR3*)&selectedNodes[i]->Position, &port, &camera->proj(), &camera->view(), scene->GetWorldMatrix()); 
if(xyCoords.z>0.0f && xyCoords.z<1.0f)
{
rct.left   = xyCoords.x;
rct.right  = xyCoords.x;
rct.top    = xyCoords.y;
rct.bottom = xyCoords.y;

m_font->DrawTextA(NULL, "hi", -1, &rct, DT_LEFT|DT_NOCLIP ,   D3DXCOLOR(1,0,0,1));
}
}
rct.left = 10;
rct.right = 1010;
rct.top = 10;
rct.bottom = 90;
Device->SetTransform(D3DTS_WORLD, scene->GetWorldMatrix());
if(toolStripButton4->Checked)
{
Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
for(DWORD i=0, numRails = rails.size(); i<numRails; i++)
{
Device->SetStreamSource(0, rails[i].verts, 0, sizeof(Vertex));
Device->SetIndices(rails[i].indices);
Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, rails[i].numVerts, 0, rails[i].numIndices/3);
}
Device->SetRenderState(D3DRS_CULLMODE, scene->cullMode);
}
}
else if(toolStripButton4->Checked)
{
Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
if(numSelectedTris == 0)
{
for(DWORD i=0; i<MAX_PASSES; i++) Device->SetTexture(i,NULL);
}
for(DWORD i=0, numRails = rails.size(); i<numRails; i++)
{
Device->SetStreamSource(0, rails[i].verts, 0, sizeof(Vertex));
Device->SetIndices(rails[i].indices);
Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, rails[i].numVerts, 0, rails[i].numIndices/3);
}
Device->SetRenderState(D3DRS_CULLMODE, scene->cullMode);
}
}
else if(toolStripButton4->Checked)
{
Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
if(numSelectedTris == 0)
{
for(DWORD i=0; i<MAX_PASSES; i++) Device->SetTexture(i,NULL);
}
for(DWORD i=0, numRails = rails.size(); i<numRails; i++)
{
Device->SetStreamSource(0, rails[i].verts, 0, sizeof(Vertex));
Device->SetIndices(rails[i].indices);
Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, rails[i].numVerts, 0, rails[i].numIndices/3);
}
Device->SetRenderState(D3DRS_CULLMODE, scene->cullMode);
}
}

m_font->DrawTextA(0, info, -1, &rct, 0, D3DCOLOR_ARGB(255,0,0,255) );
/*const __restrict int fps = 12;

int frameTime = Environment::TickCount-tNow;
if ( frameTime < fps)
{
Sleep(fps - frameTime);
// }
Device->EndScene();    // ends the 3D scene
//rendering=false;
/*HRESULT hr = Device->Present(NULL, NULL, NULL, NULL);    // displays the created frame
//prevTimeStamp = currTimeStamp;

/*if(hr==D3DERR_DEVICELOST)
{
MessageBox::Show("Lost");
deviceLost = true;
//}
}
*/
void THPSLevel::InitRenderScene()
{
  /*BOOL enabled;
  DwmIsCompositionEnabled(&enabled);
  if(enabled)
  DwmEnableComposition(DWM_EC_DISABLECOMPOSITION);*/
  handleWindow = static_cast<HWND>(this->panel6->Handle.ToPointer());
  Direct3DCreate9Ex(D3D_SDK_VERSION, &d3d);

  HRESULT hr = 0;

  ZeroMemory(&DevParams, sizeof(DevParams));
  DevParams.Windowed = TRUE;
  DevParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
  DevParams.hDeviceWindow = handleWindow;
  DevParams.BackBufferHeight = 0;//768;
  DevParams.BackBufferWidth = 0;//1024;
  DevParams.BackBufferFormat = D3DFMT_A8R8G8B8;
  DevParams.BackBufferCount = 1;
  DevParams.PresentationInterval  = D3DPRESENT_INTERVAL_IMMEDIATE;
  DevParams.EnableAutoDepthStencil = TRUE;
  DevParams.AutoDepthStencilFormat = D3DFMT_D24S8;
  DevParams.MultiSampleType = D3DMULTISAMPLE_8_SAMPLES;


  hr = d3d->CreateDeviceEx(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,handleWindow,D3DCREATE_HARDWARE_VERTEXPROCESSING|D3DCREATE_PUREDEVICE,&DevParams, NULL, &Device);
  InitInputDevices((HINSTANCE)GetWindowLong(handleWindow, GWL_HINSTANCE) , handleWindow);
  if(FAILED(hr))
    MessageBox::Show("Failed to create device");
  SetDeviceStates();
  //LPD3DXBUFFER pD3DXMtrlBuffer;

  // Load the mesh from the specified file
  HRSRC resource = FindResource(NULL, MAKEINTRESOURCE(2053 ), RT_RCDATA);
  if(resource!=NULL)
  {
    HGLOBAL resourceData = LoadResource(NULL, resource);
    if(resourceData!=NULL)
    {
      void* pFile = LockResource(resourceData);
      if(pFile!=NULL)
      {
        arrow = new Th4Mesh((DWORD)pFile, 71);
        arrow->CreateBuffers(Device, D3DFVF_XYZ|D3DFVF_DIFFUSE/*|MAX_TEXCOORDS*/);
        UnlockResource(pFile);
      }
    }
    //CloseHandle(resource);
  }
  /*D3DXMATERIAL* d3dxMaterials = ( D3DXMATERIAL* )pD3DXMtrlBuffer->GetBufferPointer();
  g_pMeshMaterials = new D3DMATERIAL9[g_dwNumMaterials];
  g_pMeshTextures = new LPDIRECT3DTEXTURE9[g_dwNumMaterials];

  for( DWORD i = 0; i < g_dwNumMaterials; i++ )
  {
  // Copy the material
  g_pMeshMaterials[i] = d3dxMaterials[i].MatD3D;

  // Set the ambient color for the material (D3DX does not do this)
  g_pMeshMaterials[i].Ambient = g_pMeshMaterials[i].Diffuse;

  g_pMeshTextures[i] = NULL;
  if( d3dxMaterials[i].pTextureFilename != NULL &&
  lstrlenA( d3dxMaterials[i].pTextureFilename ) > 0 )
  {
  // Create the texture
  if( FAILED( D3DXCreateTextureFromFileA( Device,
  d3dxMaterials[i].pTextureFilename,
  &g_pMeshTextures[i] ) ) )
  MessageBox::Show("failed2");
  }
  }*/
  /*QueryPerformanceFrequency((LARGE_INTEGER*)&cntsPerSec);
  QueryPerformanceCounter((LARGE_INTEGER*)&prevTimeStamp);
  secsPerCnt = 1.0f / (float)cntsPerSec;
  Device->SetRenderState( D3DRS_AMBIENT, 0xffffffff );*/
  //pD3DXMtrlBuffer->Release();
}

void THPSLevel::ResizeRenderWindow(System::Object^  sender, System::EventArgs^  e) {
  //SafeTurnOffRendering();
  render = false;
  //DeleteManagedObjects();

  DevParams.BackBufferWidth=panel6->Size.Width;
  DevParams.BackBufferHeight=panel6->Size.Height;

  Device->ResetEx(&DevParams, NULL);

  //SetDeviceStates();
  //CreateManagedObjects();
  Device->GetViewport(&port);
  CreateCamera();
  render = true;
}

void THPSLevel::CleanupAppdata()
{
  /* DEBUGSTART()
  {*/
  if(scene)
  {
    UnloadScene();
  }
  else if(camera)
  {
    CleanupEvents();
    delete camera;
  }
  CleanupInputDevices();
  if(m_font)
    m_font->Release();
  delete arrow;
  Device->Release();
  d3d->Release();
  /* }
  DEBUGEND()*/
}

#pragma unmanaged