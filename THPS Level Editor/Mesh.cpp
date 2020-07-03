#include "stdafx.h"
#include "Mesh.h"
#include <d3dx9.h>
#include <D3dx9math.h>
#include "Node.h"
#include "Scene.h"
#pragma unmanaged
extern IDirect3DDevice9Ex* __restrict Device; static const float bias = -0.6f;


//extern DWORD MAX_PASSES; static const float bias = -0.6f;

__declspec (noalias) void Mesh::RenderCollision(const bool isSelected) const
{
  if (collision.get() && collision->GetNumFaces())
    collision->Render((IDirect3DDevice9Ex* const __restrict)Device, isSelected, this);
}

__declspec (noalias) void Mesh::Move(const D3DXVECTOR3 *direction)
{
  verts->Release();
  verts = NULL;
  Vertex bboxMin, bboxMax;
  //D3DXVECTOR3 angle = node->GetAngle();
  /*D3DXMATRIX rotation;
  D3DXMatrixRotationYawPitchRoll(&rotation,-angle.y+D3DX_PI,angle.x,angle.z);
  D3DXVECTOR3 translation;
  D3DXVec3TransformCoord(&translation,direction,&rotation);´*/

  for (DWORD i = 0; i < vertices.size(); i++)
  {
    //Vertex *vertex = (Vertex*)&vertices[i];
    *(D3DXVECTOR3*)&vertices[i] += *direction;//translation;
    /*if(i==0)
    {
    bboxMin = *vertex;
    bboxMax = *vertex;
    }
    else
    {
    if(vertex->x<bboxMin.x)
    bboxMin.x=vertex->x;
    if(vertex->y<bboxMin.y)
    bboxMin.y=vertex->y;
    if(vertex->z<bboxMin.z)
    bboxMin.z=vertex->z;

    if(vertex->x>bboxMax.x)
    bboxMax.x=vertex->x;
    if(vertex->y>bboxMax.y)
    bboxMax.y=vertex->y;
    if(vertex->z>bboxMax.z)
    bboxMax.z=vertex->z;
    }*/
  }
  DWORD fvf;
  Device->GetFVF(&fvf);
  if (FAILED(Device->CreateVertexBuffer(vertices.size()*sizeof(Vertex), D3DUSAGE_WRITEONLY, fvf, D3DPOOL_DEFAULT, &verts, NULL)))
    MessageBox(0, "failed to create vertex buffer", "", 0);


  Vertex* pVertices;
  verts->Lock(0, 0, (void**)&pVertices, 0);
  memcpy(pVertices, &vertices.front(), vertices.size()*sizeof(Vertex));
  verts->Unlock();
  //SetBBox(bboxMin,bboxMax);
  //node->SetPosition(GetCenter());
}

__declspec (noalias)void Mesh::Scale(float scale)
{
    if (verts)
        verts->Release();
    D3DXVECTOR3 center = GetCenter();
    center *= scale;
    SetCenter(center);
    extern Scene* __restrict scene;
    /*Node* node = scene->GetNode(name);
    if (node)
        *(D3DXVECTOR3*)&node->Position *= scale;
    */
    for (DWORD i = 0; i < vertices.size(); i++)
    {
        vertices[i].x *= scale;
        vertices[i].y *= scale;
        vertices[i].z *= scale;
    }
    if (verts)
    {
        DWORD fvf;
        Device->GetFVF(&fvf);
        if (FAILED(Device->CreateVertexBuffer(vertices.size() * sizeof(Vertex), D3DUSAGE_WRITEONLY, fvf, D3DPOOL_DEFAULT, &verts, NULL)))
            MessageBox(0, "failed to create vertex buffer", "", 0);


        Vertex* pVertices;
        verts->Lock(0, 0, (void**)&pVertices, 0);
        memcpy(pVertices, &vertices.front(), vertices.size() * sizeof(Vertex));
        verts->Unlock();
    }
}

__declspec (noalias)void Mesh::MoveRelative(const D3DXVECTOR3* relDist)
{
  /*char pos[256]="";
  sprintf(pos,"%f %f %f", relDist->x, relDist->y, relDist->z);
  MessageBox(0,pos,NULL,0);*/
  if (verts)
    verts->Release();
  SetCenter(GetCenter() + *relDist);
  extern Scene* __restrict scene;
  Node* node = scene->GetNode(name);
  if (node)
    *(D3DXVECTOR3*)&node->Position += *relDist;
  for (DWORD i = 0, numVertices = vertices.size(); i < numVertices; i++)
  {
    *(D3DXVECTOR3*)&vertices[i] += *relDist;
  }
  if (verts)
  {
    DWORD fvf;
    Device->GetFVF(&fvf);
    if (FAILED(Device->CreateVertexBuffer(vertices.size()*sizeof(Vertex), D3DUSAGE_WRITEONLY, fvf, D3DPOOL_DEFAULT, &verts, NULL)))
      MessageBox(0, "failed to create vertex buffer", "", 0);


    Vertex* pVertices;
    verts->Lock(0, 0, (void**)&pVertices, 0);
    memcpy(pVertices, &vertices.front(), vertices.size()*sizeof(Vertex));
    verts->Unlock();
  }
}

