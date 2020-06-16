#ifndef SCENE_H
#define SCENE_H

//#include "th4scene.h"
#include "Mesh.h"
#include "Node.h"
#include <memory>
#include "Material.h"
#include "mesh.h"
using std::vector;
#pragma unmanaged
struct Global
{
  Checksum name;
  std::shared_ptr<BYTE> data;
  Global(Checksum& chc)
  {
    name=chc;
  }
  Global(Checksum chc)
  {
    name=chc;
  }
  Global(Checksum&& chc)
  {
    name=chc;
  }
  BYTE* GetData()
  {
    data.get();
  }
};
struct TexFiles
{
  DWORD numFiles;
  char** fileNames;
};

struct MultiLayeredVertex
{
  float x;
  float y;
  float z;
  Colour colour;

  TexCoord tUV[7];
};

struct ExtraLayerMesh
{
  Checksum name;
  vector<MaterialSplit> matSplits;
  DWORD numPasses;
  vector<MultiLayeredVertex> vertices;
  //vector<vector<MultiLayeredVertex>> vertices;

  /*~ExtraLayerMesh()
  {
  for(DWORD i=0; i<matSplits.size(); i++)
  delete matSplits[i];
  }*/

  __declspec (noalias) void SetPosition(const D3DXVECTOR3 const*__restrict position, const D3DXVECTOR3 const*__restrict angle)
  {
    //D3DXVECTOR3 center = GetCenter();
    /**(D3DXVECTOR3*)&bbox.Max = *position+(*(D3DXVECTOR3*)&bbox.Max-center);
    *(D3DXVECTOR3*)&bbox.Min = *position-(*(D3DXVECTOR3*)&bbox.Min+center);*/
    D3DXMATRIX rotation;
    D3DXMATRIX translation;
    D3DXMATRIX result;
    D3DXMatrixRotationYawPitchRoll(&rotation, -angle->y + D3DX_PI, angle->x, angle->z);
    D3DXMatrixTranslation(&translation, position->x, position->y, position->z);
    D3DXMatrixMultiply(&result, &rotation, &translation);
    /*D3DXVec3TransformCoord((D3DXVECTOR3*)&bbox.Max, (D3DXVECTOR3*)&bbox.Max, &result);
    D3DXVec3TransformCoord((D3DXVECTOR3*)&bbox.Min, (D3DXVECTOR3*)&bbox.Min, &result);
    if(bbox.Min.x > bbox.Max.x)
    {
    const float temp = bbox.Max.x;
    bbox.Max.x = bbox.Min.x;
    bbox.Min.x = temp;
    }
    if(bbox.Min.y > bbox.Max.y)
    {
    const float temp = bbox.Max.y;
    bbox.Max.y = bbox.Min.y;
    bbox.Min.y = temp;
    }
    if(bbox.Min.x > bbox.Max.x)
    {
    const float temp = bbox.Max.z;
    bbox.Max.z = bbox.Min.z;
    bbox.Min.z = temp;
    }
    */
    for (DWORD i = 0, numVertices = vertices.size(); i<numVertices; i++)
    {
      //*(D3DXVECTOR3*)&vertices[i]-=center;
      D3DXVec3TransformCoord((D3DXVECTOR3*)&vertices[i], (D3DXVECTOR3*)&vertices[i], &result);
    }
  }

  ExtraLayerMesh(Checksum name, MaterialSplit* split, const Vertex* vertices, DWORD numVertices, TexCoord* texCoords, DWORD numTexCoords)
  {
    this->name = name;
    matSplits.push_back(MaterialSplit());
    matSplits.back().matId = split->matId;
    matSplits.back().numIndices = split->numIndices;
    matSplits.back().Indices.assign(split->Indices.begin(), split->Indices.end());
    numPasses = numTexCoords - 1;
    this->vertices.resize(numVertices);
    for (DWORD j = 0; j<numVertices; j++)
    {
      for (DWORD k = 1; k<numTexCoords; k++)
      {
        this->vertices[j].x = vertices[j].x;
        this->vertices[j].y = vertices[j].y;
        this->vertices[j].z = vertices[j].z;
        this->vertices[j].colour = vertices[j].colour;
        this->vertices[j].colour.a = 0x80;
        this->vertices[j].tUV[k - 1] = texCoords[j*numTexCoords + k];
      }
    }
    /*DWORD vertexIndex = this->vertices.size();
    this->vertices.resize(vertexIndex+1);
    this->vertices[vertexIndex].resize(numVertices);
    for(DWORD j=0; j<numVertices; j++)
    {
    for(DWORD k=1; k<numTexCoords; k++)
    {
    this->vertices[vertexIndex][j].x = vertices[j].x;
    this->vertices[vertexIndex][j].y = vertices[j].y;
    this->vertices[vertexIndex][j].z = vertices[j].z;
    this->vertices[vertexIndex][j].colour=vertices[j].colour;
    this->vertices[vertexIndex][j].colour.a=0x80;
    this->vertices[vertexIndex][j].tUV[k-1] = texCoords[j*numTexCoords+k];
    }
    }*/
  }
};

struct Scene
{
protected:
  //bool show;
  vector <Mesh> meshes;
  D3DXMATRIX matWorld;
  D3DXMATRIX matSky;
public:
  Scene* skyDome;
  MaterialList matList;
  vector <Node> nodes;
  DWORD cullMode;
  vector <Checksum> triggers;
  vector <KnownScript> KnownScripts;
  vector <Checksum> globals;
  vector <UVAnim> animations;
protected:
  BYTE blendValue;//2 for th3/thug(colour*2) 1 for th4(colour*1)
  char name[MAX_PATH];

public:
  void CreateBuffers(IDirect3DDevice9Ex* const __restrict Device, const DWORD &fvf);
  void ReleaseBuffers();
  void ProcessTexFile(DWORD ptr, bool sky);
  void SetName(const char* name);
  void ExportObj(const char* path);
  void ExportBSP(const char* path);
  void ExportScn(const char* const __restrict path);
  //void ImportBSP(const char* path);
  void AddMaterial(Material* material);
  void AddMesh(Mesh* mesh);
  __declspec(restrict) Mesh* const __restrict GetMesh(const DWORD &index);
  __declspec(restrict) Mesh* const __restrict GetMesh(const Checksum &name);
  __declspec(restrict) Mesh* const __restrict GetMesh(const Checksum &name, DWORD &index);
  char* GetName();
  const DWORD GetNumVertices();
  const DWORD GetNumTriangles();
  void Render(IDirect3DDevice9Ex* Device);
  void RenderNoTrans(IDirect3DDevice9Ex* Device);
  void RenderShaded(IDirect3DDevice9Ex* Device);
  void RenderTrans(IDirect3DDevice9Ex* Device);
  bool Pickmesh(DWORD index, const D3DXVECTOR3* rayOrigin, const D3DXVECTOR3* rayDirection, float* outDistance, DWORD* outMatIndex, DWORD* outTriangleIndex);
  bool Intersect(const D3DXVECTOR3* rayOrigin, const D3DXVECTOR3* rayDirection, float* outDistance, DWORD* outMeshIndex, DWORD* outMatIndex = NULL, DWORD* outTriangleIndex = NULL);
  bool Intersect(const Ray & __restrict ray, CollisionResult & __restrict results) const;
  __declspec (noalias) bool CastRay(const Ray & __restrict ray, CollisionResult & __restrict result) const;
  DWORD Collide(const Ray & __restrict ray) const;//, D3DXVECTOR3 & __restrict newPos) const;
  DWORD Collide(const Ray & __restrict ray, const Ray & __restrict downRay, D3DXVECTOR3 & __restrict newPos) const;
  __declspec (noalias) bool Scene::AboveSurface(const Ray & __restrict ray) const;
  __declspec (noalias)DWORD Scene::GetState(const Ray & __restrict ray, D3DXVECTOR3 & __restrict newPos) const;

