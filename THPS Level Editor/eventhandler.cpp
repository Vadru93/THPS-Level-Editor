#include "stdafx.h"
#define DIRECTINPUT_VERSION 0x0800
#include "eventhandler.h"
#include "Resource.h"
#include "player.h"
#pragma unmanaged
extern DIMOUSESTATE mousestate; extern HWND handleWindow; extern Camera* __restrict camera; extern D3DVIEWPORT9 port; extern Scene* __restrict scene; extern bool renderCollision; extern BYTE selectionMode; extern bool resetMouse; extern void ClearObjectList();

#define DIRECTINPUT_VERSION 0x0800

LPDIRECTINPUT8 din;
LPDIRECTINPUTDEVICE8 dinKeyboard;
LPDIRECTINPUTDEVICE8 dinMouse;
BYTE keystate[256];
DIMOUSESTATE oldMousestate;
vector <SelectedObject> selectedObjects;
vector <SelectedTri> selectedTriangles;
//vector <Node> addedNodes;
extern vector<SelectedMaterial> selectedMaterials;
vector <Event> events;
POINT mouse;
//bool oldz = false;
bool heldCntrl = true;
extern void SelectObject(DWORD index);
extern void SelectMaterial(DWORD meshIndex);
extern void UpdateCollisionInfo();
void SelectedAlreadySelected(DWORD index);
extern void SelectNode();
extern void DeleteSelectedNodes();
bool snapping = false;
//bool redraw=false;
bool addingNode = false;
bool reverselinking = false;
bool addedRail = false;
CollisionResult lastHit;

inline bool MouseMoved()
{
  if (mousestate.lX || mousestate.lY)
  {
    return true;
  }
  return false;
}

__declspec(noalias) void DrawFrame();
__declspec(noalias) void DrawFPSFrame();

void UndoEvent(const BYTE direction)
{
  //redraw=true;
  int currentEvent = (int)events.size() - 1;

  if (currentEvent != -1)
  {
    switch (events[currentEvent].type)
    {
    case Event::eDelete:
      for (DWORD i = 0; i < events[currentEvent].data.size(); i++)
      {
        ((Mesh*)events[currentEvent].data[i])->SetFlag(MeshFlags::deleted, false);
        ((Mesh*)events[currentEvent].data[i])->SetFlag(MeshFlags::visible, true);
        selectedObjects.push_back(SelectedObject(((Mesh*)events[currentEvent].data[i]), false));
      }
      break;
    case Event::eSelection:
    {
                            selectedObjects.pop_back();
                            break;
    }
    case Event::eUnselection:
    {
                              selectedObjects.clear();
                              for (DWORD i = 0; i < events[currentEvent].data.size(); i++)
                              {
                                selectedObjects.push_back(SelectedObject(((Mesh*)events[currentEvent].data[i]), false));
                              }
                              break;
    }
    }
  }
  events.pop_back();
  extern bool forceRedraw;
  if (!forceRedraw)
    DrawFrame();
}

inline void GetMouseRay(D3DXVECTOR3* outRayDirection, D3DXVECTOR3* outRayOrigin)
{
  POINT Mouse;
  GetCursorPos(&Mouse);
  ScreenToClient(handleWindow, &Mouse);

  D3DXMATRIX Proj = camera->proj();
  D3DXVECTOR3 mousePos((((2.0f*(float)Mouse.x) / (float)port.Width) - 1.0f) / Proj._11, -(((2.0f*(float)Mouse.y) / (float)port.Height) - 1.0f) / Proj._22, 1.0f);

  D3DXMATRIX invView;
  D3DXMatrixInverse(&invView, NULL, &camera->view());

  outRayDirection->x = mousePos.x*invView._11 + mousePos.y*invView._21 + mousePos.z*invView._31;
  outRayDirection->y = mousePos.x*invView._12 + mousePos.y*invView._22 + mousePos.z*invView._32;
  outRayDirection->z = mousePos.x*invView._13 + mousePos.y*invView._23 + mousePos.z*invView._33;
  //D3DXVec3Normalize(&rayDirection,&rayDirection);

  outRayOrigin->x = invView._41;
  outRayOrigin->y = invView._42;
  outRayOrigin->z = invView._43;
}

void GetMouseRay(D3DXVECTOR3* outRayDirection, D3DXVECTOR3* outRayOrigin, const D3DXMATRIX *world)
{
  POINT Mouse;
  GetCursorPos(&Mouse);
  ScreenToClient(handleWindow, &Mouse);

  D3DXMATRIX Proj = camera->proj();
  D3DXVECTOR3 mousePos((((2.0f*(float)Mouse.x) / (float)port.Width) - 1.0f) / Proj._11, -(((2.0f*(float)Mouse.y) / (float)port.Height) - 1.0f) / Proj._22, 1.0f);

  D3DXMATRIX invView, invWorld;
  D3DXMatrixInverse(&invView, NULL, &camera->view());

  outRayDirection->x = mousePos.x*invView._11 + mousePos.y*invView._21 + mousePos.z*invView._31;
  outRayDirection->y = mousePos.x*invView._12 + mousePos.y*invView._22 + mousePos.z*invView._32;
  outRayDirection->z = mousePos.x*invView._13 + mousePos.y*invView._23 + mousePos.z*invView._33;
  //D3DXVec3Normalize(&rayDirection,&rayDirection);

  outRayOrigin->x = invView._41;
  outRayOrigin->y = invView._42;
  outRayOrigin->z = invView._43;

  D3DXMatrixInverse(&invWorld, NULL, world);
  D3DXVec3TransformCoord(outRayOrigin, outRayOrigin, &invWorld);
  D3DXVec3TransformNormal(outRayDirection, outRayDirection, &invWorld);
}

/*void ChangeCollisionEvent(DWORD collisionType)
{
WORD collision;
if(collisionType==IDM_WALLRIDE)
collision = 0x0004;
else if(collisionType==IDM_SKATE)
collision = 0x0001;
else if(collisionType==IDM_RAMP)
collision = 0x0008;
else if(collisionType==IDM_NOCOL)
collision = 0x0010;

for(DWORD i=0; i<selectedTriangles.size(); i++)
{
if(selectedTriangles[i].collision)
{
selectedTriangles[i].collision->flags &= ~0x00FF;
selectedTriangles[i].collision->flags |= collision;
}
}
}*/

struct SelectedVertex
{
  Mesh* mesh;
  Vertex* vertex;

  SelectedVertex()
  {
    mesh = NULL;
    vertex = NULL;
  };
}; SelectedVertex selectedVertex;

void SelectVertex()
{
  selectedTriangles.clear();
  selectedObjects.clear();
  D3DXVECTOR3 rayOrigin, rayDirection;
  Vertex* vertex=NULL;
  Mesh* mesh;
  float distance = 0;

  GetMouseRay(&rayDirection, &rayOrigin);

  if (scene->GetClosestVertex(&rayOrigin, &rayDirection, vertex, mesh, &distance))
  {
    selectedVertex.mesh = mesh;
    selectedVertex.vertex = vertex;
  }
}

