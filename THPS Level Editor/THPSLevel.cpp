// THPS Level Editor.cpp : main project file.

#include "stdafx.h"
#include <iostream>
#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <varargs.h>
#include <wchar.h>
#include <wctype.h>
#include <locale.h>
#include <fstream>
#include <windows.h>
#include <conio.h>
#include <io.h>
#include <vector>
#include <fcntl.h>
#include <io.h>
#include "Psapi.h"
#include <iostream>
#include <tlhelp32.h> 
#include <shlwapi.h>
#include "player.h"
#include <windows.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>
#pragma unmanaged
#define DIRECTINPUT_VERSION 0x0800
//#define MEMORY_LEAK_CHECK
#ifdef MEMORY_LEAK_CHECK
#define VLD_FORCE_ENABLE
#include <vld.h>
//#include <vldapi.h>
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


D3DPRESENT_PARAMETERS DevParams = { 0 };
TCHAR* szWindowClass = "YeaClass";
LRESULT CALLBACK WndProcc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
IDirect3DDevice9Ex* __restrict Device = NULL;
IDirect3D9Ex* __restrict d3d = NULL;
bool deviceLost = false;
DWORD MAX_PASSES = 1;
DWORD MAX_TEXCOORDS = 1;
Scene* __restrict scene = NULL;
D3DVIEWPORT9 port;
bool resetMouse = false;
DIMOUSESTATE mousestate = { 0 };
HWND handleWindow = NULL;
HWND pictureHandle = NULL;
Camera* __restrict camera = NULL;
Player* __restrict player = NULL;
bool renderCollision = false;
BYTE selectionMode = 0;
const float bias = -1.0f;
DWORD defaultCulling = D3DCULL_NONE;
DWORD alphaRef = 8;
MaterialList* __restrict globalMaterialList = NULL;
MaterialSplit* lastSplit = NULL;
DWORD numLastSplit = 0;
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
Mesh** __restrict opaque = NULL;
Mesh** __restrict trans = NULL;
DWORD numTrans = 0;
DWORD numOpaque = 0;
BYTE pass = 0;
DWORD numAnimations;
UVAnim* __restrict animations;
bool forceRedraw = false;
bool render = false;
LPDIRECT3DSWAPCHAIN9 imageChain = NULL;
bool quickMaterialMode = false;
bool updateMaterial = true;

vector<SelectedMaterial> selectedMaterials;


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


LPD3DXSPRITE image = NULL;
LPDIRECT3DTEXTURE9 texture = NULL;
D3DVIEWPORT9 imagePort;

void LoadPicture(char* path)
{
  if (texture)
    texture->Release();
  D3DXCreateTextureFromFileExA(Device, path, D3DX_DEFAULT, D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, D3DUSAGE_DYNAMIC, D3DFMT_FROM_FILE, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &texture);

  D3DXVECTOR3 scale;

  D3DXIMAGE_INFO info;
  D3DXGetImageInfoFromFile(path, &info);

  D3DXMATRIX scaleMatrix;
  D3DXMatrixScaling(&scaleMatrix, (float)imagePort.Width / (float)info.Width, (float)imagePort.Height / (float)info.Height, 0.0f);
  image->SetTransform(&scaleMatrix);
}
IDirect3DSurface9* imageSurface;

void RenderImage(HWND hwnd)
{
  LPDIRECT3DSURFACE9 pBackBuffer = NULL;
  IDirect3DSurface9* pRender = NULL;
  IDirect3DSurface9* pSurface = NULL;
  D3DXMATRIX ident;
  D3DXMatrixIdentity(&ident);

  Device->SetTransform(D3DTS_WORLD, &ident);
  Device->SetTransform(D3DTS_VIEW, &ident);
  Device->SetViewport(&imagePort);
  D3DXMatrixPerspectiveFovLH(&ident, D3DXToRadian(70.0f), float((float)imagePort.Width / (float)imagePort.Height), 12.0f, 95250.0f);
  Device->SetTransform(D3DTS_PROJECTION, &ident);
  Device->GetRenderTarget(0, &pRender);
  imageChain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);

  Device->SetRenderTarget(0, pBackBuffer);

  Device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
    D3DCOLOR_COLORVALUE(0.0f, 1.0f, 0.0f, 1.0f), 1.0f, 0);
  Device->BeginScene();

  image->Begin(D3DXSPRITE_ALPHABLEND);
  image->Draw(texture, NULL, NULL, NULL, D3DCOLOR_RGBA(255, 255, 255, 255));
  image->End();
  Device->EndScene();
  imageChain->Present(0, 0, hwnd, 0, 0);

  pBackBuffer->Release();
  Device->SetRenderTarget(0, pRender);
  pRender->Release();
  Device->SetViewport(&port);
  Device->SetTransform(D3DTS_PROJECTION, &camera->proj());
  Device->SetTransform(D3DTS_VIEW, &camera->view());
}


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
  //ExtractResource(hInstance, 102, "boost_system-vc100-mt-1_47.dll");
  //ExtractResource(hInstance, 103, "boost_filesystem-vc100-mt-1_47.dll");
  // Enabling Windows XP visual effects before any controls are created
  Application::EnableVisualStyles();
  Application::SetCompatibleTextRenderingDefault(false);

  // Create the main window and run it
  //Application::Idle += gcnew EventHandler(DrawFrame); 



  WNDCLASSEX     wndclass = { 0 };
  wndclass.cbSize = sizeof(WNDCLASSEX);
  wndclass.style = CS_HREDRAW | CS_VREDRAW;
  wndclass.lpfnWndProc = WndProcc;
  wndclass.cbClsExtra = 0;
  wndclass.cbWndExtra = 0;
  wndclass.hInstance = hInstance;
  wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
  wndclass.hbrBackground = 0;// (HBRUSH)GetStockObject(WHITE_BRUSH);
  wndclass.lpszMenuName = NULL;
  wndclass.lpszClassName = szWindowClass;
  if (RegisterClassEx(&wndclass) == 0)
  {
    MessageBox::Show("WHY?" + GetLastError().ToString());
  }


  

  //CreateConsole();

  THPSLevel^ app = gcnew THPSLevel();
  Application::Exit();
  //Application::Run(gcnew THPSLevel());

  return 0;
}

#pragma managed(push, off)
void InitConsole()
{
    if (AllocConsole())
    {
        FILE* fpstdin = stdin, * fpstdout = stdout, * fpstderr = stderr;

        freopen_s(&fpstdin, "CONIN$", "r", stdin);
        freopen_s(&fpstdout, "CONOUT$", "w", stdout);
        freopen_s(&fpstderr, "CONOUT$", "w", stderr);
    }
}
#pragma managed(pop)
#pragma unmanaged
static const WORD MAX_CONSOLE_LINES = 500;

void RedirectIOToConsole()
{
    int hConHandle;
    long lStdHandle;
    CONSOLE_SCREEN_BUFFER_INFO coninfo;
    FILE* fp;

    // allocate a console for this app
    AllocConsole();

    // set the screen buffer to be big enough to let us scroll text
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
    coninfo.dwSize.Y = MAX_CONSOLE_LINES;
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

    // redirect unbuffered STDOUT to the console
    lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen(hConHandle, "w");
    *stdout = *fp;
    setvbuf(stdout, NULL, _IONBF, 0);

    // redirect unbuffered STDIN to the console
    lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen(hConHandle, "r");
    *stdin = *fp;
    setvbuf(stdin, NULL, _IONBF, 0);

    // redirect unbuffered STDERR to the console
    lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen(hConHandle, "w");
    *stderr = *fp;
    setvbuf(stderr, NULL, _IONBF, 0);

    // make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
    // point to console as well
    std::ios::sync_with_stdio();
}

void CreateCamera()
{
  if (!camera)
  {
    camera = new Camera();
    player = new Player();
    camera->lookAt(D3DXVECTOR3(0.0f, 250.0f, -100.0f), D3DXVECTOR3(0.0f, 250.0f, 100.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f));
  }
  camera->setLens(D3DXToRadian(70.0f), float((float)::port.Width / (float)::port.Height), 12.0f, 95250.0f);
  Device->SetTransform(D3DTS_PROJECTION, &camera->proj());
  Device->SetTransform(D3DTS_VIEW, &camera->view());
}

void SaveTextures(char* name)
{
  DWORD len = strlen(name);
  name[len - 3] = 't';
  name[len - 2] = 'e';
  name[len - 1] = 'x';

  FILE* f = fopen(name, "wb");
  if (f)
  {
    const DWORD numImages = texFiles.numFiles;
    fwrite(&numImages, 4, 1, f);
    for (DWORD i = 0; i < numImages; i++)
    {
      fwrite(texFiles.fileNames[i], strlen(texFiles.fileNames[i]) + 1, 1, f);
      FILE* temp = fopen(texFiles.fileNames[i], "rb");
      if (temp)
      {
        fseek(temp, 0, SEEK_END);
        const DWORD size = ftell(temp);
        BYTE* data = new BYTE[size];
        fseek(temp, 0, SEEK_SET);
        fread(data, size, 1, temp);
        fclose(temp);
        fwrite(&size, 4, 1, f);
        fwrite(data, size, 1, f);
        delete[] data;
      }
    }
    fclose(f);
  }
}

void SaveMaterials(char* name)
{
  FILE* f = fopen(name, "wb");
  if (f)
  {
    const DWORD numMaterials = scene->matList.GetNumMaterials();
    fwrite(&numMaterials, 4, 1, f);
    for (DWORD i = 0; i < numMaterials; i++)
    {
      Material* material = scene->matList.GetMaterial(i);

      DWORD numTextures = material->GetNumTextures();
      fwrite(&numTextures, 4, 1, f);
      DWORD matId = material->GetMatId();
      fwrite(&matId, 4, 1, f);
      fwrite(&material->drawOrder, 4, 1, f);
      fwrite(&material->transparent, 1, 1, f);

      for (DWORD j = 0; j < numTextures; j++)
      {
        Texture* texture = material->GetTexture(j);
        fwrite(&texture->uAddress, 4, 1, f);
        fwrite(&texture->vAddress, 4, 1, f);
        fwrite(&texture->fixedAlpha, 4, 1, f);
        fwrite(&texture->blendMode, 4, 1, f);
        DWORD temp = texture->GetTexId();
        fwrite(&temp, 4, 1, f);
        temp = texture->GetFlags();
        fwrite(&temp, 4, 1, f);
      }
    }
    fclose(f);
  }
}

void SaveScripts(char* name)
{
  DWORD len = strlen(name);
  name[len - 3] = 't';
  name[len - 2] = 'r';
  name[len - 1] = 'g';

  FILE* f = fopen(name, "wb");
  if (f)
  {
    const DWORD numNodes = scene->nodes.size();
    fwrite(&numNodes, 4, 1, f);
    for (DWORD i = 0; i < numNodes; i++)
    {
      fwrite(&scene->nodes[i], 12, 4, f);

      DWORD numLinks = scene->nodes[i].Links.size();
      fwrite(&numLinks, 4, 1, f);
      if (numLinks)
        fwrite(&scene->nodes[i].Links.front(), sizeof(WORD), numLinks, f);
      //fwrite(&scene->nodes[i].type, 2, 1, f);
      fwrite(&scene->nodes[i].TerrainType, 4, 1, f);
      fwrite(&scene->nodes[i].CreatedAtStart, 1, 1, f);
      fwrite(&scene->nodes[i].AbsentInNetGames, 1, 1, f);
      fwrite(&scene->nodes[i].NetEnabled, 1, 1, f);
      fwrite(&scene->nodes[i].Permanent, 1, 1, f);
      fwrite(&scene->nodes[i].TrickObject, 1, 1, f);
    }

    const DWORD numScripts = Scripts.size();
    fwrite(&numScripts, 4, 1, f);
    if (numScripts)
      fwrite(&Scripts.front(), sizeof(Script), numScripts, f);

    const DWORD numKnownScripts = scene->KnownScripts.size();
    fwrite(&numKnownScripts, 4, 1, f);
    if (numKnownScripts)
      fwrite(&scene->KnownScripts.front(), sizeof(KnownScript), numKnownScripts, f);

    const DWORD numTriggers = scene->triggers.size();
    fwrite(&numTriggers, 4, 1, f);
    if (numTriggers)
      fwrite(&scene->triggers.front(), sizeof(Checksum), numTriggers, f);

    const DWORD numGlobals = scene->globals.size();
    fwrite(&numGlobals, 4, 1, f);
    if (numGlobals)
      fwrite(&scene->globals.front(), sizeof(Checksum), numGlobals, f);
    fclose(f);
  }
}

void SaveState(char* name)
{
  _fcloseall();
  if (!scene)
    return;

  char* path = scene->GetName();
  DWORD pos = ((std::string)path).find_last_of("\\");

  strcat(name, &path[pos]);
  DWORD len = strlen(name);

  char shortName[128] = "";

  DWORD i = pos + 1;
  DWORD j = 0;
  while (path[i] != 0x0)
  {
    shortName[j] = path[i];
    i++;
    j++;
  }
  shortName[j] = 0x0;
  DWORD shortLen = strlen(shortName);

  name[len - 3] = 'z';
  name[len - 2] = 'i';
  name[len - 1] = 'p';

  HZIP hz = CreateZip(name, NULL);
  name[len - 3] = 'm';
  name[len - 2] = 's';
  name[len - 1] = 'h';

  FILE* f = fopen(name, "wb");
  if (f)
  {
    const static float version = 3.0f;
    fwrite(&version, 4, 1, f);
    fwrite(scene->GetName(), 260, 1, f);
    fwrite(&scene->GetBlendMode(), 1, 1, f);
    fwrite(&scene->cullMode, 4, 1, f);
    fwrite(&MAX_PASSES, 4, 1, f);
    fwrite(&MAX_TEXCOORDS, 4, 1, f);

    SaveTextures(name);
    shortName[shortLen - 3] = 't';
    shortName[shortLen - 2] = 'e';
    shortName[shortLen - 1] = 'x';
    ZipAdd(hz, shortName, name);
    DeleteFile(name);


    const DWORD numMaterials = scene->matList.GetNumMaterials();
    fwrite(&numMaterials, 4, 1, f);
    for (DWORD i = 0; i < numMaterials; i++)
    {
      Material* material = scene->matList.GetMaterial(i);

      DWORD numTextures = material->GetNumTextures();
      fwrite(&numTextures, 4, 1, f);
      DWORD matId = material->GetMatId();
      fwrite(&matId, 4, 1, f);
      fwrite(&material->drawOrder, 4, 1, f);
      fwrite(&material->transparent, 1, 1, f);
      fwrite(&material->doubled, 1, 1, f);
      fwrite(&material->reverse, 1, 1, f);
      fwrite(&material->invisible, 1, 1, f);
      fwrite(&material->ignoreAlpha, 1, 1, f);
      fwrite(&material->alphaMap, 1, 1, f);
      fwrite(&material->animated, 1, 1, f);
      if (material->animated)
      {
        DWORD zero = 0;
        Animation* anim = scene->GetAnimation(material->GetMatId());
        if (anim)
        {
          fwrite(anim, 4, 8, f);
        }
        else
          fwrite(&zero, 4, 8, f);
      }

      for (DWORD j = 0; j < numTextures; j++)
      {
        Texture* texture = material->GetTexture(j);
        fwrite(&texture->uAddress, 4, 1, f);
        fwrite(&texture->vAddress, 4, 1, f);
        fwrite(&texture->fixedAlpha, 4, 1, f);
        fwrite(&texture->blendMode, 4, 1, f);
        DWORD temp = texture->GetTexId();
        fwrite(&temp, 4, 1, f);
        temp = texture->GetFlags();
        fwrite(&temp, 4, 1, f);
      }
    }

    const DWORD numMeshes = scene->GetNumMeshes();
    fwrite(&numMeshes, 4, 1, f);

    for (DWORD i = 0; i < numMeshes; i++)
    {
      Mesh* mesh = scene->GetMesh(i);
      fwrite(&mesh->GetFlags(), 4, 1, f);
      fwrite(&mesh->GetName(), 4, 1, f);
      DWORD numVertices = mesh->GetNumVertices();
      fwrite(&numVertices, 4, 1, f);
      if (numVertices)
        fwrite(mesh->GetVertices(), sizeof(Vertex), numVertices, f);
      Collision* collision = mesh->GetCollision();
      bool hasCollision = collision != NULL;
      fwrite(&hasCollision, 1, 1, f);
      if (hasCollision)
      {
        numVertices = collision->GetNumVertices();
        fwrite(&numVertices, 4, 1, f);
        if (numVertices)
        {
          fwrite(collision->GetVertices(), sizeof(Vertex), numVertices, f);
        }
        DWORD numFaces = collision->GetNumFaces();
        fwrite(&numFaces, 4, 1, f);
        if (numFaces)
        {
          fwrite(collision->GetFaces(), sizeof(SimpleFace), numFaces, f);
          fwrite(collision->GetFacesInfo(), sizeof(FaceInfo), numFaces, f);
        }
      }
      DWORD numSplits = mesh->GetNumSplits();
      fwrite(&numSplits, 4, 1, f);
      for (DWORD j = 0; j < numSplits; j++)
      {
        MaterialSplit* matSplit = mesh->GetSplit(j);
        fwrite(&matSplit->matId, 4, 1, f);
        DWORD numIndices = matSplit->Indices.size();
        fwrite(&numIndices, 4, 1, f);
        if (numIndices)
          fwrite(&matSplit->Indices.front(), sizeof(WORD), numIndices, f);
      }
    }

    bool hasSky = scene->skyDome != 0;
    fwrite(&hasSky, 1, 1, f);
    if (hasSky)
    {
      const DWORD numSkyMaterials = scene->skyDome->matList.GetNumMaterials();
      fwrite(&numSkyMaterials, 4, 1, f);
      for (DWORD i = 0; i < numSkyMaterials; i++)
      {
        Material* material = scene->skyDome->matList.GetMaterial(i);

        DWORD numTextures = material->GetNumTextures();
        fwrite(&numTextures, 4, 1, f);
        DWORD matId = material->GetMatId();
        fwrite(&matId, 4, 1, f);
        fwrite(&material->drawOrder, 4, 1, f);
        fwrite(&material->transparent, 1, 1, f);

        for (DWORD j = 0; j < numTextures; j++)
        {
          Texture* texture = material->GetTexture(j);
          fwrite(&texture->uAddress, 4, 1, f);
          fwrite(&texture->vAddress, 4, 1, f);
          fwrite(&texture->fixedAlpha, 4, 1, f);
          fwrite(&texture->blendMode, 4, 1, f);
          DWORD temp = texture->GetTexId();
          fwrite(&temp, 4, 1, f);
          temp = texture->GetFlags();
          fwrite(&temp, 4, 1, f);
        }
      }

      const DWORD numSkyMeshes = scene->skyDome->GetNumMeshes();
      fwrite(&numSkyMeshes, 4, 1, f);

      for (DWORD i = 0; i < numSkyMeshes; i++)
      {
        Mesh* mesh = scene->skyDome->GetMesh(i);
        fwrite(&mesh->GetFlags(), 4, 1, f);
        fwrite(&mesh->GetName(), 4, 1, f);
        DWORD numVertices = mesh->GetNumVertices();
        fwrite(&numVertices, 4, 1, f);
        if (numVertices)
          fwrite(mesh->GetVertices(), sizeof(Vertex), numVertices, f);
        Collision* collision = mesh->GetCollision();
        bool hasCollision = collision != NULL;
        fwrite(&hasCollision, 1, 1, f);
        if (hasCollision)
        {
          numVertices = collision->GetNumVertices();
          fwrite(&numVertices, 4, 1, f);
          if (numVertices)
          {
            fwrite(collision->GetVertices(), sizeof(Vertex), numVertices, f);
          }
          DWORD numFaces = collision->GetNumFaces();
          fwrite(&numFaces, 4, 1, f);
          if (numFaces)
          {
            fwrite(collision->GetFaces(), sizeof(SimpleFace), numFaces, f);
            fwrite(collision->GetFacesInfo(), sizeof(FaceInfo), numFaces, f);
          }
        }
        DWORD numSplits = mesh->GetNumSplits();
        fwrite(&numSplits, 4, 1, f);
        for (DWORD j = 0; j < numSplits; j++)
        {
          MaterialSplit* matSplit = mesh->GetSplit(j);
          fwrite(&matSplit->matId, 4, 1, f);
          DWORD numIndices = matSplit->Indices.size();
          fwrite(&numIndices, 4, 1, f);
          if (numIndices)
            fwrite(&matSplit->Indices.front(), sizeof(WORD), numIndices, f);
        }
      }
    }
    SaveScripts(name);
    //}
    if (ferror(f))
      ::MessageBox(0, "Error", "Error", 0);
    fflush(f);
    if (ferror(f))
      ::MessageBox(0, "Error", "Error", 0);
    fclose(f);
    if (ferror(f))
      ::MessageBox(0, "Error", "Error", 0);
    shortName[shortLen - 3] = 't';
    shortName[shortLen - 2] = 'r';
    shortName[shortLen - 1] = 'g';
    ZipAdd(hz, shortName, name);
    DeleteFile(name);

    name[len - 3] = 'm';
    name[len - 2] = 's';
    name[len - 1] = 'h';
    shortName[shortLen - 3] = 'm';
    shortName[shortLen - 2] = 's';
    shortName[shortLen - 1] = 'h';
    ZipAdd(hz, shortName, name);
    DeleteFile(name);
    ::MessageBox(0, "done", "done", 0);
  }
  else
    ::MessageBox(0, "Error", "file...", 0);
  if (ferror(f))
    ::MessageBox(0, "Error", "Error", 0);
  _fcloseall();
  CloseZip(hz);
}

