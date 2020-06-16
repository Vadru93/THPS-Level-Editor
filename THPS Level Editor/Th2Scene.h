#ifndef TH2_SCENE_H
#define TH2_SCENE_H

#include "Scene.h"
#include "ScnReader.h"
#include "test.h"
#pragma unmanaged


struct Th2Scene : public Scene
{
private:
  //...
  void ProcessMaterials(DWORD ptr, DWORD numMeshes, DWORD* meshNames, bool sky);

  void ProcessMeshes(DWORD ptr);
  void ProcessTextures(DWORD ptr, bool sky);
  void ProcessXBOXTextures(DWORD ptr, bool sky);
  void ProcessXBOXMaterials(DWORD ptr, bool sky);
public:
  Th2Scene(char* Path, bool sky = false);
};

#endif