  __declspec (noalias) bool GetHitPoint(const Ray & __restrict ray, CollisionResult & __restrict closestHit) const;

  Animation* GetAnimation(DWORD matId)
  {
    for(DWORD i=0; i<animations.size(); i++)
    {
      if(animations[i].matId==matId)
        return &animations[i].anim;
    }
    return NULL;
  }

  void ProcessTRG(BYTE* pFile, DWORD size);

  DWORD GetNumAnimations()
  {
    return animations.size();
  }

  UVAnim* GetAnimations()
  {
    return animations.size() == 0 ? NULL : &animations.front();
  }

  void AddAnimation(Checksum matId, const BYTE* __restrict pFile)
  {
    animations.push_back(UVAnim(matId.checksum, pFile));
  }

  void AddAnimation(DWORD matId, BYTE type)
  {
    animations.push_back(UVAnim(matId, type));
  }

  void AddAnimation(DWORD matId, float speedX, float speedY)
  {
    animations.push_back(UVAnim(matId, speedX, speedY));
  }

  void SetBlend(BYTE blend)
  {
    blendValue = blend;
  }

  void Resize(DWORD numMeshes, Mesh mesh)
  {
    meshes.resize(numMeshes, mesh);
  }

  void Resize(DWORD numMeshes)
  {
    meshes.resize(numMeshes);
  }

  void EraseScript(DWORD name)
  {
    for(DWORD i=0; i<triggers.size(); i++)
    {
      if(triggers[i].checksum==name)
      {
        triggers.erase(triggers.begin()+i);
        break;
      }
    }
    bool found=false;
    for(DWORD i=0; i<triggers.size(); i++)
    {
      if(triggers[i].checksum==name)
      {
        found=true;
        break;
      }
    }
    if(!found)
    {
      for(DWORD i=0; i<KnownScripts.size(); i++)
      {
        if(KnownScripts[i].name.checksum==name)
        {
          KnownScripts.erase(KnownScripts.begin()+i);
          break;
        }
      }
    }
    return;
  }