void AddLinkedRails(const Node* const __restrict parent, const WORD index)
{
  Node* node = &scene->nodes[index];
  static ColouredVertex vertex = ColouredVertex(0, 255, 0);
  RenderableRail* rail = &rails.back();
  rail->nodes.push_back(node->Name);
  float MAX_DIST = parent->Position.x - node->Position.x;
  MAX_DIST *= MAX_DIST;
  static bool axis = false;
  if (((parent->Position.z - node->Position.z)*(parent->Position.z - node->Position.z)) > MAX_DIST)
    axis = true;
  else
    axis = false;

  if (parent->Links.size())
  {
    vertex.x = node->Position.x;
    vertex.y = node->Position.y + 2.0f;
    vertex.z = node->Position.z;
    if (axis)
      vertex.x -= 2.0f;
    else
      vertex.z -= 2.0f;

    rail->vertices.push_back(vertex);

    vertex.y -= 4.0f;
    rail->vertices.push_back(vertex);

    if (axis)
      vertex.x += 4.0f;
    else
      vertex.z += 4.0f;
    rail->vertices.push_back(vertex);

    vertex.y += 4.0f;
    rail->vertices.push_back(vertex);

    rail->Indices.push_back(rail->numIndices + 1);
    rail->Indices.push_back(rail->numIndices + 4);
    rail->Indices.push_back(rail->numIndices + 5);
    rail->Indices.push_back(rail->numIndices + 1);
    rail->Indices.push_back(rail->numIndices);
    rail->Indices.push_back(rail->numIndices + 4);
    rail->Indices.push_back(rail->numIndices);
    rail->Indices.push_back(rail->numIndices + 4);
    rail->Indices.push_back(rail->numIndices + 3);
    rail->Indices.push_back(rail->numIndices + 3);
    rail->Indices.push_back(rail->numIndices + 4);
    rail->Indices.push_back(rail->numIndices + 7);
    rail->Indices.push_back(rail->numIndices + 3);
    rail->Indices.push_back(rail->numIndices + 6);
    rail->Indices.push_back(rail->numIndices + 7);
    rail->Indices.push_back(rail->numIndices + 6);
    rail->Indices.push_back(rail->numIndices + 3);
    rail->Indices.push_back(rail->numIndices + 2);
    rail->Indices.push_back(rail->numIndices + 1);
    rail->Indices.push_back(rail->numIndices + 5);
    rail->Indices.push_back(rail->numIndices + 6);
    rail->Indices.push_back(rail->numIndices + 6);
    rail->Indices.push_back(rail->numIndices + 1);
    rail->Indices.push_back(rail->numIndices + 2);
    rail->numIndices += 4;
  }
  else
  {
    vertex.x = parent->Position.x;
    vertex.y = parent->Position.y + 2.0f;
    vertex.z = parent->Position.z;
    if (axis)
      vertex.x -= 2.0f;
    else
      vertex.z -= 2.0f;

    rail->vertices.push_back(vertex);

    vertex.y -= 4.0f;
    rail->vertices.push_back(vertex);

    if (axis)
      vertex.x += 4.0f;
    else
      vertex.z += 4.0f;
    rail->vertices.push_back(vertex);

    vertex.y += 4.0f;
    rail->vertices.push_back(vertex);


    vertex.x = node->Position.x;
    vertex.y = node->Position.y + 2.0f;
    vertex.z = node->Position.z;
    if (axis)
      vertex.x -= 2.0f;
    else
      vertex.z -= 2.0f;

    rail->vertices.push_back(vertex);

    vertex.y -= 4.0f;
    rail->vertices.push_back(vertex);

    if (axis)
      vertex.x += 4.0f;
    else
      vertex.z += 4.0f;
    rail->vertices.push_back(vertex);

    vertex.y += 4.0f;
    rail->vertices.push_back(vertex);

    rail->Indices.push_back(rail->numIndices + 1);
    rail->Indices.push_back(rail->numIndices + 4);
    rail->Indices.push_back(rail->numIndices + 5);
    rail->Indices.push_back(rail->numIndices + 1);
    rail->Indices.push_back(rail->numIndices);
    rail->Indices.push_back(rail->numIndices + 4);
    rail->Indices.push_back(rail->numIndices);
    rail->Indices.push_back(rail->numIndices + 4);
    rail->Indices.push_back(rail->numIndices + 3);
    rail->Indices.push_back(rail->numIndices + 3);
    rail->Indices.push_back(rail->numIndices + 4);
    rail->Indices.push_back(rail->numIndices + 7);
    rail->Indices.push_back(rail->numIndices + 3);
    rail->Indices.push_back(rail->numIndices + 6);
    rail->Indices.push_back(rail->numIndices + 7);
    rail->Indices.push_back(rail->numIndices + 6);
    rail->Indices.push_back(rail->numIndices + 3);
    rail->Indices.push_back(rail->numIndices + 2);
    rail->Indices.push_back(rail->numIndices + 1);
    rail->Indices.push_back(rail->numIndices + 5);
    rail->Indices.push_back(rail->numIndices + 6);
    rail->Indices.push_back(rail->numIndices + 6);
    rail->Indices.push_back(rail->numIndices + 1);
    rail->Indices.push_back(rail->numIndices + 2);
    rail->numIndices += 4;


  }
  for (DWORD i = 0, numNodes = scene->nodes.size(); i < numNodes; i++)
  {
    /*if(scene->nodes[i].Class.checksum == RailNode::GetClass() && !scene->nodes[i].Links.empty() && scene->nodes[i].Links[0] == index && !scene->nodes[i].IsAdded())
    {
    AddLinkedRails(node, i);
    break;
    }*/
    if (scene->nodes[i].Class.checksum == RailNode::GetClass())
    {
      for (DWORD j = 0, numLinks = scene->nodes[i].Links.size(); j < numLinks; j++)
      {
        if (scene->nodes[i].Links[j] == index && !scene->nodes[i].IsAdded())
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
  static ColouredVertex vertex = ColouredVertex(0, 255, 0);
  RenderableRail* rail = &rails.back();
  rail->nodes.push_back(node->Name);
  float MAX_DIST = parent->Position.x - node->Position.x;
  MAX_DIST *= MAX_DIST;
  static bool axis = false;
  if (((parent->Position.z - node->Position.z)*(parent->Position.z - node->Position.z)) > MAX_DIST)
    axis = true;
  else
    axis = false;

  if (parent->Links.size())
  {
    vertex.x = node->Position.x;
    vertex.y = node->Position.y + 2.0f;
    vertex.z = node->Position.z;
    if (axis)
      vertex.x -= 2.0f;
    else
      vertex.z -= 2.0f;

    rail->vertices.push_back(vertex);

    vertex.y -= 4.0f;
    rail->vertices.push_back(vertex);

    if (axis)
      vertex.x += 4.0f;
    else
      vertex.z += 4.0f;
    rail->vertices.push_back(vertex);

    vertex.y += 4.0f;
    rail->vertices.push_back(vertex);

    rail->Indices.push_back(rail->numIndices + 1);
    rail->Indices.push_back(rail->numIndices + 4);
    rail->Indices.push_back(rail->numIndices + 5);
    rail->Indices.push_back(rail->numIndices + 1);
    rail->Indices.push_back(rail->numIndices);
    rail->Indices.push_back(rail->numIndices + 4);
    rail->Indices.push_back(rail->numIndices);
    rail->Indices.push_back(rail->numIndices + 4);
    rail->Indices.push_back(rail->numIndices + 3);
    rail->Indices.push_back(rail->numIndices + 3);
    rail->Indices.push_back(rail->numIndices + 4);
    rail->Indices.push_back(rail->numIndices + 7);
    rail->Indices.push_back(rail->numIndices + 3);
    rail->Indices.push_back(rail->numIndices + 6);
    rail->Indices.push_back(rail->numIndices + 7);
    rail->Indices.push_back(rail->numIndices + 6);
    rail->Indices.push_back(rail->numIndices + 3);
    rail->Indices.push_back(rail->numIndices + 2);
    rail->Indices.push_back(rail->numIndices + 1);
    rail->Indices.push_back(rail->numIndices + 5);
    rail->Indices.push_back(rail->numIndices + 6);
    rail->Indices.push_back(rail->numIndices + 6);
    rail->Indices.push_back(rail->numIndices + 1);
    rail->Indices.push_back(rail->numIndices + 2);
    rail->numIndices += 4;
  }
  else
  {
    vertex.x = parent->Position.x;
    vertex.y = parent->Position.y + 2.0f;
    vertex.z = parent->Position.z;
    if (axis)
      vertex.x -= 2.0f;
    else
      vertex.z -= 2.0f;

    rail->vertices.push_back(vertex);

    vertex.y -= 4.0f;
    rail->vertices.push_back(vertex);

    if (axis)
      vertex.x += 4.0f;
    else
      vertex.z += 4.0f;
    rail->vertices.push_back(vertex);

    vertex.y += 4.0f;
    rail->vertices.push_back(vertex);


    vertex.x = node->Position.x;
    vertex.y = node->Position.y + 2.0f;
    vertex.z = node->Position.z;
    if (axis)
      vertex.x -= 2.0f;
    else
      vertex.z -= 2.0f;

    rail->vertices.push_back(vertex);

    vertex.y -= 4.0f;
    rail->vertices.push_back(vertex);

    if (axis)
      vertex.x += 4.0f;
    else
      vertex.z += 4.0f;
    rail->vertices.push_back(vertex);

    vertex.y += 4.0f;
    rail->vertices.push_back(vertex);

    rail->Indices.push_back(rail->numIndices + 1);
    rail->Indices.push_back(rail->numIndices + 4);
    rail->Indices.push_back(rail->numIndices + 5);
    rail->Indices.push_back(rail->numIndices + 1);
    rail->Indices.push_back(rail->numIndices);
    rail->Indices.push_back(rail->numIndices + 4);
    rail->Indices.push_back(rail->numIndices);
    rail->Indices.push_back(rail->numIndices + 4);
    rail->Indices.push_back(rail->numIndices + 3);
    rail->Indices.push_back(rail->numIndices + 3);
    rail->Indices.push_back(rail->numIndices + 4);
    rail->Indices.push_back(rail->numIndices + 7);
    rail->Indices.push_back(rail->numIndices + 3);
    rail->Indices.push_back(rail->numIndices + 6);
    rail->Indices.push_back(rail->numIndices + 7);
    rail->Indices.push_back(rail->numIndices + 6);
    rail->Indices.push_back(rail->numIndices + 3);
    rail->Indices.push_back(rail->numIndices + 2);
    rail->Indices.push_back(rail->numIndices + 1);
    rail->Indices.push_back(rail->numIndices + 5);
    rail->Indices.push_back(rail->numIndices + 6);
    rail->Indices.push_back(rail->numIndices + 6);
    rail->Indices.push_back(rail->numIndices + 1);
    rail->Indices.push_back(rail->numIndices + 2);
    rail->numIndices += 4;


  }
  for (DWORD i = 0, numNodes = scene->nodes.size(); i < numNodes; i++)
  {
    if (scene->nodes[i].Class.checksum == RailNode::GetClass())
    {
      for (DWORD j = 0, numLinks = scene->nodes[i].Links.size(); j < numLinks; j++)
      {
        if (scene->nodes[i].Links[j] == index)
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
  static ColouredVertex vertex = ColouredVertex(0, 255, 0);
  RenderableRail* rail = &rails.back();
  rail->nodes.push_back(node->Name);
  float MAX_DIST = parent->Position.x - node->Position.x;
  MAX_DIST *= MAX_DIST;
  static bool axis = false;
  if (((parent->Position.z - node->Position.z)*(parent->Position.z - node->Position.z)) > MAX_DIST)
    axis = true;
  else
    axis = false;


  vertex.x = parent->Position.x;
  vertex.y = parent->Position.y + 2.0f;
  vertex.z = parent->Position.z;
  if (axis)
    vertex.x -= 2.0f;
  else
    vertex.z -= 2.0f;

  rail->vertices.push_back(vertex);

  vertex.y -= 4.0f;
  rail->vertices.push_back(vertex);

  if (axis)
    vertex.x += 4.0f;
  else
    vertex.z += 4.0f;
  rail->vertices.push_back(vertex);

  vertex.y += 4.0f;
  rail->vertices.push_back(vertex);


  vertex.x = node->Position.x;
  vertex.y = node->Position.y + 2.0f;
  vertex.z = node->Position.z;
  if (axis)
    vertex.x -= 2.0f;
  else
    vertex.z -= 2.0f;

  rail->vertices.push_back(vertex);

  vertex.y -= 4.0f;
  rail->vertices.push_back(vertex);

  if (axis)
    vertex.x += 4.0f;
  else
    vertex.z += 4.0f;
  rail->vertices.push_back(vertex);

  vertex.y += 4.0f;
  rail->vertices.push_back(vertex);

  rail->Indices.push_back(rail->numIndices + 1);
  rail->Indices.push_back(rail->numIndices + 4);
  rail->Indices.push_back(rail->numIndices + 5);
  rail->Indices.push_back(rail->numIndices + 1);
  rail->Indices.push_back(rail->numIndices);
  rail->Indices.push_back(rail->numIndices + 4);
  rail->Indices.push_back(rail->numIndices);
  rail->Indices.push_back(rail->numIndices + 4);
  rail->Indices.push_back(rail->numIndices + 3);
  rail->Indices.push_back(rail->numIndices + 3);
  rail->Indices.push_back(rail->numIndices + 4);
  rail->Indices.push_back(rail->numIndices + 7);
  rail->Indices.push_back(rail->numIndices + 3);
  rail->Indices.push_back(rail->numIndices + 6);
  rail->Indices.push_back(rail->numIndices + 7);
  rail->Indices.push_back(rail->numIndices + 6);
  rail->Indices.push_back(rail->numIndices + 3);
  rail->Indices.push_back(rail->numIndices + 2);
  rail->Indices.push_back(rail->numIndices + 1);
  rail->Indices.push_back(rail->numIndices + 5);
  rail->Indices.push_back(rail->numIndices + 6);
  rail->Indices.push_back(rail->numIndices + 6);
  rail->Indices.push_back(rail->numIndices + 1);
  rail->Indices.push_back(rail->numIndices + 2);
  rail->numIndices += 4;

  for (DWORD i = 0, numNodes = scene->nodes.size(); i < numNodes; i++)
  {
    if (scene->nodes[i].Class.checksum == RailNode::GetClass())
    {
      for (DWORD j = 0, numLinks = scene->nodes[i].Links.size(); j < numLinks; j++)
      {
        if (scene->nodes[i].Links[j] == index && !scene->nodes[i].IsAdded())
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
  for (DWORD i = 0, numNodes = scene->nodes.size(); i < numNodes; i++)
  {
    if (scene->nodes[i].Class.checksum == RailNode::GetClass())
    {
      for (DWORD j = 0, numLinks = scene->nodes[i].Links.size(); j < numLinks; j++)
      {
        if (scene->nodes[i].Links[j] == index && !scene->nodes[i].IsAdded())
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
  for (DWORD i = 0, numNodes = scene->nodes.size(); i < numNodes; i++)
  {
    if (scene->nodes[i].Class.checksum == RailNode::GetClass())
    {
      for (DWORD j = 0, numLinks = scene->nodes[i].Links.size(); j < numLinks; j++)
      {
        if (scene->nodes[i].Links[j] == index)
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
  pFile += 4;
  if (texFiles.numFiles)
  {
    texFiles.fileNames = new char*[texFiles.numFiles];//(char**)malloc(texFiles.numFiles);

    for (DWORD i = 0, numFiles = texFiles.numFiles; i < numFiles; i++)
    {
      const DWORD len = strlen((char*)pFile) + 1;
      texFiles.fileNames[i] = new char[len];
      memcpy(texFiles.fileNames[i], pFile, len);
      pFile += len;
      FILE* temp = fopen(texFiles.fileNames[i], "wb");
      if (temp)
      {
        const DWORD size = *(DWORD*)pFile;
        pFile += 4;
        fwrite(pFile, size, 1, temp);
        pFile += size;
        fclose(temp);
        _fcloseall();
      }
    }
  }
}

void LoadMaterials(register const BYTE const* __restrict pFile)
{
  const DWORD numMaterials = *(DWORD*)pFile;
  pFile += 4;
  scene->matList.Resize(numMaterials);
  ZeroMemory(scene->matList.GetMaterial(0), numMaterials*sizeof(Material));
  for (DWORD i = 0; i < numMaterials; i++)
  {
    Material* mat = scene->matList.GetMaterial(i);
    const DWORD numTextures = *(DWORD*)pFile;
    pFile += 4;
    mat->Reserve(numTextures);
    mat->SetMatId(*(DWORD*)pFile);
    pFile += 4;
    mat->drawOrder = *(float*)pFile;
    pFile += 4;
    mat->transparent = *(bool*)pFile;
    pFile++;

    for (DWORD j = 0; j < numTextures; j++)
    {
      Texture texture;
      texture.uAddress = *(DWORD*)pFile;
      pFile += 4;
      texture.vAddress = *(DWORD*)pFile;
      pFile += 4;
      texture.fixedAlpha = *(DWORD*)pFile;
      pFile += 4;
      texture.blendMode = *(DWORD*)pFile;
      pFile += 4;
      texture.SetId(*(DWORD*)pFile);
      pFile += 4;
      texture.CreateTexture();
      mat->AddTexture2(&texture);
    }
  }
}

void LoadScripts(register const BYTE const* __restrict pFile, float version)
{
  const DWORD numNodes = *(DWORD*)pFile;
  pFile += 4;
  if (numNodes)
  {
    scene->nodes.resize(numNodes);
    ZeroMemory(&scene->nodes.front(), numNodes*sizeof(Node));
    for (DWORD i = 0; i < numNodes; i++)
    {
      std::copy((DWORD*)pFile, (DWORD*)pFile + 12, (DWORD*)&scene->nodes[i]);
      pFile += 48;

      const DWORD numLinks = *(DWORD*)pFile;
      pFile += 4;
      scene->nodes[i].Links.assign((WORD*)pFile, ((WORD*)pFile) + numLinks);
      pFile += numLinks * 2;

      if (version > 2.0f)
      {
        scene->nodes[i].TerrainType = *(Checksum*)pFile;
        pFile += 4;
      }
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
  }

  const DWORD numScripts = *(DWORD*)pFile;
  pFile += 4;
  if (numScripts)
  {
    Scripts.reserve(numScripts);
    Scripts.assign((Script*)pFile, ((Script*)pFile) + numScripts);
    pFile += numScripts*sizeof(Script);
  }

  const DWORD numKnownScripts = *(DWORD*)pFile;
  pFile += 4;
  if (numKnownScripts)
  {
    scene->KnownScripts.reserve(numKnownScripts);
    scene->KnownScripts.assign((KnownScript*)pFile, ((KnownScript*)pFile) + numKnownScripts);
    pFile += numKnownScripts*sizeof(KnownScript);
  }

  const DWORD numTriggers = *(DWORD*)pFile;
  pFile += 4;
  if (numTriggers)
  {
    scene->triggers.reserve(numTriggers);
    scene->triggers.assign((Checksum*)pFile, ((Checksum*)pFile) + numTriggers);
    pFile += numTriggers*sizeof(Checksum);
  }

  const DWORD numGlobals = *(DWORD*)pFile;
  pFile += 4;
  if (numGlobals)
  {
    scene->globals.reserve(numGlobals);
    scene->globals.assign((Checksum*)pFile, ((Checksum*)pFile) + numGlobals);
    pFile += numGlobals*sizeof(Checksum);
  }
}

bool LoadState_unmanaged(register const BYTE const* __restrict pFile)
{
  if (pFile)
  {
    CreateCamera();

    pFile += 260;
    scene->SetBlend(*pFile);
    pFile++;
    scene->cullMode = *(DWORD*)pFile;
    pFile += 4;

    MAX_PASSES = *(DWORD*)pFile;
    pFile += 4;
    MAX_TEXCOORDS = *(DWORD*)pFile;
    const DWORD fvf = D3DFVF_XYZ | D3DFVF_DIFFUSE | MAX_TEXCOORDS;
    pFile += 4;

    const DWORD numMaterials = *(DWORD*)pFile;
    pFile += 4;
    scene->matList.Resize(numMaterials);
    ZeroMemory(scene->matList.GetMaterial(0), numMaterials*sizeof(Material));
    for (DWORD i = 0; i < numMaterials; i++)
    {
      Material* mat = scene->matList.GetMaterial(i);
      const DWORD numTextures = *(DWORD*)pFile;
      pFile += 4;
      mat->Reserve(numTextures);
      mat->SetMatId(*(DWORD*)pFile);
      pFile += 4;
      mat->drawOrder = *(float*)pFile;
      pFile += 4;
      mat->transparent = *(bool*)pFile;
      pFile++;
      mat->doubled = *(bool*)pFile;
      pFile++;
      mat->reverse = *(bool*)pFile;
      pFile++;
      mat->invisible = *(bool*)pFile;
      pFile++;
      mat->ignoreAlpha = *(bool*)pFile;
      pFile++;
      mat->alphaMap = *(bool*)pFile;
      pFile++;
      mat->animated = *(bool*)pFile;
      pFile++;
      if (mat->animated)
      {
        DWORD matId = mat->GetMatId();
        scene->AddAnimation(*(Checksum*)&matId, pFile);
        pFile += 4*8;
      }

      for (DWORD j = 0; j < numTextures; j++)
      {
        Texture texture;
        texture.uAddress = *(DWORD*)pFile;
        pFile += 4;
        texture.vAddress = *(DWORD*)pFile;
        pFile += 4;
        texture.fixedAlpha = *(DWORD*)pFile;
        pFile += 4;
        texture.blendMode = *(DWORD*)pFile;
        pFile += 4;
        texture.SetId(*(DWORD*)pFile);
        pFile += 4;
        texture.SetFlags(*(DWORD*)pFile);
        pFile += 4;
        texture.CreateTexture();
        mat->AddTexture2(&texture);
      }
    }

    const DWORD numMeshes = *(DWORD*)pFile;
    pFile += 4;
    scene->Resize(numMeshes, Mesh());

    for (DWORD i = 0; i < numMeshes; i++)
    {
      Mesh* mesh = scene->GetMesh(i);
      mesh->flags = *(DWORD*)pFile;
      pFile += 4;
      mesh->SetName(*(Checksum*)pFile);
      pFile += 4;
      DWORD numVertices = *(DWORD*)pFile;
      mesh->Reserve(numVertices);
      pFile += 4;
      if (numVertices)
      {
        mesh->AddVertices((Vertex*)pFile, numVertices);
        pFile += sizeof(Vertex)*numVertices;
      }
      bool hasCollision = *(bool*)pFile;
      pFile++;
      if (hasCollision)
      {
        mesh->InitCollision();
        Collision* collision = mesh->GetCollision();
        numVertices = *(DWORD*)pFile;
        pFile += 4;
        if (numVertices)
        {
          collision->AddVertices((Vertex*)pFile, numVertices);
          pFile += sizeof(Vertex)*numVertices;
        }
        const DWORD numFaces = *(DWORD*)pFile;
        pFile += 4;
        if (numFaces)
        {
          collision->AddFaces((SimpleFace*)pFile, numFaces);
          pFile += sizeof(SimpleFace)*numFaces;
          collision->AddFaces((FaceInfo*)pFile, numFaces);
          pFile += sizeof(FaceInfo)*numFaces;
        }
      }
      const DWORD numSplits = *(DWORD*)pFile;
      pFile += 4;

      if (numSplits)
      {
        mesh->matSplits.resize(numSplits, MaterialSplit());
        for (DWORD j = 0; j < numSplits; j++)
        {
          mesh->matSplits[j].matId = *(DWORD*)pFile;
          pFile += 4;
          const DWORD numIndices = *(DWORD*)pFile;
          pFile += 4;
          if (numIndices)
          {
            mesh->matSplits[j].Indices.reserve(numIndices);
            mesh->matSplits[j].Indices.assign((WORD*)pFile, ((WORD*)pFile) + numIndices);
            mesh->matSplits[j].numIndices = numIndices - 2;
            pFile += numIndices*sizeof(WORD);
          }
        }
      }
    }

    bool hasSky = *(bool*)pFile;
    pFile++;
    if (hasSky)
    {
      scene->skyDome = new Scene();

      const DWORD numSkyMaterials = *(DWORD*)pFile;
      pFile += 4;
      scene->skyDome->matList.Resize(numSkyMaterials);
      ZeroMemory(scene->skyDome->matList.GetMaterial(0), numSkyMaterials*sizeof(Material));
      for (DWORD i = 0; i < numSkyMaterials; i++)
      {
        Material* mat = scene->skyDome->matList.GetMaterial(i);
        const DWORD numTextures = *(DWORD*)pFile;
        pFile += 4;
        mat->Reserve(numTextures);
        mat->SetMatId(*(DWORD*)pFile);
        pFile += 4;
        mat->drawOrder = *(float*)pFile;
        pFile += 4;
        mat->transparent = *(bool*)pFile;
        pFile++;

        for (DWORD j = 0; j < numTextures; j++)
        {
          Texture texture;
          texture.uAddress = *(DWORD*)pFile;
          pFile += 4;
          texture.vAddress = *(DWORD*)pFile;
          pFile += 4;
          texture.fixedAlpha = *(DWORD*)pFile;
          pFile += 4;
          texture.blendMode = *(DWORD*)pFile;
          pFile += 4;
          texture.SetId(*(DWORD*)pFile);
          pFile += 4;
          texture.SetFlags(*(DWORD*)pFile);
          pFile += 4;
          texture.CreateTexture();
          mat->AddTexture2(&texture);
        }
      }

      const DWORD numSkyMeshes = *(DWORD*)pFile;
      pFile += 4;
      scene->skyDome->Resize(numMeshes, Mesh());

      for (DWORD i = 0; i < numSkyMeshes; i++)
      {
        Mesh* mesh = scene->skyDome->GetMesh(i);
        mesh->flags = *(DWORD*)pFile;
        pFile += 4;
        mesh->SetName(*(Checksum*)pFile);
        pFile += 4;
        DWORD numVertices = *(DWORD*)pFile;
        mesh->Reserve(numVertices);
        pFile += 4;
        if (numVertices)
        {
          mesh->AddVertices((Vertex*)pFile, numVertices);

          pFile += sizeof(Vertex)*numVertices;
        }
        bool hasCollision = *(bool*)pFile;
        pFile++;
        if (hasCollision)
        {
          mesh->InitCollision();
          Collision* collision = mesh->GetCollision();
          numVertices = *(DWORD*)pFile;
          pFile += 4;
          if (numVertices)
          {
            collision->AddVertices((Vertex*)pFile, numVertices);
            pFile += sizeof(Vertex)*numVertices;
          }
          const DWORD numFaces = *(DWORD*)pFile;
          pFile += 4;
          if (numFaces)
          {
            collision->AddFaces((SimpleFace*)pFile, numFaces);
            pFile += sizeof(SimpleFace)*numFaces;
            collision->AddFaces((FaceInfo*)pFile, numFaces);
            pFile += sizeof(FaceInfo)*numFaces;
          }
        }
        const DWORD numSplits = *(DWORD*)pFile;
        pFile += 4;
        if (numSplits)
        {
          mesh->matSplits.resize(numSplits, MaterialSplit());

          for (DWORD j = 0; j < numSplits; j++)
          {
            mesh->matSplits[j].matId = *(DWORD*)pFile;
            pFile += 4;
            const DWORD numIndices = *(DWORD*)pFile;
            pFile += 4;
            if (numIndices)
            {
              mesh->matSplits[j].Indices.reserve(numIndices);
              mesh->matSplits[j].Indices.assign((WORD*)pFile, ((WORD*)pFile) + numIndices);
              pFile += numIndices*sizeof(WORD);
              mesh->matSplits[j].numIndices = numIndices - 2;
            }
          }
        }
      }
    }
    return true;
  }
  return false;
}

//extern struct Th3Scene : Scene { Th3Scene(char* path); };
#pragma managed
bool THPSLevel::LoadScene(char* path)
{
  loading = true;
  bool sort = false;
  this->sceneName = gcnew String(path);
  toolStripStatusLabel1->Text = "Loading Level: " + sceneName;
  this->Refresh();
  CreateCamera();

  DWORD i = 0;
  while (path[i])
  {
    char c = path[i];
    if (c >= 'A' && c <= 'Z') c += 32;
    path[i] = c;
    i++;
  }

  if (strstr(path, "scn.dat") || strstr(path, "skin.dat") || strstr(path, "mdl.dat"))
  {
    scene = new Th4Scene(path);
    sort = true;
  }
  else if (strstr(path, ".scn.xbx") || strstr(path, ".skin.xbx") || strstr(path, ".mdl.xbx"))
  {
    char Path[MAX_PATH] = "";

    memcpy(Path, path, strlen(path) + 1);
    char tmp[25] = "";
    std::string s_path(path);
    DWORD pos = s_path.find_last_of("\\");
    printf("pos %d\n", pos);
    DWORD endPos = s_path.find("_net.scn.xbx");
    if (endPos == std::string::npos)
      endPos = s_path.find(".scn.xbx");
    if(endPos == std::string::npos)
      endPos = s_path.find("_net.mdl.xbx");
    if (endPos == std::string::npos)
      endPos = s_path.find(".mdl.xbx");
    if (endPos == std::string::npos)
      endPos = s_path.find("_net.skin.xbx");
    if (endPos == std::string::npos)
      endPos = s_path.find(".skin.xbx");

    printf("endPos %d\n", endPos);

    if (endPos != std::string::npos)
    {
      for (DWORD i = pos; i < endPos; i++)
      {
        tmp[i - pos] = path[i];
      }
      char tmp2[100] = "";
      sprintf(tmp2, "%s.col.xbx", tmp);
      memcpy(&Path[pos], tmp2, strlen(tmp2) + 1);
      FILE* f = fopen(Path, "rb");
      //fseek(f, SEEK_SET, 1);
      DWORD version;
      fread(&version, 4, 1, f);
      fclose(f);
      if (version == 0x9)
          scene = new ThugScene(path);
      else if (version == 0xA)
          scene = new Thug2Scene(path);
      else
          MessageBox::Show("Unknown Version");
    }
    else
    {
        printf("Whyy???\n");
        scene = new Thug2Scene(path);
    }
    //sort = true;
  }
  else if (strstr(path, ".psx"))
  {
    scene = new Th2xScene(path);
  }
  else if (strstr(path, ".bsp"))
  {
    scene = new Th3Scene(path);
  }
  else
    MessageBox::Show("unsupported scene");
  _fcloseall();

  if (scene)
  {
    for (DWORD i = 0, numMeshes = scene->GetNumMeshes(); i < numMeshes; i++)
    {
      Mesh* mesh = scene->GetMesh(i);
      if (mesh && mesh->GetNumVertices())
      {
        mesh->CopyVertsToColl();
      }
    }
    for (DWORD i = 0, numMeshes = scene->GetNumMeshes(); i < numMeshes; i++)
    {
      if (scene->GetMesh(i)->GetNumSplits() == 0 || scene->GetMesh(i)->GetNumVertices() == 0)
        scene->GetMesh(i)->SetVisible(false);
    }
    globalMaterialList = &scene->matList;
    scene->sort();
    for (DWORD i = 1; i < scene->nodes.size(); i++)
    {
      for (DWORD j = 0; j < i; j++)
      {
        if (scene->nodes[i].Name.checksum == scene->nodes[j].Name.checksum)
        {
          scene->DeleteNode(j);
          j--;
          i--;
        }
      }
    }
    for (DWORD i = 0, numNodes = scene->nodes.size(); i < numNodes; i++)
    {
      const DWORD name = scene->nodes[i].Trigger.checksum;
      if (name && name != Checksum("SetTh2Physics").checksum && name != Checksum("sk3_killskater").checksum && name != Checksum("LoadSounds").checksum && name != Checksum("sk3_teleporttonode").checksum)
      {
        for (DWORD j = 0, numScripts = scene->KnownScripts.size(); j < numScripts; j++)
        {
          if (scene->KnownScripts[j].name.checksum == name)
          {
            if (scene->KnownScripts[j].size == 0)
            {
              scene->KnownScripts.erase(scene->KnownScripts.begin() + j);
              scene->nodes[i].Trigger.checksum = 0;
              goto notfound;
            }
            else if (!scene->nodes[i].NetEnabled)
            {
              BYTE* pFile = scene->KnownScripts[j].func;
              BYTE* eof = pFile + scene->KnownScripts[j].size;
              while (pFile < eof)
              {
                const BYTE opcode = *pFile;
                pFile++;
                switch (opcode)
                {
                case 0x1C:
                case 0x1B:
                  pFile += *(DWORD*)pFile + 4;
                  break;
                case 0x17:
                case 0x1A:
                case 0x2E:
                  pFile += 4;
                  break;
                case 2:
                  pFile += 4;
                  break;
                case 1:
                  break;
                case 0x1F:
                  pFile += 8;
                  break;
                case 0x1E:
                  pFile += 12;
                  break;
                case 0x40:
                case 0x2F:
                case 0x37:
                  pFile += *(DWORD*)pFile * 4 + 4;
                  break;
                case 0x16:
                  if (*(DWORD*)pFile == 0xEEC24148 || *(DWORD*)pFile == 0x6E77719D || *(DWORD*)pFile == 0xF8D6A996 ||
                    *(DWORD*)pFile == 0x70291F04 || *(DWORD*)pFile == 0xCB8B5900 || *(DWORD*)pFile == 0x854F17A6)
                  {
                    scene->nodes[i].NetEnabled = true;
                    scene->nodes[i].Permanent = true;
                    goto foundere;
                  }
                  pFile += 4;
                  break;
                }
              }
            }
            goto foundere;
          }
        }

        for (DWORD j = 0, numScripts = scene->KnownScripts.size(); j < numScripts; j++)
        {
          BYTE* pFile = scene->KnownScripts[j].func;
          BYTE* eof = pFile + scene->KnownScripts[j].size;
          BYTE* offset = NULL;
          while (pFile < eof)
          {
            const BYTE opcode = *pFile;
            pFile++;
            switch (opcode)
            {
            case 0x1C:
            case 0x1B:
              pFile += *(DWORD*)pFile + 4;
              break;
            case 0x17:
            case 0x1A:
            case 0x2E:
              pFile += 4;
              break;
            case 2:
              pFile += 4;
              break;
            case 1:
              break;
            case 0x1F:
              pFile += 8;
              break;
            case 0x1E:
              pFile += 12;
              break;
            case 0x40:
            case 0x2F:
            case 0x37:
              pFile += *(DWORD*)pFile * 4 + 4;
              break;
            case 0x16:
              if (*(DWORD*)pFile == name)
                goto foundere;
              pFile += 4;
              break;
            }
          }
        }
      notfound:

        scene->RemoveTriggers(name);
        for (DWORD j = 0; j < scene->KnownScripts.size(); j++)
        {
          BYTE* pFile = scene->KnownScripts[j].func;
          BYTE* eof = pFile + scene->KnownScripts[j].size;
          BYTE* offset = NULL;
          while (pFile < eof)
          {
            const BYTE opcode = *pFile;
            pFile++;
            switch (opcode)
            {
            case 0x1C:
            case 0x1B:
              pFile += *(DWORD*)pFile + 4;
              break;
            case 0x17:
            case 0x1A:
            case 0x2E:
              pFile += 4;
              break;
            case 2:
              pFile += 4;
              break;
            case 1:
              break;
            case 0x1F:
              pFile += 8;
              break;
            case 0x1E:
              pFile += 12;
              break;
            case 0x40:
            case 0x2F:
            case 0x37:
              pFile += *(DWORD*)pFile * 4 + 4;
              break;
            case 0x16:
              if (*(DWORD*)pFile == name)
              {
                scene->KnownScripts[j].Remove(pFile - 1, 5);
                pFile--;
                eof -= 5;
              }
              else
                pFile += 4;
              break;
            }
          }
        }
      foundere:;
      }
    }
    for (DWORD i = 1; i < scene->triggers.size(); i++)
    {
      DWORD name2 = scene->triggers[i].checksum;
      if (name2 == 0x772D5283 || name2 == Checksum("SetTh2Physics").checksum || name2 == Checksum("sk3_killskater").checksum || name2 == Checksum("LoadSounds").checksum || name2 == Checksum("sk3_teleporttonode").checksum)
        continue;
      bool found = false;
      for (DWORD j = 0, numScripts = scene->KnownScripts.size(); j < numScripts; j++)
      {
        if (scene->KnownScripts[j].name.checksum == scene->triggers[i].checksum)
        {
          found = true;
          if (scene->KnownScripts[j].size == 0)
          {
            DWORD name = scene->KnownScripts[j].name.checksum;


            for (DWORD k = 0; k < numScripts; k++)
            {
              BYTE* pFile = scene->KnownScripts[k].func;
              BYTE* eof = pFile + scene->KnownScripts[k].size;
              BYTE* offset = NULL;
              while (pFile < eof)
              {
                const BYTE opcode = *pFile;
                pFile++;
                switch (opcode)
                {
                case 0x1C:
                case 0x1B:
                  pFile += *(DWORD*)pFile + 4;
                  break;
                case 0x17:
                case 0x1A:
                case 0x2E:
                  pFile += 4;
                  break;
                case 2:
                  pFile += 4;
                  break;
                case 1:
                  break;
                case 0x1F:
                  pFile += 8;
                  break;
                case 0x1E:
                  pFile += 12;
                  break;
                case 0x40:
                case 0x2F:
                case 0x37:
                  pFile += *(DWORD*)pFile * 4 + 4;
                  break;
                case 0x16:
                  if (*(DWORD*)pFile == name)
                    goto found;
                  pFile += 4;
                  break;
                }
              }
            }
            for (DWORD k = 1; k < scene->triggers.size(); k++)
            {
              if (scene->triggers[k].checksum == name)
              {
                i--;
                scene->triggers.erase(scene->triggers.begin() + k);
                break;
              }
            }
            scene->KnownScripts.erase(scene->KnownScripts.begin() + j);
            numScripts--;
            j--;
            scene->RemoveTriggers(name);
          found:;
          }
        }
      }
      if (!found)
      {
        DWORD name = scene->triggers[i].checksum;


        for (DWORD j = 0, numScripts = scene->KnownScripts.size(); j < numScripts; j++)
        {
          BYTE* pFile = scene->KnownScripts[j].func;
          BYTE* eof = pFile + scene->KnownScripts[j].size;
          BYTE* offset = NULL;
          while (pFile < eof)
          {
            const BYTE opcode = *pFile;
            pFile++;
            switch (opcode)
            {
            case 0x1C:
            case 0x1B:
              pFile += *(DWORD*)pFile + 4;
              break;
            case 0x17:
            case 0x1A:
            case 0x2E:
              pFile += 4;
              break;
            case 2:
              pFile += 4;
              break;
            case 1:
              break;
            case 0x1F:
              pFile += 8;
              break;
            case 0x1E:
              pFile += 12;
              break;
            case 0x40:
            case 0x2F:
            case 0x37:
              pFile += *(DWORD*)pFile * 4 + 4;
              break;
            case 0x16:
              if (*(DWORD*)pFile == name)
                goto founder;
              pFile += 4;
              break;
            }
          }
        }
        for (DWORD j = 1, numTriggers = scene->triggers.size(); j < numTriggers; j++)
        {
          if (scene->triggers[j].checksum == name)
          {
            i--;
            scene->triggers.erase(scene->triggers.begin() + j);
            break;
          }
        }
        scene->RemoveTriggers(name);
      founder:;
      }
    }

    for (DWORD i = 1, numTriggers = scene->triggers.size(); i < numTriggers; i++)
    {
      DWORD name2 = scene->triggers[i].checksum;
      if (name2 == 0x772D5283 || name2 == Checksum("SetTh2Physics").checksum || name2 == Checksum("sk3_killskater").checksum || name2 == Checksum("LoadSounds").checksum || name2 == Checksum("sk3_teleporttonode").checksum)
        continue;
      for (DWORD j = 0; j < scene->nodes.size(); j++)
      {
        if (scene->nodes[j].Trigger.checksum == scene->triggers[i].checksum)
          goto next;
      }
      for (DWORD j = 0; j < scene->KnownScripts.size(); j++)
      {
        BYTE* pFile = scene->KnownScripts[j].func;
        BYTE* eof = pFile + scene->KnownScripts[j].size;
        BYTE* offset = NULL;
        while (pFile < eof)
        {
          const BYTE opcode = *pFile;
          pFile++;
          switch (opcode)
          {
          case 0x1C:
          case 0x1B:
            pFile += *(DWORD*)pFile + 4;
            break;
          case 0x17:
          case 0x1A:
          case 0x2E:
            pFile += 4;
            break;
          case 2:
            pFile += 4;
            break;
          case 1:
            break;
          case 0x1F:
            pFile += 8;
            break;
          case 0x1E:
            pFile += 12;
            break;
          case 0x40:
          case 0x2F:
          case 0x37:
            pFile += *(DWORD*)pFile * 4 + 4;
            break;
          case 0x16:
            if (*(DWORD*)pFile == scene->triggers[i].checksum)
              goto next;
            pFile += 4;
            break;
          }
        }
      }
      for (DWORD j = 1; j < scene->KnownScripts.size(); j++)
      {
        if (scene->triggers[i].checksum == scene->KnownScripts[j].name.checksum)
        {
          scene->KnownScripts.erase(scene->KnownScripts.begin() + j);
          break;
        }
      }
      scene->triggers.erase(scene->triggers.begin() + i);
      numTriggers--;
      i--;
    next:;
    }
    for (DWORD i = 0; i < scene->nodes.size(); i++)
    {
      if (scene->nodes[i].Class.checksum == EnvironmentObject::GetClass())
      {
        if (scene->GetMesh(scene->nodes[i].Name))
        {
          if (scene->GetMesh(scene->nodes[i].Name)->IsDeleted())
          {
            scene->DeleteNode(i);
            i--;
          }
        }
        else
        {
          scene->DeleteNode(i);
          i--;
        }

      }
    }
    globalMaterialList = &scene->matList;
    //scene->RemoveUnusedMaterials();//scene->OptimizeMaterials();
    scene->CreateBuffers(Device, D3DFVF_XYZ | D3DFVF_DIFFUSE | MAX_TEXCOORDS);
    numAnimations = scene->GetNumAnimations();
    if (numAnimations)
      forceRedraw = true;
    animations = scene->GetAnimations();
    if (sort)
    {
      for (DWORD i = 0, numMeshes = scene->GetNumMeshes(); i < numMeshes; i++) { Mesh* tmpMesh = scene->GetMesh(i); /*tmpMesh->Sort();*/ listBox1->Items->Add(gcnew String(tmpMesh->GetName().GetString()) + "[" + i.ToString() + "]"); properties->Add(gcnew MeshProperties(tmpMesh));  if (tmpMesh->IsTransparent() && tmpMesh->flags & MeshFlags::isVisible) numTrans++; else if (tmpMesh->flags & MeshFlags::isVisible) numOpaque++; }
    }
    else
    {
      for (DWORD i = 0, numMeshes = scene->GetNumMeshes(); i < numMeshes; i++) { Mesh* tmpMesh = scene->GetMesh(i); /*tmpMesh->Sort();*/ listBox1->Items->Add(gcnew String(tmpMesh->GetName().GetString()) + "[" + i.ToString() + "]"); properties->Add(gcnew MeshProperties(tmpMesh));  if (tmpMesh->IsTransparent() && tmpMesh->flags & MeshFlags::isVisible) numTrans++; else if (tmpMesh->flags & MeshFlags::isVisible) numOpaque++; }
    }

    opaque = new Mesh*[numOpaque];
    numOpaque = 0;
    trans = new Mesh*[numTrans];
    numTrans = 0;

    for (DWORD i = 0, numMeshes = scene->GetNumMeshes(); i < numMeshes; i++)
    {
      Mesh* tmpMesh = scene->GetMesh(i);
      if (tmpMesh->flags & MeshFlags::isTransparent && tmpMesh->flags & MeshFlags::isVisible)
      {
        trans[numTrans] = tmpMesh; numTrans++;
      }
      else if (tmpMesh->flags & MeshFlags::isVisible)
      {
        opaque[numOpaque] = tmpMesh; numOpaque++;
      }
    }

    const DWORD numNodes = scene->nodes.size();
    Node* nodes = NULL;
    if(numNodes) nodes = &scene->nodes.front();


    for (DWORD i = 0; i < numNodes; i++)
    {
      if (nodes[i].Class.checksum == 0x1806DDF8)
      {
        spawns.push_back(&nodes[i]);
      }
      else if (nodes[i].Class.checksum == RailNode::GetClass())
      {
        for (DWORD j = 0; j < nodes[i].Links.size(); j++)
        {
          if (nodes[nodes[i].Links[j]].Class.checksum != RailNode::GetClass())
          {
            nodes[i].Links.erase(nodes[i].Links.begin() + j);
            j--;
          }
        }//yeaa erasesers?
        if (!nodes[i].Links.size())
        {
          rails.push_back(RenderableRail());
          AddRails(i);
        }
      }
    }

    for (DWORD i = 0; i < numNodes; i++)
    {
      if (nodes[i].Class.checksum == RailNode::GetClass() && nodes[i].Links.size() && !nodes[i].IsAdded())
      {
        rails.push_back(RenderableRail());
        AddLinked(i);
      }
    }


    for (DWORD i = 0, numRails = rails.size(); i < numRails; i++)
    {
      const DWORD numVerts = rails[i].vertices.size();

      if (numVerts != 0)
      {
        const DWORD numIndices = rails[i].Indices.size();
        rails[i].numVerts = numVerts;
        rails[i].numIndices = numIndices / 3;
        if (FAILED(Device->CreateVertexBuffer(numVerts*sizeof(ColouredVertex), D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &rails[i].verts, NULL)))
          MessageBox::Show("failed to create vertex buffer");


        ColouredVertex* pVertices;
        rails[i].verts->Lock(0, 0, (void**)&pVertices, 0);
        memcpy(pVertices, &rails[i].vertices.front(), numVerts*sizeof(ColouredVertex));
        rails[i].verts->Unlock();
        rails[i].vertices.clear();


        Device->CreateIndexBuffer(numIndices * 2, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &rails[i].indices, NULL);


        WORD* indices;
        rails[i].indices->Lock(0, 0, (void**)&indices, 0);
        memcpy(indices, &rails[i].Indices.front(), numIndices * 2);
        rails[i].indices->Unlock();
        rails[i].Indices.clear();
      }
      else
        rails[i].Indices.clear();
    }

    vector<char*> numFiles;

    for (DWORD i = 0; i < texFiles.numFiles; i++)
    {
      numFiles.push_back(texFiles.fileNames[i]);
    }

    delete[] texFiles.fileNames;//free(texFiles.fileNames);
  redo:
    for (DWORD i = 0; i < numFiles.size(); i++)
    {
      for (DWORD j = i + 1; j < numFiles.size(); j++)
      {
        if (numFiles[i] == NULL || !stricmp(numFiles[i], numFiles[j]))
        {
          delete[] numFiles[i];
          numFiles[i] = NULL;
          numFiles.erase(numFiles.begin() + i);
          goto redo;
        }
      }
    }
    texFiles.numFiles = numFiles.size();
    texFiles.fileNames = new char*[texFiles.numFiles];//(char**)malloc(numFiles.size()*sizeof(TexFiles));
    for (DWORD i = 0; i < texFiles.numFiles; i++)
    {
      texFiles.fileNames[i] = numFiles[i];
      numFiles[i] = NULL;
    }

    numFiles.clear();

    selectedObjects.reserve(scene->GetNumMeshes());
    D3DXVECTOR3 faceNormal, normalized;

    /*for (DWORD i = 0; i < scene->GetNumMeshes(); i++)
    {
    Mesh* mesh = scene->GetMesh(i);
    Collision* collision = mesh->GetCollision();
    if (collision)
    {
    for (DWORD j = 0; j < collision->GetNumFaces(); j++)
    {
    if (collision->GetFaceInfo(j)->flags != 0x0)
    goto Next;
    }
    if (mesh->IsFlat(&faceNormal) && !mesh->IsSelected())
    {
    if (mesh->ReallyFlat())
    {
    extern void SelectObjectOnly(Mesh* mesh, bool);
    SelectObjectOnly(mesh, true);
    D3DXVec3Normalize(&normalized, &faceNormal);
    normalized.x *= -1.1f;//-distance;
    normalized.y *= -1.1f;//-distance;
    normalized.z *= -1.1f;//-distance;
    mesh->MoveRelative(&normalized);
    }
    }
    }
    Next:;
    }*/
    scene->FixZFighting2();
    sprintf(info, "%d Verts %d Tris", scene->GetNumVertices(), scene->GetNumTriangles());
    Checksum("Shatter");
    Checksum("Kill");
    THPSLevel::toolStripStatusLabel1->Text = sceneName;
    loading = false;
    if (FPS)
    {
      POINT mouse;
      DWORD middleX = port.Width / 2;
      DWORD middleY = port.Height / 2;
      mouse.x = middleX;
      mouse.y = middleY;
      ClientToScreen(handleWindow, &mouse);
      SetCursorPos(mouse.x, mouse.y);
      this->panel6->Focus();
    }
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
  if (value > THPSLevel::toolStripProgressBar1->Maximum)
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
  if (THPSLevel::toolStripProgressBar1->Value < THPSLevel::toolStripProgressBar1->Maximum)
    THPSLevel::toolStripProgressBar1->Value++;
}

#pragma managed
bool THPSLevel::LoadState(char* name)
{
  render = false;
  _fcloseall();
  if (scene)
    UnloadScene();
  HZIP hz = OpenZip(name, NULL);
  DWORD pos = ((std::string)name).find_last_of("\\");
  char shortName[128] = "";

  DWORD i = pos + 1;
  DWORD j = 0;
  while (name[i] != 0x0)
  {
    shortName[j] = name[i];
    i++;
    j++;
  }
  shortName[j] = 0x0;
  DWORD shortLen = strlen(shortName);

  shortName[shortLen - 3] = 't';
  shortName[shortLen - 2] = 'e';
  shortName[shortLen - 1] = 'x';

  int index;
  ZIPENTRY ze;

  FindZipItem(hz, shortName, true, &index, &ze);
  if (index == -1)
  {
    CloseZip(hz);
    return false;
  }
  int size = ze.unc_size;
  if (size == -1)
    size = 5016487;
  BYTE* pFile = new byte[size];
  UnzipItem(hz, index, pFile, size);

  LoadTextures(pFile);
  delete[] pFile;

  scene = new Scene();

  shortName[shortLen - 3] = 't';
  shortName[shortLen - 2] = 'r';
  shortName[shortLen - 1] = 'g';

  FindZipItem(hz, shortName, true, &index, &ze);
  if (index == -1)
  {
    CloseZip(hz);
    return false;
  }
  size = ze.unc_size;
  if (size == -1)
    size = 5016487;
  BYTE* pScriptFile = new byte[size];
  UnzipItem(hz, index, pScriptFile, size);

  shortName[shortLen - 3] = 'm';
  shortName[shortLen - 2] = 's';
  shortName[shortLen - 1] = 'h';

  FindZipItem(hz, shortName, true, &index, &ze);
  if (index == -1)
  {
    CloseZip(hz);
    return false;
  }
  size = ze.unc_size;
  if (size == -1)
    size = 5016487;
  pFile = new byte[size];
  UnzipItem(hz, index, pFile, size);

  ::MAX_PASSES = 1;
  ::MAX_TEXCOORDS = 1;
  resetMouse = false;

  const static float version = 3.0f;
  float oldVer;
  oldVer = *(float*)pFile;
  float supported = 2.0f;
  pFile += 4;
  LoadScripts(pScriptFile, oldVer);
  delete[] pScriptFile;

  _fcloseall();
  if (oldVer == version || oldVer == supported)
  {
    scene->SetName((char*)pFile);

    loading = true;
    this->sceneName = gcnew String((char*)pFile);
    toolStripStatusLabel1->Text = "Loading Level: " + sceneName;
    this->Refresh();

    if (LoadState_unmanaged(pFile))
    {
      pFile -= 4;
      delete[] pFile;
      if (scene)
      {

        globalMaterialList = &scene->matList;
        scene->CreateBuffers(Device, D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX0);

        for (DWORD i = 0, numMeshes = scene->GetNumMeshes(); i < numMeshes; i++) { Mesh* tmpMesh = scene->GetMesh(i); listBox1->Items->Add(gcnew String(tmpMesh->GetName().GetString()) + "[" + i.ToString() + "]"); properties->Add(gcnew MeshProperties(tmpMesh));  if (tmpMesh->IsTransparent()) numTrans++; else numOpaque++; }

        opaque = new Mesh*[numOpaque];
        numOpaque = 0;
        trans = new Mesh*[numTrans];
        numTrans = 0;

        for (DWORD i = 0, numMeshes = scene->GetNumMeshes(); i < numMeshes; i++)
        {
          Mesh* tmpMesh = scene->GetMesh(i);
          if (tmpMesh->flags & MeshFlags::isTransparent)
          {
            trans[numTrans] = tmpMesh; numTrans++;
          }
          else
          {
            opaque[numOpaque] = tmpMesh; numOpaque++;
          }
        }

        const DWORD numNodes = scene->nodes.size();
        Node* nodes = NULL;
        if(numNodes)
          nodes = &scene->nodes.front();


        for (DWORD i = 0; i < numNodes; i++)
        {
          if (nodes[i].Class.checksum == 0x1806DDF8)
          {
            spawns.push_back(&nodes[i]);
          }
          else if (nodes[i].Class.checksum == RailNode::GetClass())
          {
            for (DWORD j = 0; j < nodes[i].Links.size(); j++)
            {
              if (nodes[nodes[i].Links[j]].Class.checksum != RailNode::GetClass())
              {
                nodes[i].Links.erase(nodes[i].Links.begin() + j);
                j--;
              }
            }
            if (!nodes[i].Links.size())
            {
              rails.push_back(RenderableRail());
              AddRails(i);
            }
          }
        }

        for (DWORD i = 0; i < numNodes; i++)
        {
          if (nodes[i].Class.checksum == RailNode::GetClass() && nodes[i].Links.size() && !nodes[i].IsAdded())
          {
            rails.push_back(RenderableRail());
            AddLinked(i);
          }
        }


        for (DWORD i = 0, numRails = rails.size(); i < numRails; i++)
        {
          const DWORD numVerts = rails[i].vertices.size();

          if (numVerts != 0)
          {
            const DWORD numIndices = rails[i].Indices.size();
            rails[i].numVerts = numVerts;
            rails[i].numIndices = numIndices;
            if (FAILED(Device->CreateVertexBuffer(numVerts*sizeof(ColouredVertex), D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &rails[i].verts, NULL)))
              MessageBox::Show("failed to create vertex buffer");


            ColouredVertex* pVertices;
            rails[i].verts->Lock(0, 0, (void**)&pVertices, 0);
            memcpy(pVertices, &rails[i].vertices.front(), numVerts*sizeof(ColouredVertex));
            rails[i].verts->Unlock();
            rails[i].vertices.clear();


            Device->CreateIndexBuffer(numIndices * 2, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &rails[i].indices, NULL);


            WORD* indices;
            rails[i].indices->Lock(0, 0, (void**)&indices, 0);
            memcpy(indices, &rails[i].Indices.front(), numIndices * 2);
            rails[i].indices->Unlock();
            rails[i].Indices.clear();
          }
          else
            rails[i].Indices.clear();
        }
        selectedObjects.reserve(scene->GetNumMeshes());
        //scene->FixZFighting();
        sprintf(info, "%d Verts %d Tris", scene->GetNumVertices(), scene->GetNumTriangles());
        Checksum("Shatter");
        Checksum("Kill");
        THPSLevel::toolStripStatusLabel1->Text = sceneName;

        ResetProgressbar();
        scene->SetSkyPos(camera->pos());
        SetDeviceStates();

        render = true;
        _fcloseall();
        loading = false;
        if (!forceRedraw)
          DrawFrame();
        return true;
      }
    }
    else
    {
      loading = false;
      render = true;
      delete[] pFile;
      _fcloseall();
      if (scene)
        UnloadScene();
      scene = NULL;

      THPSLevel::toolStripStatusLabel1->Text = "?";

      return false;
    }
  }
  CloseZip(hz);
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
  if (scene)
  {

    TCHAR path[MAX_PATH] = "";

    BROWSEINFO bi = { 0 };
    LPMALLOC pMalloc = NULL;
    bi.hwndOwner = (HWND)(void*)this->Handle.ToPointer();

    if (SUCCEEDED(SHGetMalloc(&pMalloc)))
    {
      bi.lpszTitle = "Choose folder";
      bi.ulFlags = BIF_NEWDIALOGSTYLE;

      LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

      if (pidl != 0)
      {
        SHGetPathFromIDList(pidl, path);
        DWORD pos = ((std::string)path).find_last_of("\\");
        char tmp[25] = "";
        strcpy(tmp, &path[pos]);
        strcat(path, tmp);
        strcat(path, ".bsp");

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
  }
  else
    MessageBox::Show("Load a Scene first");
}

System::Void THPSLevel::tHUG2ToolStripMenuItem1_Click(System::Object^  sender, System::EventArgs^  e) {
  if (scene)
  {

    TCHAR path[MAX_PATH] = "";

    BROWSEINFO bi = { 0 };
    LPMALLOC pMalloc = NULL;
    bi.hwndOwner = (HWND)(void*)this->Handle.ToPointer();

    if (SUCCEEDED(SHGetMalloc(&pMalloc)))
    {
      bi.lpszTitle = "Choose folder";
      bi.ulFlags = BIF_NEWDIALOGSTYLE;

      LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

      if (pidl != 0)
      {
        SHGetPathFromIDList(pidl, path);
        DWORD pos = ((std::string)path).find_last_of("\\");
        char tmp[25] = "";
        strcpy(tmp, &path[pos]);
        strcat(path, tmp);
        strcat(path, ".scn.xbx");

        toolStripStatusLabel1->Text = "Exporting Level: " + gcnew String(path);
        this->Refresh();
        scene->ExportScn(path);
        _fcloseall();
        toolStripStatusLabel1->Text = this->sceneName;
        ResetProgressbar();
      }
      pMalloc->Free(pidl);
      pMalloc->Release();
    }
  }
  else
    MessageBox::Show("Load a Scene first");
}

void THPSLevel::UnloadScene()
{
  numOpaque = 0;
  numTrans = 0;
  delete opaque;
  delete trans;
  spawns.clear();
  rails.clear();
  listBox1->Items->Clear();
  selectedNodes.clear();

  for (DWORD i = 0; i < texFiles.numFiles; i++)
  {
    if (texFiles.fileNames[i])
    {
      DeleteFile(texFiles.fileNames[i]);
      delete[] texFiles.fileNames[i];
    }
  }

  texFiles.numFiles = 0;
  delete[] texFiles.fileNames;
  texFiles.fileNames = NULL;
  ReleaseTextures();
  if (scene->skyDome)
    delete scene->skyDome;
  CleanupEvents();

  delete scene;
  extern vector<RenderableRail> addedRails;
  addedRails.clear();

  scene = NULL;
  Scripts.clear();
  delete camera;
  delete player;
  camera = NULL;
  propertyGrid1->SelectedObject = 0;
  properties->Clear();
}

#pragma unmanaged

void ReleaseTextures()
{
  if (scene)
  {
    scene->matList.ReleaseTextures();
    if (scene->skyDome)
    {
      scene->skyDome->matList.ReleaseTextures();
    }

  }
}

#pragma managed

void THPSLevel::DeleteManagedObjects()
{
  m_font->Release();
  m_font = NULL;

  ReleaseTextures();
  scene->ReleaseBuffers();
}

#pragma unmanaged

inline void CreateTextures()
{
  scene->CreateTextures();
}

#pragma managed

void THPSLevel::CreateManagedObjects()
{
  D3DXCreateFontA(Device, 25, 0, FW_NORMAL, 0, FALSE, DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &m_font);
  if (scene)
  {
    CreateTextures();
    scene->CreateBuffers(Device, D3DFVF_XYZ | D3DFVF_DIFFUSE | MAX_TEXCOORDS);
  }
}

System::Void THPSLevel::objectsToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
  toolStripDropDownButton1->Text = "Mesh";

  renderCollision = false;
  showTextures = toolStripButton1->Checked;
  toolStripButton1->Enabled = true;
  toolStripButton1->Visible = true;
  Device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | MAX_TEXCOORDS);
  Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
  Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
  Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
  Device->SetRenderState(D3DRS_BLENDFACTOR, D3DXCOLOR(255, 0, 0, 255));
  Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
  Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
  Device->SetRenderState(D3DRS_ALPHAREF, 8);
  Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
  if (!forceRedraw)
    DrawFrame();
}

System::Void THPSLevel::button1_Click(System::Object^ sender, System::EventArgs^ e)
{
  extern bool addingNode;
  addingNode = true;
}

System::Void THPSLevel::collisionToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
  toolStripDropDownButton1->Text = "Collision";

  renderCollision = true;
  //toolStripButton1->Checked=false;
  showTextures = false;
  toolStripButton1->Enabled = false;
  toolStripButton1->Visible = false;
  for (DWORD i = 0; i < MAX_PASSES; i++) Device->SetTexture(i, NULL);
  Device->SetRenderState(D3DRS_ALPHAREF, 128);
  Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
  Device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
  Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
  if (!forceRedraw)
    DrawFrame();
}


void THPSLevel::OpenLoadSceneDialog(const char* const filter)
{
  render = false;
  if (scene)
  {
    UnloadScene();
  }
  ::MAX_PASSES = 1;
  ::MAX_TEXCOORDS = 1;
  ::resetMouse = false;
  bool file;
  char theFileName[MAX_PATH];
  OPENFILENAME ofn;
  ZeroMemory(&ofn, sizeof(ofn));
  ZeroMemory(theFileName, sizeof(theFileName));

  ofn.lStructSize = sizeof(ofn);
  ofn.hwndOwner = NULL;
  ofn.lpstrFilter = filter;
  ofn.lpstrFile = theFileName;
  ofn.nMaxFile = MAX_PATH;
  ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
  ofn.lpstrInitialDir = NULL;

  if (!GetOpenFileName(&ofn))
    file = false;
  else
    file = true;
  if (file)
  {
    if (LoadScene(theFileName))
    {
      //addingNode=true;
      MAX_PASSES = 1;
      MAX_TEXCOORDS = 0x100;
      ResetProgressbar();
      scene->SetSkyPos(camera->pos());
      for (DWORD i = 0; i < scene->nodes.size(); i++)
      {
        if (scene->nodes[i].Class.checksum == Restart::GetClass())
        {
          player->position.x = scene->nodes[i].Position.x;
          player->position.y = scene->nodes[i].Position.y + 1.0f;
          player->position.z = scene->nodes[i].Position.z;
          player->state = OnGround;
          extern bool jumping;
          jumping = false;
          extern bool spaceDown;
          spaceDown = false;
          break;
        }
      }

      SetDeviceStates();
      ::defaultCulling = scene->cullMode;
      for (DWORD i = 0; i < scene->GetNumMeshes(); i++)
      {
        Mesh* mesh = scene->GetMesh(i);
        if (mesh)
        if (mesh->GetName().checksum == 0xE18484C7 || mesh->GetName().checksum == 0xC78484E1)
        {
          if (mesh->GetVertex(2).colour.a != 0xFF)
            ::MessageBox(0, "Yaw2", "Yaw2", 0);
        }
      }
    }
  }
  else
    scene = NULL;
  render = true;
  if (!forceRedraw)
    DrawFrame();
}
//"All supported MDL/SKIN/SCN/PSX/BSP (*.mdl *mdl.xbx *mdl.dat *.skin *skin.xbx *skin.dat *.scn *scn.xbx *scn.dat *.psx *.bsp)\0*.mdl;*mdl.xbx;*mdl.dat;*.skin;*skin.xbx;*skin.dat;*.scn;*scn.xbx;*scn.dat;*.psx;*.bsp\0THPS4 MDL/SKIN/SCN (*.mdl *mdl.xbx *mdl.dat *.skin *skin.xbx *skin.dat *.scn *scn.xbx *scn.dat)\0*.mdl;*mdl.xbx;*mdl.dat;*.skin;*skin.xbx;*skin.dat;*.scn;*scn.xbx;*scn.dat\0THPS2 psx (*.psx)\0*.psx\0THPS3 BSP (*.BSP)\0*.bsp\0\0";
System::Void THPSLevel::tHPS2ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
  OpenLoadSceneDialog("THPS2x PSX (*.psx)\0*.psx\0\0");
}

System::Void THPSLevel::tHPS4ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
  OpenLoadSceneDialog("THPS4+ MDL/SKIN/SCN (*.mdl *mdl.xbx *mdl.dat *.skin *skin.xbx *skin.dat *.scn *scn.xbx *scn.dat)\0*.mdl;*mdl.xbx;*mdl.dat;*.skin;*skin.xbx;*skin.dat;*.scn;*scn.xbx;*scn.dat\0\0");
}

System::Void THPSLevel::tHPS3ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
  OpenLoadSceneDialog("THPS3 BSP (*.bsp)\0*.bsp\0\0");
}

void ClearObjectList()
{
  THPSLevel::listBox1->ClearSelected();
  THPSLevel::MaterialList->Nodes->Clear();
}

void SelectedAlreadySelected(DWORD index)
{
  if (heldCntrl)
    THPSLevel::listBox1->SetSelected(index, false);
}

Material* THPSLevel::GetSelectedMaterial()
{
  Mesh* tmpMesh = NULL;
  if (MaterialList->SelectedNode)
  {
    DWORD index = MaterialList->SelectedNode->Index;
    DWORD count = 0;
    Mesh* tmpMesh = NULL;
    for (DWORD i = 0; i < selectedObjects.size(); i++)
    {
      if (index < selectedObjects[i].mesh->GetNumSplits() + count)
      {
        tmpMesh = selectedObjects[i].mesh;
      }
      count += selectedObjects[i].mesh->GetNumSplits();
    }
    if (tmpMesh)
    {
      return globalMaterialList->GetMaterial(tmpMesh->GetSplit(index)->matId);
    }
  }

  return NULL;
}

System::Void THPSLevel::transparent_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
  Material* tmpMat = GetSelectedMaterial();
  if (tmpMat)
    tmpMat->transparent = transparent->Checked;
}

System::Void THPSLevel::draworder_TextChanged(System::Object^  sender, System::EventArgs^  e) {
  Material* tmpMat = GetSelectedMaterial();
  bool ex = false;
  if (tmpMat)
  {
    float temp = 0.0f;
    try {
      temp = Convert::ToSingle(draworder->Text);
    }
    catch (FormatException^) {
      MessageBox::Show("FormatException");
      ex = true;
    }
    catch (OverflowException^) {
      MessageBox::Show("OverflowException");
      ex = true;
    }
    catch (ArgumentNullException^) {
    }
    if (!ex)
      tmpMat->drawOrder = temp;
  }
}

System::Void THPSLevel::uvel_TextChanged(System::Object^  sender, System::EventArgs^  e) {
  Material* tmpMat = GetSelectedMaterial();
  bool ex = false;
  if (tmpMat)
  {
    Animation* anim = scene->GetAnimation(tmpMat->GetMatId());
    if (anim)
    {
      float temp = 0.0f;
      try {
        temp = Convert::ToSingle(uvel->Text);
      }
      catch (FormatException^) {
        MessageBox::Show("FormatException");
        ex = true;
      }
      catch (OverflowException^) {
        MessageBox::Show("OverflowException");
        ex = true;
      }
      catch (ArgumentNullException^) {
      }
      if (!ex)
        anim->UVel = temp;
    }
  }
}

System::Void THPSLevel::water_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
  if (water->Checked)
  {
    linear->Checked = false;
    Material* tmpMat = GetSelectedMaterial();
    if (tmpMat)
    {
      Animation* anim = scene->GetAnimation(tmpMat->GetMatId());
      if (anim)
      {
        *anim = Animation(0);
        uvel->Text = anim->UVel.ToString();
        vvel->Text = anim->VVel.ToString();
      }
      else
      {
        scene->AddAnimation(tmpMat->GetMatId(), 0);
        numAnimations++;
        forceRedraw = true;
        animations = scene->GetAnimations();
      }
    }
  }
}

System::Void THPSLevel::linear_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
  if (linear->Checked)
  {
    water->Checked = false;
    Material* tmpMat = GetSelectedMaterial();
    if (tmpMat)
    {
      Animation* anim = scene->GetAnimation(tmpMat->GetMatId());
      if (anim)
      {
        *anim = Animation(1);
        uvel->Text = anim->UVel.ToString();
        vvel->Text = anim->VVel.ToString();
      }
      else
      {
        scene->AddAnimation(tmpMat->GetMatId(), 1);
        numAnimations++;
        forceRedraw = true;
        animations = scene->GetAnimations();
      }
    }
  }
}

System::Void THPSLevel::vvel_TextChanged(System::Object^  sender, System::EventArgs^  e) {
  Material* tmpMat = GetSelectedMaterial();
  bool ex = false;
  if (tmpMat)
  {
    Animation* anim = scene->GetAnimation(tmpMat->GetMatId());
    if (anim)
    {
      float temp = 0.0f;
      try {
        temp = Convert::ToSingle(vvel->Text);
      }
      catch (FormatException^) {
        MessageBox::Show("FormatException");
        ex = true;
      }
      catch (OverflowException^) {
        MessageBox::Show("OverflowException");
        ex = true;
      }
      catch (ArgumentNullException^) {
      }
      if (!ex)
        anim->VVel = temp;
    }
  }
}
vector<WORD> alreadyScaled;
bool AlreadyScaled(DWORD index)
{
  for (DWORD i = 0; i < alreadyScaled.size(); i++)
  {
    if (alreadyScaled[i] == index)
      return true;
  }
  return false;
}




System::Void THPSLevel::uSlider_Scroll(System::Object^ /*sender*/, System::EventArgs^ /*e*/)
{
  Material* tmpMat = GetSelectedMaterial();

  if (tmpMat)
  {
    for (DWORD i = 0; i < selectedObjects.size(); i++)
    {
      Mesh* mesh = selectedObjects[i].mesh;
      Vertex* vertices = (Vertex*)mesh->GetVertices();
      for (DWORD j = 0; j < mesh->matSplits.size(); j++)
      {
        if (tmpMat->GetMatId() == scene->matList.GetMaterial(mesh->matSplits[j].matId)->GetMatId())
        {
          for (DWORD k = 0; k < mesh->matSplits[j].numIndices; k++)
          {
            if (!AlreadyScaled(k))
            {
              alreadyScaled.push_back(k);
              vertices[mesh->matSplits[j].Indices[k]].tUV[0].u*(float)THPSLevel::uSlider->Value;
            }
          }
        }
      }
      mesh->ReloadVertexBuffer(Device, D3DFVF_XYZ | D3DFVF_DIFFUSE | MAX_TEXCOORDS);
    }
    alreadyScaled.clear();
    THPSLevel::vSlider->Value = 1;
  }
}
System::Void THPSLevel::vSlider_Scroll(System::Object^ /*sender*/, System::EventArgs^ /*e*/)
{
  Material* tmpMat = GetSelectedMaterial();

  if (tmpMat)
  {
    for (DWORD i = 0; i < selectedObjects.size(); i++)
    {
      Mesh* mesh = selectedObjects[i].mesh;
      Vertex* vertices = (Vertex*)mesh->GetVertices();
      for (DWORD j = 0; j < mesh->matSplits.size(); j++)
      {
        if (tmpMat->GetMatId() == scene->matList.GetMaterial(mesh->matSplits[j].matId)->GetMatId())
        {
          for (DWORD k = 0; k < mesh->matSplits[j].numIndices; k++)
          {
            if (!AlreadyScaled(k))
            {
              alreadyScaled.push_back(k);
              vertices[mesh->matSplits[j].Indices[k]].tUV[0].v*(float)THPSLevel::vSlider->Value;
            }
          }
        }
      }
    }
    alreadyScaled.clear();
    THPSLevel::vSlider->Value = 1;
  }
}

System::Void THPSLevel::animated_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
  Material* tmpMat = GetSelectedMaterial();
  if (tmpMat)
    tmpMat->animated = animated->Checked;
  animgroup->Enabled = animated->Checked;
}

System::Void THPSLevel::doubled_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
  Material* tmpMat = GetSelectedMaterial();
  if (tmpMat)
    tmpMat->doubled = doubled->Checked;
}

System::Void THPSLevel::matList_checkChanged(System::Object^  sender, System::Windows::Forms::TreeViewEventArgs ^  e)
{
  Material* tmpMat = GetSelectedMaterial();
  if (tmpMat)
  {
    char imageName[MAX_PATH];
    static char tmp[25];
    GetEnvironmentVariable("TEMP", imageName, MAX_PATH);
    DWORD texId = tmpMat->GetTexture(0)->GetTexId();
    Checksum textureId = *(Checksum*)&texId;

    GetEnvironmentVariable("TEMP", imageName, MAX_PATH);
    char* name = textureId.GetString();
    if (name == NULL)
    {
      sprintf(tmp, "\\%08X.dds", textureId.checksum);
      strcat(imageName, tmp);
    }
    else
    {
      DWORD len = strlen(imageName);
      imageName[len] = '\\';
      len++;
      strcpy(&imageName[len], name);
      strcat(imageName, ".dds");
    }
    FILE*f = fopen(imageName, "rb");
    if (f)
    {
      LoadPicture(imageName);
      RenderImage(pictureHandle);
    }
    else
    {
      GetEnvironmentVariable("TEMP", imageName, MAX_PATH);
      if (name == NULL)
      {
        sprintf(tmp, "\\%08X.tga\0", textureId.checksum);
        strcat(imageName, tmp);
      }
      else
      {
        DWORD len = strlen(imageName);
        imageName[len] = '\\';
        len++;
        strcpy(&imageName[len], name);
        strcat(imageName, ".tga");
      }
      f = fopen(imageName, "rb");
      if (f)
      {
        LoadPicture(imageName);
        RenderImage(pictureHandle);
      }
    }
    THPSLevel::transparent->Checked = tmpMat->transparent;
    THPSLevel::doubled->Checked = tmpMat->doubled;
    THPSLevel::draworder->Text = gcnew String(tmpMat->drawOrder.ToString());
    THPSLevel::animated->Checked = tmpMat->animated;
    if (animated->Checked)
    {
      Animation* anim = scene->GetAnimation(tmpMat->GetMatId());
      if (anim)
      {
        uvel->Text = anim->UVel.ToString();
        vvel->Text = anim->VVel.ToString();
        if (anim->GetType() == 0)
          water->Checked = true;
        else if (anim->GetType() == 1)
          linear->Checked = true;
        else
        {
          water->Checked = false;
          linear->Checked = false;
        }
      }
    }
  }
}

System::Void THPSLevel::checkedListBox1_itemCheck(System::Object^  sender, System::Windows::Forms::ItemCheckEventArgs ^  e)
{
  for (DWORD i = 0; i < selectedTriangles.size(); i++)
  {
    if (selectedTriangles[i].collision)
    {
      int index = e->Index;
      switch (index)
      {
      case 0:
        if (e->CurrentValue == System::Windows::Forms::CheckState::Unchecked)
          selectedTriangles[i].collision->flags |= 0x1;
        else
          selectedTriangles[i].collision->flags &= ~0x1;
        break;
      case 1:
        if (e->CurrentValue == System::Windows::Forms::CheckState::Unchecked)
          selectedTriangles[i].collision->flags |= 0x4;
        else
          selectedTriangles[i].collision->flags &= ~0x4;
        break;
      case 2:
        if (e->CurrentValue == System::Windows::Forms::CheckState::Unchecked)
          selectedTriangles[i].collision->flags |= 0x8;
        else
          selectedTriangles[i].collision->flags &= ~0x8;
        break;
      case 3:
        if (e->CurrentValue == System::Windows::Forms::CheckState::Unchecked)
          selectedTriangles[i].collision->flags &= ~0x10;
        else
          selectedTriangles[i].collision->flags |= 0x10;
        break;
      case 4:
        if (e->CurrentValue == System::Windows::Forms::CheckState::Unchecked)
          selectedTriangles[i].collision->flags |= 0x200;
        else
          selectedTriangles[i].collision->flags &= ~0x200;
        break;
      case 5:
        if (e->CurrentValue == System::Windows::Forms::CheckState::Unchecked)
          selectedTriangles[i].collision->flags |= 0x40;
        else
          selectedTriangles[i].collision->flags &= ~0x40;
        break;
      }
    }
  }
}

bool editScript = false;
HWND hEdit;
HFONT g_hfText;

System::Void THPSLevel::triggerButton1_Click(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
{
  using namespace std;
  using namespace System;
  using namespace System::Runtime::InteropServices;

  if (editScript)
  {
    THPSLevel::panel6->Focus();
    disableKeys = false;
    render = true;

    if (THPSLevel::comboBox2->Text->CompareTo("None"))
    {
      IntPtr triggerName = Marshal::StringToHGlobalAnsi(THPSLevel::comboBox2->Text);
      char* TriggerName = static_cast<char*>(triggerName.ToPointer());

      KnownScript* trigger = scene->GetScript(TriggerName);
      if (trigger)
      {
        char text[300000];
        GetWindowText(hEdit, text, 300000);
        trigger->Compile(text);
      }
    }

    buffer.clear();
    editScript = false;
    DestroyWindow(hEdit);
    DeleteObject(g_hfText);
    if (!forceRedraw)
      DrawFrame();
  }
  else
  {
    THPSLevel::triggerButton1->Focus();
    disableKeys = true;
    render = false;

    editScript = true;
    HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(handleWindow, GWLP_HINSTANCE);
    hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "edit", "",//script->buffer.c_str(),
      WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_LEFT | ES_MULTILINE | ES_AUTOHSCROLL | WS_VSCROLL | WS_HSCROLL,
      0, 0, this->panel6->Size.Width/*1120*/, this->panel6->Size.Height/*650*/,
      handleWindow, (HMENU)102,
      hInstance, NULL);

    HDC hdcScreen = GetDC(HWND_DESKTOP);

    g_hfText = CreateFont(-MulDiv(11, GetDeviceCaps(hdcScreen, LOGPIXELSY), 72), // 11pt
      0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS,
      CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, TEXT("courier"));

    ReleaseDC(HWND_DESKTOP, hdcScreen);



    SendMessage(hEdit, WM_SETFONT, (WPARAM)g_hfText, FALSE);

    SendMessage(hEdit, EM_LIMITTEXT, 0, 0);
    SetFocus(hEdit
      );


    if (THPSLevel::comboBox2->Text->CompareTo("None"))
    {
      IntPtr triggerName = Marshal::StringToHGlobalAnsi(THPSLevel::comboBox2->Text);
      char* TriggerName = static_cast<char*>(triggerName.ToPointer());

      KnownScript* trigger = scene->GetScript(TriggerName);
      if (trigger)
        SetWindowText(hEdit, trigger->Decompile());
      buffer.clear();
    }
  }
}

void UpdateCollisionInfo()
{
  extern CollisionResult lastHit;
  if (lastHit.mesh)
  {
    Collision* collision = lastHit.mesh->GetCollision();
    if (collision)
    {
      THPSLevel::numericUpDown1->Value = System::Decimal((unsigned int)lastHit.triIndex);
      THPSLevel::checkedListBox1->SetItemChecked(0, collision->GetFaceInfo(lastHit.triIndex)->flags & 0x1);
      THPSLevel::checkedListBox1->SetItemChecked(1, collision->GetFaceInfo(lastHit.triIndex)->flags & 0x4);
      THPSLevel::checkedListBox1->SetItemChecked(2, collision->GetFaceInfo(lastHit.triIndex)->flags & 0x8);
      THPSLevel::checkedListBox1->SetItemChecked(3, !(collision->GetFaceInfo(lastHit.triIndex)->flags & 0x10));
      THPSLevel::checkedListBox1->SetItemChecked(4, collision->GetFaceInfo(lastHit.triIndex)->flags & 0x0200);
      THPSLevel::checkedListBox1->SetItemChecked(5, collision->GetFaceInfo(lastHit.triIndex)->flags & 0x40);
      if (collision->GetFaceInfo(lastHit.triIndex)->flags & 0x40)
      {
        Node* node = scene->GetNode(lastHit.mesh->GetName());
        if (node)
        {
          if (node->Trigger.checksum)
            THPSLevel::comboBox2->Text = gcnew String(node->Trigger.GetString());
          else
            THPSLevel::comboBox2->Text = L"None";
        }
        else
          THPSLevel::comboBox2->Text = L"None";
      }
      else
        THPSLevel::comboBox2->Text = L"None";
    }
    else
    {
      THPSLevel::numericUpDown1->Value = System::Decimal((unsigned int)lastHit.triIndex);
      THPSLevel::comboBox2->Text = L"None";
      THPSLevel::checkedListBox1->SetItemChecked(0, false);
      THPSLevel::checkedListBox1->SetItemChecked(1, false);
      THPSLevel::checkedListBox1->SetItemChecked(2, false);
      THPSLevel::checkedListBox1->SetItemChecked(3, false);
      THPSLevel::checkedListBox1->SetItemChecked(4, false);
      THPSLevel::checkedListBox1->SetItemChecked(5, false);
    }
  }
  else
  {
    THPSLevel::numericUpDown1->Value = System::Decimal((unsigned int)lastHit.triIndex);
    THPSLevel::comboBox2->Text = L"None";
    THPSLevel::checkedListBox1->SetItemChecked(0, false);
    THPSLevel::checkedListBox1->SetItemChecked(1, false);
    THPSLevel::checkedListBox1->SetItemChecked(2, false);
    THPSLevel::checkedListBox1->SetItemChecked(3, false);
    THPSLevel::checkedListBox1->SetItemChecked(4, false);
    THPSLevel::checkedListBox1->SetItemChecked(5, false);
  }
}

void SelectMaterial(DWORD meshIndex)
{
  if (!heldCntrl)
  {
    selectedMaterials.clear();
    THPSLevel::listBox1->ClearSelected();
    if (updateMaterial)
      THPSLevel::MaterialList->Nodes->Clear();
  }

  THPSLevel::listBox1->SetSelected(meshIndex, true);
  if (updateMaterial)
  {
    extern CollisionResult lastHit;
    if (lastHit.mesh)
    {
      Mesh* tmpMesh = (Mesh*)lastHit.mesh;
      DWORD matId = tmpMesh->GetSplit(lastHit.matIndex)->matId;
      Material* tmpMat = globalMaterialList->GetMaterial(matId);
      if (!tmpMat)
      {
        matId = globalMaterialList->GetNumMaterials();
        matId--;
        tmpMesh->GetSplit(lastHit.matIndex)->matId = matId;
        tmpMat = globalMaterialList->GetMaterial(matId);
      }
      TreeNode^ tn;
      if (tmpMat)
      {
        matId = tmpMat->GetMatId();
        tn = gcnew TreeNode(/*index.ToString() + "[" + */matId.ToString("X")/* + "]"*/);
        for (DWORD j = 0; j < tmpMat->GetNumTextures(); j++)
        {
          Texture* tmpTex = tmpMat->GetTexture(j);
          DWORD texId = tmpTex->GetTexId();
          char* name = (*(Checksum*)&texId).GetString();
          if (!name)
          {
            char nm[25];
            name = nm;
            sprintf(name, "%08X", texId);
          }
          tn->Nodes->Add(gcnew String(name));
        }

        THPSLevel::MaterialList->Nodes->Add(tn);
        THPSLevel::MaterialList->SelectedNode = THPSLevel::MaterialList->Nodes[0];
        char imageName[MAX_PATH];
        static char tmp[25];
        GetEnvironmentVariable("TEMP", imageName, MAX_PATH);
        DWORD texId = tmpMat->GetTexture(0)->GetTexId();
        Checksum textureId = *(Checksum*)&texId;

        GetEnvironmentVariable("TEMP", imageName, MAX_PATH);
        char* name = textureId.GetString();
        if (name == NULL)
        {
          sprintf(tmp, "\\%08X.dds", textureId);
          strcat(imageName, tmp);
        }
        else
        {
          DWORD len = strlen(imageName);
          imageName[len] = '\\';
          len++;
          strcpy(&imageName[len], name);
          strcat(imageName, ".dds");
        }
        FILE*f = fopen(imageName, "rb");
        if (f)
        {
          LoadPicture(imageName);
          RenderImage(pictureHandle);
        }
        else
        {
          GetEnvironmentVariable("TEMP", imageName, MAX_PATH);
          if (name == NULL)
          {
            sprintf(tmp, "\\%08X.tga\0", textureId);
            strcat(imageName, tmp);
          }
          else
          {
            DWORD len = strlen(imageName);
            imageName[len] = '\\';
            len++;
            strcpy(&imageName[len], name);
            strcat(imageName, ".tga");
          }
          f = fopen(imageName, "rb");
          if (f)
          {
            LoadPicture(imageName);
            RenderImage(pictureHandle);
          }
        }

      }
      else
      {
        tn = gcnew TreeNode(/*index.ToString() + */"[no mat]");
        THPSLevel::MaterialList->Nodes->Add(tn);
        THPSLevel::MaterialList->SelectedNode = THPSLevel::MaterialList->Nodes[0];
      }

    }

  }
}

void SelectObject(DWORD index)
{
  if (!heldCntrl)
  {
    THPSLevel::listBox1->ClearSelected();
    if (updateMaterial)
      THPSLevel::MaterialList->Nodes->Clear();
  }

  THPSLevel::listBox1->SetSelected(index, true);
  Mesh* tmpMesh = scene->GetMesh(index);
  if (updateMaterial)
  {
    if (tmpMesh)
    {
      DWORD numSplits = tmpMesh->GetNumSplits();

      for (DWORD i = 0; i < numSplits; i++)
      {
        DWORD matId = tmpMesh->GetSplit(i)->matId;

        Material* tmpMat = globalMaterialList->GetMaterial(matId);
        TreeNode^ tn;
        if (tmpMat)
        {
          matId = tmpMat->GetMatId();
          tn = gcnew TreeNode(/*index.ToString() + "[" + */matId.ToString("X")/* + "]"*/);
          for (DWORD j = 0; j < tmpMat->GetNumTextures(); j++)
          {
            Texture* tmpTex = tmpMat->GetTexture(j);
            DWORD texId = tmpTex->GetTexId();
            char* name = (*(Checksum*)&texId).GetString();
            if (!name)
            {
              char nm[25];
              name = nm;
              sprintf(name, "%08X", texId);
            }
            tn->Nodes->Add(gcnew String(name));
          }
        }
        else
          tn = gcnew TreeNode(/*index.ToString() + */"[no mat]");
        THPSLevel::MaterialList->Nodes->Add(tn);
      already:;
      }
      if (numSplits)
        THPSLevel::MaterialList->SelectedNode = THPSLevel::MaterialList->Nodes[0];
    }
  }
}
#pragma unmanaged
void SelectObjectOnly(Mesh* mesh, bool multiselected)
{
  if (!multiselected)
    selectedObjects.clear();

  selectedObjects.push_back(mesh);
}
#pragma managed
System::Void THPSLevel::LostFocus(System::Object^  sender, System::EventArgs^  e) {
  if (!FPS)
    render = false;
  //MessageBox::Show("Lost");
}

System::Void THPSLevel::GotFocus(System::Object^  sender, System::EventArgs^  e) {
  if (!FPS)
  {
    if (!loading && scene)
      render = true;
    if (!forceRedraw)
      DrawFrame();
  }
}

#pragma unmanaged

SelectedObject::SelectedObject(Mesh* mesh, bool e)
{
  this->mesh = mesh;
  if (e)
    events.push_back(Event(Event::eSelection, selectedObjects));
}

#pragma managed
System::Void THPSLevel::listBox1_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
  listBox2->Items->Clear();
  if (listBox1->SelectedIndex >= 0 && listBox1->SelectedIndex < scene->GetNumMeshes())
    propertyGrid1->SelectedObject = properties[listBox1->SelectedIndex];
  else
    propertyGrid1->SelectedObject = 0;
  if (selectionMode == 0)
  {
    selectedObjects.clear();
    Mesh* tmpMesh = NULL;
    for (int x = 0; x < listBox1->SelectedIndices->Count; x++)
    {
      tmpMesh = scene->GetMesh((DWORD)listBox1->SelectedIndices[x]);
      selectedObjects.push_back(SelectedObject(tmpMesh));
    }

  }
  else if (selectionMode == 3)
  {
    if (propertyGrid1->SelectedObject != 0)
    {
      extern CollisionResult lastHit;
      Mesh* mesh = (Mesh*)lastHit.mesh;
      selectedMaterials.push_back(SelectedMaterial(mesh, mesh->GetSplit(lastHit.matIndex)));
    }
  }

}

void THPSLevel::SetDeviceStates()
{
  rct.left = 10;
  rct.right = 1010;
  rct.top = 10;
  rct.bottom = 90;
  if (m_font)
    m_font->Release();
  D3DXCreateFontA(Device, 25, 0, FW_NORMAL, 0, FALSE, DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &m_font);
  Device->SetRenderState(D3DRS_ZENABLE, TRUE);
  Device->SetRenderState(D3DRS_LIGHTING, FALSE);
  Device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
  Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
  if (scene)
    Device->SetRenderState(D3DRS_CULLMODE, scene->cullMode);
  else
    Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
  Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
  Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
  Device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

  Device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
  Device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | MAX_TEXCOORDS);
  Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
  Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

  Device->SetRenderState(D3DRS_BLENDFACTOR, D3DXCOLOR(255, 0, 0, 255));

  Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
  Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
  Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
  Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

  static const float mipLodBias = -1.0;
  Device->SetSamplerState(0, D3DSAMP_MIPMAPLODBIAS, *((DWORD*)&mipLodBias));
  D3DCAPS9 caps;
  Device->GetDeviceCaps(&caps);
  Device->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, caps.MaxAnisotropy);

  Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

  if (scene)
  {
    if (scene->GetBlendMode() == 1)
    {
      Device->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(128, 30, 30, 30));
      Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

      Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    }
    else
    {
      Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE2X);
      Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    }
  }
  else
  {
    Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
  }
  Device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
  Device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
  Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
  Device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

  Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);

  Device->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);

  Device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
  Device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
  Device->SetSamplerState(0, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);

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

  if (hr == D3DERR_DEVICENOTRESET)
  {
    Device->Reset(&DevParams);
    deviceLost = false;
  }
}

void THPSLevel::SafeTurnOffRendering()
{
  render = false;
  while (rendering)
    Sleep(10);
}
bool changingMode = false;

System::Void THPSLevel::surfaceModeToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
  toolStripSplitButton1->Text = "Face Mode";

  selectionMode = 1;
  this->tabControl1->SelectedIndex = 2;
  selectedObjects.clear();
  selectedMaterials.clear();
  ClearObjectList();
  changingMode = true;
}
System::Void THPSLevel::railModeToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
  toolStripSplitButton1->Text = "Node Mode";

  selectionMode = 2;
  this->tabControl1->SelectedIndex = 1;
  changingMode = true;
}
System::Void THPSLevel::materialToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
{
  toolStripSplitButton1->Text = "Material Mode";

  selectionMode = 3;
  this->tabControl1->SelectedIndex = 3;
  selectedObjects.clear();
  selectedMaterials.clear();
  ClearObjectList();
  updateMaterial = true;
  changingMode = true;
}
System::Void THPSLevel::objectModeToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
  toolStripSplitButton1->Text = "Object Mode";

  selectionMode = 0;
  this->tabControl1->SelectedIndex = 0;
  selectedTriangles.clear();
  ClearObjectList();
  updateMaterial = true;
  changingMode = true;
}
bool disableKeys = false;
bool FPS = false;
LRESULT CALLBACK WndProcc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

  int wmId, wmEvent;

  switch (message)
  {
  case WM_ACTIVATEAPP:
    if (wParam == TRUE)
      render = true;
    else
      render = false;
    break;

  case WM_KEYDOWN:
    switch (wParam)
    {
    case VK_ESCAPE:
      DestroyWindow(hWnd);
      break;
    }
    break;

  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}