void SelectTriangle()
{
  D3DXVECTOR3 rayOrigin, rayDirection;
  float distance = 0;
  DWORD meshIndex = 0;
  DWORD faceIndex = 0;
  DWORD matIndex = 0;
  lastHit.mesh = NULL;

  GetMouseRay(&rayDirection, &rayOrigin);

  if (scene->Intersect(&rayOrigin, &rayDirection, &distance, &meshIndex, &matIndex, &faceIndex))
  {
    //printf("meshIndex %d matIndex %d faceIndex %d\n",meshIndex,matIndex,faceIndex);
    Mesh* mesh = scene->GetMesh(meshIndex);
    bool isSelected = false;

    if (mesh)
    {
      SelectObject(meshIndex);
      if (!renderCollision)
      {

        for (DWORD i = 0; i < selectedTriangles.size(); i++)
        {
          if (selectedTriangles[i].face == mesh->GetSplit(matIndex) && selectedTriangles[i].faceIndex == faceIndex)
          {
            isSelected = true;
            if (heldCntrl)
            {
              selectedTriangles.erase(selectedTriangles.begin() + i);
            }
            break;
          }
        }

        if (!isSelected)
        {
          MaterialSplit* matSplit = mesh->GetSplit(matIndex);
          const WORD* indices = &matSplit->Indices.front();
          WORD v1, v2, v3;
          bool tmp = true;
          DWORD indexCount = 0;

          for (DWORD i = 0, numIndices = matSplit->numIndices; i < numIndices; i++)
          {

            if (tmp)
            {
              v1 = indices[i];
              v2 = indices[i + 1];
              v3 = indices[i + 2];
              tmp = false;
            }
            else
            {
              v1 = v3;
              v3 = indices[i + 2];
              tmp = true;
            }
            if (v1 != v2 && v1 != v3 && v2 != v3)
            {
              if (v1 == indices[faceIndex] && v2 == indices[faceIndex + 1] && v3 == indices[faceIndex + 2])
              {
                //printf("Face %d CollFlags 0x%X\n",indexCount,mesh->GetCollision()->GetFaceInfo(indexCount)->flags);
                break;
              }
              indexCount++;
            }
          }

          selectedTriangles.push_back(SelectedTri(matSplit, mesh->GetCollision()->GetFaceInfo(indexCount), faceIndex, mesh->GetVertices()));

          if (mesh->GetCollision() && mesh->PickCollision(&rayOrigin, &rayDirection, &distance, &faceIndex))
          {
            lastHit.mesh = mesh;
            lastHit.distance = distance;
            lastHit.matIndex = matIndex;
            lastHit.triIndex = faceIndex;
            selectedTriangles.back().collision = mesh->GetCollision()->GetFaceInfo(faceIndex);
            //selectedTriangles.back().faceIndex = faceIndex;//selectedTriangles.push_back(SelectedTri(matSplit, mesh->GetCollision()->GetFaceInfo(faceIndex), faceIndex, mesh->GetVertices()));
          }
        }
      }
      else
      {
        for (DWORD i = 0; i < selectedTriangles.size(); i++)
        {
          if (selectedTriangles[i].face == mesh->GetCollision()->GetFace(faceIndex) && selectedTriangles[i].faceIndex == 0xFFFFFFFF)
          {
            isSelected = true;
            if (heldCntrl)
            {
              selectedTriangles.erase(selectedTriangles.begin() + i);
            }
            break;
          }
        }
        const Vertex* const vertices = mesh->GetCollision()->GetNumVertices() == 0 ? mesh->GetVertices() : mesh->GetCollision()->GetVertices();

        if (!isSelected)
        {
          selectedTriangles.push_back(SelectedTri(mesh->GetCollision()->GetFace(faceIndex), mesh->GetCollision()->GetFaceInfo(faceIndex), vertices));
          lastHit.mesh = mesh;
          lastHit.distance = distance;
          lastHit.matIndex = matIndex;
          lastHit.triIndex = faceIndex;
        }
      }
    }
    else if (!heldCntrl)
    {
      selectedObjects.clear();
    }
  }
}

const Mesh* GetClosestMesh(DWORD* outMeshIndex = 0)
{
  D3DXVECTOR3 rayOrigin, rayDirection;
  //float distance = 0.f;
  //DWORD meshIndex = 0;

  GetMouseRay(&rayDirection, &rayOrigin);

  /* D3DXVECTOR3 rayOrigin((float)Mouse.x,(float)Mouse.y,0.0f);
  D3DXVECTOR3 rayDirection((float)Mouse.x,(float)Mouse.y,1.0f);
  D3DXMATRIX matWorld;
  D3DXMatrixIdentity(&matWorld);
  D3DXVec3Unproject(&rayOrigin,&rayOrigin,&port,&camera->proj(),&camera->view(),&matWorld);
  D3DXVec3Unproject(&rayDirection,&rayDirection,&port,&camera->proj(),&camera->view(),&matWorld);
  rayDirection -= rayOrigin;
  // D3DXVec3Normalize(&rayDir,&rayDir);*/

  CollisionResult result;
  Ray ray(rayOrigin, rayDirection);
  if (scene->Intersect(ray, result))
  {
    if (outMeshIndex)
      *outMeshIndex = (DWORD)((BYTE*)result.mesh - (BYTE*)scene->GetMesh(0)) / sizeof(Mesh);
    return result.mesh;
  }

  /*if(scene->Intersect(&rayOrigin, &rayDirection, &distance, &meshIndex))
  {
  if(outMeshIndex)
  *outMeshIndex = meshIndex;
  return scene->GetMesh(meshIndex);
  }*/

  return NULL;
}

void CleanupEvents()
{
  selectedObjects.clear();
  selectedTriangles.clear();
  events.clear();
}

bool MouseIsOnRenderScene()
{
  GetCursorPos(&mouse);
  ScreenToClient(handleWindow, &mouse);

  if (mouse.x < 0 || mouse.x > port.Width || mouse.y < 0 || mouse.y > port.Height)
    return false;

  return true;
}

DWORD selectedId = 0xFFFFFFFF;