  const D3DXVECTOR3& GetClosestHitPoint(const D3DXVECTOR3* rayOrigin, const D3DXVECTOR3* rayDirection, const POINT* mouse)
  {
    static float closest = 0.0f;
    static float distance = 0.0f;
    bool intersected = false;
    D3DXVECTOR3 hitPoint, closestHitPoint;




    for(DWORD i=0, numMeshes = meshes.size(); i<numMeshes; i++)
    {
      if(meshes[i].Intersect(rayOrigin, rayDirection, mouse, &distance, &hitPoint) && (distance <= closest || !intersected))
      {
        closest = distance;
        closestHitPoint = hitPoint;
        intersected = true;
      }
    }
    if(intersected)
      return closestHitPoint;
    else
      return D3DXVECTOR3(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
  }

  inline const BYTE &GetBlendMode() const
  {
    return blendValue;
  }

  void scaleSky(float amount)
  {
    D3DXMatrixIdentity(&matSky);
    D3DXMatrixScaling(&matSky,amount,amount,amount);
  }

  /* inline void SortAlpha()
  {
  sort(meshes.begin(),meshes.end());
  }*/

  /*void Show(bool bShow)
  {
  show = bShow;
  }

  bool Show()
  {
  return show;
  }*/

  char* GetLevelName()
  {
    char* name = new char[25];
    DWORD pos = ((std::string)this->name).find_last_of("\\");
    strcpy(name,&this->name[pos]);
    return name;
  }

  void AddGlobal(Checksum global)
  {
    for(DWORD i=0; i<globals.size(); i++)
    {
      if(global.checksum==globals[i].checksum)
        return;
    }
    globals.push_back(global);
  }

  __forceinline void SetSkyPos(const D3DXVECTOR3 & pos)
  {
    matSky(3,0) = pos.x;
    matSky(3,1) = pos.y;
    matSky(3,2) = pos.z;

  }

  __forceinline D3DXMATRIX* GetSkyMatrix()
  {
    return &matSky;
  }

  __forceinline D3DXMATRIX* GetWorldMatrix()
  {
    return &matWorld;
  }

  /*void AddCollision(const NSCollisionObject &collisionData, const DWORD version)
  {
  Collision collisionObject;
  collisionObject.SetData(collisionData, version);

  collision.push_back(collisionObject);
  }*/

  const DWORD GetNumMeshes() const
  {
    return meshes.size();
  }

  void* GetScript(const char* name, DWORD* outSize = NULL)
  {
    Checksum checksum = Checksum(name);
    for(DWORD i=0; i<KnownScripts.size(); i++)
    {
      if(checksum.checksum==KnownScripts[i].name.checksum)
      {
        if(outSize)
          *outSize = KnownScripts[i].size;
        return KnownScripts[i].func;
      } 
    }
    return NULL;
  }

  KnownScript* GetScript(DWORD name)
  {
    for(DWORD i=0, numScripts = KnownScripts.size(); i<numScripts; i++)
    {
      if(KnownScripts[i].name.checksum==name)
        return &KnownScripts[i];
    }
    return NULL;
  }

  KnownScript* GetScript(char* name)
  {
    Checksum checksum = Checksum(name);
    for (DWORD i = 0, numScripts = KnownScripts.size(); i<numScripts; i++)
    {
      if (KnownScripts[i].name.checksum == checksum.checksum)
        return &KnownScripts[i];
    }
    return NULL;
  }

  bool GetTrigger(DWORD name)
  {
    for(DWORD i=0, numTriggers = triggers.size(); i<numTriggers; i++)
    {
      if(triggers[i].checksum==name)
        return true;
    }
    return false;
  }

  const void* GetScript(const DWORD &checksum, DWORD* outSize = NULL) const
  {
    for(DWORD i=0; i<KnownScripts.size(); i++)
    {
      if(checksum==KnownScripts[i].name.checksum)
      {
        if(outSize)
          *outSize = KnownScripts[i].size;
        return KnownScripts[i].func;
      }
    }
    return NULL;
  }

  void EraseMesh(const DWORD index)
  {
    meshes.erase(meshes.begin()+index);
  }

  bool GetClosestVertex(D3DXVECTOR3 *rayOrigin, D3DXVECTOR3 *rayDirection, Vertex* outVertex, Mesh* outMesh, float *outDistance)
  {
    float closestDist = 0;
    bool hasHit = false;
    for(DWORD i=0, numMeshes = meshes.size(); i<numMeshes; i++)
    {
      if(meshes[i].GetClosestVertex(rayOrigin,rayDirection,outVertex, outDistance))
      {
        if(*outDistance<closestDist || !hasHit)
        {
          hasHit=true;
          closestDist=*outDistance;
          outMesh=&meshes[i];
        }
      }
    }
    *outDistance = closestDist;
    return hasHit;
  }

  bool UnusedMaterial(const DWORD index)
  {
    for(DWORD i=0, numMeshes = meshes.size(); i<numMeshes; i++)
    {
      for(DWORD j=0, numSplits = meshes[i].matSplits.size(); j<numSplits; j++)
      {
        if(meshes[i].matSplits[j].matId == index)
          return false;
      }
    }
    return true;
  }

  void EraseMaterial(const DWORD index)
  {
    for(DWORD i=0, numMeshes = meshes.size(); i<numMeshes; i++)
    {
      for(DWORD j=0, numSplits = meshes[i].matSplits.size(); j<numSplits; j++)
      {
        if(meshes[i].matSplits[j].matId>index)
          meshes[i].matSplits[j].matId--;
      }
    }
    matList.EraseMaterial(index);
  }

  void EraseMaterial(const DWORD index, DWORD newIndex)
  {
    if(newIndex>index)
      newIndex--;
    for(DWORD i=0, numMeshes = meshes.size(); i<numMeshes; i++)
    {
      for(DWORD j=0, numSplits = meshes[i].matSplits.size(); j<numSplits; j++)
      {
        if(meshes[i].matSplits[j].matId>index)
          meshes[i].matSplits[j].matId--;
        else if(meshes[i].matSplits[j].matId==index)
          meshes[i].matSplits[j].matId=newIndex;
      }
    }
    matList.EraseMaterial(index);
  }

  void RemoveUnusedMaterials()
  {
    for(DWORD i=0; i<matList.GetNumMaterials(); i++)
    {
      for(DWORD j=i+1; j<matList.GetNumMaterials(); j++)
      {
        Material* tmpMat = matList.GetMaterial(i);
        Material* other = matList.GetMaterial(j);

        if(tmpMat->transparent == other->transparent && tmpMat->GetNumTextures() == 1 && other->GetNumTextures() == 1)
        {
          if(tmpMat->GetTexture(0)->GetTexId() == other->GetTexture(0)->GetTexId())
          {
            EraseMaterial((DWORD)i, j);
            i--;
            j--;
            break;
          }
        }
        else if(UnusedMaterial(i))
        {
          EraseMaterial(i);
          i--;
          j--;
          break;
        }
      }
    }

    if(skyDome)
      skyDome->RemoveUnusedMaterials();
  }

  /*void EraseMaterial(const DWORD index, DWORD newIndex)
  {
  if(newIndex>index)
  newIndex--;
  for(DWORD i=0, numMeshes = meshes.size(); i<numMeshes; i++)
  {
  for(DWORD j=0, numSplits = meshes[i].matSplits.size(); j<numSplits; j++)
  {
  if(meshes[i].matSplits[j].matId>index)
  meshes[i].matSplits[j].matId--;
  else if(meshes[i].matSplits[j].matId==index)
  meshes[i].matSplits[j].matId=newIndex;
  }
  }
  matList.EraseMaterial(index);
  }

  bool UnusedMaterial(const DWORD index)
  {
  for(DWORD i=0, numMaterials = matList.GetNumMaterials(); i<numMaterials; i++)
  {
  for(DWORD j=0, numMeshes = meshes.size(); j<numMeshes; j++)
  {
  for(DWORD k=0, numSplits = meshes[j].matSplits.size(); k<numSplits; k++)
  {
  if(meshes[j].matSplits[k].matId == index)
  return false;
  }
  }
  }
  return true;
  }

  void OptimizeMaterials()
  {
  for(DWORD int i=0; i<matList.GetNumMaterials(); i++)
  {
  for(DWORD j=i+1; j<matList.GetNumMaterials(); j++)
  {
  Material* tmpMat = matList.GetMaterial(i);
  Material* other = matList.GetMaterial(j);

  if(tmpMat->transparent == other->transparent && tmpMat->GetNumTextures() && other->GetNumTextures())
  {
  if(tmpMat->GetTexture(0)->GetTexId() == other->GetTexture(0)->GetTexId())
  {
  EraseMaterial((DWORD)i, j);
  i--;
  j--;
  break;
  }
  }
  else if(UnusedMaterial(i))
  {
  EraseMaterial((DWORD)i, j);
  i--;
  j--;
  break;
  }
  }
  }


  if(skyDome)
  skyDome->OptimizeMaterials();
  }*/

  const DWORD &GetNodeIndex(const Checksum &name) const
  {
    for(DWORD i=0, numNodes = nodes.size(); i<numNodes; i++)
    {
      if(name.checksum==nodes[i].Name.checksum)
        return i;
    }
    return 0xFFFFFFFF;
  }

  Node* GetNode(const Checksum name)
  {
    for(DWORD i=0, numNodes = nodes.size(); i<numNodes; i++)
    {
      if(name.checksum==nodes[i].Name.checksum)
        return &nodes[i];
    }
    return NULL;
  }

  void DeleteNode(const Checksum name)
  {
    DWORD index = GetNodeIndex(name);
    if(index!=0xFFFFFFFF)
    {
      nodes.erase(nodes.begin()+index);

      for(DWORD i=0, numNodes = nodes.size(); i<numNodes; i++)
      {
        for(DWORD j=0; j<nodes[i].Links.size(); j++)
        {
          if(nodes[i].Links[j]==index)
          {
            nodes[i].Links.erase(nodes[i].Links.begin()+j);
            j--;
          }
          else if(nodes[i].Links[j]>index)
            nodes[i].Links[j]--;
        }
      }
    }
  }

  void DeleteNode(const DWORD index)
  {
    nodes.erase(nodes.begin()+index);

    for(DWORD i=0, numNodes = nodes.size(); i<numNodes; i++)
    {
      for(DWORD j=0; j<nodes[i].Links.size(); j++)
      {
        if(nodes[i].Links[j]==index)
          nodes[i].Links.erase(nodes[i].Links.begin()+j);
        else if(nodes[i].Links[j]>index)
          nodes[i].Links[j]--;
      }
    }
  }

  __declspec (noalias) bool FixLinksTHUG();
  __declspec (noalias) bool FixLinksFinal();
  __declspec (noalias) bool FixLinks();
  __declspec (noalias) void ParseQBTable(register const BYTE const* __restrict pFile, const BYTE const* __restrict eof);
  void LoadSound(Checksum sound, vector<Script> &sounds);
  __declspec (noalias) void AddCompressedNodes(register const BYTE const* __restrict pFile, const BYTE const* __restrict eof, vector<Node> &compressedNodes);//, vector<KnownScript> &scripts);
  __declspec (noalias) void LoadThugNodeArray(register const BYTE const* __restrict pFile, const BYTE const* __restrict eof, vector<KnownScript> &scripts, vector<Script> &sounds, vector<ExtraLayerMesh> &extraLayerMeshes);
  //__declspec (noalias) void LoadProSkaterNodeArray(register const BYTE const* __restrict pFile, const BYTE const* __restrict eof, vector<KnownScript> &scripts, vector<Script> &sounds);
  __declspec (noalias) void LoadScriptsFile(char* path, vector<KnownScript> &scripts, vector<Script> &sounds, bool THUG=true);
   _declspec (noalias) const BYTE const* __restrict ParseScript(register const BYTE const* __restrict pFile, KnownScript** pScript, vector<KnownScript> &scripts, vector<Script> &sounds, bool THUG=true, vector<Checksum> *extraTriggers=0);

  void CloneMesh(Mesh* mesh)
  {
    //AddMesh(mesh);
    meshes.push_back( mesh[0]);
    /*Collision* collision = mesh->GetCollision();
    if(collision)
    {
    Mesh &pMesh = meshes.back();
    pMesh.DeinitCollision();
    pMesh.InitCollision();
    *pMesh.GetCollision() = *mesh->GetCollision();
    }*/
  }

  __declspec(noalias) void RenderOpaque()
  {
    register MaterialList *const __restrict list = &matList;
    vector<Mesh>::iterator it = meshes.begin();
    const vector<Mesh>::iterator end = meshes.end();
    while(it!=end)
    {
      if(it->flags & isVisible && !(it->flags & isTransparent)) it->Render0(list);
      it++;
    }
  }

  __declspec(noalias) void RenderTransparent()
  {
    register MaterialList *const __restrict list = &matList;
    vector<Mesh>::iterator it = meshes.begin();
    const vector<Mesh>::iterator end = meshes.end();
    while(it!=end)
    {
      if(it->flags & (isVisible|isTransparent)) it->Render2(list);
      it++;
    }
  }

  void RemoveTriggers(DWORD name)
  {
    for(DWORD i=0; i<nodes.size(); i++)
    {
      if(nodes[i].Trigger.checksum==name)
        nodes[i].Trigger.checksum=0;
    }
  }

  bool MoveObject(Scene* scene, Checksum &name, const SimpleVertex &position, const SimpleVertex &angle)
  {
    extern vector <Script> Scripts;
    Texture texture;
    ZeroMemory(&texture,sizeof(Texture));
    texture.vAddress = D3DTADDRESS_WRAP;
    texture.uAddress = D3DTADDRESS_WRAP;
    texture.blendMode = 20;
    char chc[128]="";

    for(DWORD i=0; i<meshes.size(); i++)
    {
      if(meshes[i].GetName().checksum==name.checksum)
      {
        for(DWORD j=0; j<meshes[i].matSplits.size(); j++)
        {
          MaterialSplit* matSplit = meshes[i].GetSplit(j);

          Material* tmpMat = matList.GetMaterial(matSplit->matId);
          if(tmpMat)
          {
            if(meshes[i].IsDeleted())
            {
              matSplit->matId = scene->matList.GetMaterialIndex(tmpMat->GetTexture(0)->GetTexId());
            }
            else
            {
              Texture* tmpTex = tmpMat->GetTexture(0);
              matSplit->matId = scene->matList.GetNumMaterials();
              Material* material = new Material();
              material->Reserve(tmpMat->GetNumTextures());
              material->drawOrder = tmpMat->drawOrder;
              material->transparent = tmpMat->transparent;
              material->animated = tmpMat->animated;
              DWORD id = tmpMat->GetMatId();
              material->SetMatId(id);
              id = tmpTex->GetTexId();
              texture.SetId(id);
              texture.CreateTexture();
              material->AddTexture2(&texture);
              scene->AddMaterial(material);
              delete material;
            }
          }

          if(!name.GetString())
          {
            sprintf(chc,"obj%d",scene->GetNumMeshes());
            Scripts.push_back((Script(name.checksum, chc)));
          }
          /*scene->nodes.push_back(EnvironmentObject(name,0,0,true));
          this->meshes[i].node = &scene->nodes.back();*/
          //this->meshes[i].Move(position,angle);
        }
        scene->CloneMesh(&meshes[i]);
        //meshes[i].DeinitCollision();
        scene->meshes.back().Move(position,angle);
        if(meshes[i].IsDeleted())
        {
          Mesh &mesh = scene->meshes.back();
          mesh.SetFlag(MeshFlags::isDeleted,false);
          sprintf(chc,"%s_%04X",name.GetString(),scene->GetNumMeshes());
          //MessageBox(0,chc,chc,0);
          name.checksum+=scene->GetNumMeshes();
          Scripts.push_back((Script(name.checksum, chc)));
          mesh.SetName(name);
        }
        else
          meshes[i].SetFlag(MeshFlags::isDeleted,true);
        //meshes.erase(meshes.begin()+i);
        return true;
      }
    }
    return false;
  }

  void SaveScene(const char* path);

  void LoadScene(const char* path);

  void FixZFighting2()
  {
    extern void SelectObjectOnly(Mesh* mesh, bool);
    float distance;
    D3DXVECTOR3 faceNormal, normalized;
    D3DXVECTOR3 dummy;
    DWORD numObjects = meshes.size();
    DWORD axis = 0;

    for (DWORD i = 0; i < numObjects; i++)
    {
      if (meshes[i].IsVisible())
      {
        if (meshes[i].IsFlat(&faceNormal) && meshes[i].NonCollidable() && (meshes[i].GetNumVertices() <= 8 || strstr(meshes[i].GetName().GetString2(), "Shadow")))
        {
          /*D3DXVec3Normalize(&normalized, &faceNormal);//uncomment this if only wanna move object thats flat on specific axis

          float highest = fabsf(normalized.x);
          DWORD trueAxis = 0;
          if (fabsf(normalized.y) > highest)
          {
            highest = fabsf(normalized.y);
            trueAxis = 1;
          }
          if (fabsf(normalized.z) > highest)
          {
            highest = fabsf(normalized.z);
            trueAxis = 2;
          }

          if (axis == trueAxis)
          {*/
            SelectObjectOnly(&meshes[i], true);

            D3DXVec3Normalize(&normalized, &faceNormal);
            normalized.x *= -0.8f;//-distance;
            normalized.y *= -0.8f;//-distance;
            normalized.z *= -0.8f;//-distance;
            meshes[i].MoveRelative(&normalized);
          //}
        }
      }
    }
  }

  void FixZFighting()
  {
    MessageBox(0, "trying to fix zfighting", "", 0);
    extern void SelectObjectOnly(Mesh* mesh, bool);
    float distance;
    D3DXVECTOR3 faceNormal , normalized;
    D3DXVECTOR3 dummy;
    DWORD numObjects = meshes.size();
    DWORD axis=0;

    for(DWORD i=0; i<numObjects; i++)
    {
      if(meshes[i].IsVisible() && !meshes[i].IsSelected())
      {
        for(DWORD j=i+1; j<numObjects; j++)
        {
          if (meshes[j].IsVisible() && !meshes[j].IsSelected())
          {
            if((/*meshes[i].FlickersWith(&meshes[j])) || */(meshes[i].GetClosestDistance(&meshes[j], &axis)<0.5f)))///*distance = meshes[i].GetDistanceTo(&meshes[j]))<0.1f || (distance = meshes[i].GetNormalDistanceTo(&meshes[j]))<0.1f*/ || (distance = D3DXVec3Length(&(meshes[i].GetCenter()-meshes[j].GetCenter()))<0.1f))
            {

              if (meshes[i].IsFlat(&faceNormal) && meshes[i].GetNumVertices() <= 6)
              {
                if (meshes[j].IsFlat(&dummy) && meshes[i].GetNumVertices() <= 6)
                {
                  D3DXVec3Normalize(&normalized, &faceNormal);
                  float highest = fabsf(normalized.x);
                  DWORD trueAxis = 0;
                  if (fabsf(normalized.y) > highest)
                  {
                    highest = fabsf(normalized.y);
                    trueAxis = 1;
                  }
                  if (fabsf(normalized.z) > highest)
                  {
                    highest = fabsf(normalized.z);
                    trueAxis = 2;
                  }

                  if (meshes[i].GetSize(axis) < meshes[j].GetSize(axis))
                  {
                    //if ((meshes[j].GetSize(axis) *0.8f)>meshes[i].GetSize(axis))
                    {
                      if (axis == trueAxis && meshes[i].inside(&meshes[j], axis) && !meshes[i].IsSelected())
                      {
                        SelectObjectOnly(&meshes[i], true);

                        D3DXVec3Normalize(&normalized, &faceNormal);
                        normalized.x *= -0.8f;//-distance;
                        normalized.y *= -0.8f;//-distance;
                        normalized.z *= -0.8f;//-distance;
                        meshes[i].MoveRelative(&normalized);
                      }
                    }
                  }
                  else if (meshes[i].GetSize(axis) > meshes[j].GetSize(axis))
                  {
                    //if ((meshes[i].GetSize(axis) *0.8f) > meshes[j].GetSize(axis))
                    {
                      if (axis == trueAxis && meshes[j].inside(&meshes[i], axis) && !meshes[j].IsSelected())
                      {
                        SelectObjectOnly(&meshes[j], true);

                        D3DXVec3Normalize(&normalized, &dummy);
                        normalized.x *= -0.8f;//-distance;
                        normalized.y *= -0.8f;//-distance;
                        normalized.z *= -0.8f;//-distance;
                        meshes[j].MoveRelative(&normalized);
                      }
                    }
                  }
                  else if (meshes[i].GetNumVertices() < 4)
                  {
                    if (axis == trueAxis && meshes[i].inside(&meshes[j], axis) && !meshes[i].IsSelected())
                    {
                      SelectObjectOnly(&meshes[i], true);

                      D3DXVec3Normalize(&normalized, &faceNormal);
                      normalized.x *= -0.8f;//-distance;
                      normalized.y *= -0.8f;//-distance;
                      normalized.z *= -0.8f;//-distance;
                      meshes[i].MoveRelative(&normalized);
                    }
                  }
                }
                else
                {
                  if (meshes[i].GetSize(axis) < meshes[j].GetSize(axis))
                  {
                    //if ((meshes[j].GetSize(axis) *0.8f) > meshes[i].GetSize(axis))
                    {
                      float highest = fabsf(normalized.x);
                      DWORD trueAxis = 0;
                      if (fabsf(normalized.y) > highest)
                      {
                        highest = fabsf(normalized.y);
                        trueAxis = 1;
                      }
                      if (fabsf(normalized.z) > highest)
                      {
                        highest = fabsf(normalized.z);
                        trueAxis = 2;
                      }


                      if (axis == trueAxis && meshes[i].inside(&meshes[j], axis) && !meshes[i].IsSelected())
                      {
                        SelectObjectOnly(&meshes[i], true);

                        D3DXVec3Normalize(&normalized, &faceNormal);
                        normalized.x *= -0.8f;//-distance;
                        normalized.y *= -0.8f;//-distance;
                        normalized.z *= -0.8f;//-distance;
                        meshes[i].MoveRelative(&normalized);
                      }
                    }
                  }
                }
              }
              else if (meshes[j].IsFlat(&dummy) && meshes[i].GetNumVertices() <= 6)
              {
                if (meshes[j].GetSize(axis) < meshes[i].GetSize(axis))
                {
                  //if ((meshes[i].GetSize(axis) *0.8f) > meshes[j].GetSize(axis))
                  {
                    float highest = fabsf(normalized.x);
                    DWORD trueAxis = 0;
                    if (fabsf(normalized.y) > highest)
                    {
                      highest = fabsf(normalized.y);
                      trueAxis = 1;
                    }
                    if (fabsf(normalized.z) > highest)
                    {
                      highest = fabsf(normalized.z);
                      trueAxis = 2;
                    }


                    if (axis == trueAxis && meshes[j].inside(&meshes[i], axis) && !meshes[j].IsSelected())
                    {
                      SelectObjectOnly(&meshes[j], true);

                      D3DXVec3Normalize(&normalized, &faceNormal);
                      normalized.x *= -0.8f;//-distance;
                      normalized.y *= -0.8f;//-distance;
                      normalized.z *= -0.8f;//-distance;
                      meshes[j].MoveRelative(&normalized);
                    }
                  }
                }
              }

              /*if(meshes[i].GetNumVertices()<=16 && (meshes[i].IsFlat(&faceNormal)))
              {
                if(meshes[j].GetNumVertices()<=16 && meshes[j].IsFlat(&dummy))
                {*/
                  //SelectObjectOnly(&meshes[i], true);
                  //MessageBox(0,"wuut?","wuut?",0);
                  //MessageBox(0,meshes[i].GetName().GetString(),meshes[j].GetName().GetString(),0);

                  /* D3DXVec3Normalize(faceNormal,faceNormal);
                  D3DXVECTOR3 center = meshes[i].GetCenter();

                  faceNormal->x*=20.0f-distance+center.x;
                  faceNormal->y*=20.0f-distance+center.y;
                  faceNormal->z*=20.0f-distance+center.z;

                  meshes[i].Move(faceNormal);*/
                  /*D3DXVec3Normalize(&normalized,&faceNormal);
                  normalized.x*=0.1f-distance;
                  normalized.y*=0.1f-distance;
                  normalized.z*=0.1f-distance;

                  meshes[i].MoveRelative(&normalized);*/
                /*}
                else
                {
                  //SelectObjectOnly(&meshes[i], true);
                  //MessageBox(0,meshes[i].GetName().GetString(),NULL,0);
                  float highest = fabsf(normalized.x);
                  DWORD trueAxis = 0;
                  if (fabsf(normalized.y) > highest)
                  {
                    highest = fabsf(normalized.y);
                    trueAxis = 1;
                  }
                  if (fabsf(normalized).z > highest)
                  {
                    highest = fabsf(normalized.z);
                    trueAxis = 2;
                  }
                  if (axis == trueAxis && !meshes[i].IsSelected())
                  {
                    SelectObjectOnly(&meshes[i], true);

                    D3DXVec3Normalize(&normalized, &faceNormal);
                    normalized.x *= 0.1f;//-distance;
                    normalized.y *= 0.1f;//-distance;
                    normalized.z *= 0.1f;//-distance;
                    meshes[i].MoveRelative(&normalized);
                  }
                }
              }*/
              /*else if(meshes[j].GetNumVertices()<=16 && (meshes[j].IsFlat(&faceNormal)))
              {
                //SelectObjectOnly(&meshes[j], true);
                //MessageBox(0,meshes[j].GetName().GetString(),NULL,0);
                float highest = fabsf(normalized.x);
                DWORD trueAxis = 0;
                if (fabsf(normalized.y > highest))
                {
                  highest = fabsf(normalized.y);
                  trueAxis = 1;
                }
                if (fabsf(normalized.z > highest))
                {
                  highest = fabsf(normalized.z);
                  trueAxis = 2;
                }
                if (axis == trueAxis && !meshes[j].IsSelected())
                {
                  SelectObjectOnly(&meshes[i], true);
                  D3DXVec3Normalize(&normalized, &faceNormal);
                  normalized.x *= 0.1f;//-distance;
                  normalized.y *= 0.1f;//-distance;
                  normalized.z *= 0.1f;//-distance;

                  meshes[j].MoveRelative(&normalized);
                }
              }*/
            }
          }
        }
      }
    }
    //delete normalized;
    //delete faceNormal;
  }

  void CenterScene()
  {
    BBox worldBox = meshes[0].GetBBox();

    DWORD numObjects = meshes.size();

    for(DWORD i=1; i<numObjects; i++)
    {
      worldBox.refitBBox(meshes[i].GetBBox());
    }

    D3DXVECTOR3 worldCenter = ((*(D3DXVECTOR3*)&worldBox.Max+*(D3DXVECTOR3*)&worldBox.Min)/2.0f);

    for(DWORD i=0; i<numObjects; i++)
    {
      meshes[i].MoveRelativeNeg(&worldCenter);
    }

    for(DWORD i=0, numNodes = nodes.size(); i<numNodes; i++)
    {
      *(D3DXVECTOR3*)&nodes[i].Position-=worldCenter;
    }

    /*if(skyDome)
    skyDome->CenterScene();*/
  }

  void MoveObjects(Scene* scene, DWORD skyName)
  {
    matList.ReleaseTextures();
    extern vector <Script> Scripts;
    Texture texture;
    ZeroMemory(&texture,sizeof(Texture));
    texture.vAddress = D3DTADDRESS_WRAP;
    texture.uAddress = D3DTADDRESS_WRAP;
    texture.blendMode = 20;
    char chc[128]="";

    for(DWORD i=0; i<meshes.size(); i++)
    {
      char* tmp = meshes[i].GetName().GetString();

      char name[128] = "";
      DWORD j=0;
      while(tmp[j])
      {
        char c = tmp[j];
        if(c >= 'A' && c <= 'Z') c += 32;
        name[j] = c;
        j++;
      }
      if(meshes[i].GetName().checksum!=skyName && !strstr(name, "sky"))
      {
        for(DWORD j=0; j<meshes[i].matSplits.size(); j++)
        {
          MaterialSplit* matSplit = meshes[i].GetSplit(j);

          Material* tmpMat = matList.GetMaterial(matSplit->matId);
          if(tmpMat)
          {
            Texture* tmpTex = tmpMat->GetTexture(0);
            matSplit->matId = scene->matList.GetNumMaterials();
            Material* material = new Material();
            material->Reserve(tmpMat->GetNumTextures());
            material->drawOrder = tmpMat->drawOrder;
            material->transparent = tmpMat->transparent;
            material->animated = tmpMat->animated;
            DWORD id = tmpMat->GetMatId();
            material->SetMatId(id);
            id = tmpTex->GetTexId();
            texture.SetId(id);
            texture.CreateTexture();
            material->AddTexture2(&texture);
            scene->AddMaterial(material);
            delete material;
          }

          sprintf(chc,"Mesh%u",scene->GetNumMeshes());
          Checksum namec = this->meshes[i].GetName();
          if(!namec.GetString())
            Scripts.push_back((Script(namec.checksum, chc)));
          scene->nodes.push_back((EnvironmentObject(namec,0,0,true)));
          //this->meshes[i].node = &scene->nodes.back();
        }
        scene->CloneMesh(&meshes[i]);
        //meshes[i].DeinitCollision();
        meshes.erase(meshes.begin()+i);
      }
    }
    matList.CreateTextures();
  }

  void FixLink()
  {
    for (DWORD i = 0; i < nodes.size(); i++)
    {
      for (DWORD j = 0; j < nodes[i].Links.size(); j++)
      {
        if (nodes[i].Links[j] >= nodes.size())
        {
          MessageBox(0, "too high link", nodes[i].Name.GetString2(), 0);
          nodes[i].Links.erase(nodes[i].Links.begin() + j);
          j--;
        }
      }
    }
  }

  //void FixNodes();

  void AppendScript(const Checksum &name, const DWORD &appendSize, const DWORD &dataSize, const BYTE* const data)
  {
    BYTE tmpData[2500];
    for(DWORD i=0; i<KnownScripts.size(); i++)
    {
      if(KnownScripts[i].name.checksum==name.checksum)
      {
        KnownScript* script = &KnownScripts[i];
        DWORD overSize = dataSize-appendSize;

        DWORD appendIndex = script->size-(overSize);

        if(overSize)
          memcpy(tmpData, &script->func[appendIndex],overSize);
        script->size += appendSize;
        memcpy(&script->func[appendIndex],data,dataSize);
        if(overSize)
          memcpy(&script->func[script->size],tmpData,overSize);
        if(script->size>325)
          MessageBox(0,"Script Too Big...",NULL,0);
        break;
      }
    }
  }

  void AppendScript(const DWORD &index, const DWORD &appendSize, const DWORD &dataSize, const BYTE* const data)
  {
    BYTE tmpData[2500];
    KnownScript* script = &KnownScripts[index];
    DWORD overSize = dataSize-appendSize;

    DWORD appendIndex = script->size-(overSize);

    if(overSize)
      memcpy(tmpData, &script->func[appendIndex],overSize);
    script->size += appendSize;
    memcpy(&script->func[appendIndex],data,dataSize);
    if(overSize)
      memcpy(&script->func[script->size],tmpData,overSize);
    if(script->size>325)
      MessageBox(0,"Script Too Big...",NULL,0);
  }


  void CreateTextures();
  void SortMeshes();
  void sort();

  Scene()
  {
    triggers.push_back((Checksum("LoadTerrain")));
    cullMode = D3DCULL_NONE;
    ZeroMemory(name,MAX_PATH);
    blendValue=0;
    //show = false;
    skyDome = NULL;
    D3DXMatrixIdentity(&matWorld);
    D3DXMatrixIdentity(&matSky);
    D3DXMatrixScaling(&matSky, 2.0f,2.0f,2.0f);
  }

  ~Scene()
  {
    meshes.clear();
    /*for(DWORD i=0; i<collision.size(); i++)
    {
    delete collision[i];
    }*/
    //collision.clear();
    nodes.clear();
    triggers.clear();
    KnownScripts.clear();
    globals.clear();
    //matList.~MaterialList();
  }
};

struct RwTextureExtension
{
    DWORD id;
    DWORD size;
    DWORD version;