System::Void THPSLevel::FKeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
  if (disableKeys)
  {
    e->SuppressKeyPress = false;
  }
  else
  {
    if (e->KeyCode == Keys::C)
    {
      if (listBox1->SelectedIndex >= 0)
      {
        using namespace std;
        using namespace System;
        using namespace System::Runtime::InteropServices;
        IntPtr name = Marshal::StringToHGlobalAnsi(properties[listBox1->SelectedIndex]->Name);
        char* Name = static_cast<char*>(name.ToPointer());
        OpenClipboard(GetDesktopWindow());
        EmptyClipboard();
        DWORD len = strlen(Name);
        HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, len + 1);
        if (!hg){
          CloseClipboard();
          return;
        }
        memcpy(GlobalLock(hg), Name, len + 1);
        GlobalUnlock(hg);
        SetClipboardData(CF_TEXT, hg);
        CloseClipboard();
        GlobalFree(hg);
      }
    }
    if (e->KeyCode == Keys::P)
    {
      render = false;

      D3DDISPLAYMODE mode;
      d3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode);
      DevParams.BackBufferWidth = mode.Width;
      DevParams.BackBufferHeight = mode.Height;

      HINSTANCE hInstance = GetModuleHandle(NULL);

      HWND hWnd = CreateWindowEx(NULL,
        szWindowClass,
        "",
        WS_OVERLAPPEDWINDOW,
        0, 0,
        mode.Width, mode.Height,    // set window to new resolution
        NULL,
        NULL,
        hInstance,
        NULL);
      if (!hWnd)
      {
        ::MessageBox(0, "Couldn't Create Window", "", MB_OK);
        return;
      }
      handleWindow = hWnd;
      SetWindowLong(hWnd, GWL_STYLE, WS_POPUPWINDOW);

      SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0,

        SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
      DevParams.hDeviceWindow = handleWindow;
      CleanupInputDevices();