__declspec (noalias) void Mesh::MoveWithColl(const D3DXVECTOR3 const* __restrict position, const D3DXVECTOR3 const* __restrict angle)
{
  verts->Release();
  D3DXMATRIX rotation;
  D3DXMatrixRotationYawPitchRoll(&rotation, -angle->y + D3DX_PI, angle->x, angle->z);
  D3DXVECTOR3 translation;
  D3DXVec3TransformCoord(&translation, position, &rotation);

  for (DWORD i = 0; i<vertices.size(); i++)
  {
    *(D3DXVECTOR3*)&vertices[i] += translation;
  }

  *(D3DXVECTOR3*)&bbox.Min += translation;
  *(D3DXVECTOR3*)&bbox.Max += translation;
  if (bbox.Min.x>bbox.Max.x)
  {
    float temp = bbox.Min.x;
    bbox.Min.x = bbox.Max.x;
    bbox.Max.x = temp;
  }
  if (bbox.Min.y > bbox.Max.y)
  {
    float temp = bbox.Min.y;
    bbox.Min.y = bbox.Max.y;
    bbox.Max.y = temp;
  }
  if (bbox.Min.z > bbox.Max.z)
  {
    float temp = bbox.Min.z;
    bbox.Min.z = bbox.Max.z;
    bbox.Max.z = temp;
  }

  if (collision.get())
    collision->Move(position, angle);

  DWORD fvf;
  Device->GetFVF(&fvf);
  if (FAILED(Device->CreateVertexBuffer(vertices.size()*sizeof(Vertex), D3DUSAGE_WRITEONLY, fvf, D3DPOOL_DEFAULT, &verts, NULL)))
    MessageBox(0, "failed to create vertex buffer", "", 0);


  Vertex* pVertices;
  verts->Lock(0, 0, (void**)&pVertices, 0);
  memcpy(pVertices, &vertices.front(), vertices.size()*sizeof(Vertex));
  verts->Unlock();
}

__declspec (noalias) void Mesh::Move(const D3DXVECTOR3 *direction, const D3DXVECTOR3 *angle)
{
  verts->Release();
  verts = NULL;
  Vertex bboxMin, bboxMax;
  D3DXMATRIX rotation;
  D3DXMatrixRotationYawPitchRoll(&rotation, -angle->y + D3DX_PI, angle->x, angle->z);
  D3DXVECTOR3 translation;
  D3DXVec3TransformCoord(&translation, direction, &rotation);

  for (DWORD i = 0; i < vertices.size(); i++)
  {
    //Vertex *vertex = (Vertex*)&vertices[i];
    *(D3DXVECTOR3*)&vertices[i] += translation;
    /*if(i==0)
    {
    bboxMin = *vertex;
    bboxMax = *vertex;
    }
    else
    {
    if(vertex->x<bboxMin.x)
    bboxMin.x=vertex->x;
    if(vertex->y<bboxMin.y)
    bboxMin.y=vertex->y;
    if(vertex->z<bboxMin.z)
    bboxMin.z=vertex->z;

    if(vertex->x>bboxMax.x)
    bboxMax.x=vertex->x;
    if(vertex->y>bboxMax.y)
    bboxMax.y=vertex->y;
    if(vertex->z>bboxMax.z)
    bboxMax.z=vertex->z;
    }*/
  }
  DWORD fvf;
  Device->GetFVF(&fvf);
  if (FAILED(Device->CreateVertexBuffer(vertices.size()*sizeof(Vertex), D3DUSAGE_WRITEONLY, fvf, D3DPOOL_DEFAULT, &verts, NULL)))
    MessageBox(0, "failed to create vertex buffer", "", 0);


  Vertex* pVertices;
  verts->Lock(0, 0, (void**)&pVertices, 0);
  memcpy(pVertices, &vertices.front(), vertices.size()*sizeof(Vertex));
  verts->Unlock();
  //SetBBox(bboxMin,bboxMax);
  //node->SetPosition(GetCenter());
}

__declspec (noalias) void Mesh::SetPosition(const D3DXVECTOR3 const*__restrict position, const D3DXVECTOR3 const*__restrict angle)
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
  D3DXVec3TransformCoord((D3DXVECTOR3*)&bbox.Max, (D3DXVECTOR3*)&bbox.Max, &result);
  D3DXVec3TransformCoord((D3DXVECTOR3*)&bbox.Min, (D3DXVECTOR3*)&bbox.Min, &result);
  if (bbox.Min.x > bbox.Max.x)
  {
    const float temp = bbox.Max.x;
    bbox.Max.x = bbox.Min.x;
    bbox.Min.x = temp;
  }
  if (bbox.Min.y > bbox.Max.y)
  {
    const float temp = bbox.Max.y;
    bbox.Max.y = bbox.Min.y;
    bbox.Min.y = temp;
  }
  if (bbox.Min.x > bbox.Max.x)
  {
    const float temp = bbox.Max.z;
    bbox.Max.z = bbox.Min.z;
    bbox.Min.z = temp;
  }

  for (DWORD i = 0, numVertices = vertices.size(); i < numVertices; i++)
  {
    //*(D3DXVECTOR3*)&vertices[i]-=center;
    D3DXVec3TransformCoord((D3DXVECTOR3*)&vertices[i], (D3DXVECTOR3*)&vertices[i], &result);
  }

  if (collision.get())
    collision->SetPosition(position, angle);
}

