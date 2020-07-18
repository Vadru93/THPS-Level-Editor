#include "stdafx.h"
#include "windows.h"
#include <d3d9.h>
#include "Th4Scene.h"
#include "Th2Scene.h"
#include "Th2xScene.h"
#include <windowsx.h>
#include <Commdlg.h>
#include <d3dx9.h>
#include "shlobj.h"
#include "Camera.h"
#include <dinput.h>
#include "eventhandler.h"
#include <io.h>
#include <fcntl.h>
#include <stdlib.h>
#include "Dwmapi.h"

#pragma unmanaged

extern D3DPRESENT_PARAMETERS DevParams;
extern IDirect3DDevice9Ex* __restrict Device;
extern IDirect3D9Ex* __restrict d3d;
extern bool deviceLost;
extern DWORD MAX_PASSES;
extern DWORD MAX_TEXCOORDS;
extern Scene* __restrict scene;
extern D3DVIEWPORT9 port;
extern bool resetMouse;
extern DIMOUSESTATE mousestate;
extern HWND handleWindow;
extern Camera* __restrict camera;
extern bool renderCollision;
extern BYTE selectionMode;
extern const float bias;
extern DWORD defaultCulling;
extern DWORD alphaRef;
extern MaterialList* __restrict globalMaterialList;
extern vector <Script> Scripts;
extern TexFiles texFiles;
extern LPD3DXFONT m_font;
extern Mesh* __restrict arrow;
extern RECT rct;
extern char info[75];
extern DIMOUSESTATE oldMousestate;
extern bool rendering;
extern bool loading;
extern vector <RenderableRail> rails;
extern Mesh** __restrict opaque;
extern Mesh** __restrict trans;
extern DWORD numTrans;
extern DWORD numOpaque;
extern BYTE pass;
extern DWORD numAnimations;
extern UVAnim* __restrict animations;

extern vector <Node*> spawns;
extern vector <Node*> selectedNodes;

extern vector <SelectedObject> selectedObjects;
extern vector <SelectedTri> selectedTriangles;
extern vector <Event> events;
extern bool heldCntrl;
extern vector<SelectedMaterial> selectedMaterials;

bool showTextures = true;
bool showSky = true;
bool enableTransparency = true;
bool showRails = true;
bool showSpawns = true;
bool showFlags = false;
bool showTrickob = false;
bool update = true;

__declspec(noalias) void DrawFPSFrame()
{
  IDirect3DDevice9Ex* const __restrict Dev = Device;

  Dev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
  Dev->BeginScene();
  if (scene->skyDome)
  {
    Dev->SetRenderState(D3DRS_ZENABLE, FALSE);
    scene->SetSkyPos(camera->pos());
    Dev->SetTransform(D3DTS_WORLD, scene->GetSkyMatrix());

    scene->skyDome->Render(Dev);

    Dev->SetTransform(D3DTS_WORLD, scene->GetWorldMatrix());
    Dev->SetRenderState(D3DRS_ZENABLE, TRUE);
  }

  static const D3DXMATRIX* const __restrict matWorld = scene->GetWorldMatrix();
  D3DXVECTOR3 pos, pos2;
  Dev->SetRenderState(D3DRS_ALPHAREF, 128);
  Mesh** const __restrict opaqueMeshes = opaque;
  MaterialList* const __restrict matList = globalMaterialList;
  for (DWORD i = 0, numMeshes = numOpaque; i< numMeshes; i++)
  {
    if (opaqueMeshes[i]->flags & MeshFlags::visible)
    {
      BBox bbox = opaqueMeshes[i]->GetBBoxX();
      D3DXVec3Project(&pos, (D3DXVECTOR3*)&bbox.Max, &port, &camera->proj(), &camera->view(), matWorld);
      D3DXVec3Project(&pos2, (D3DXVECTOR3*)&bbox.Min, &port, &camera->proj(), &camera->view(), matWorld);
      if ((pos.z >= 1.0f && pos.z <= 0.0f) && (pos2.z >= 1.0f && pos2.z <= 0.0f))
        continue;
      opaqueMeshes[i]->Render0(matList);
    }
  }

  Mesh** const __restrict transMeshes = trans;
  Dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
  for (DWORD i = 0, numMeshes = numTrans; i< numMeshes; i++)
  {
    if (transMeshes[i]->flags & MeshFlags::visible)
    {
      BBox bbox = transMeshes[i]->GetBBoxX();
      D3DXVec3Project(&pos, (D3DXVECTOR3*)&bbox.Max, &port, &camera->proj(), &camera->view(), matWorld);
      D3DXVec3Project(&pos2, (D3DXVECTOR3*)&bbox.Min, &port, &camera->proj(), &camera->view(), matWorld);
      if ((pos.z >= 1.0f && pos.z <= 0.0f) && (pos2.z >= 1.0f && pos2.z <= 0.0f))
        continue;
      transMeshes[i]->Render2(matList);
    }
  }
  Dev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

  Dev->EndScene();
  Dev->Present(0, 0, 0, 0);
}