#ifndef GWL_HINSTANCE
      InitInputDevices((HINSTANCE)GetWindowLongPtr(handleWindow, GWLP_HINSTANCE), handleWindow);
#else
      InitInputDevices((HINSTANCE)GetWindowLong(handleWindow, GWL_HINSTANCE), handleWindow);
#endif
      port.Height = mode.Height;
      port.Width = mode.Width;

      Device->SetViewport(&port);
      Device->TestCooperativeLevel();

      HRESULT result = Device->ResetEx(&DevParams, NULL);
      if (result != D3D_OK)
      {
        if (m_font)
          m_font->Release();
        m_font = NULL;
        if (texture)
          texture->Release();
        if (image)
          image->Release();
        ReleaseTextures();
        scene->ReleaseBuffers();

        result = Device->Reset(&DevParams);
        if (result != D3D_OK)
          MessageBox::Show("WTF?");
        scene->CreateTextures();
        scene->CreateBuffers(Device, D3DFVF_XYZ | D3DFVF_DIFFUSE | MAX_TEXCOORDS);
        D3DXCreateFontA(Device, 25, 0, FW_NORMAL, 0, FALSE, DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &m_font);
      }

      FPS = true;
      Camera oldCamera = *camera;
      this->Visible = false;
      this->Enabled = false;

      imageChain->Release();

      Device->GetViewport(&port);
      CreateCamera();
      for (DWORD i = 0; i < scene->nodes.size(); i++)
      {
        if (scene->nodes[i].Class.checksum == Restart::GetClass())
        {
          player->position.x = scene->nodes[i].Position.x;
          player->position.y = scene->nodes[i].Position.y + 1.0f;
          player->position.z = scene->nodes[i].Position.z;
          player->state = OnGround;
          camera->lookAt(camera->pos(), player->position, D3DXVECTOR3(0.0f, 1.0f, 0.0f));
          extern bool jumping;
          jumping = false;
          extern bool jumped;
          jumped = false;
          extern bool spaceDown;
          spaceDown = false;
          break;
        }
      }
      render = true;
      MSG msg = MSG();
      while (msg.message != WM_QUIT)
      {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
          TranslateMessage(&msg);
          DispatchMessage(&msg);
          // }
        }
        else
        {
          UpdateFPS();
        }
      }
      FPS = false;
      handleWindow = static_cast<HWND>(this->panel6->Handle.ToPointer());
      DevParams.hDeviceWindow = handleWindow;
      CleanupInputDevices();