    DWORD unknown[3];
    DWORD unknown2[3];
    DWORD unknown3;

    RwTextureExtension()
    {
        ZeroMemory(this, sizeof(RwTextureExtension));
        id = 0x294AF01;
        size = 0x1C;
        version = 0x310;
        unknown[0] = 0x30303030; unknown[1] = 0x30303030; unknown[2] = 0x30303030;
        unknown3 = 0xC0E9999A;
    }

    void Export(FILE* const __restrict f);
};

struct RwTexture : Texture
{
    DWORD structSize;
    DWORD imageSize;
    char name[128];
    DWORD extensionSize;
    RwTextureExtension extension;
    bool alphaMap;

    /*const DWORD GetFlags() const
    {
      return flags;
    }

    void SetFlags(const DWORD flags)
    {
      this->flags = flags;
    }*/

    void Export(FILE* const __restrict f);
    
    RwTexture()
    {
        ZeroMemory(this, sizeof(Texture));
        ZeroMemory(name, 128);
        /*DWORD flag = 0x106;
        SetFlags(flag);*/
        imageSize = 4;
        structSize = 0;
        extensionSize = 0x28;

    }

    /*virtual ~RwTexture()
    {

    }*/
};


struct RwMaterialExtension
{
    DWORD th3extensionSize;
    DWORD flags;
    int index;
    BYTE sound;
    BYTE alpha;


