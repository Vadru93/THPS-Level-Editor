#ifndef  GLOBALS_H
#define GLOBALS_H

#define DIRECTINPUT_VERSION 0x0800
#include "Scene.h"
#include "Camera.h"
#include "d3d9.h"
#include "d3dx9.h"
#include "dinput.h"
#include "Material.h"
#include "Structs.h"

using namespace std;

namespace globals
{
  /*static LPDIRECT3DDEVICE9 Device = NULL;
  static LPDIRECT3D9 d3d = NULL;
  static D3DPRESENT_PARAMETERS DevParams = {0};
  static bool deviceLost = false;
  static DWORD MAX_PASSES = 1;
  static DWORD MAX_TEXCOORDS = 1;
  static Scene* scene = NULL;
  static D3DVIEWPORT9 port;
  static bool resetMouse = false;
  static DIMOUSESTATE mousestate = {0};
  static HWND handleWindow = NULL;
  static Camera* camera = NULL;
  static bool renderCollision = false;
  static BYTE selectionMode = 0;
  static const float bias = -1.0f;
  static DWORD defaultCulling = D3DCULL_NONE;
  static DWORD alphaRef = 8;
  static MaterialList* globalMaterialList = NULL;
  static vector <Script> Scripts;
  static TexFiles texFiles;*/
};
#endif