#ifndef GWL_HINSTANCE
      InitInputDevices((HINSTANCE)GetWindowLongPtr(handleWindow, GWLP_HINSTANCE), handleWindow);
#else
      InitInputDevices((HINSTANCE)GetWindowLong(handleWindow, GWL_HINSTANCE), handleWindow);
#endif

      DevParams.BackBufferWidth = panel6->Size.Width;
      DevParams.BackBufferHeight = panel6->Size.Height;
      DevParams.Windowed = TRUE;
      //DevParams.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
      port.Width = panel6->Size.Width;
      port.Height = panel6->Size.Height;

      Device->SetViewport(&port);
      Device->TestCooperativeLevel();
      result = Device->ResetEx(&DevParams, NULL);
      if (result != D3D_OK)
        MessageBox::Show("WTF?");
      Device->GetViewport(&port);
      *camera = oldCamera;
      CreateCamera();
      DevParams.hDeviceWindow = pictureHandle;
      imagePort.Width = pictureBox1->Size.Width;
      imagePort.Height = pictureBox1->Size.Height;
      Device->CreateAdditionalSwapChain(&DevParams, &imageChain);

      DevParams.hDeviceWindow = handleWindow;
      render = true;

      this->Enabled = true;
      this->Visible = true;
      panel6->Focus();
    }
    if (e->KeyCode == Keys::Z)
    {
      if (camera)
      {
        resetMouse = !resetMouse;
        if (resetMouse && render)
        {
          POINT mouse;
          DWORD middleX = port.Width / 2;
          DWORD middleY = port.Height / 2;
          mouse.x = middleX;
          mouse.y = middleY;
          ClientToScreen(handleWindow, &mouse);
          SetCursorPos(mouse.x, mouse.y);
          this->panel6->Focus();
        }
      }
    }
    else if (e->KeyCode == Keys::Escape)//(System::Windows::Forms::Keys)27)
      this->Close();
  }
}

