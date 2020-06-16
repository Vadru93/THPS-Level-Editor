#ifndef TH2X_SCENE_H
#define TH2X_SCENE_H

#include "Scene.h"
#include "ScnReader.h"
#include "Th2Scene.h"
#pragma unmanaged
struct Th2xScene : public Scene
{
private:
  //...
  void LoadDDM(DWORD ptr, bool sky);
  void ProcessMaterials(DWORD ptr, DWORD numMeshes, DWORD* meshNames, bool sky);

  void ProcessMeshes(BYTE* pFile, DWORD size, bool sky);
  void ProcessTextures(DWORD ptr, bool sky);
  void ProcessXBOXTextures(DWORD ptr, bool sky);
  void ProcessXBOXMaterials(DWORD ptr, bool sky);
public:
  Th2xScene(char* Path, bool sky = false);
};

#endif