__declspec (noalias) void Mesh::SetPosition(const D3DXVECTOR3 &position)
{
  extern Scene* __restrict scene;
  Node* node = scene->GetNode(name);
  node->SetPosition(position);

  D3DXVECTOR3 center = GetCenter();
  *(D3DXVECTOR3*)&bbox.Max = position + (*(D3DXVECTOR3*)&bbox.Max - center);
  *(D3DXVECTOR3*)&bbox.Min = position - (*(D3DXVECTOR3*)&bbox.Min + center);
  D3DXMATRIX translation;
  D3DXMatrixTranslation(&translation, position.x, position.y, position.z);

  for (DWORD i = 0; i < vertices.size(); i++)
  {
    Vertex *vertex = (Vertex*)&vertices[i];
    *(D3DXVECTOR3*)&vertices[i] -= center;
    D3DXVec3TransformCoord((D3DXVECTOR3*)vertex, (D3DXVECTOR3*)vertex, &translation);
  }
}

__declspec (noalias) void Mesh::RenderShadedWithCurrentVBuffer()
{
  DWORD numSplits = matSplits.size();
  for (DWORD i = 0; i < numSplits; i++)
  {
    Device->SetIndices(matSplits[i].indices);
    Device->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, vertices.size(), 0, matSplits[i].numIndices);
  }
}

__declspec (noalias) void Mesh::RenderShadedWithCurrentBuffers()
{
  Device->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, vertices.size(), 0, matSplits[0].numIndices);
}

__declspec (noalias) void Mesh::RenderShaded()
{
  DWORD numSplits = matSplits.size();
  for (DWORD i = 0; i < numSplits; i++)
  {
    if (IsSelected())
    {
      Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
      Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
      Device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLESTRIP, 0, vertices.size(), matSplits[i].numIndices, &matSplits[i].Indices.front(), D3DFMT_INDEX16, &vertices.front(), sizeof(Vertex));
      Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    }
    else
      Device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLESTRIP, 0, vertices.size(), matSplits[i].numIndices, &matSplits[i].Indices.front(), D3DFMT_INDEX16, &vertices.front(), sizeof(Vertex));
  }
}

__declspec (noalias) void Mesh::Render(MaterialList* matList)
{
  Device->SetStreamSource(0, verts, 0, sizeof(Vertex));
  DWORD numSplits = matSplits.size();
  for (DWORD i = 0; i < numSplits; i++)
  {
    Device->SetIndices(matSplits[i].indices);

    Material* tmpMat = matList->GetMaterial(matSplits[i].matId);
    if (tmpMat)
    {
      tmpMat->Submit();
      if (IsSelected())
      {
        Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
        Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
        Device->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, vertices.size(), 0, matSplits[i].numIndices);//Device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLESTRIP,0,vertices.size(),matSplits[i].numIndices,&matSplits[i].Indices.front(),D3DFMT_INDEX16,&vertices.front(),sizeof(Vertex));
        Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
      }
      else
        Device->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, vertices.size(), 0, matSplits[i].numIndices);//Device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLESTRIP,0,vertices.size(),matSplits[i].numIndices,&matSplits[i].Indices.front(),D3DFMT_INDEX16,&vertices.front(),sizeof(Vertex));
    }
    else
    {
      Device->SetTexture(0, NULL);
      Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
      Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
      if (IsSelected())
      {
        Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
        Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
        Device->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, vertices.size(), 0, matSplits[i].numIndices);//Device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLESTRIP,0,vertices.size(),matSplits[i].numIndices,&matSplits[i].Indices.front(),D3DFMT_INDEX16,&vertices.front(),sizeof(Vertex));
        Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
      }
      else
        Device->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, vertices.size(), 0, matSplits[i].numIndices);//Device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLESTRIP,0,vertices.size(),matSplits[i].numIndices,&matSplits[i].Indices.front(),D3DFMT_INDEX16,&vertices.front(),sizeof(Vertex));
    }
  }
}

__declspec (noalias) void Mesh::CreateBuffers(IDirect3DDevice9Ex* const __restrict dev, const DWORD &fvf)
{
  const DWORD numVerts = vertices.size();

  if (numVerts != 0)
  {
    if (FAILED(dev->CreateVertexBuffer(numVerts*sizeof(Vertex), D3DUSAGE_WRITEONLY, fvf, D3DPOOL_DEFAULT, &verts, NULL)))
      MessageBox(0, "failed to create vertex buffer", "", 0);


    Vertex* pVertices;
    verts->Lock(0, 0, (void**)&pVertices, 0);
    memcpy(pVertices, &vertices.front(), numVerts*sizeof(Vertex));
    verts->Unlock();


    for (DWORD i = 0; i < matSplits.size(); i++)
    {
      const DWORD numIndices = matSplits[i].Indices.size();
      if (FAILED(dev->CreateIndexBuffer(numIndices * 2, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &matSplits[i].indices, NULL)))
      {
        matSplits[i].indices = NULL;
        char error[256] = "";
        sprintf(error, "name %s numidx %u", this->GetName().GetString2(), numIndices);
        MessageBox(0, error, "failed to create index buffer", 0);
      }
      else
      {

        WORD* indices;
        matSplits[i].indices->Lock(0, 0, (void**)&indices, 0);
        memcpy(indices, &matSplits[i].Indices.front(), numIndices * 2);
        matSplits[i].indices->Unlock();
      }
    }
  }
}