System::Void THPSLevel::FKeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) {
  if (disableKeys)
  {
    e->KeyChar = 0;
    e->Handled = true;
  }
}

System::Void THPSLevel::toolStripButton1_Click(System::Object^  sender, System::EventArgs^  e)
{
  for (DWORD i = 0; i < MAX_PASSES; i++) Device->SetTexture(i, NULL);

  showTextures = !showTextures;
  Device->SetRenderState(D3DRS_ALPHAREF, 128);
  Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
  if (showTextures)
    Device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | MAX_TEXCOORDS);
  else
    Device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
  if (!forceRedraw)
    DrawFrame();
}

#pragma unmanaged

void DeleteSelectedNodes()
{
  DWORD numSelectedNodes = selectedNodes.size();
  if (numSelectedNodes)
  {
    WORD* deletedNodes = new WORD[numSelectedNodes];
    Node* nodes = &scene->nodes.front();
    bool repopulateSpawns = false;
    bool repopulateRails = false;
    for (DWORD i = 0; i < numSelectedNodes; i++)
    {
      deletedNodes[i] = ((DWORD)selectedNodes[i] - (DWORD)nodes) / sizeof(Node);
      if (selectedNodes[i]->Class.checksum == 0x1806DDF8)
      {
        for (DWORD j = 0; j < spawns.size(); j++)
        {
          if (spawns[j]->Name.checksum == selectedNodes[i]->Name.checksum)
          {
            spawns.erase(spawns.begin() + j);
            break;
          }
        }
      }
    }
    for (DWORD i = 0; i < numSelectedNodes; i++)
    {
      scene->DeleteNode(deletedNodes[i]);
      repopulateRails = true;
      repopulateSpawns = true;
    }
    delete deletedNodes;
    selectedNodes.clear();

    if (repopulateSpawns)
    {
      DWORD numSpawns = 0;
      for (DWORD i = 0; i < scene->nodes.size(); i++)
      {
        if (scene->nodes[i].Class.checksum == Restart::GetClass())
        {
          spawns[numSpawns] = &scene->nodes[i];
          numSpawns++;
        }
      }
    }

    if (repopulateRails)
    {
      rails.clear();
      DWORD numNodes = scene->nodes.size();
      for (DWORD i = 0; i < numNodes; i++)
      {
        if (nodes[i].Class.checksum == RailNode::GetClass())
        {
          if (!nodes[i].Links.size())
          {
            rails.push_back(RenderableRail());
            AddRails(i);
          }
        }
      }

      for (DWORD i = 0; i < numNodes; i++)
      {
        if (nodes[i].Class.checksum == RailNode::GetClass() && nodes[i].Links.size() && !nodes[i].IsAdded())
        {
          rails.push_back(RenderableRail());
          AddLinked(i);
        }
      }


      for (DWORD i = 0, numRails = rails.size(); i < numRails; i++)
      {
        const DWORD numVerts = rails[i].vertices.size();

        if (numVerts != 0)
        {
          const DWORD numIndices = rails[i].Indices.size();
          rails[i].numVerts = numVerts;
          rails[i].numIndices = numIndices / 3;
          if (FAILED(Device->CreateVertexBuffer(numVerts*sizeof(ColouredVertex), D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &rails[i].verts, NULL)))
            ::MessageBox(0, "Error", "failed to create vertex buffer", 0);


          ColouredVertex* pVertices;
          rails[i].verts->Lock(0, 0, (void**)&pVertices, 0);
          memcpy(pVertices, &rails[i].vertices.front(), numVerts*sizeof(ColouredVertex));
          rails[i].verts->Unlock();
          rails[i].vertices.clear();


          Device->CreateIndexBuffer(numIndices * 2, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &rails[i].indices, NULL);


          WORD* indices;
          rails[i].indices->Lock(0, 0, (void**)&indices, 0);
          memcpy(indices, &rails[i].Indices.front(), numIndices * 2);
          rails[i].indices->Unlock();
          rails[i].Indices.clear();
        }
        else
          rails[i].Indices.clear();
      }
    }
    if (!forceRedraw)
      DrawFrame();
  }
}