    RwMaterialExtension()
    {
        th3extensionSize = 119;
        flags = 0x010101UL;
        index = 0;
        sound = 1;
        alpha = 0;
    };

    void Export(FILE* const __restrict f);
};

struct RwMaterial : Material
{
    DWORD size;
    DWORD structSize;
    DWORD flags;
    DWORD textureSize;
    DWORD extensionSize;
    RwTexture texture;
    RwMaterialExtension extension;
    Animation anim;

    void Export(FILE* const __restrict f);

    RwMaterial()
    {
        //ZeroMemory(this,sizeof(Material));
        size = 0;
        structSize = 0;
        flags = 1;
        textureSize = 0;
        extensionSize = 0;
        ZeroMemory(&anim, sizeof(anim));
        extension.flags |= 0x2 << 24;
    }

    RwMaterial(Material* const __restrict material)
    {
        //ZeroMemory(this,sizeof(Material));
        size = 0;
        structSize = 0;
        flags = 1;
        textureSize = 0;
        extensionSize = 0;
        ZeroMemory(&anim, sizeof(anim));

        char* name = material->GetTexture(0)->GetName();
        if (name)
            sprintf(texture.name, "%s", name);
        else
            sprintf(texture.name, "%08X", material->GetTexture(0)->GetTexId());
        /*if (!strcmp("grate", name))
          MessageBox(0, name, "AW", 0);*/
        texture.SetId(material->GetTexture(0)->GetTexId());
        texture.imageSize = strlen(texture.name) + 1;
        texture.blendMode = material->GetTexture(0)->blendMode;
        /*texture.SetBlue(material->GetTexture(0)->GetBlue());
        texture.SetRed(material->GetTexture(0)->GetRed());
        texture.SetGreen(material->GetTexture(0)->GetGreen());*/
        texture.uAddress = material->GetTexture(0)->uAddress;
        texture.vAddress = material->GetTexture(0)->vAddress;
        /*if(material->transparent)
        {*/
        transparent = material->transparent;
        animated = material->animated;
        invisible = material->invisible;
        extraLayer = material->extraLayer;
        /*if(material->GetTexture(0)->fixedAlpha)
        extension.alpha=material->GetTexture(0)->fixedAlpha;*/
        if (transparent)
            extension.alpha = 0x44;
        extension.index = (int)material->drawOrder;
        if (animated)
        {
            extern Scene* __restrict scene;
            Animation* anim = scene->GetAnimation(material->GetMatId());
            if (anim)
            {
                this->anim = *anim;


                if (this->anim.UPhase)
                {
                    if (this->anim.UVel > 1.0f || this->anim.UVel < -1.0f)
                    {
                        this->anim.UPhase /= 1000.0f;
                        this->anim.UVel /= 1000.0f;
                    }
                    else
                    {
                        //this->anim.UPhase/=10.0f;
                        this->anim.UVel /= 10.0f;
                    }
                }
                if (this->anim.VPhase)
                {
                    if (this->anim.VVel > 1.0f || this->anim.VVel < -1.0f)
                    {
                        this->anim.VPhase /= 1000.0f;
                        this->anim.VVel /= 1000.0f;
                    }
                    else
                    {
                        //this->anim.VPhase/=10.0f;
                        this->anim.VVel /= 10.0f;
                    }
                }
                if (this->anim.UVel == 0.0f && this->anim.VVel == 0.0f)
                {
                    if (this->anim.UAmplitude || this->anim.UFrequency || this->anim.UPhase)
                        this->anim.UVel = 0.001f;
                    if (this->anim.VAmplitude || this->anim.VFrequency || this->anim.VPhase)
                        this->anim.VVel = 0.001f;
                }
            }
        }
        /*if(material->GetTexture(0)->fixedAlpha)
        extension.alpha=material->GetTexture(0)->fixedAlpha;
        else
        extension.alpha=0x44;*/
        /* extension.alpha=0xFF;
        }*/
        /*if(material->GetTexture(0)->fixedAlpha)
        extension.alpha=material->GetTexture(0)->fixedAlpha;
        else if(material->transparent)
        extension.alpha=0xFF;
        else
        extension.alpha=0;*/
        if (invisible)
            extension.flags |= 0x2 << 24;
        else if (material->transparent || material->doubled)//else if(material->doubled)
            extension.flags |= 0x8 << 24;
        if (material->reverse)
            extension.alpha = 0x68;
        this->ignoreAlpha = material->ignoreAlpha;
        this->texture.alphaMap = false;// material->alphaMap;
    }