void AddSelection(const BYTE direction)
{
  extern bool changingMode;
  if (changingMode)
  {
    changingMode = !changingMode;
    return;
  }
  if (MouseIsOnRenderScene())
  {
    if (scene)
    {
      extern bool quickMaterialMode;
      if (quickMaterialMode)
      {
        extern bool updateMaterial;
        updateMaterial = keystate[DIK_LSHIFT] != 0;
        if (selectionMode == SELECTBYOBJECT)
        {
          D3DXVECTOR3 rayOrigin, rayDirection;
          //float distance = 0.f;
          //DWORD meshIndex = 0;

          GetMouseRay(&rayDirection, &rayOrigin);

          /* D3DXVECTOR3 rayOrigin((float)Mouse.x,(float)Mouse.y,0.0f);
          D3DXVECTOR3 rayDirection((float)Mouse.x,(float)Mouse.y,1.0f);
          D3DXMATRIX matWorld;
          D3DXMatrixIdentity(&matWorld);
          D3DXVec3Unproject(&rayOrigin,&rayOrigin,&port,&camera->proj(),&camera->view(),&matWorld);
          D3DXVec3Unproject(&rayDirection,&rayDirection,&port,&camera->proj(),&camera->view(),&matWorld);
          rayDirection -= rayOrigin;
          // D3DXVec3Normalize(&rayDir,&rayDir);*/

          CollisionResult result;
          Ray ray(rayOrigin, rayDirection);
          if (scene->Intersect(ray, result))
          {
            Mesh* mesh = (Mesh*)result.mesh;
            if (mesh)
            {
              DWORD meshIndex = (DWORD)((BYTE*)mesh - (BYTE*)scene->GetMesh(0)) / sizeof(Mesh);
              if (!updateMaterial && selectedId != 0xFFFFFFFF)
              {
                for (DWORD i = 0; i < mesh->GetNumSplits(); i++)
                {
                  MaterialSplit* matSplit = mesh->GetSplit(i);
                  matSplit->matId = selectedId;
                }
              }
              else
              {
                heldCntrl = false;
                updateMaterial = true;
                selectedId = mesh->GetSplit(result.matIndex)->matId;
                if (!result.mesh->IsSelected())
                  SelectObject(meshIndex);
              }
            }
          }
        }
        else if (selectionMode == SELECTBYMATERIAL)
        {
          D3DXVECTOR3 rayOrigin, rayDirection;
          //float distance = 0.f;
          //DWORD meshIndex = 0;

          GetMouseRay(&rayDirection, &rayOrigin);

          /* D3DXVECTOR3 rayOrigin((float)Mouse.x,(float)Mouse.y,0.0f);
          D3DXVECTOR3 rayDirection((float)Mouse.x,(float)Mouse.y,1.0f);
          D3DXMATRIX matWorld;
          D3DXMatrixIdentity(&matWorld);
          D3DXVec3Unproject(&rayOrigin,&rayOrigin,&port,&camera->proj(),&camera->view(),&matWorld);
          D3DXVec3Unproject(&rayDirection,&rayDirection,&port,&camera->proj(),&camera->view(),&matWorld);
          rayDirection -= rayOrigin;
          // D3DXVec3Normalize(&rayDir,&rayDir);*/

          CollisionResult result;
          Ray ray(rayOrigin, rayDirection);
          if (scene->Intersect(ray, result))
          {
            Mesh* mesh = (Mesh*)result.mesh;
            if (mesh)
            {
              lastHit = result;
              DWORD meshIndex = (DWORD)((BYTE*)mesh - (BYTE*)scene->GetMesh(0)) / sizeof(Mesh);
              if (!updateMaterial && selectedId != 0xFFFFFFFF)
              {
                mesh->GetSplit(result.matIndex)->matId = selectedId;
              }
              else
              {
                heldCntrl = false;
                updateMaterial = true;
                selectedId = mesh->GetSplit(result.matIndex)->matId;
                if (!mesh->GetSplit(result.matIndex)->IsSelected())
                  SelectMaterial(meshIndex);
              }
            }
          }
        }
        /* else if(selectionMode==SELECTBYTRI)
         {
         D3DXVECTOR3 rayOrigin, rayDirection;

         GetMouseRay(&rayDirection, &rayOrigin);


         CollisionResult result;
         Ray ray(rayOrigin, rayDirection);
         if(scene->Intersect(ray, result))
         {
         Mesh* mesh = (Mesh*)result.mesh;
         if(mesh)
         {
         DWORD meshIndex = (DWORD)((BYTE*)mesh-(BYTE*)scene->GetMesh(0))/sizeof(Mesh);
         if(!updateMaterial && selectedId != 0xFFFFFFFF)
         {
         //mesh->GetSplit(result.matIndex)->matId=selectedId;
         for(DWORD i=0; i<mesh->GetNumSplits(); i++)
         {
         MaterialSplit* matSplit = mesh->GetSplit(i);
         if(matSplit->matId==selectedId)
         {
         MaterialSplit* tmpSplit = mesh->GetSplit(result.matIndex);
         matSplit->Indices.push_back(tmpSplit->Indices[result.triIndex]);
         matSplit->Indices.push_back(tmpSplit->Indices[result.triIndex+1]);
         matSplit->Indices.push_back(tmpSplit->Indices[result.triIndex+2]);
         matSplit->numIndices+=3;
         goto found;
         }
         }
         found:;
         }
         else
         {
         heldCntrl = false;
         updateMaterial=true;
         selectedId=mesh->GetSplit(result.matIndex)->matId;
         if(!result.mesh->IsSelected())
         SelectObject(meshIndex);
         }
         }
         }
         }*/
        return;
      }
      //redraw=true;
      if (!keystate[DIK_LCONTROL])
      {
        heldCntrl = false;
        events.push_back(Event(Event::eUnselection, selectedObjects));
        //selectedObjects.clear();
        selectedTriangles.clear();
      }

      /*DWORD lastSelectionSize = selectedObjects.size();
      for(DWORD i=0; i<lastSelectionSize; i++)
      selectedObjects[i].mesh->ToggleFlag(MeshFlags::visible);*/
      if (selectionMode == SELECTBYOBJECT)
      {
        DWORD meshIndex;
        const Mesh* mesh = GetClosestMesh(&meshIndex);

        if (mesh)
        {

          /*selectedObjects.push_back(SelectedObject(mesh));
          events.push_back(Event(Event::eSelection, selectedObjects));*/
          if (mesh->IsSelected())
            SelectedAlreadySelected(meshIndex);
          else
            SelectObject(meshIndex);
          /* printf("Id %08X\n",mesh->GetName().checksum);
          printf("---------------\n\n");
          printf("Splits %d\n",mesh->GetNumSplits());
          printf("---------------\n\n");
          for(DWORD i=0; i<mesh->GetNumSplits(); i++)
          {
          printf("Split%d",i);
          MaterialSplit* tmpSplit = mesh->GetSplit(i);

          Material* tmpMat = (Material*)scene->matList.GetMaterial(tmpSplit->matId);
          if(tmpMat)
          {
          printf("Textures %d\n",tmpMat->GetNumTextures());
          printf("---------------\n");
          for(DWORD j=0; j<tmpMat->GetNumTextures(); j++)
          {
          printf("Texture%d\n",j);
          printf("---------------\n\n");
          Texture* tmpTex = tmpMat->GetTexture(j);
          printf("Id %08X flags %08X r %f g %f b %f blendMode %d\n",tmpTex->GetTexId(), tmpTex->GetFlags(), tmpTex->GetRed(), tmpTex->GetGreen(), tmpTex->GetBlue(), tmpTex->blendMode);
          printf("---------------\n");
          }
          printf("---------------\n");
          }
          printf("---------------\n");
          }
          printf("---------------\n");*/
        }
        else if (!heldCntrl)
        {
          selectedObjects.clear();
          ClearObjectList();
        }
      }
      else if (selectionMode == SELECTBYTRI)
      {
        SelectTriangle();
        UpdateCollisionInfo();
      }
      else if (selectionMode == SELECTBYMATERIAL)
      {
        D3DXVECTOR3 rayOrigin, rayDirection;
        //float distance = 0.f;
        //DWORD meshIndex = 0;

        GetMouseRay(&rayDirection, &rayOrigin);

        /* D3DXVECTOR3 rayOrigin((float)Mouse.x,(float)Mouse.y,0.0f);
        D3DXVECTOR3 rayDirection((float)Mouse.x,(float)Mouse.y,1.0f);
        D3DXMATRIX matWorld;
        D3DXMatrixIdentity(&matWorld);
        D3DXVec3Unproject(&rayOrigin,&rayOrigin,&port,&camera->proj(),&camera->view(),&matWorld);
        D3DXVec3Unproject(&rayDirection,&rayDirection,&port,&camera->proj(),&camera->view(),&matWorld);
        rayDirection -= rayOrigin;
        // D3DXVec3Normalize(&rayDir,&rayDir);*/

        CollisionResult result;
        Ray ray(rayOrigin, rayDirection);
        if (scene->Intersect(ray, result))
        {
          Mesh* mesh = (Mesh*)result.mesh;
          if (mesh)
          {
            lastHit = result;
            DWORD meshIndex = (DWORD)((BYTE*)mesh - (BYTE*)scene->GetMesh(0)) / sizeof(Mesh);
            if (!mesh->GetSplit(result.matIndex)->IsSelected())
              SelectMaterial(meshIndex);
          }
          else if (!heldCntrl)
          {
            selectedMaterials.clear();
            ClearObjectList();
          }
        }
      }
      else
        SelectNode();
      /*for(DWORD i=0; i<lastSelectionSize; i++)
      selectedObjects[i].mesh->ToggleFlag(MeshFlags::visible);*/
      heldCntrl = true;
      extern bool forceRedraw;
      if (!forceRedraw)
        DrawFrame();
    }
  }
}