__declspec (noalias) void Mesh::ReleaseBuffers()
{
  const DWORD numVerts = vertices.size();
  if (numVerts != 0)
  {
    if (verts)
      verts->Release();
    verts = NULL;

    for (DWORD i = 0; i < matSplits.size(); i++)
    {
      if (matSplits[i].indices)
        matSplits[i].indices->Release();
      matSplits[i].indices = NULL;
    }
  }
}

__declspec (noalias) void Mesh::RenderSelected(MaterialList* const __restrict materialList) const//render opaque
{
  //_asm prefetcht0 [ecx]
  register IDirect3DDevice9Ex* const __restrict Dev = Device;
  Dev->SetStreamSource(0, verts, 0, sizeof(Vertex));
  register const DWORD numSplits = matSplits.size();
  register const DWORD numVertices = vertices.size();
  register MaterialList* const __restrict matList = materialList;
  for (register DWORD i = 0; i < numSplits; i++)
  {
    Dev->SetIndices(matSplits[i].indices);

    register Material* const __restrict tmpMat = matList->GetMaterial(matSplits[i].matId);
    if (tmpMat)
    {
      const Texture*  __restrict const tex = tmpMat->GetTexture(0);
      Dev->SetSamplerState(0, D3DSAMP_ADDRESSU, tex->uAddress);
      Dev->SetSamplerState(0, D3DSAMP_ADDRESSV, tex->vAddress);
      Dev->SetTexture(0, tex->GetImage());
      if (tmpMat->animated)
      {
        extern DWORD numAnimations;
        extern UVAnim* __restrict animations;
        const UVAnim const* __restrict const anims = animations;
        const Animation* __restrict anim = NULL;
        for (DWORD j = 0, numAnims = numAnimations; j < numAnims; j++)
        {
          if (anims[j].matId == tmpMat->GetMatId())
          {
            anim = &anims[j].anim;
            break;
          }
        }
        if (anim)
        {
          static D3DMATRIX uv_mat = { 1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f };
          Dev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
          //extern float timeNow;
          extern float frameDelta;
          const float t = frameDelta;//(float)ticker*0.01666666666667f;//const float t = (float)GetTickCount()* 0.001f;

          float uoff, voff;

          uoff = (t * anim->UVel) + (anim->UAmplitude * sinf(anim->UFrequency * t + anim->UPhase));
          voff = (t * anim->VVel) + (anim->VAmplitude * sinf(anim->VFrequency * t + anim->VPhase));

          // Reduce offset mod 16 and put it in the range -8 to +8.
          /*uoff	+= 8.0f;
          uoff	-= (float)(( (int)uoff >> 4 ) << 4 );
          voff	+= 8.0f;
          voff	-= (float)(( (int)voff >> 4 ) << 4 );*/
          uoff -= (float)(int)uoff;
          voff -= (float)(int)voff;


          uv_mat._31 = uoff;//( uoff < 0.0f ) ? ( uoff + 8.0f ) : ( uoff - 8.0f );
          uv_mat._32 = voff;//( voff < 0.0f ) ? ( voff + 8.0f ) : ( voff - 8.0f );

          Dev->SetTransform((D3DTRANSFORMSTATETYPE)(D3DTS_TEXTURE0), &uv_mat);
        }
      }
    }
    else
    {
      Dev->SetTexture(0, NULL);
    }

    Dev->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, numVertices, 0, matSplits[i].numIndices);
  }
}

__declspec (noalias) void Mesh::Render0(MaterialList* const __restrict materialList) const//render opaque
{
  //_asm prefetcht0 [ecx]
  register IDirect3DDevice9Ex* const __restrict Dev = Device;
  Dev->SetStreamSource(0, verts, 0, sizeof(Vertex));
  register const DWORD numSplits = matSplits.size();
  register const DWORD numVertices = vertices.size();
  register MaterialList* const __restrict matList = materialList;
  for (register DWORD i = 0; i < numSplits; i++)
  {
    Dev->SetIndices(matSplits[i].indices);

    register Material* const __restrict tmpMat = matList->GetMaterial(matSplits[i].matId);
    //__assume(tmpMat!=NULL)
    if (tmpMat!=NULL)
      tmpMat->Submit();
    else
    {
      Dev->SetTexture(0, NULL);
      Dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
      Dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    }

    //if(!IsSelected())
    Dev->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, numVertices, 0, matSplits[i].numIndices);
    /*else
    {
    Dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
    Dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    Dev->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, numVertices, 0, matSplits[i].numIndices);
    Dev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    }*/
    /*if(IsSelected())
    {
    // Device->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, *(DWORD *)&bias);
    Dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
    Dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    //Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    Dev->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0,0,numVertices,0,matSplits[i].numIndices);//Device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLESTRIP,0,vertices.size(),matSplits[i].numIndices,&matSplits[i].Indices.front(),D3DFMT_INDEX16,&vertices.front(),sizeof(Vertex));
    //Device->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
    Dev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    //Device->SetRenderState(D3DRS_CULLMODE, defaultCulling);
    // Device->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, 0);
    }
    else
    Dev->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0,0,numVertices,0,matSplits[i].numIndices);*///Device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLESTRIP,0,vertices.size(),matSplits[i].numIndices,&matSplits[i].Indices.front(),D3DFMT_INDEX16,&vertices.front(),sizeof(Vertex));
  }
}