    /*virtual ~RwMaterial()
    {
    texture.~RwTexture();
    }*/
};

struct expo
{
    char* name;
    DWORD uAddress;
    DWORD vAddress;
    expo(RwTexture* tex, char* name)
    {
        this->name = name;
        uAddress = tex->uAddress;
        vAddress = tex->vAddress;
    }

    expo(Texture* tex, char* name)
    {
        this->name = name;
        uAddress = tex->uAddress;
        vAddress = tex->vAddress;
    }
};



struct RwMaterialDictionary
{
    DWORD size;
    DWORD structSize;
    vector <DWORD> blend;
    vector <RwMaterial> materials;
    //DWORD numMaterials;
    vector <expo> alreadyExported;
    DWORD numCopied;

    RwMaterialDictionary()
    {
        size = 0;
        numCopied = 0;
        structSize = 0;
        //numMaterials=0;
    }

    bool AlreadyExported(RwTexture* tex, char** pName);



    void EraseMaterial(const DWORD index)
    {
        //numMaterials--;
        materials.erase(materials.begin() + index);
    }

    void ExportTDX(FILE* const __restrict f, bool sky = false);
   
    RwMaterialDictionary(MaterialList* const __restrict matList)
    {
        size = 0;
        structSize = 0;

        if (matList)
        {
            DWORD numMaterials = matList->GetNumMaterials();
            materials.reserve(numMaterials + 1);
            for (DWORD i = 0; i < numMaterials; i++)
            {
                Material* tmpMat = (Material*)matList->GetMaterial(i);
                if (tmpMat)
                    materials.push_back(RwMaterial(tmpMat));
                else
                    MessageBox(0, "WTF?", "WF?", 0);
            }
        }
        materials.push_back(RwMaterial());
    }