/*void Pick(HWND hWnd)
{
if((lastPick+175)<GetTickCount())
{
lastPick = GetTickCount();
POINT Mouse;
GetCursorPos(&Mouse);
ScreenToClient(hWnd,&Mouse);

D3DXMATRIX Proj = camera->proj();
D3DXVECTOR3 mousePos((((2.0f*(float)Mouse.x)/(float)port.Width)-1.0f)/Proj._11, -(((2.0f*(float)Mouse.y)/(float)port.Height)-1.0f)/Proj._22, 1.0f);

D3DXMATRIX invView;
D3DXMatrixInverse(&invView, NULL, &camera->view());
D3DXVECTOR3 rayOrigin, rayDirection;

rayDirection.x = mousePos.x*invView._11 + mousePos.y*invView._21 + mousePos.z*invView._31;
rayDirection.y = mousePos.x*invView._12 + mousePos.y*invView._22 + mousePos.z*invView._32;
rayDirection.z = mousePos.x*invView._13 + mousePos.y*invView._23 + mousePos.z*invView._33;
//D3DXVec3Normalize(&rayDirection,&rayDirection);

rayOrigin.x = invView._41;
rayOrigin.y = invView._42;
rayOrigin.z = invView._43;

float distance = 0;
DWORD meshIndex = 0;

if(scene->Intersect(&rayOrigin, &rayDirection, &distance, &meshIndex))
{
Mesh* mesh = scene->GetMesh(meshIndex);
if(!mesh->IsSelected())
{
selectedObjects.push_back(SelectedObject(mesh->GetName(), mesh));
}
}
else
selectedObjects.clear();
/*  float distance=0;
DWORD meshIndex = 0;

D3DXVECTOR3 rayPos((float)Mouse.x,(float)Mouse.y,0.0f);
D3DXVECTOR3 rayDir((float)Mouse.x,(float)Mouse.y,1.0f);
D3DXMATRIX matWorld;
D3DXMatrixIdentity(&matWorld);
D3DXVec3Unproject(&rayPos,&rayPos,&port,&camera->proj(),&camera->view(),&matWorld);
D3DXVec3Unproject(&rayDir,&rayDir,&port,&camera->proj(),&camera->view(),&matWorld);
rayDir -= rayPos;
D3DXVec3Normalize(&rayDir,&rayDir);

if(scene->Intersect(&rayPos, &rayDir, &distance, &meshIndex))
{
if(!scene->GetMesh(meshIndex)->IsSelected())
{
selectedObjects.push_back(SelectedObject(scene->GetMesh(meshIndex)->GetName(), scene->GetMesh(meshIndex)));
}
}*/
/*  }
}*/
/*
void FixSelection()
{
for(DWORD i=0; i<selectedObjects.size(); i++)
{
selectedObjects[i].mesh = scene->GetMesh(selectedObjects[i].name);
}
}*/

vector<RenderableRail> addedRails;
AddingRail addRail;
bool newRail = false;

DWORD nodeIndex = 0;