__declspec(noalias) void DrawFrame()
{
  IDirect3DDevice9Ex* const __restrict Dev = Device;

  Dev->Clear(0,NULL,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,0,1.0f,0);
  Dev->BeginScene();
  if(scene)
  {
    /*static __int64 currTimeStamp=0;
    QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);
    timeNow=(float)currTimeStamp*0.001f;*/
    /*static DWORD tLast = 0;
    const DWORD tNow = GetTickCount();
    if(update)
      Update(tNow-tLast);
    else
      Update();
    tLast = tNow;*/
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
      static const D3DXMATRIX* matWorld = scene->GetWorldMatrix();
      D3DXVECTOR3 pos, pos2;
      Dev->SetRenderState(D3DRS_ALPHAREF, 128);
      Mesh** const __restrict opaqueMeshes = opaque;
      MaterialList* const __restrict matList = globalMaterialList;
      for(DWORD i=0, numMeshes = numOpaque; i< numMeshes; i++)
      {
        if (opaqueMeshes[i]->flags & MeshFlags::visible)
        {
          BBox bbox = opaqueMeshes[i]->GetBBoxX();
          D3DXVec3Project(&pos, (D3DXVECTOR3*)&bbox.Max, &port, &camera->proj(), &camera->view(), matWorld);
          D3DXVec3Project(&pos2, (D3DXVECTOR3*)&bbox.Min, &port, &camera->proj(), &camera->view(), matWorld);
          if ((pos.z >= 1.0f && pos.z <= 0.0f) && (pos2.z >= 1.0f && pos2.z <= 0.0f))
            continue;
          opaqueMeshes[i]->Render0(matList);
        }
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

              m_font->DrawTextA(NULL, "hi", -1, &rct, DT_LEFT|DT_NOCLIP ,   D3DXCOLOR(1.f,0.f,0.f,1.f));
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
            extern bool drawRail;
            extern struct AddingRail addRail;
            extern vector<RenderableRail> addedRails;
            if(addedRails.size())
            {
              //drawRail=false;
              /*D3DXMATRIX matx = *scene->GetWorldMatrix();
              D3DXMatrixScaling(&matx, 20.0f, 20.0f, 20.0f);
              Dev->SetTransform(D3DTS_WORLD, &matx);
              
              Dev->DrawPrimitiveUP(D3DPT_LINELIST, 1, &addRail, sizeof(ColouredVertex));
              Dev->SetTransform(D3DTS_WORLD, scene->GetWorldMatrix());*/
              Dev->SetStreamSource(0, addedRails[0].verts, 0, sizeof(ColouredVertex));
              Dev->SetIndices(addedRails[0].indices);
              Dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 8);///3);
            }
            for(DWORD i=0, numRails = rails.size(); i<numRails; i++)
            {
              Dev->SetStreamSource(0, rails[i].verts, 0, sizeof(ColouredVertex));
              Dev->SetIndices(rails[i].indices);
              Dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, rails[i].numVerts, 0, rails[i].numIndices);///3);
            }
            /*for (DWORD i = 0, numRails = addedRails.size(); i < numRails; i++)
            {
              Dev->SetStreamSource(0, addedRails[i].verts, 0, sizeof(ColouredVertex));
              Dev->SetIndices(addedRails[i].indices);
              Dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, addedRails[i].numVerts, 0, addedRails[i].numIndices);///3);
            }*/
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
            Dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, rails[i].numVerts, 0, rails[i].numIndices);///3);
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
          Dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, rails[i].numVerts, 0, rails[i].numIndices);///3);
        }
        Dev->SetRenderState(D3DRS_CULLMODE, scene->cullMode);
        Dev->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|MAX_TEXCOORDS);
      }

      Dev->SetRenderState(D3DRS_ALPHAREF, alphaRef);
      Dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

      Dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
      for(DWORD i=0, numSel = selectedObjects.size(); i<numSel; i++)
      {
        Mesh * __restrict mesh = selectedObjects[i].mesh;
        if(!mesh->transparent()) mesh->RenderSelected(matList);
      }
      for(DWORD i=0, numSel = selectedMaterials.size(); i<numSel; i++)
      {
        Mesh * __restrict mesh = selectedMaterials[i].mesh;
        Dev->SetStreamSource(0, mesh->verts, 0, sizeof(Vertex));
        MaterialSplit * __restrict split = selectedMaterials[i].split;
        if(!mesh->transparent()) split->RenderSelected(matList, mesh->GetNumVertices());
      }
      Dev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

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
      Mesh** const __restrict transMeshes = trans;
      Dev->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
      for(DWORD i=0, numMeshes = numTrans; i< numMeshes; i++)
      {
        if (transMeshes[i]->flags & MeshFlags::visible)
        {
          BBox bbox = transMeshes[i]->GetBBoxX();
          D3DXVec3Project(&pos, (D3DXVECTOR3*)&bbox.Max, &port, &camera->proj(), &camera->view(), matWorld);
          D3DXVec3Project(&pos2, (D3DXVECTOR3*)&bbox.Min, &port, &camera->proj(), &camera->view(), matWorld);
          if ((pos.z >= 1.0f && pos.z <= 0.0f) && (pos2.z >= 1.0f && pos2.z <= 0.0f))
            continue;
          transMeshes[i]->Render2(matList);
        }
      }
      Dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
      for(DWORD i=0, numSel = selectedObjects.size(); i<numSel; i++)
      {
        Mesh *mesh = selectedObjects[i].mesh;
        if(mesh->transparent()) mesh->RenderSelected(matList);
      }
      for(DWORD i=0, numSel = selectedMaterials.size(); i<numSel; i++)
      {
        Mesh * __restrict mesh = selectedMaterials[i].mesh;
        Dev->SetStreamSource(0, mesh->verts, 0, sizeof(Vertex));
        MaterialSplit * __restrict split = selectedMaterials[i].split;
        if(mesh->transparent()) split->RenderSelected(matList, mesh->GetNumVertices());
      }
      Dev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
      Dev->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
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

              m_font->DrawTextA(NULL, "hi", -1, &rct, DT_LEFT|DT_NOCLIP ,   D3DXCOLOR(1.f,0.f,0.f,1.f));
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
              Dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, rails[i].numVerts, 0, rails[i].numIndices);///3);
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
            Dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, rails[i].numVerts, 0, rails[i].numIndices);///3);
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
          Dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, rails[i].numVerts, 0, rails[i].numIndices);///3);
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