    void Export(FILE* const __restrict f);

    void SetSize()
    {
        structSize = 4 + (4 * materials.size());
        size = structSize + 12;
        for (DWORD i = 0; i < materials.size(); i++)
        {
            materials[i].structSize = 0x1C;
            materials[i].textureSize = (12 * 4) + 8 + materials[i].texture.imageSize + materials[i].texture.extensionSize;
            materials[i].extension.th3extensionSize += materials[i].textureSize;
            materials[i].extensionSize = materials[i].extension.th3extensionSize + 12;
            materials[i].texture.structSize = 4;
            materials[i].size = 36 + (12 * 7) + materials[i].texture.imageSize + materials[i].texture.extensionSize + materials[i].extensionSize;
            if (materials[i].texture.alphaMap)
                materials[i].size += materials[i].texture.imageSize - 4;
            size += materials[i].size + 12;
        }
    }
    ~RwMaterialDictionary()
    {
        blend.clear();
        materials.clear();
        alreadyExported.clear();
    }
};

struct TreeNodez
{
    BYTE type;
    WORD axis;
    WORD index;
    BBox bbox;
    WORD left;
    WORD right;
    vector<DWORD> faces;
    //vector<SimpleFace> faces;

    /*TreeNode(SimpleFace* faces, DWORD numFaces)
    {
    type = 1;
    //this->numFaces = numFaces;
    this->faces.resize(numFaces);
    std::copy(faces, faces+numFaces, this->faces.begin());
    axis = -1;
    }*/