void AddNode()
{
  extern vector <RenderableRail> rails;
  if (keystate[DIK_LCONTROL])
  {
    D3DXVECTOR3 rayDir, rayPos;
    D3DXMATRIX world;
    D3DXMatrixIdentity(&world);
    GetMouseRay(&rayDir, &rayPos, &world);
    Ray ray(rayPos, rayDir);
    CollisionResult result;
    if (scene->GetHitPoint(ray, result))
    {
      if (addRail.addedAnchor)
      {
        addRail.hit = result.GetHitPoint();
        //addRail.hit.y += 5.0f;
        if (newRail)
          rails.push_back(RenderableRail(addRail, true));
        else
        {
          rails.back().AddRail(addRail);
        }
        newRail = false;

        extern IDirect3DDevice9Ex* __restrict Device;
        if (FAILED(Device->CreateVertexBuffer(rails.back().numVerts*sizeof(ColouredVertex), D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &rails.back().verts, NULL)))
          MessageBox(0, "failed to create vertex buffer", "", 0);


        ColouredVertex* pVertices=NULL;
        rails.back().verts->Lock(0, 0, (void**)&pVertices, 0);
        memcpy(pVertices, &rails.back().vertices.front(), rails.back().numVerts*sizeof(ColouredVertex));
        rails.back().verts->Unlock();
        //addedRails.back().vertices.clear();

        /*static ColouredVertex vertex = ColouredVertex(0, 0, 255);
        vertex.x = addRail.anchor.x;
        vertex.y = addRail.anchor.y;
        vertex.z = addRail.anchor.z;
        addedRails.back().vertices.push_back(vertex);
        vertex.x = addRail.hit.x;
        vertex.y = addRail.hit.y;
        vertex.z = addRail.hit.z;
        addedRails.back().vertices.push_back(vertex);*/

        char name[256] = "";
        sprintf(name, "AddedNode%u", scene->nodes.size() + 1);
        if (nodeIndex == 0xFFFFFFFF)
          scene->nodes.push_back(RailNode(addRail.hit, Checksum(name), (scene->nodes.size() - 1)));
        else if (reverselinking)
          scene->nodes.push_back(RailNode(addRail.hit, Checksum(name), (scene->nodes.size() + 1)));
        else
          scene->nodes.push_back(RailNode(addRail.hit, Checksum(name), addedRail ? (scene->nodes.size()-1) : nodeIndex));
        addedRail = true;

        Device->CreateIndexBuffer(rails.back().numIndices * 2, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &rails.back().indices, NULL);


        WORD* idx=NULL;
        rails.back().indices->Lock(0, 0, (void**)&idx, 0);
        memcpy(idx, &rails.back().Indices.front(), rails.back().numIndices * 2);
        rails.back().numIndices /= 3;
        rails.back().indices->Unlock();
        addedRails.clear();
        //addedRails.back().Indices.clear();

        addRail.anchor = result.GetHitPoint();
        //addRail.anchor.y += 5.0f;
      }
      else
      {
        if (newRail)
        {
          scene->nodes.pop_back();
          if (reverselinking)
            scene->nodes[nodeIndex].Links.clear();
        }
        reverselinking = false;
        nodeIndex = 0xFFFFFFFF;

        newRail = true;
        addRail.addedAnchor = true;
        addRail.anchor = result.GetHitPoint();
        float closest = FLT_MAX;
        Node* node = NULL;
        for (DWORD j = 0; j < scene->nodes.size(); j++)
        {
          if (scene->nodes[j].Class.checksum == RailNode::GetClass())
          {
            float dist = D3DXVec3Length(&(addRail.anchor - *(D3DXVECTOR3*)&scene->nodes[j].Position));
            if (dist < 15.0f && dist < closest)
            {
              closest = dist;
              nodeIndex = j;
              node = &scene->nodes[j];
              MessageBox(0, node->Name.GetString(), "", 0);
            }
          }
        }

        if (node)
        {
          addRail.anchor = *(D3DXVECTOR3*)&node->Position;
          if (node->Links.size() == 0)
          {
            reverselinking = true;
            node->Links.push_back(scene->nodes.size());
          }
          else
          {
            for (DWORD j = 0; j < scene->nodes.size(); j++)
            {
              if (scene->nodes[j].Class.checksum == RailNode::GetClass())
              {
                for (DWORD k = 0; k < scene->nodes[j].Links.size(); k++)
                {
                  if (scene->nodes[j].Links[k] == nodeIndex)
                  {
                    reverselinking = false;
                    goto skip;
                  }
                }
              }
            }
          }
        }
        else
        {
        skip:
          char name[256] = "";
          sprintf(name, "AddedNode%u", scene->nodes.size() + 1);
          scene->nodes.push_back(RailNode(addRail.anchor, Checksum(name)));
          /*if (reverselinking)
          scene->nodes.back().Links.push_back(scene->nodes.size());
          else
          scene->nodes.back().Links.push_back(nodeIndex);*/
        }

        //addRail.anchor.y += 5.0f;
        /*if (node)
        {
          if (reverselinking)
            scene->nodes.back().Links.push_back(scene->nodes.size());
          else
            scene->nodes.back().Links.push_back(nodeIndex);
        }*/
      }
    }
  }
  else
  {
    D3DXVECTOR3 rayDir, rayPos;
    D3DXMATRIX world;
    D3DXMatrixIdentity(&world);
    GetMouseRay(&rayDir, &rayPos, &world);
    Ray ray(rayPos, rayDir);
    CollisionResult result;
    if (scene->GetHitPoint(ray, result))
    {
      if (newRail)
      {
        scene->nodes.pop_back();
        if (reverselinking)
          scene->nodes[nodeIndex].Links.clear();
      }
      reverselinking = false;
      nodeIndex = 0xFFFFFFFF;
      
      newRail = true;
      addRail.anchor = result.GetHitPoint();
      float closest = FLT_MAX;
      Node* node = NULL;
      for (DWORD j = 0; j < scene->nodes.size(); j++)
      {
        if (scene->nodes[j].Class.checksum == RailNode::GetClass())
        {
          float dist = D3DXVec3Length(&(addRail.anchor - *(D3DXVECTOR3*)&scene->nodes[j].Position));
          if (dist < 15.0f && dist < closest)
          {
            closest = dist;
            nodeIndex = j;
            node = &scene->nodes[j];
          }
        }
      }
      
      if (node)
      {
        addRail.anchor = *(D3DXVECTOR3*)&node->Position;
        if (node->Links.size() == 0)
        {
          reverselinking = true;
          node->Links.push_back(scene->nodes.size());
        }
        else
        {
          for (DWORD j = 0; j < scene->nodes.size(); j++)
          {
            if (scene->nodes[j].Class.checksum == RailNode::GetClass())
            {
              for (DWORD k = 0; k < scene->nodes[j].Links.size(); k++)
              {
                if (scene->nodes[j].Links[k] == nodeIndex)
                {
                  reverselinking = false;
                  goto skip2;
                }
              }
            }
          }
        }
      }
      else
      {
      skip2:
        char name[256] = "";
        sprintf(name, "AddedNode%u", scene->nodes.size() + 1);
        scene->nodes.push_back(RailNode(addRail.anchor, Checksum(name)));
        /*if (reverselinking)
          scene->nodes.back().Links.push_back(scene->nodes.size());
        else
          scene->nodes.back().Links.push_back(nodeIndex);*/
      }
      
      //addRail.anchor.y += 5.0f;
      addRail.addedAnchor = true;

      //MessageBox(0, "added anchor", "", 0);
    }
  }
}

void Update()
{
  if (FAILED(dinKeyboard->GetDeviceState(256, keystate)))
  {
    while (FAILED(dinKeyboard->Acquire()))
      Sleep(5);
    while (FAILED(dinMouse->Acquire()))
      Sleep(5);
    dinKeyboard->GetDeviceState(256, keystate);
  }
  dinMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mousestate);// mouse.x += mousestate.lX; mouse.y += mousestate.lY;

  oldMousestate.rgbButtons[0] = mousestate.rgbButtons[0];
}

//__declspec(noalias) void DrawFrame();

bool drawRail = false;

float frameDelta;
bool jumping = false;
bool jumped = false;
float jumpLimit = 10.0f;
//float jumpStart = 0.0f;
float maxHeight = 225.0f;
float startHeight = 0.0f;
float jumpTime = 0.0f;
Node* startNode;
Node* endNode;
bool spaceDown = false;
D3DXVECTOR3 forwardSpeed;

float d(float x1, float y1, float z1, float x2, float y2, float z2)
{
 return sqrt(((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)) + ((z1 - z2) * (z1 - z2)));
}