__declspec (noalias) bool Mesh::PickCollision(const D3DXVECTOR3* rayOrigin, const D3DXVECTOR3* rayDirection, float* outDistance, DWORD* outTriangleIndex)
{
  if (collision.get())
    return collision->Intersect(rayOrigin, rayDirection, outDistance, &vertices.front(), outTriangleIndex);
  else
    return false;
}

__declspec (noalias) bool Mesh::PickMesh(const D3DXVECTOR3* rayOrigin, const D3DXVECTOR3* rayDirection, float* outDistance, DWORD* outMatSplitIndex, DWORD* outTriangleIndex)
{
  float closestIntersect = 0.0f;
  bool intersect = false;
  DWORD matSplit, closestTri;

  for (DWORD i = 0; i < matSplits.size(); i++)
  {

    for (DWORD j = 0; j<matSplits[i].numIndices; j++)
    {
      if (TriangleIntersection((D3DXVECTOR3*)&vertices[matSplits[i].Indices[j]], (D3DXVECTOR3*)&vertices[matSplits[i].Indices[j + 1]], (D3DXVECTOR3*)&vertices[matSplits[i].Indices[j + 2]], rayOrigin, rayDirection/*,NULL,NULL*/, outDistance))
      {
        if (closestIntersect>*outDistance || !intersect)
        {
          intersect = true;
          closestIntersect = *outDistance;

          matSplit = i;
          closestTri = j;
        }
      }
    }
  }
  if (intersect)
  {
    if (outTriangleIndex)
      *outTriangleIndex = closestTri;
    if (outMatSplitIndex)
      *outMatSplitIndex = matSplit;
    /*WORD v1, v2, v3;
    DWORD tmp = 0;

    WORD* indices = &matSplits[matSplit].Indices.front();
    DWORD indexCount = 0;

    for(DWORD i=0; i<matSplits[matSplit].Indices.size(); i++)
    {

    if(tmp==0)
    {
    v1 = indices[i];
    v2 = indices[i+1];
    v3 = indices[i+2];
    tmp=1;
    }
    else
    {
    v1 = v3;
    v3 = indices[i+2];
    tmp=0;
    }
    if(v1 != v2 && v1 != v3 && v2 != v3)
    {
    if(v1==indices[closestTri] && v2==indices[closestTri+1] && v3==indices[closestTri+2])
    {
    //printf("Face %d CollFlags 0x%X\n",indexCount,collision.GetFaceInfo(indexCount)->flags);
    }
    indexCount++;
    }
    }*/
  }

  /*if(intersect)
  {
  //D3DXVECTOR3 screenspace;
  // extern D3DVIEWPORT9 port;
  // D3DXMATRIX world;
  // D3DXMatrixIdentity(&world);
  //D3DXVec3Project(&screenspace, (D3DXVECTOR3*)&vertices[matSplits[matSplit].Indices[closestTri]],&port,&camera->proj(),&camera->view(),&world);
  printf("\nA %08X uv %.5f %.5f\n",matSplits[matSplit].Indices[closestTri],vertices[matSplits[matSplit].Indices[closestTri]].tUV[0].u,vertices[matSplits[matSplit].Indices[closestTri]].tUV[0].v);
  //D3DXVec3Project(&screenspace, (D3DXVECTOR3*)&vertices[matSplits[matSplit].Indices[closestTri+1]],&port,&camera->proj(),&camera->view(),&world);
  printf("B %08X uv %.5f %.5f\n",matSplits[matSplit].Indices[closestTri+1],vertices[matSplits[matSplit].Indices[closestTri+1]].tUV[0].u,vertices[matSplits[matSplit].Indices[closestTri+1]].tUV[0].v);
  //D3DXVec3Project(&screenspace, (D3DXVECTOR3*)&vertices[matSplits[matSplit].Indices[closestTri+2]],&port,&camera->proj(),&camera->view(),&world);
  printf("C %08X uv %.5f %.5f\n\n",matSplits[matSplit].Indices[closestTri+2],vertices[matSplits[matSplit].Indices[closestTri+2]].tUV[0].u,vertices[matSplits[matSplit].Indices[closestTri+2]].tUV[0].v);
  }*/
  *outDistance = closestIntersect;
  return intersect;
}

__declspec (noalias) void Mesh::Render1(MaterialList* const __restrict matList)//render alpha 1st order
{
  int numSplits = (int)matSplits.size(); numSplits--;

  for (int i = numSplits; i >= 0; i--)
  {
    Material* tmpMat = matList->GetMaterial(matSplits[i].matId);
    if (tmpMat)
    {
      /*for(DWORD j=0; j<MAX_PASSES; j++)
      Device->SetTexture(j,tmpMat->GetTexturePass(j));*/
      ((Material*)tmpMat)->Submit();
      if (IsSelected())
      {
        // Device->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, *(DWORD *)&bias);
        //Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
        Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
        Device->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, vertices.size(), 0, matSplits[i].numIndices);//Device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLESTRIP,0,vertices.size(),matSplits[i].numIndices,&matSplits[i].Indices.front(),D3DFMT_INDEX16,&vertices.front(),sizeof(Vertex));
        //Device->SetRenderState(D3DRS_CULLMODE, defaultCulling);
        //Device->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
        // Device->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, 0);
      }
      else
        Device->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, vertices.size(), 0, matSplits[i].numIndices);//Device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLESTRIP,0,vertices.size(),matSplits[i].numIndices,&matSplits[i].Indices.front(),D3DFMT_INDEX16,&vertices.front(),sizeof(Vertex));
    }
  }
}