    TreeNodez(const DWORD numFaces)
    {
        type = 1;
        this->faces.resize(numFaces);
        for (DWORD i = 0; i < numFaces; i++)
        {
            this->faces[i] = i;
        }
    }

    TreeNodez(const DWORD numFaces, const DWORD* __restrict const faces)
    {
        type = 1;
        this->faces.resize(numFaces);
        for (DWORD i = 0; i < numFaces; i++)
        {
            this->faces[i] = faces[i];
        }
        /*type = 1;
        //this->numFaces = numFaces;
        this->faces.resize(numFaces);
        for(DWORD i=0; i<numFaces; i++)
        {
        this->faces[i].a = faces[i].a;
        this->faces[i].b = faces[i].b;
        this->faces[i].c = faces[i].c;
        }
        /*this->faces.resize(numFaces);
        std::copy(faces, faces+numFaces, this->faces.begin());*/
        //axis = -1;*/
    }

    TreeNodez(const WORD axis)
    {
        type = 2;
        this->axis = axis;
        //numFaces=0;
        left = 0xFFFF;
        right = 0xFFFF;
        //faces=0;
        //branch.left = 0xFFFFFFFF;
        //branch.right = 0xFFFFFFFF;
    }
    TreeNodez()
    {
        type = 0;
        axis = 0;
        left = 0;
        right = 0;
    }

    ~TreeNodez()
    {
        if (type == 1)
            faces.clear();
    }
};

struct Th3Mesh : Mesh
{
    DWORD size;
    DWORD planeSize;
    bool secondAtomic;
    bool generateFromMesh;
    WORD meshVertexCount;
    DWORD structSize;
    DWORD th3ExtensionSize;
    DWORD extensionSize;
    DWORD effectSize;
    DWORD splitSize;
    DWORD collisionSize;
    DWORD numSplits;
    vector <Face> Polygons;
    DWORD collPolyCount;
    WORD numLeafs;
    WORD numBranches;
    vector<TreeNodez> CollNodes;


    WORD CreateTree(const BBox& bbox, const Face* __restrict const faces, const DWORD* __restrict const faceIndices, const DWORD numFaces, DWORD level);
    
    WORD CreateTree(const BBox& bbox, const Face* __restrict const faces, const DWORD numFaces, DWORD level = 1);
    

    DWORD numIndices;
    const MaterialSplit* const GetMatSplits() const
    {
        if (matSplits.size())
            return &matSplits.front();
        else
            MessageBox(0, "tried", "", 0);
        return NULL;
    }

    Th3Mesh()
    {
        secondAtomic = false;
        size = 0;
        structSize = 0;
        extensionSize = 0;
        effectSize = 0;
        splitSize = 0;
        collisionSize = 0;
        meshVertexCount = 0;
        //meshPolyCount=0;
        collPolyCount = 0;
        th3ExtensionSize = 0;
        numIndices = 0;
        numSplits = 0;
        generateFromMesh = false;
        numLeafs = 0;
        numBranches = 0;
    }

    void Export(FILE* const __restrict f);
    
    void GeneratePolygons(const MaterialSplit* const __restrict splits, Collision* const __restrict collision, DWORD* vertexIndices, bool renderShadow);
   
    Th3Mesh(Mesh* const __restrict mesh, const BYTE blendValue, const bool renderShadow = true);

    void SetSize()
    {
        /*numLeafs=0;
        DWORD numNodes = CollNodes.size();
        for(DWORD i=0; i<numNodes; i++)
        {
        if(CollNodes[i].type==1)
        {
        collPolyCount+=CollNodes[i].faces.size();
        numLeafs++;
        }
        }
        */
        /*DWORD numNodes = CollNodes.size();
        WORD numBranches = numNodes-numLeafs;*/
        th3ExtensionSize = ((15) + (2 * Polygons.size()));
        effectSize = 4;
        splitSize = 12;

        for (DWORD i = 0; i < numSplits; i++)
        {
            splitSize += ((matSplits[i].numIndices * 4));
            splitSize += 8;
        }

        extensionSize = splitSize + effectSize + th3ExtensionSize + (3 * 12);

        if (collPolyCount)
        {
            //collisionSize = (DWORD)collPolyCount*4+4+8;
            collisionSize = 8 + (DWORD)collPolyCount * 4 + numLeafs * 4 + numBranches * 16;
            extensionSize += collisionSize + 12;
        }

        structSize = ((vertices.size() * 12) + (vertices.size() * 4) + (vertices.size() * 8) + (Polygons.size() * 8) + (16) + (sizeof(BBox)) + (4));
        size = structSize + extensionSize + (12 * 2);
    }

    virtual ~Th3Mesh()
    {
        Polygons.clear();
        CollNodes.clear();
    }
};

struct RwChunkHeader
{
    DWORD id;
    DWORD size;
    DWORD version;

    RwChunkHeader()
    {
        id = 0;
        size = 0;
        version = 0x310;
    };
};

struct Th3LoadMesh
{
    BBox bbox;
    DWORD numPolygons;
    DWORD numVerts;
    vector<Vertex> verts;
    vector<Face> collision;

    void clear()
    {
        memset(this, 0, sizeof(Th3LoadMesh));
    }
};


struct Th3Scene : Scene
{
    DWORD size;
    DWORD structSize;
    DWORD numPolygons;
    DWORD numVertices;
    DWORD flags;
    vector <Th3Mesh> meshes;
    RwMaterialDictionary RWmatList;

    Th3LoadMesh mesh;

    void EraseMaterial(const DWORD index);

    void EraseMaterial(const DWORD index, DWORD newIndex);

    bool UnusedMaterial(const DWORD index);

    void OptimizeMaterials();
    DWORD ProcessMaterialChunk(DWORD ptr, DWORD end);
    DWORD ProcessChunkHeader(DWORD ptr);

    DWORD ProcessMesh(DWORD ptr, DWORD end);
    DWORD ProcessPlaneChunk(DWORD ptr, DWORD end);

    //Th3Scene(char* path);
    Th3Scene::Th3Scene(char* path, bool sky = false);

    void Export(FILE* const __restrict f);

    Th3Scene();

    void ProcessTDXParent(DWORD ptr, RwChunkHeader* parent);

    void ProcessTDXFile(DWORD ptr, DWORD eof);

    //Th3Scene(char* Path, bool sky);
Th3Scene(Scene* const __restrict scene);

    DWORD GetPlaneSize(const DWORD index) const;

    void SetSize();

    virtual ~Th3Scene()
    {
        meshes.clear();
    }
};
#endif