float
DistanceBetweenLines(D3DXVECTOR3& origin0, D3DXVECTOR3& direction0, D3DXVECTOR3& origin1, D3DXVECTOR3& direction1)
{
  D3DXVECTOR3   u = direction0 - origin0;
  D3DXVECTOR3   v = direction1 - origin1;
  D3DXVECTOR3   w = origin0 - origin1;
  const float    a = D3DXVec3Dot(&u, &u);        // always >= 0
  const float    b = D3DXVec3Dot(&u, &v);
  const float    c = D3DXVec3Dot(&v, &v);        // always >= 0
  const float    d = D3DXVec3Dot(&u, &w);
  const float    e = D3DXVec3Dot(&v, &w);
  const float    D = a*c - b*b;       // always >= 0
  static float    sc, tc;

  // compute the line parameters of the two closest points
  if (D < 0.00000001f) {         // the lines are almost parallel
    sc = 0.0;
    tc = (b>c ? d / b : e / c);   // use the largest denominator
  }
  else {
    sc = (b*e - c*d) / D;
    tc = (a*e - b*d) / D;
  }

  // get the difference of the two closest points
  D3DXVECTOR3   dP = w + (sc * u) - (tc * v);  // = L1(sc) - L2(tc)

  return sqrtf(D3DXVec3Dot(&dP, &dP));   // return the closest distance
}