__declspec (noalias) void Mesh::Render2(MaterialList* const __restrict materialList) const//render alpha 2nd order
{
  //_asm prefetcht0 [ecx]
  register IDirect3DDevice9Ex* const __restrict Dev = Device;
  Dev->SetStreamSource(0, verts, 0, sizeof(Vertex));
  register const DWORD numSplits = matSplits.size();
  register const DWORD numVertices = vertices.size();
  register MaterialList* const __restrict matList = materialList;
  for (register DWORD i = 0; i < numSplits; i++)
  {
    Dev->SetIndices(matSplits[i].indices);

    register Material *const __restrict tmpMat = matList->GetMaterial(matSplits[i].matId);
    if (tmpMat)
    {
      tmpMat->Submit();

      /*if(IsSelected())
        Dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);*/
      if (!tmpMat->invisible)
        Dev->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, numVertices, 0, matSplits[i].numIndices);
    }
  }
}

__declspec (noalias) void Mesh::Render3(MaterialList* const __restrict matList)//th2
{
  DWORD numSplits = matSplits.size();
  for (DWORD i = 0; i < numSplits; i++)
  {
    if (IsSelected())
    {
      Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
      Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
      Device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLESTRIP, 0, vertices.size(), matSplits[i].numIndices, &matSplits[i].Indices.front(), D3DFMT_INDEX16, &vertices.front(), sizeof(Vertex));
      Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    }
    else
      Device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLESTRIP, 0, vertices.size(), matSplits[i].numIndices, &matSplits[i].Indices.front(), D3DFMT_INDEX16, &vertices.front(), sizeof(Vertex));
  }
}

Th4Mesh::Th4Mesh(DWORD ptr, const DWORD index, const bool materials)
{
#define TEXFLAG_UNKNOWN 1
#define TEXFLAG_EXTENSION 2

#define MESHFLAG_TEXCOORD 1
#define MESHFLAG_COLOUR 2
#define MESHFLAG_NORMAL 4
#define MESHFLAG_SKIN 16
#define MESHFLAG_EXTENSION 2048
  ptr += 12;

  DWORD *numMaterials = (DWORD*)ptr;
  ptr += 4;
  for (DWORD i = 0; i < *numMaterials; i++)
  {

    Th4Material::Data* materialData = (Th4Material::Data*)ptr;

    ptr += 19;
    if (materialData->unkFlag)
      ptr += 8;

    for (DWORD j = 0; j < materialData->numTextures; j++)
    {
      Th4Texture::Data* textureData = (Th4Texture::Data*)ptr;
      ptr += 77;

      if (textureData->flags & TEXFLAG_UNKNOWN)
        ptr += 32;
      if (textureData->flags & TEXFLAG_EXTENSION)
      {
        DWORD *numExtensions = (DWORD*)ptr;
        ptr += 4;

        for (DWORD k = 0; k < *numExtensions; k++)
        {
          DWORD *numItems = (DWORD*)ptr;
          ptr += 8;
          for (DWORD l = 0; l < *numItems; l++)
          {
            ptr += 8;
          }
        }
      }
      ptr += 16;
    }
  }

  DWORD numObjects = ((DWORD*)ptr)[0];
  ptr += 4;
  for (DWORD i = 0; i < numObjects; i++)
  {

    if (i == index)
    {
      data = (Th4Mesh::Data*)malloc(sizeof(Th4Mesh::Data));
      memcpy(data, (Th4Mesh::Data*)ptr, 64);

      SetName(data->name);
      SetCenter(data->sphereCenter);
      SetBBox(data->bboxMin, data->bboxMax);

      ptr += 64;

      data->vertices = (SimpleVertex*)ptr;
      ptr += 12 * data->numVertices;

      if (data->flags & MESHFLAG_NORMAL)
      {
        data->normals = (SimpleVertex*)ptr;
        ptr += 12 * data->numVertices;
      }
      else
        data->normals = NULL;

      if (data->flags & MESHFLAG_SKIN)
      {
        ptr += 24 * data->numVertices;
      }
      DWORD numTexCoords;
      if (data->flags & MESHFLAG_TEXCOORD)
      {
        // meshData->texCoords = new TexCoord[meshData->numVertices];


        numTexCoords = ((DWORD*)ptr)[0]; data->texCoords = new TexCoord[data->numVertices*numTexCoords]; //if(numTexCoords>MAX_TEXCOORDS) MAX_TEXCOORDS = numTexCoords;//mesh->SetNumTexCoords(numTexCoords);
        ptr += 4;
        for (DWORD j = 0; j < data->numVertices*numTexCoords; j++)
        {
          data->texCoords[j] = ((TexCoord*)ptr)[0];
          ptr += 8;//*numTexCoords;
        }

      }
      else
        data->texCoords = NULL;

      if (data->flags & MESHFLAG_COLOUR)
      {
        data->colours = (Colour*)ptr;
        ptr += 4 * data->numVertices;
      }
      else
        data->colours = NULL;

      if (data->flags & MESHFLAG_EXTENSION)
      {
        ptr += data->numVertices;//colour wibble data?
      }


      Vertex* vertices = new Vertex[data->numVertices];

      for (int j = 0; j < data->numVertices; j++)
      { //data->vertices[j].z *=-1;
        memcpy(&vertices[j], &data->vertices[j], 12);
        vertices[j].z *= -1;

        if (data->texCoords)
        {
          /*for(DWORD k=0; k<numTexCoords; k++)
          {
          vertices[j].tUV[k].u = data->texCoords[j*numTexCoords+k].u;
          vertices[j].tUV[k].v = data->texCoords[j*numTexCoords+k].v;
          }

          for(DWORD k=numTexCoords; k<4; k++)
          {
          vertices[j].tUV[k].u = data->texCoords[j*numTexCoords+numTexCoords].u;
          vertices[j].tUV[k].u = data->texCoords[j*numTexCoords+numTexCoords].v;
          }
          */

          vertices[j].tUV[0] = data->texCoords[j*numTexCoords];
          /*for(DWORD k=0; k<numTexCoords; k++)
          {
          vertices[j].uv[k*2] = meshData->texCoords[j+k].u;
          vertices[j].uv[k*2+1] = meshData->texCoords[j+k].v;
          }*/
          /*vertices[j].u = meshData->texCoords[j].u;
          vertices[j].v = meshData->texCoords[j].v;*/
        }

        if (data->colours)
        {
          //meshData->colours[j].a = 0xFF;
          vertices[j].colour = data->colours[j];
        }

      }

      delete[] data->texCoords;

      this->Reserve(data->numVertices);
      this->AddVertices(vertices, data->numVertices);

      delete[] vertices;

      for (DWORD j = 0; j < data->numSplits; j++)
      {
        // MaterialSplit* matSplit = new MaterialSplit;
        // memcpy(matSplit,(MaterialSplit*)ptr,12);

        MaterialSplit matSplite;
        MaterialSplit* matSplit = (MaterialSplit*)ptr;
        //memset(&matSplit->Indices, 0, 12);
        matSplite.matId = matSplit->matId;
        matSplite.type = 0;
        //matSplit->type=0;
        ptr += 12;

        WORD* Indices = (WORD*)ptr;
        //matSplit->AddIndices(Indices,matSplit->numIndices);
        this->AddMatSplit(&matSplite, NULL);
        this->matSplits.back().AddIndices(Indices, matSplit->numIndices);
        this->matSplits.back().numIndices = matSplit->numIndices - 2;
        ptr += 2 * matSplit->numIndices;
        //delete matSplit;
      }
      free(data);
      break;
    }
    else
    {
      data = (Th4Mesh::Data*)ptr;
      ptr += 64;
      ptr += 12 * data->numVertices;

      if (data->flags & MESHFLAG_NORMAL)
      {
        ptr += 12 * data->numVertices;
      }

      if (data->flags & MESHFLAG_SKIN)
      {
        ptr += 24 * data->numVertices;
      }

      if (data->flags & MESHFLAG_TEXCOORD)
      {
        const DWORD numTexCoords = ((DWORD*)ptr)[0];
        ptr += 4;
        ptr += 8 * data->numVertices*numTexCoords;
      }

      if (data->flags & MESHFLAG_COLOUR)
      {
        ptr += 4 * data->numVertices;
      }

      if (data->flags & MESHFLAG_EXTENSION)
      {
        ptr += data->numVertices;//colour wibble data?
      }

      for (DWORD j = 0; j < data->numSplits; j++)
      {
        MaterialSplit* matSplit = (MaterialSplit*)ptr;
        ptr += 2 * matSplit->numIndices + 12;
      }
    }
  }
}