void UpdateFPS()
{
  extern bool update;
  extern bool render;
  static DWORD tLast = 0;
  const DWORD tNow = GetTickCount();
  const DWORD dt = tNow - tLast;
  tLast = tNow;

  if (FAILED(dinKeyboard->GetDeviceState(256, keystate)))
  {
    while (FAILED(dinKeyboard->Acquire()))
      Sleep(5);
    while (FAILED(dinMouse->Acquire()))
      Sleep(5);
    dinKeyboard->GetDeviceState(256, keystate);
  }
  dinMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mousestate);

  if (render && scene)
  {
    extern Player* __restrict player;
    const float delta = (float)dt;
    if (mousestate.rgbButtons[0] && !oldMousestate.rgbButtons[0])
    {
      oldMousestate.rgbButtons[0] = mousestate.rgbButtons[0];

      D3DXVECTOR3 rayOrigin, rayDirection;
      //float distance = 0.f;
      //DWORD meshIndex = 0;

      GetMouseRay(&rayDirection, &rayOrigin);

      /* D3DXVECTOR3 rayOrigin((float)Mouse.x,(float)Mouse.y,0.0f);
      D3DXVECTOR3 rayDirection((float)Mouse.x,(float)Mouse.y,1.0f);
      D3DXMATRIX matWorld;
      D3DXMatrixIdentity(&matWorld);
      D3DXVec3Unproject(&rayOrigin,&rayOrigin,&port,&camera->proj(),&camera->view(),&matWorld);
      D3DXVec3Unproject(&rayDirection,&rayDirection,&port,&camera->proj(),&camera->view(),&matWorld);
      rayDirection -= rayOrigin;
      // D3DXVec3Normalize(&rayDir,&rayDir);*/

      CollisionResult result;
      Ray ray(rayOrigin, rayDirection);
      if (scene->CastRay(ray, result))
      {
        D3DXVECTOR3 endPos = *(D3DXVECTOR3*)&result.hitPoint;
      }
    }
    else
      oldMousestate.rgbButtons[0] = mousestate.rgbButtons[0];
    if (player->state == OnRail && startNode && endNode)
    {
      D3DXVECTOR3 endPos = *(D3DXVECTOR3*)&endNode->Position;
      endPos.y += 10.0f;
      float distance = D3DXVec3Length(&(player->position - endPos));
      if (distance < 10.0f)
      {
        startNode = endNode;
        for (DWORD i = 0; i < endNode->Links.size(); i++)
        {
          if (scene->nodes[endNode->Links[i]].Class.checksum == RailNode::GetClass())
          {
            endNode = &scene->nodes[endNode->Links[i]];
            endPos = *(D3DXVECTOR3*)&endNode->Position;
            endPos.y += 10.0f;
            D3DXVECTOR3 direction = endPos - player->position;
            D3DXVec3Normalize(&player->velocity, &direction);
            player->velocity *= 1.0f;
            goto done;
          }
        }
        player->state = OnGround;
        startNode = NULL;
        endNode = NULL;
        player->velocity = D3DXVECTOR3(0, 0, 0);
      done:;
      }
      else
      if (startNode && endNode)
      {
        D3DXVECTOR3 direction = endPos - player->position;
        D3DXVec3Normalize(&player->velocity, &direction);
        player->velocity *= 1.0f;
      }
    }
    else if (keystate[DIK_I])
    {
      bool reverse = false;
      const DWORD numNodes = scene->nodes.size();
      Node* nodes = &scene->nodes.front();
      D3DXMATRIX nodeRotation;
      D3DXMATRIX nodeTranslation;
      D3DXVECTOR3 rayOrigin, rayDirection, translatedOrigin, translatedDirection;
      static float distance = 0.0f;
      static float closest = FLT_MAX;
      bool intersected = false;
      DWORD nodeIndex = 0;
      static DWORD numLinks = 0;
      static const D3DXMATRIX* matWorld = scene->GetWorldMatrix();
      D3DXMATRIX world, invWorld;
      rayOrigin = player->position;
      rayDirection = player->velocity;
      D3DXVECTOR3 hitPoint;



      for (DWORD i = 0; i < numNodes; i++)
      {
        if (nodes[i].Class.checksum == RailNode::GetClass())
        {
          D3DXVECTOR3 pos, pos2;
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
                  /*D3DXVECTOR3 & b = *(D3DXVECTOR3*)&nodes[nodes[i].Links[j]].Position;
                  D3DXVECTOR3 & a = *(D3DXVECTOR3*)&nodes[i].Position;
                  D3DXVECTOR3 n = b - a;
                  D3DXVECTOR3 pa = a - player->position;
                  D3DXVECTOR3 c = n * (D3DXVec3Dot(&pa, &n) / D3DXVec3Dot(&n, &n));
                  D3DXVECTOR3 d = pa - c;
                  float distance = sqrtf(D3DXVec3Dot(&d, &d));
                  if (distance < closest)
                  {
                    intersected = true;
                    closest = distance;
                    hitPoint = c;
                    nodeIndex = i;
                  }*/
                  float A = player->position.x - nodes[i].Position.x;
                  float B = player->position.y - nodes[i].Position.y;
                  float BE = player->position.z - nodes[i].Position.z;
                  float C = nodes[nodes[i].Links[j]].Position.x - nodes[i].Position.x;
                  float D = nodes[nodes[i].Links[j]].Position.y - nodes[i].Position.y;
                  float DE = nodes[nodes[i].Links[j]].Position.z - nodes[i].Position.z;

                  float dot = A * C + B * D + BE * DE;
                  float len_sq = C * C + D * D + DE * DE;
                  float param = dot / len_sq;

                  float xx, yy, zz;

                  if (param < 0)
                  {
                    xx = nodes[i].Position.x;
                    yy = nodes[i].Position.y;
                    zz = nodes[i].Position.z;
                  }
                  else if (param > 1)
                  {
                    xx = nodes[nodes[i].Links[j]].Position.x;
                    yy = nodes[nodes[i].Links[j]].Position.y;
                    zz = nodes[nodes[i].Links[j]].Position.z;
                  }
                  else
                  {
                    xx = nodes[i].Position.x + param * C;
                    yy = nodes[i].Position.y + param * D;
                    zz = nodes[i].Position.z + param * DE;
                  }
                  float dist = d(player->position.x, player->position.y, player->position.z, xx, yy, zz);//your distance function
                  if (dist < closest)
                  {
                    intersected = true;
                    closest = dist;
                    nodeIndex = i;
                    hitPoint.x = xx;
                    hitPoint.y = yy+10.0f;
                    hitPoint.z = zz;
                  }
                }
                break;
              }
            }
          }
        }
      }

      if (intersected && closest < 100.0f)
      {
        startNode = NULL;
        endNode = NULL;
        if (nodes[nodeIndex].Class.checksum == RailNode::GetClass())
        {
          if (nodes[nodeIndex].Links.size())
          {
            for (DWORD j = 0; j < nodes[nodeIndex].Links.size(); j++)
            {
              if (nodes[nodes[nodeIndex].Links[j]].Class.checksum == RailNode::GetClass())
              {
                startNode = &nodes[nodeIndex];
                endNode = &nodes[nodes[nodeIndex].Links[j]];
                goto found;
              }
            }
          }
          else
          {
            for (DWORD i = 0; i < numNodes; i++)
            {
              if (nodes[i].Class.checksum == RailNode::GetClass())
              {
                for (DWORD j = 0; j < nodes[i].Links.size(); j++)
                {
                  if (nodes[i].Links[j] == nodeIndex)
                  {
                    startNode = &nodes[i];
                    endNode = &nodes[nodeIndex];
                    goto found;
                  }
                }
              }
            }
          found:;
            if (startNode && endNode)
            {
              player->state = OnRail;
              jumped = false;
              jumping = false;
              player->position = hitPoint;
              D3DXVECTOR3 endPos = *(D3DXVECTOR3*)&endNode->Position;
              endPos.y += 10.0f;
              D3DXVECTOR3 direction = endPos - player->position;
              D3DXVec3Normalize(&player->velocity, &direction);
              player->velocity *= 1.0f;
            }
            else
              player->velocity = D3DXVECTOR3(0, 0, 0);
          }
          /*std::string info = nodes[nodeIndex].Name.GetString() + std::string(" ") + nodes[nodeIndex].Cluster.GetString() + std::string("\r\n");
          char tmp[256];
          if (nodes[nodeIndex].Links.size()) info.append(std::string(" [0]") + nodes[nodes[nodeIndex].Links[0]].Name.GetString());
          if (nodes[nodeIndex].Links.size() > 1) info.append(std::string(" [1]") + nodes[nodes[nodeIndex].Links[1]].Name.GetString());
          ::MessageBox(0, info.c_str(), info.c_str(), 0);*/
        }
      }
      if (player->state == OnGround)
      {
        if (keystate[DIK_W])
        {
          D3DXVec3Normalize(&player->velocity, &camera->right());
          D3DXMATRIX ROT;
          D3DXMatrixIdentity(&ROT);
          D3DXMatrixRotationY(&ROT, -(D3DX_PI / 2.0f));
          D3DXVec3TransformCoord(&player->velocity, &player->velocity, &ROT);
          player->velocity *= 0.7f;
        }
      }
    }
    else
    if (keystate[DIK_W] && player->state==OnGround)
    {
      D3DXVec3Normalize(&player->velocity, &camera->right());
      D3DXMATRIX ROT;
      D3DXMatrixIdentity(&ROT);
      D3DXMatrixRotationY(&ROT, -(D3DX_PI / 2.0f));
      D3DXVec3TransformCoord(&player->velocity, &player->velocity, &ROT);
      player->velocity *= 0.7f;
    }
    else if(!jumped)
    {
      /*if (player->state == InAir && jumped)
        player->velocity.y = 0.0f;
      else*/
        player->velocity = D3DXVECTOR3(0, 0, 0);
    }
    if (jumping)
    {
      if (player->state == InAir)
      {
        jumpTime += delta*0.01f;
        player->velocity = forwardSpeed*delta / jumpTime*0.01f;
        float currentHeight = (player->position.y - startHeight);
        if (/*jumpTime >= jumpLimit || */currentHeight >= maxHeight)
        {
          jumping = false;
          player->position.y -= (currentHeight - maxHeight);
          player->velocity.y = 0.0f;
        }
        else
          player->velocity.y = 2.5f*delta / jumpTime*0.8f;
      }
      else
      {
        jumping = false;
        player->velocity.y = 0.0f;
      }
    }
    else
    if (player->state != InAir && !spaceDown)
    {
      if (keystate[DIK_SPACE])
      {
        jumped = true;
        spaceDown = true;
        player->state = OnGround;
        forwardSpeed = player->velocity*2.0f;
        player->velocity.y = 0.8f;
        jumping = true;
        //jumpStart = tNow;
        startHeight = player->position.y;
        jumpTime = 0.0f;
      }
    }
    if (spaceDown && !keystate[DIK_SPACE])
      spaceDown = false;
    frameDelta += delta*0.001f;
    camera->updateFPS();
    player->Update(scene, delta);
    DrawFPSFrame();
  }
}

