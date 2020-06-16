#ifndef TH4_SCENE_H
#define TH4_SCENE_H

#include "Scene.h"
#include "ScnReader.h"
#pragma unmanaged

struct Grass
{
  DWORD matId;
  DWORD grassLayers;
  float grassHeight;

  Grass(DWORD id, DWORD layers, float height)
  {
    matId=id;
    grassLayers=layers;
    grassHeight=height;
  }
};

struct Th4Scene : public Scene
{
private:
  vector<Grass> grass;
  vector<ExtraLayerMesh> extraLayerMeshes;
  __declspec (noalias) void LoadProSkaterNodeArray(register const BYTE const* __restrict pFile, const BYTE const* __restrict eof, vector<KnownScript> &scripts, vector<Script> &sounds);
  //...

  Grass* GetGrass(DWORD matId)
  {
    for(DWORD i=0; i<grass.size(); i++)
    {
      if(grass[i].matId==matId)
        return &grass[i];
    }
    return NULL;
  }

  DWORD ProcessMaterials(DWORD ptr);

  void ProcessMeshes(DWORD ptr);
public:
  Th4Scene(char* Path, bool sky = false);
};

struct ThugScene : public Scene
{
private:
  vector<ExtraLayerMesh> extraLayerMeshes;
  //...
  DWORD ProcessMaterials(DWORD ptr, bool sky);

  void ProcessMeshes(DWORD ptr);
public:
  ThugScene(char* Path, bool sky = false);
};

struct Thug2Scene : public Scene
{
private:
  vector<ExtraLayerMesh> extraLayerMeshes;
  //...
  DWORD ProcessMaterials(DWORD ptr, bool sky);

  void ProcessMeshes(DWORD ptr);
public:
  Thug2Scene(char* Path, bool sky = false);
};
#endif