void SelectNode()
{
  if (!heldCntrl)
    selectedNodes.clear();
  const DWORD numNodes = scene->nodes.size();
  Node* nodes = &scene->nodes.front();
  D3DXMATRIX nodeRotation;
  D3DXMATRIX nodeTranslation;

  D3DXVECTOR3 rayOrigin, rayDirection, translatedOrigin, translatedDirection;
  static float distance = 0.0f;
  static float closest = 0.0f;
  bool intersected = false;
  DWORD nodeIndex = 0;
  static DWORD numLinks = 0;
  static const D3DXMATRIX* matWorld = scene->GetWorldMatrix();
  D3DXMATRIX world, invWorld;
  GetMouseRay(&rayDirection, &rayOrigin);
  POINT Mouse;
  GetCursorPos(&Mouse);
  ScreenToClient(handleWindow, &Mouse);
  D3DXVECTOR3 hitPoint;
  DWORD tmp = defaultCulling;
  defaultCulling = D3DCULL_NONE;

  for (DWORD i = 0; i < numNodes; i++)
  {
    if (nodes[i].Class.checksum == 0x1806DDF8)
    {
      if (showSpawns)
      {
        D3DXMatrixRotationYawPitchRoll(&nodeRotation, -nodes[i].Angles.y + D3DX_PI, nodes[i].Angles.x, nodes[i].Angles.z);
        D3DXMatrixTranslation(&nodeTranslation, nodes[i].Position.x, nodes[i].Position.y, nodes[i].Position.z);
        D3DXMatrixMultiply(&world, &nodeRotation, &nodeTranslation);
        D3DXMatrixInverse(&invWorld, NULL, &world);
        D3DXVec3TransformCoord(&translatedOrigin, &rayOrigin, &invWorld);
        D3DXVec3TransformNormal(&translatedDirection, &rayDirection, &invWorld);


        if (arrow->Intersect(&translatedOrigin, &translatedDirection, &distance/*,NULL,NULL*/) && (closest >= distance || !intersected))
        {
          closest = distance;
          nodeIndex = i;
          intersected = true;
        }

      }
    }
    else if (nodes[i].Class.checksum == RailNode::GetClass())
    {
      D3DXVECTOR3 pos, pos2;
      if (showRails)
      {
        const DWORD numLinks = nodes[i].Links.size();
        if (numLinks)
        {
          for (DWORD j = 0; j < numLinks; j++)
          {
            if (nodes[nodes[i].Links[j]].Class.checksum == RailNode::GetClass())
            {
              D3DXVec3Project(&pos, (D3DXVECTOR3*)&nodes[i], &port, &camera->proj(), &camera->view(), matWorld);
              D3DXVec3Project(&pos2, (D3DXVECTOR3*)&nodes[nodes[i].Links[j]], &port, &camera->proj(), &camera->view(), matWorld);
              if ((pos.z<1.0f && pos.z>0.0f) || (pos2.z<1.0f && pos2.z>0.0f))
              {
                float tmpDist;

                float MAX_DIST = nodes[i].Position.x - nodes[nodes[i].Links[j]].Position.x;
                MAX_DIST *= MAX_DIST;
                static bool axis = false;
                if (((nodes[i].Position.z - nodes[nodes[i].Links[j]].Position.z)*(nodes[i].Position.z - nodes[nodes[i].Links[j]].Position.z)) > MAX_DIST)
                  axis = true;
                else
                  axis = false;

                if (TriangleIntersection(&D3DXVECTOR3(axis ? nodes[i].Position.x + 10.0f : nodes[i].Position.x, nodes[i].Position.y - 10.0f, axis ? nodes[i].Position.z : nodes[i].Position.z + 10.0f), &D3DXVECTOR3(axis ? nodes[i].Position.x + 10.0f : nodes[i].Position.x, nodes[i].Position.y + 10.0f, axis ? nodes[i].Position.z : nodes[i].Position.z + 10.0f), &D3DXVECTOR3(axis ? nodes[nodes[i].Links[j]].Position.x + 10.0f : nodes[nodes[i].Links[j]].Position.x, nodes[nodes[i].Links[j]].Position.y - 10.0f, axis ? nodes[nodes[i].Links[j]].Position.z : nodes[nodes[i].Links[j]].Position.z + 10.0f), &rayOrigin, &rayDirection, &hitPoint))
                {
                  distance = D3DXVec3Length(&(*(D3DXVECTOR3*)&nodes[i].Position - hitPoint));

                  if ((tmpDist = D3DXVec3Length(&(*(D3DXVECTOR3*)&nodes[nodes[i].Links[j]].Position - hitPoint))) < distance)
                  {
                    distance = tmpDist;
                    if (distance < closest || !intersected)
                    {
                      intersected = true;
                      closest = D3DXVec3Length(&(rayOrigin - hitPoint));//closest = distance;
                      nodeIndex = nodes[i].Links[j];
                    }
                  }
                  else
                  {
                    if (distance < closest || !intersected)
                    {
                      intersected = true;
                      closest = D3DXVec3Length(&(rayOrigin - hitPoint));//closest = distance;
                      nodeIndex = i;
                    }
                  }
                }
                if (TriangleIntersection(&D3DXVECTOR3(axis ? nodes[i].Position.x + 10.0f : nodes[i].Position.x, nodes[i].Position.y - 10.0f, axis ? nodes[i].Position.z : nodes[i].Position.z + 10.0f), &D3DXVECTOR3(axis ? nodes[nodes[i].Links[j]].Position.x + 10.0f : nodes[nodes[i].Links[j]].Position.x, nodes[nodes[i].Links[j]].Position.y + 10.0f, axis ? nodes[nodes[i].Links[j]].Position.z : nodes[nodes[i].Links[j]].Position.z + 10.0f), &D3DXVECTOR3(axis ? nodes[nodes[i].Links[j]].Position.x + 10.0f : nodes[nodes[i].Links[j]].Position.x, nodes[nodes[i].Links[j]].Position.y - 10.0f, axis ? nodes[nodes[i].Links[j]].Position.z : nodes[nodes[i].Links[j]].Position.z + 10.0f), &rayOrigin, &rayDirection, &hitPoint))
                {
                  distance = D3DXVec3Length(&(*(D3DXVECTOR3*)&nodes[i].Position - hitPoint));

                  if ((tmpDist = D3DXVec3Length(&(*(D3DXVECTOR3*)&nodes[nodes[i].Links[j]].Position - hitPoint))) < distance)
                  {
                    distance = tmpDist;
                    if (distance < closest || !intersected)
                    {
                      intersected = true;
                      closest = D3DXVec3Length(&(rayOrigin - hitPoint));//closest = distance;
                      nodeIndex = nodes[i].Links[j];
                    }
                  }
                  else
                  {
                    if (distance < closest || !intersected)
                    {
                      intersected = true;
                      closest = D3DXVec3Length(&(rayOrigin - hitPoint));//closest = distance;
                      nodeIndex = i;
                    }
                  }
                }


                if (TriangleIntersection(&D3DXVECTOR3(axis ? nodes[i].Position.x - 10.0f : nodes[i].Position.x, nodes[i].Position.y - 10.0f, axis ? nodes[i].Position.z : nodes[i].Position.z - 10.0f), &D3DXVECTOR3(axis ? nodes[i].Position.x - 10.0f : nodes[i].Position.x, nodes[i].Position.y + 10.0f, axis ? nodes[i].Position.z : nodes[i].Position.z - 10.0f), &D3DXVECTOR3(axis ? nodes[nodes[i].Links[j]].Position.x - 10.0f : nodes[nodes[i].Links[j]].Position.x, nodes[nodes[i].Links[j]].Position.y - 10.0f, axis ? nodes[nodes[i].Links[j]].Position.z : nodes[nodes[i].Links[j]].Position.z - 10.0f), &rayOrigin, &rayDirection, &hitPoint))
                {
                  distance = D3DXVec3Length(&(*(D3DXVECTOR3*)&nodes[i].Position - hitPoint));

                  if ((tmpDist = D3DXVec3Length(&(*(D3DXVECTOR3*)&nodes[nodes[i].Links[j]].Position - hitPoint))) < distance)
                  {
                    distance = tmpDist;
                    if (distance < closest || !intersected)
                    {
                      intersected = true;
                      closest = D3DXVec3Length(&(rayOrigin - hitPoint));//closest = distance;
                      nodeIndex = nodes[i].Links[j];
                    }
                  }
                  else
                  {
                    if (distance < closest || !intersected)
                    {
                      intersected = true;
                      closest = D3DXVec3Length(&(rayOrigin - hitPoint));//closest = distance;
                      nodeIndex = i;
                    }
                  }
                }
                if (TriangleIntersection(&D3DXVECTOR3(axis ? nodes[i].Position.x - 10.0f : nodes[i].Position.x, nodes[i].Position.y - 10.0f, axis ? nodes[i].Position.z : nodes[i].Position.z - 10.0f), &D3DXVECTOR3(axis ? nodes[nodes[i].Links[j]].Position.x - 10.0f : nodes[nodes[i].Links[j]].Position.x, nodes[nodes[i].Links[j]].Position.y + 10.0f, axis ? nodes[nodes[i].Links[j]].Position.z : nodes[nodes[i].Links[j]].Position.z - 10.0f), &D3DXVECTOR3(axis ? nodes[nodes[i].Links[j]].Position.x - 10.0f : nodes[nodes[i].Links[j]].Position.x, nodes[nodes[i].Links[j]].Position.y - 10.0f, axis ? nodes[nodes[i].Links[j]].Position.z : nodes[nodes[i].Links[j]].Position.z - 10.0f), &rayOrigin, &rayDirection, &hitPoint))
                {
                  distance = D3DXVec3Length(&(*(D3DXVECTOR3*)&nodes[i].Position - hitPoint));

                  if ((tmpDist = D3DXVec3Length(&(*(D3DXVECTOR3*)&nodes[nodes[i].Links[j]].Position - hitPoint))) < distance)
                  {
                    distance = tmpDist;
                    if (distance < closest || !intersected)
                    {
                      intersected = true;
                      closest = D3DXVec3Length(&(rayOrigin - hitPoint));//closest = distance;
                      nodeIndex = nodes[i].Links[j];
                    }
                  }
                  else
                  {
                    if (distance < closest || !intersected)
                    {
                      intersected = true;
                      closest = D3DXVec3Length(&(rayOrigin - hitPoint));//closest = distance;
                      nodeIndex = i;
                    }
                  }
                }
              }
            }
          }
        }
        else
        {
          D3DXVec3Project(&pos, (D3DXVECTOR3*)&nodes[i].Position, &port, &camera->proj(), &camera->view(), matWorld);
          if (pos.z<1.0f && pos.z>0.0f)
          {
            const float xDist = pos.x - Mouse.x;
            const float yDist = pos.y - Mouse.y;

            if (((distance = xDist*xDist + yDist*yDist)) < 100.0f)
            {
              if (distance == 0)
                distance = pos.z + 65000.0f;
              else
                distance = (xDist*xDist + yDist*yDist)*pos.z + 95000.0f;

              if (!intersected || distance < closest)
              {
                closest = distance;
                intersected = true;
                nodeIndex = i;
              }
            }
          }
        }
      }
    }
  }
  defaultCulling = tmp;
  if (intersected)
  {
    if (nodes[nodeIndex].Class.checksum == RailNode::GetClass())
    {
      std::string info = nodes[nodeIndex].Name.GetString() + std::string(" ") + nodes[nodeIndex].Cluster.GetString() + std::string("\r\n");
      char tmp[256];
      if (nodes[nodeIndex].Links.size()) info.append(std::string(" [0]") + nodes[nodes[nodeIndex].Links[0]].Name.GetString());
      if (nodes[nodeIndex].Links.size() > 1) info.append(std::string(" [1]") + nodes[nodes[nodeIndex].Links[1]].Name.GetString());
      ::MessageBox(0, info.c_str(), info.c_str(), 0);
    }
    if (!nodes[nodeIndex].IsSelected())
      selectedNodes.push_back(&nodes[nodeIndex]);
  }
}

float timeNow;

//int tNow;


#pragma managed
void THPSLevel::InitRenderScene()
{
  handleWindow = static_cast<HWND>(this->panel6->Handle.ToPointer());
  pictureHandle = static_cast<HWND>(this->pictureBox1->Handle.ToPointer());
  Direct3DCreate9Ex(D3D_SDK_VERSION, &d3d);

  HRESULT hr = 0;

  D3DDISPLAYMODE mode;
  d3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode);
  ZeroMemory(&DevParams, sizeof(DevParams));
  DevParams.Windowed = TRUE;
  DevParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
  DevParams.hDeviceWindow = handleWindow;
  DevParams.BackBufferHeight = 0;//0;//768;
  DevParams.BackBufferWidth = 0;//0;//1024;
  DevParams.BackBufferFormat = mode.Format;
  DevParams.BackBufferCount = 1;
  DevParams.PresentationInterval = D3DPRESENT_INTERVAL_ONE;//D3DPRESENT_INTERVAL_IMMEDIATE;
  DevParams.EnableAutoDepthStencil = TRUE;
  DevParams.AutoDepthStencilFormat = D3DFMT_D24X8;//D3DFMT_D16;


  hr = d3d->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, handleWindow, D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE | D3DCREATE_FPU_PRESERVE, &DevParams, NULL, &Device);
#ifndef GWL_HINSTANCE
  InitInputDevices((HINSTANCE)GetWindowLongPtr(handleWindow, GWLP_HINSTANCE), handleWindow);
#else
  InitInputDevices((HINSTANCE)GetWindowLong(handleWindow, GWL_HINSTANCE), handleWindow);
#endif
  if (FAILED(hr))
    MessageBox::Show("Failed to create device");
  SetDeviceStates();
  DevParams.BackBufferWidth = pictureBox1->Size.Width;
  DevParams.BackBufferHeight = pictureBox1->Size.Height;
  DevParams.hDeviceWindow = pictureHandle;

  Device->CreateAdditionalSwapChain(&DevParams, &imageChain);
  D3DXCreateSprite(Device, &image);

  imagePort.X = 0;
  imagePort.Y = 0;
  imagePort.Width = this->pictureBox1->Size.Width;
  imagePort.Height = this->pictureBox1->Size.Height;
  imagePort.MinZ = 0.0f;
  imagePort.MaxZ = 1.0f;

  DevParams.hDeviceWindow = handleWindow;
  //LPD3DXBUFFER pD3DXMtrlBuffer;

  // Load the mesh from the specified file
  HRSRC resource = FindResource(NULL, MAKEINTRESOURCE(2053), RT_RCDATA);
  if (resource != NULL)
  {
    HGLOBAL resourceData = LoadResource(NULL, resource);
    if (resourceData != NULL)
    {
      void* pZip = LockResource(resourceData);
      if (pZip != NULL)
      {
        HZIP hz = OpenZip(pZip, SizeofResource(NULL, resource), 0);
        BYTE* pFile = new BYTE[1600000];

        if (pFile != NULL)
        {
          int index;
          ZIPENTRY ze;

          FindZipItem(hz, "Sk4Edscn.dat", true, &index, &ze);
          if (index == -1)
          {
            ::MessageBox(0, "error loading resource", "arrow", 0);
            CloseZip(hz);
            return;
          }
          UnzipItem(hz, index, pFile, 1600000);
          
          arrow = new Th4Mesh((DWORD)pFile, 71);
          arrow->CreateBuffers(Device, D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX0);///*|MAX_TEXCOORDS*//*);
          delete[] pFile;
        }
        UnlockResource(pZip);
        CloseZip(hz);
      }
    }

  }
  InitConsole();
}

void THPSLevel::ResizePictureWindow(System::Object^  sender, System::EventArgs^  e) {
  DevParams.BackBufferWidth = pictureBox1->Size.Width;
  DevParams.BackBufferHeight = pictureBox1->Size.Height;
  HWND hwnd = pictureHandle;
  DevParams.hDeviceWindow = hwnd;
  imagePort.Width = pictureBox1->Size.Width;
  imagePort.Height = pictureBox1->Size.Height;
  imageChain->Release();
  Device->CreateAdditionalSwapChain(&DevParams, &imageChain);

  LPDIRECT3DSURFACE9 pBackBuffer = NULL;
  IDirect3DSurface9* pRender = NULL;
  IDirect3DSurface9* pSurface = NULL;
  D3DXMATRIX ident;
  D3DXMatrixIdentity(&ident);

  Device->SetTransform(D3DTS_WORLD, &ident);
  Device->SetTransform(D3DTS_VIEW, &ident);
  Device->SetViewport(&imagePort);
  D3DXMatrixPerspectiveFovLH(&ident, D3DXToRadian(70.0f), float((float)imagePort.Width / (float)imagePort.Height), 12.0f, 95250.0f);
  Device->SetTransform(D3DTS_PROJECTION, &ident);
  Device->GetRenderTarget(0, &pRender);
  imageChain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);

  Device->SetRenderTarget(0, pBackBuffer);

  Device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
    D3DCOLOR_COLORVALUE(0.0f, 1.0f, 0.0f, 1.0f), 1.0f, 0);

  Device->BeginScene();
  image->Begin(D3DXSPRITE_ALPHABLEND);
  image->Draw(texture, NULL, NULL, NULL, D3DCOLOR_RGBA(255, 255, 255, 255));
  image->End();
  Device->EndScene();
  imageChain->Present(0, 0, hwnd, 0, 0);

  pBackBuffer->Release();
  Device->SetRenderTarget(0, pRender);
  pRender->Release();
  Device->SetViewport(&port);
  Device->SetTransform(D3DTS_PROJECTION, &camera->proj());
  Device->SetTransform(D3DTS_VIEW, &camera->view());

  DevParams.hDeviceWindow = handleWindow;
}

System::Void THPSLevel::toolStripButton8_Click(System::Object^  sender, System::EventArgs^  e)
{
  quickMaterialMode = !quickMaterialMode;
  if (!quickMaterialMode)
    updateMaterial = true;
  else
  {
    this->tabControl1->SelectedIndex = 3;
  }
}

void THPSLevel::ResizeRenderWindow(System::Object^  sender, System::EventArgs^  e) {
  if (FPS || render == false)
    return;
  render = false;

  if (!FPS)
  {
    DevParams.BackBufferWidth = panel6->Size.Width;
    DevParams.BackBufferHeight = panel6->Size.Height;
  }

  Device->ResetEx(&DevParams, NULL);

  DevParams.BackBufferWidth = pictureBox1->Size.Width;
  DevParams.BackBufferHeight = pictureBox1->Size.Height;

  DevParams.hDeviceWindow = pictureHandle;
  imagePort.Width = pictureBox1->Size.Width;
  imagePort.Height = pictureBox1->Size.Height;
  imageChain->Release();
  Device->CreateAdditionalSwapChain(&DevParams, &imageChain);

  DevParams.hDeviceWindow = handleWindow;

  Device->GetViewport(&port);
  CreateCamera();
  render = true;
  if (!forceRedraw)
    DrawFrame();
}

void THPSLevel::CleanupAppdata()
{
  /* DEBUGSTART()
  {*/
  if (scene)
  {
    UnloadScene();
  }
  else if (camera)
  {
    CleanupEvents();
    delete camera;
    delete player;
  }
  CleanupInputDevices();
  if (m_font)
    m_font->Release();
  delete arrow;
  imageChain->Release();
  if (texture)
    texture->Release();
  if (image)
    image->Release();
  Device->Release();
  d3d->Release();
  /* }
  DEBUGEND()*/
}

#pragma unmanaged