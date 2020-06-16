#include <windows.h>
#include <dinput.h>
#include "Camera.h"
#include "scene.h"

#define DIRECTINPUT_VERSION 0x0800
#define FORWARD 1
#define BACKWARD 2
#define LEFT 4
#define RIGHT 8
#define MOUSE 16
#define SELECTBYOBJECT 0
#define SELECTBYTRI 1
#define SELECTBYMATERIAL 3

void InitInputDevices(HINSTANCE hInstance, HWND hWnd);
void Update(const DWORD dt);
void UpdateAndRender();
void UpdateFPS();
void Update();
void CleanupInputDevices(void);
void UndoEvent();
void CleanupEvents();
void ChangeCollisionEvent(DWORD collisionType);
void GetMouseRay(D3DXVECTOR3* outRayDirection, D3DXVECTOR3* outRayOrigin);

struct MoveEvent
{
  DWORD numVertices;
  Vertex* pVertices;
  Vertex* pOldVertices;

  MoveEvent(Mesh* mesh)
  {
    numVertices = mesh->GetNumVertices();
    pVertices = (Vertex*)mesh->GetVertices();
    pOldVertices = new Vertex[numVertices];
    memcpy(pOldVertices,pVertices,sizeof(Vertex)*numVertices);
  }

  ~MoveEvent()
  {
    delete [] pOldVertices;
  }
};

struct Event
{
  static const enum types
  {
    eDelete,
    eAdd,
    eMove,
    eSelection,
    eUnselection,
  };
  BYTE type;
  vector <void*> data;
  Event(BYTE type, vector <SelectedObject> &objects)
  {
    this->type = type;
    DWORD numObjects = objects.size();
    if(type==eMove)
    {
      for(DWORD i=0; i<numObjects; i++)
      {
        data.push_back(new MoveEvent(objects[i].mesh));
      }
    }
    else
    {
      for(DWORD i=0; i<numObjects; i++)
      {
        data.push_back(objects[i].mesh);
      }
    }
  }

  ~Event()
  {
    if(type==eMove)
    {
      for(DWORD i=0; i<data.size(); i++)
      {
        delete [] ((MoveEvent*)data[i]);
      }
    }
  }
};