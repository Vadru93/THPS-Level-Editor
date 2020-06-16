#include "Drawer.h"
#include <d3d9.h>
#include <d3dx9.h>
#include "Camera.h"
#include "Scene.h"
#pragma unmanaged

extern void detect_input(int dt);
extern Scene* scene;
extern IDirect3DDevice9Ex* Device;
extern bool showSky;
extern bool showTextures;
extern DWORD numOpaque;
extern DWORD MAX_PASSES;
extern DWORD MAX_TEXCOORDS;
extern bool showSpawns;
extern MaterialList* globalMaterialList;
extern bool showRails;
extern Mesh* arrow;
extern Mesh** opaque;
extern Mesh** trans;
extern vector <Node*> spawns;
extern Camera* camera;
extern DWORD alphaRef;
extern vector <SelectedTri> selectedTriangles;
extern D3DVIEWPORT9 port;

extern bool renderCollision;

extern LPD3DXFONT m_font;
extern RECT rct;
extern char info[75];
extern vector <RenderableRail> rails;

extern DWORD numTrans;
extern vector <Node*> selectedNodes;


void DrawFrame()
{
  Device->Clear(0,NULL,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,0,1.0f,0);
  Device->BeginScene();
  if(scene)
  {
    static int tLast = 0;
    const int tNow = GetTickCount();
    detect_input(tNow-tLast);
    tLast = tNow;
    if(showSky)
    {
      if(scene->skyDome)
      {
        Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        Device->SetRenderState(D3DRS_ZENABLE, FALSE);
        scene->SetSkyPos(camera->pos());
        Device->SetTransform(D3DTS_WORLD, scene->GetSkyMatrix());
        /*for(DWORD i=0; i<MAX_PASSES; i++)
        Device->SetSamplerState( i, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP );//Smoothly stretch the sky width*/
        if(showTextures)
        {
          //if(enableTransparency)
          scene->skyDome->Render(Device);
          /*else
          scene->skyDome->RenderNoTrans(Device);*/
        }
        else
          scene->skyDome->RenderShaded(Device);
        /*for(DWORD i=0; i<MAX_PASSES; i++)
        Device->SetSamplerState( i, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP );*/
        Device->SetTransform(D3DTS_WORLD, scene->GetWorldMatrix());
        Device->SetRenderState(D3DRS_ZENABLE, TRUE);
      }
    }

    Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

    if(showTextures)
    {
      Device->SetRenderState(D3DRS_ALPHAREF, 128);
      for(DWORD i=0; i< numOpaque; i++)
      {
        if(opaque[i]->IsVisible()) opaque[i]->Render0(globalMaterialList);
      }


      if(showSpawns/* || this->toolStripButton4->Checked || this->toolStripButton6->Checked*/)
      {
        const DWORD numSpawns = spawns.size();

        if(numSpawns != 0)
        {
          //Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVBLENDFACTOR);
          Device->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
          Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
          Device->SetStreamSource(0, arrow->verts, 0, sizeof(Vertex));
          Node** __restrict nodes = &spawns.front();
          for(DWORD i=0; i<MAX_PASSES; i++) Device->SetTexture(i,NULL);

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
          if(showRails)
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
          Device->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|MAX_TEXCOORDS);
        }
        else if(showRails)
        {
          //Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVBLENDFACTOR);
          Device->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
          Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
          Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
          for(DWORD i=0; i<MAX_PASSES; i++) Device->SetTexture(i,NULL);

          for(DWORD i=0, numRails = rails.size(); i<numRails; i++)
          {
            Device->SetStreamSource(0, rails[i].verts, 0, sizeof(Vertex));
            Device->SetIndices(rails[i].indices);
            Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, rails[i].numVerts, 0, rails[i].numIndices/3);
          }
          Device->SetRenderState(D3DRS_CULLMODE, scene->cullMode);
          Device->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|MAX_TEXCOORDS);
        }
      }
      else if(showRails)
      {
        //Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVBLENDFACTOR);
        Device->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
        Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
        Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
        for(DWORD i=0; i<MAX_PASSES; i++) Device->SetTexture(i,NULL);

        for(DWORD i=0, numRails = rails.size(); i<numRails; i++)
        {
          Device->SetStreamSource(0, rails[i].verts, 0, sizeof(Vertex));
          Device->SetIndices(rails[i].indices);
          Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, rails[i].numVerts, 0, rails[i].numIndices/3);
        }
        Device->SetRenderState(D3DRS_CULLMODE, scene->cullMode);
        Device->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|MAX_TEXCOORDS);
      }

      Device->SetRenderState(D3DRS_ALPHAREF, alphaRef);
      Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
      const DWORD numSelectedTris = selectedTriangles.size();

      if(numSelectedTris != 0)
      {
        Device->SetRenderState(D3DRS_ZENABLE, FALSE);
        if(!((showRails && rails.size()) || (showSpawns && spawns.size())))
        {
          //Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVBLENDFACTOR);
          Device->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
          for(DWORD i=0; i<MAX_PASSES; i++) Device->SetTexture(i,NULL);
        }
        for(DWORD i=0; i<numSelectedTris; i++)
        {
          Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
          Device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,0,3,1,&selectedTriangles[i].a,D3DFMT_INDEX16,selectedTriangles[i].vertices,sizeof(Vertex));
        }
        Device->SetRenderState(D3DRS_ZENABLE, TRUE);
        Device->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|MAX_TEXCOORDS);
      }

      for(DWORD i=0; i< numTrans; i++)
      {
        if(trans[i]->IsVisible()) trans[i]->Render2(globalMaterialList);
      }
      /*if(enableTransparency)
      scene->Render(Device);
      else
      scene->RenderNoTrans(Device);*/
    }
    else
    {
      scene->RenderShaded(Device);

      if(showSpawns/* || this->toolStripButton4->Checked || this->toolStripButton6->Checked*/)
      {
        const DWORD numSpawns = spawns.size();

        if(numSpawns != 0)
        {
          Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
          Device->SetStreamSource(0, arrow->verts, 0, sizeof(Vertex));
          const Node*const*const nodes = &spawns.front();

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
          if(showRails)
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
        else if(showRails)
        {
          Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
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
      else if(showRails)
      {
        Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
        Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

        for(DWORD i=0, numRails = rails.size(); i<numRails; i++)
        {
          Device->SetStreamSource(0, rails[i].verts, 0, sizeof(Vertex));
          Device->SetIndices(rails[i].indices);
          Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, rails[i].numVerts, 0, rails[i].numIndices/3);
        }
        Device->SetRenderState(D3DRS_CULLMODE, scene->cullMode);
      }
      Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
      DWORD numSelectedTris = selectedTriangles.size();

      if(numSelectedTris != 0)
      {
        Device->SetRenderState(D3DRS_ZENABLE, FALSE);
        for(DWORD i=0; i<numSelectedTris; i++)
        {
          Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
          Device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,0,3,1,&selectedTriangles[i].a,D3DFMT_INDEX16,selectedTriangles[i].vertices,sizeof(Vertex));
        }
        Device->SetRenderState(D3DRS_ZENABLE, TRUE);
      }
      Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    }
  }
  m_font->DrawTextA(0, info, -1, &rct, 0, D3DCOLOR_ARGB(255,0,0,255) );

  Device->EndScene();
  Device->Present(NULL, NULL, NULL, NULL);
}