Th4Mesh::Th4Mesh(const char* const __restrict path, const DWORD index, const bool materials)
{
#define TEXFLAG_UNKNOWN 1
#define TEXFLAG_EXTENSION 2

#define MESHFLAG_TEXCOORD 1
#define MESHFLAG_COLOUR 2
#define MESHFLAG_NORMAL 4
#define MESHFLAG_SKIN 16
#define MESHFLAG_EXTENSION 2048
  FILE* f = fopen(path, "rb");
  if (f)
  {
    DWORD size = 0, ptr = 0;
    fseek(f, 0, SEEK_END);

    size = ftell(f);

    BYTE* pFile = new BYTE[size];

    fseek(f, 0, SEEK_SET);
    fread(pFile, size, 1, f);
    fclose(f);

    ptr = (DWORD)(pFile);
    ptr += 12;

    DWORD *numMaterials = (DWORD*)ptr;
    ptr += 4;
    for (DWORD i = 0; i < *numMaterials; i++)
    {

      Th4Material::Data* materialData = (Th4Material::Data*)ptr;

      ptr += 19;
      if (materialData->unkFlag)
        ptr += 8;

      for (DWORD j = 0; j < materialData->numTextures; j++)
      {
        Th4Texture::Data* textureData = (Th4Texture::Data*)ptr;
        ptr += 77;

        if (textureData->flags & TEXFLAG_UNKNOWN)
          ptr += 32;
        if (textureData->flags & TEXFLAG_EXTENSION)
        {
          DWORD *numExtensions = (DWORD*)ptr;
          ptr += 4;

          for (DWORD k = 0; k < *numExtensions; k++)
          {
            DWORD *numItems = (DWORD*)ptr;
            ptr += 8;
            for (DWORD l = 0; l < *numItems; l++)
            {
              ptr += 8;
            }
          }
        }
        ptr += 16;
      }
    }

    DWORD numObjects = ((DWORD*)ptr)[0];
    ptr += 4;
    for (DWORD i = 0; i < numObjects; i++)
    {

      if (i == index)
      {
        data = (Th4Mesh::Data*)malloc(sizeof(Th4Mesh::Data));
        memcpy(data, (Th4Mesh::Data*)ptr, 64);

        SetName(data->name);
        SetCenter(data->sphereCenter);
        SetBBox(data->bboxMin, data->bboxMax);

        ptr += 64;

        data->vertices = (SimpleVertex*)ptr;
        ptr += 12 * data->numVertices;

        if (data->flags & MESHFLAG_NORMAL)
        {
          data->normals = (SimpleVertex*)ptr;
          ptr += 12 * data->numVertices;
        }
        else
          data->normals = NULL;

        if (data->flags & MESHFLAG_SKIN)
        {
          ptr += 24 * data->numVertices;
        }
        DWORD numTexCoords;
        if (data->flags & MESHFLAG_TEXCOORD)
        {
          // meshData->texCoords = new TexCoord[meshData->numVertices];


          numTexCoords = ((DWORD*)ptr)[0]; data->texCoords = new TexCoord[data->numVertices*numTexCoords]; //if(numTexCoords>MAX_TEXCOORDS) MAX_TEXCOORDS = numTexCoords;//mesh->SetNumTexCoords(numTexCoords);
          ptr += 4;
          for (DWORD j = 0; j < data->numVertices*numTexCoords; j++)
          {
            data->texCoords[j] = ((TexCoord*)ptr)[0];
            ptr += 8;//*numTexCoords;
          }

        }
        else
          data->texCoords = NULL;

        if (data->flags & MESHFLAG_COLOUR)
        {
          data->colours = (Colour*)ptr;
          ptr += 4 * data->numVertices;
        }
        else
          data->colours = NULL;

        if (data->flags & MESHFLAG_EXTENSION)
        {
          ptr += data->numVertices;//colour wibble data?
        }


        Vertex* vertices = new Vertex[data->numVertices];

        for (int j = 0; j < data->numVertices; j++)
        {
          data->vertices[j].z *= -1;
          memcpy(&vertices[j], &data->vertices[j], 12);

          if (data->texCoords)
          {
            for (DWORD k = 0; k < numTexCoords; k++)
            {
              vertices[j].tUV[k].u = data->texCoords[j*numTexCoords + k].u;
              vertices[j].tUV[k].v = data->texCoords[j*numTexCoords + k].v;
            }

            for (DWORD k = numTexCoords; k < 4; k++)
            {
              vertices[j].tUV[k].u = data->texCoords[j*numTexCoords + numTexCoords].u;
              vertices[j].tUV[k].u = data->texCoords[j*numTexCoords + numTexCoords].v;
            }

            /*for(DWORD k=0; k<numTexCoords; k++)
            {
            vertices[j].uv[k*2] = meshData->texCoords[j+k].u;
            vertices[j].uv[k*2+1] = meshData->texCoords[j+k].v;
            }*/
            /*vertices[j].u = meshData->texCoords[j].u;
            vertices[j].v = meshData->texCoords[j].v;*/
          }

          if (data->colours)
          {
            //meshData->colours[j].a = 0xFF;
            vertices[j].colour = data->colours[j];
          }

        }

        delete[] data->texCoords;

        this->Reserve(data->numVertices);
        this->AddVertices(vertices, data->numVertices);

        delete[] vertices;

        for (DWORD j = 0; j < data->numSplits; j++)
        {
          // MaterialSplit* matSplit = new MaterialSplit;
          // memcpy(matSplit,(MaterialSplit*)ptr,12);
          MaterialSplit* matSplit = (MaterialSplit*)ptr;
          memset(&matSplit->Indices, 0, 12);
          matSplit->type = 0;
          ptr += 12;

          WORD* Indices = (WORD*)ptr;
          //matSplit->AddIndices(Indices,matSplit->numIndices);
          this->AddMatSplit(matSplit, NULL);
          this->matSplits.back().AddIndices(Indices, matSplit->numIndices);
          ptr += 2 * matSplit->numIndices;
          //delete matSplit;
        }
        free(data);
        break;
      }
      else
      {
        data = (Th4Mesh::Data*)ptr;
        ptr += 64;
        ptr += 12 * data->numVertices;

        if (data->flags & MESHFLAG_NORMAL)
        {
          ptr += 12 * data->numVertices;
        }

        if (data->flags & MESHFLAG_SKIN)
        {
          ptr += 24 * data->numVertices;
        }

        if (data->flags & MESHFLAG_TEXCOORD)
        {
          const DWORD numTexCoords = ((DWORD*)ptr)[0];
          ptr += 4;
          ptr += 8 * data->numVertices*numTexCoords;
        }

        if (data->flags & MESHFLAG_COLOUR)
        {
          ptr += 4 * data->numVertices;
        }

        if (data->flags & MESHFLAG_EXTENSION)
        {
          ptr += data->numVertices;//colour wibble data?
        }

        for (DWORD j = 0; j < data->numSplits; j++)
        {
          MaterialSplit* matSplit = (MaterialSplit*)ptr;
          ptr += 2 * matSplit->numIndices + 12;
        }
      }
    }

    delete[] pFile;
  }
}