void UpdateAndRender()
{
  //static DWORD ticker = 0;
  static DWORD tLast = 0;
  const DWORD tNow = GetTickCount();
  const DWORD dt = tNow - tLast;
  tLast = tNow;
  /*char dts[25];
  sprintf(dts,"%u", dt);
  MessageBox(0,dts,dts,0);*/
  static BYTE direction = 0;
  /* dinKeyboard->Acquire();
  dinMouse->Acquire(); //ZeroMemory(keystate, 256 ); ZeroMemory(&mousestate,sizeof(DIMOUSESTATE));
  */
  if (FAILED(dinKeyboard->GetDeviceState(256, keystate)))
  {
    while (FAILED(dinKeyboard->Acquire()))
      Sleep(5);
    while (FAILED(dinMouse->Acquire()))
      Sleep(5);
    dinKeyboard->GetDeviceState(256, keystate);
  }
  dinMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mousestate);// mouse.x += mousestate.lX; mouse.y += mousestate.lY;

  extern bool update;
  extern bool render;
  if (keystate[DIK_K])
  {
    FILE* f = fopen("test.obj", "wt+");

    DWORD vcount = 1;
    for (DWORD i = 0; i < selectedObjects.size(); i++)
    {
      
      selectedObjects[i].mesh->ExportObj(f, vcount);
    }
    fclose(f);
  }
  if (update)
  {
    extern bool forceRedraw;
    if (keystate[DIK_W])// || keystate[DIK_UP])
      direction |= FORWARD;
    else if (keystate[DIK_S])// || keystate[DIK_DOWN])
      direction |= BACKWARD;
    if (keystate[DIK_A])// || keystate[DIK_LEFT])
      direction |= LEFT;
    else if (keystate[DIK_D])// || keystate[DIK_RIGHT])
      direction |= RIGHT;

    /*if(forceRedraw)
    {
    ticker++;
    frameDelta = (float)ticker*0.01666666666667f;0.0000238095238095249f
    if(direction || ((resetMouse || mousestate.rgbButtons[1]) && MouseMoved()))
    {
    camera->update((float)dt, direction);
    direction=0;
    }
    DrawFrame();
    }
    else if(direction || ((resetMouse || mousestate.rgbButtons[1]) && MouseMoved()))
    {
    camera->update((float)dt, direction);
    direction=0;
    DrawFrame();
    }*/

    if (addingNode)
    {
      if (keystate[DIK_LCONTROL])
      {
        if (MouseMoved())
        {
          D3DXVECTOR3 rayDir, rayPos;
          D3DXMATRIX world;
          D3DXMatrixIdentity(&world);
          GetMouseRay(&rayDir, &rayPos, &world);
          Ray ray(rayPos, rayDir);
          CollisionResult result;
          if (scene->GetHitPoint(ray, result))
          {
            if (addRail.addedAnchor)
            {
              addedRail = false;
              addRail.hit = result.GetHitPoint();

              if (addedRails.size())
                addedRails.clear();
              addedRails.push_back(RenderableRail(addRail));

              extern IDirect3DDevice9Ex* __restrict Device;
              if (FAILED(Device->CreateVertexBuffer(addedRails.back().numVerts*sizeof(ColouredVertex), D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &addedRails.back().verts, NULL)))
                MessageBox(0, "failed to create vertex buffer", "", 0);


              ColouredVertex* pVertices = NULL;
              addedRails.back().verts->Lock(0, 0, (void**)&pVertices, 0);
              memcpy(pVertices, &addedRails.back().vertices.front(), addedRails.back().numVerts*sizeof(ColouredVertex));
              addedRails.back().verts->Unlock();
              addedRails.back().vertices.clear();

              Device->CreateIndexBuffer(addedRails.back().numIndices * 2, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &addedRails.back().indices, NULL);
              WORD* idx=NULL;
              addedRails.back().indices->Lock(0, 0, (void**)&idx, 0);
              memcpy(idx, &addedRails.back().Indices.front(), addedRails.back().numIndices * 2);
              addedRails.back().numIndices /= 3;
              addedRails.back().indices->Unlock();
              addedRails.back().Indices.clear();

              DrawFrame();
            }
          }
        }
      }
      else if (addedRails.size())
      {
        addedRails.clear();
        addingNode = false;

        if (addedRail && reverselinking)
          scene->nodes.back().Links.clear();
        addedRail = false;
      }

    }

    if (direction || ((resetMouse || mousestate.rgbButtons[1]) && MouseMoved()))
    {
      float delta = (float)dt;
      frameDelta += delta*0.001f;
      if (keystate[DIK_LCONTROL])
        delta /= 2.0f;
      else if (keystate[DIK_LSHIFT])
        delta *= 2.0f;
      camera->update(delta, direction);
      direction = 0;
      DrawFrame();
      /*if(!renderCollision && scene)
      {
      scene->SortAlpha();
      FixSelection();
      }*/
    }
    else if (forceRedraw)
    {
      /*ticker++;
      frameDelta = (float)ticker*0.01666666666667f;*/
      frameDelta += (float)dt*0.001f;
      DrawFrame();
    }
    /*else if(redraw)
    {
    redraw=false;
    DrawFrame();
    }*/

    if (keystate[DIK_UP])
    {
      D3DXMATRIX invView;
      D3DXMatrixInverse(&invView, NULL, &camera->view());
      for (DWORD i = 0, numSelected = selectedObjects.size(); i < numSelected; i++)
      {
        selectedObjects[i].mesh->Move(&D3DXVECTOR3(invView._31, invView._32, invView._33));
      }
    }

    if (mousestate.rgbButtons[0] && !oldMousestate.rgbButtons[0])
    {
      oldMousestate.rgbButtons[0] = mousestate.rgbButtons[0];
      /*if(keystate[DIK_LSHIFT])
      {
      SelectVertex();
      snapping=true;
      }
      else*/
      if (addingNode)
        AddNode();
      else
        AddSelection(direction);
    }
    else if (keystate[DIK_DELETE])
    {
      if (selectionMode == SELECTBYOBJECT)
      {
        const DWORD selected = selectedObjects.size();
        if (selected)
        {
          /*for(DWORD i=0; i<scene->GetNumMeshes(); i++)
          {
          if(!scene->GetMesh(i)->IsSelected())
          {
          scene->GetMesh(i)->SetFlag(MeshFlags::visible,false);
          scene->GetMesh(i)->SetFlag(MeshFlags::deleted,true);
          }
          }*/
          for (DWORD i = 0; i < selected; i++)
          {
            selectedObjects[i].mesh->SetFlag(MeshFlags::visible, false);
            selectedObjects[i].mesh->SetFlag(MeshFlags::deleted, true);
          }
          events.push_back(Event(Event::eDelete, selectedObjects));
          selectedObjects.clear();
        }
      }
      else if (selectionMode != SELECTBYTRI)
      {
        DeleteSelectedNodes();
      }

    }
    else if (keystate[DIK_Z] && keystate[DIK_LCONTROL])
      UndoEvent(direction);
    else
      oldMousestate.rgbButtons[0] = mousestate.rgbButtons[0];
    /*else if(snapping)
    {
    if(!keystate[DIK_LSHIFT])
    {
    selectedVertex.mesh=NULL;
    selectedVertex.vertex=NULL;
    snapping=false;
    }
    else
    {
    if(selectedVertex.mesh)
    {
    MessageBox(0,0,0,0);
    selectedObjects.push_back(selectedVertex.mesh);
    selectedVertex.mesh->SetFlag(MeshFlags::visible, false);
    selectedVertex.vertex->x=0;
    }
    }
    }*/
    /*if(keystate[DIK_Z] && keystate[DIK_LCONTROL])
      UndoEvent();*/
  }
  else if (render)
    DrawFrame();
  else
    Sleep(200);
  //oldMousestate.rgbButtons[0] = mousestate.rgbButtons[0];
}

void Update(const DWORD dt)
{
  /*char dts[25];
  sprintf(dts,"%u", dt);
  MessageBox(0,dts,dts,0);*/
  static BYTE direction = 0;
  /* dinKeyboard->Acquire();
  dinMouse->Acquire(); //ZeroMemory(keystate, 256 ); ZeroMemory(&mousestate,sizeof(DIMOUSESTATE));
  */
  if (FAILED(dinKeyboard->GetDeviceState(256, keystate)))
  {
    while (FAILED(dinKeyboard->Acquire()))
      Sleep(5);
    while (FAILED(dinMouse->Acquire()))
      Sleep(5);
    dinKeyboard->GetDeviceState(256, keystate);
  }
  dinMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mousestate);// mouse.x += mousestate.lX; mouse.y += mousestate.lY;
  if (keystate[DIK_K])
  {
    FILE* f = fopen("test.obj", "wt+");

    DWORD vcount = 1;
    for (DWORD i = 0; i < selectedObjects.size(); i++)
    {

      selectedObjects[i].mesh->ExportObj(f, vcount);
    }
    fclose(f);
  }
  if (keystate[DIK_W])// || keystate[DIK_UP])
    direction |= FORWARD;
  else if (keystate[DIK_S])// || keystate[DIK_DOWN])
    direction |= BACKWARD;
  if (keystate[DIK_A])// || keystate[DIK_LEFT])
    direction |= LEFT;
  else if (keystate[DIK_D])// || keystate[DIK_RIGHT])
    direction |= RIGHT;

  if (direction || ((resetMouse || mousestate.rgbButtons[1]) && MouseMoved()))
  {
    camera->update((float)dt, direction);
    direction = 0;
    /*if(!renderCollision && scene)
    {
    scene->SortAlpha();
    FixSelection();
    }*/
  }
  if (keystate[DIK_UP])
  {
    D3DXMATRIX invView;
    D3DXMatrixInverse(&invView, NULL, &camera->view());
    for (DWORD i = 0, numSelected = selectedObjects.size(); i < numSelected; i++)
    {
      selectedObjects[i].mesh->Move(&D3DXVECTOR3(invView._31, invView._32, invView._33));
    }
  }

  if (mousestate.rgbButtons[0] && !oldMousestate.rgbButtons[0])
  {
    /*if(keystate[DIK_LSHIFT])
    {
    SelectVertex();
    snapping=true;
    }
    else*/
    AddSelection(direction);
  }
  else if (keystate[DIK_DELETE])
  {
    if (selectionMode == SELECTBYOBJECT)
    {
      const DWORD selected = selectedObjects.size();
      if (selected)
      {
        /*for(DWORD i=0; i<scene->GetNumMeshes(); i++)
        {
        if(!scene->GetMesh(i)->IsSelected())
        {
        scene->GetMesh(i)->SetFlag(MeshFlags::visible,false);
        scene->GetMesh(i)->SetFlag(MeshFlags::deleted,true);
        }
        }*/
        for (DWORD i = 0; i < selected; i++)
        {
          selectedObjects[i].mesh->SetFlag(MeshFlags::visible, false);
          selectedObjects[i].mesh->SetFlag(MeshFlags::deleted, true);
        }
        events.push_back(Event(Event::eDelete, selectedObjects));
        selectedObjects.clear();
      }
    }
    else if (selectionMode != SELECTBYTRI)
    {
      DeleteSelectedNodes();
    }

  }
  /*else if(snapping)
  {
  if(!keystate[DIK_LSHIFT])
  {
  selectedVertex.mesh=NULL;
  selectedVertex.vertex=NULL;
  snapping=false;
  }
  else
  {
  if(selectedVertex.mesh)
  {
  MessageBox(0,0,0,0);
  selectedObjects.push_back(selectedVertex.mesh);
  selectedVertex.mesh->SetFlag(MeshFlags::visible, false);
  selectedVertex.vertex->x=0;
  }
  }
  }*/

  oldMousestate.rgbButtons[0] = mousestate.rgbButtons[0];
  if (keystate[DIK_Z] && keystate[DIK_LCONTROL])
    UndoEvent(direction);
}

void InitInputDevices(HINSTANCE hInstance, HWND hWnd)
{
  /* #define HID_USAGE_PAGE_GENERIC         ((USHORT) 0x01)
  #define HID_USAGE_GENERIC_MOUSE        ((USHORT) 0x02)
  #define HID_USAGE_GENERIC_KEYBOARD     ((USHORT) 0x06)

  RAWINPUTDEVICE Rid[1];
  Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
  Rid[0].usUsage = HID_USAGE_GENERIC_KEYBOARD;
  Rid[0].dwFlags = RIDEV_INPUTSINK;
  Rid[0].hwndTarget = hWnd;
  RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));*/
  ZeroMemory(&mouse, sizeof(POINT));
  ZeroMemory(&mousestate, sizeof(mousestate));
  ZeroMemory(&keystate, sizeof(keystate));
  ZeroMemory(&oldMousestate, sizeof(oldMousestate));
  DirectInput8Create(hInstance,
    DIRECTINPUT_VERSION,
    IID_IDirectInput8,
    (void**)&din,
    NULL);

  din->CreateDevice(GUID_SysKeyboard,
    &dinKeyboard,
    NULL);
  din->CreateDevice(GUID_SysMouse, &dinMouse, NULL);

  dinKeyboard->SetDataFormat(&c_dfDIKeyboard);
  dinMouse->SetDataFormat(&c_dfDIMouse);

  dinKeyboard->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
  dinMouse->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
}


void CleanupInputDevices(void)
{
  dinKeyboard->Unacquire();
  dinMouse->Unacquire();
  dinMouse->Release();
  dinKeyboard->Release();
  din->Release();
}