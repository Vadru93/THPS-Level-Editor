#include "stdafx.h"
#include "Collision.h"
#include "Mesh.h"
#include "scene.h"
#include "player.h"
#pragma unmanaged
extern DWORD defaultCulling;

DWORD CollisionResult::GetCollType()
{
  if (mesh)
  {
    Collision* collision = mesh->GetCollision();
    if (collision)
      return collision->GetFaceFlag(triIndex);
  }
  return 0;
}

__declspec (noalias) bool TriangleIntersection(const D3DXVECTOR3* v1, const D3DXVECTOR3* v2, const D3DXVECTOR3* v3, const D3DXVECTOR3* rayPos, const D3DXVECTOR3* rayDir, float* outDistance, Vertex* outVertex)
{
  D3DXVECTOR3 edge1, edge2, tvec, pvec, qvec;
  float det, inv_det;

  edge1 = *v2 - *v1;
  edge2 = *v3 - *v1;

  D3DXVec3Cross(&pvec, rayDir, &edge2);
  det = D3DXVec3Dot(&edge1, &pvec);
  /*if(defaultCulling==D3DCULL_NONE)
  {*/
  if (det > -EPSILON && det < EPSILON)
    return false;
  inv_det = 1.0f / det;

  tvec = *rayPos - *v1;
  float u = D3DXVec3Dot(&tvec, &pvec)*inv_det;
  if (u<0.0f || u>1.0f)
    return false;

  D3DXVec3Cross(&qvec, &tvec, &edge1);
  float v = D3DXVec3Dot(rayDir, &qvec)*inv_det;
  if (v < 0.0f || u + v > 1.0f)
    return false;

  float t = D3DXVec3Dot(&edge2, &qvec)*inv_det;
  if (t < 0.00001f)
    return false;

  D3DXVECTOR3 target = *v1 + (u*(*v2 - *v1)) + (v*(*v3 - *v1));
  if (defaultCulling == D3DCULL_CW)
  {
    D3DXVECTOR3 faceNormal;
    D3DXVec3Cross(&faceNormal, &edge1, &edge2);
    if (D3DXVec3Dot(&faceNormal, rayDir) < 0)
      return 0;
  }
  float closestDist = D3DXVec3Length(&(target - *v1));
  outVertex = (Vertex*)v1;
  float dist;
  if ((dist = D3DXVec3Length(&(target - *v2))) < closestDist)
  {
    closestDist = dist;
    outVertex = (Vertex*)v2;
  }
  if ((dist = D3DXVec3Length(&(target - *v3))) < closestDist)
  {
    outVertex = (Vertex*)v3;
    closestDist = dist;
  }


  //D3DXVECTOR3 distance = target-*rayPos;
  *outDistance = D3DXVec3Length(&(*(D3DXVECTOR3*)outVertex - *rayPos)) + closestDist;
  return true;
}

__declspec (noalias) bool TriangleIntersection(const D3DXVECTOR3* v1, const D3DXVECTOR3* v2, const D3DXVECTOR3* v3, const D3DXVECTOR3* rayPos, const D3DXVECTOR3* rayDir)
{
  D3DXVECTOR3 edge1, edge2, tvec, pvec, qvec;
  float det, inv_det;

  edge1 = *v2 - *v1;
  edge2 = *v3 - *v1;

  D3DXVec3Cross(&pvec, rayDir, &edge2);
  det = D3DXVec3Dot(&edge1, &pvec);
  /*if(defaultCulling==D3DCULL_NONE)
  {*/
  if (det > -EPSILON && det < EPSILON)
    return false;
  inv_det = 1.0f / det;

  tvec = *rayPos - *v1;
  float u = D3DXVec3Dot(&tvec, &pvec)*inv_det;
  if (u<0.0f || u>1.0f)
    return false;

  D3DXVec3Cross(&qvec, &tvec, &edge1);
  float v = D3DXVec3Dot(rayDir, &qvec)*inv_det;
  if (v < 0.0f || u + v > 1.0f)
    return false;

  float t = D3DXVec3Dot(&edge2, &qvec)*inv_det;
  if (t < 0.00001f)
    return false;
  //*outDistance = D3DXVec3Length(&(target-*rayPos));
  return true;
}

__declspec (noalias) bool TriangleIntersection(const D3DXVECTOR3* v1, const D3DXVECTOR3* v2, const D3DXVECTOR3* v3, const D3DXVECTOR3* rayPos, const D3DXVECTOR3* rayDir, D3DXVECTOR3* outHitPoint)
{
  D3DXVECTOR3 edge1, edge2, tvec, pvec, qvec;
  float det, inv_det;

  edge1 = *v2 - *v1;
  edge2 = *v3 - *v1;

  D3DXVec3Cross(&pvec, rayDir, &edge2);
  det = D3DXVec3Dot(&edge1, &pvec);
  /*if(defaultCulling==D3DCULL_NONE)
  {*/
  if (det > -EPSILON && det < EPSILON)
    return false;
  inv_det = 1.0f / det;

  tvec = *rayPos - *v1;
  float u = D3DXVec3Dot(&tvec, &pvec)*inv_det;
  if (u<0.0f || u>1.0f)
    return false;

  D3DXVec3Cross(&qvec, &tvec, &edge1);
  float v = D3DXVec3Dot(rayDir, &qvec)*inv_det;
  if (v < 0.0f || u + v > 1.0f)
    return false;

  float t = D3DXVec3Dot(&edge2, &qvec)*inv_det;
  if (t < 0.00001f)
    return false;

  D3DXVECTOR3 target = *v1 + (u*(*v2 - *v1)) + (v*(*v3 - *v1));
  if (defaultCulling == D3DCULL_CW)
  {
    D3DXVECTOR3 faceNormal;
    D3DXVec3Cross(&faceNormal, &edge1, &edge2);
    if (D3DXVec3Dot(&faceNormal, rayDir) < 0)
      return 0;
  }
  //D3DXVECTOR3 distance = target-*rayPos;
  *outHitPoint = target;
  //*outDistance = D3DXVec3Length(&(target-*rayPos));
  return true;
}


__declspec (noalias) bool TriangleIntersection(const D3DXVECTOR3* v1, const D3DXVECTOR3* v2, const D3DXVECTOR3* v3, const D3DXVECTOR3* rayPos, const D3DXVECTOR3* rayDir, float* outDistance)
{
  D3DXVECTOR3 edge1, edge2, tvec, pvec, qvec;
  float det, inv_det;

  edge1 = *v2 - *v1;
  edge2 = *v3 - *v1;

  D3DXVec3Cross(&pvec, rayDir, &edge2);
  det = D3DXVec3Dot(&edge1, &pvec);
  /*if(defaultCulling==D3DCULL_NONE)
  {*/
  if (det > -EPSILON && det < EPSILON)
    return false;
  inv_det = 1.0f / det;

  tvec = *rayPos - *v1;
  float u = D3DXVec3Dot(&tvec, &pvec)*inv_det;
  if (u<0.0f || u>1.0f)
    return false;

  D3DXVec3Cross(&qvec, &tvec, &edge1);
  float v = D3DXVec3Dot(rayDir, &qvec)*inv_det;
  if (v < 0.0f || u + v > 1.0f)
    return false;

  float t = D3DXVec3Dot(&edge2, &qvec)*inv_det;
  if (t < 0.00001f)
    return false;

  D3DXVECTOR3 target = *v1 + (u*(*v2 - *v1)) + (v*(*v3 - *v1));
  if (defaultCulling == D3DCULL_CW)
  {
    D3DXVECTOR3 faceNormal;
    D3DXVec3Cross(&faceNormal, &edge1, &edge2);
    if (D3DXVec3Dot(&faceNormal, rayDir) < 0)
      return 0;
  }
  //D3DXVECTOR3 distance = target-*rayPos;
  *outDistance = D3DXVec3Length(&(target - *rayPos));
  return true;
}

__declspec (noalias) void Collision::GetHitPoint(const Ray & ray, CollisionResult & closestHit, const Mesh* __restrict mesh) const
{
  const Vertex* __restrict const vertices = this->vertices.size() == 0 ? mesh->GetVertices() : &this->vertices.front();
  const DWORD numFaces = faces.size();

  for (DWORD i = 0; i < numFaces; i++)
  {
    const SimpleFace & face = faces[i];
    const D3DXVECTOR3 v1 = *(D3DXVECTOR3*)&vertices[face.a];
    const D3DXVECTOR3 v2 = *(D3DXVECTOR3*)&vertices[face.b];
    const D3DXVECTOR3 v3 = *(D3DXVECTOR3*)&vertices[face.c];

    D3DXVECTOR3 edge1, edge2, tvec, pvec, qvec;
    float det, inv_det;

    edge1 = v2 - v1;
    edge2 = v3 - v1;

    D3DXVec3Cross(&pvec, &ray.direction, &edge2);
    det = D3DXVec3Dot(&edge1, &pvec);
    /*if(defaultCulling==D3DCULL_NONE)
    {*/
    if (det > -EPSILON && det < EPSILON)
      continue;
    inv_det = 1.0f / det;

    tvec = ray.origin - v1;
    const float u = D3DXVec3Dot(&tvec, &pvec)*inv_det;
    if (u<0.0f || u>1.0f)
      continue;

    D3DXVec3Cross(&qvec, &tvec, &edge1);
    const float v = D3DXVec3Dot(&ray.direction, &qvec)*inv_det;
    if (v < 0.0f || u + v > 1.0f)
      continue;

    const float t = D3DXVec3Dot(&edge2, &qvec)*inv_det;
    if (t < 0.00001f)
      continue;

    if (defaultCulling == D3DCULL_CW)
    {
      D3DXVECTOR3 faceNormal;
      D3DXVec3Cross(&faceNormal, &edge1, &edge2);
      if (D3DXVec3Dot(&faceNormal, &ray.direction) < 0)
        continue;
    }
    const D3DXVECTOR3 hitPoint = v1 + (u*(v2 - v1)) + (v*(v3 - v1));
    const float distance = D3DXVec3Length(&(hitPoint - ray.origin));
    if (distance < closestHit.distance)
    {
      closestHit.distance = distance;
      closestHit.SetHitPoint(hitPoint);
    }
  }
}

__declspec (noalias) void Collision::FindGround(const Ray & __restrict ray, const Mesh* __restrict const mesh, D3DXVECTOR3 & __restrict newPos) const
{
  const Vertex* __restrict const vertices = this->vertices.size() == 0 ? mesh->GetVertices() : &this->vertices.front();
  const DWORD numFaces = faces.size();
  extern Player* __restrict player;

  for (DWORD i = 0; i < numFaces; i++)
  {
    WORD faceFlag = GetFaceFlag(i);
    if ((faceFlag & 0x10))
      continue;

    const SimpleFace & face = faces[i];
    const D3DXVECTOR3 v1 = *(D3DXVECTOR3*)&vertices[face.a];
    const D3DXVECTOR3 v2 = *(D3DXVECTOR3*)&vertices[face.b];
    const D3DXVECTOR3 v3 = *(D3DXVECTOR3*)&vertices[face.c];

    D3DXVECTOR3 edge1, edge2, tvec, pvec, qvec;
    float det, inv_det;

    edge1 = v2 - v1;
    edge2 = v3 - v1;

    D3DXVec3Cross(&pvec, &ray.direction, &edge2);
    det = D3DXVec3Dot(&edge1, &pvec);
    /*if(defaultCulling==D3DCULL_NONE)
    {*/
    if (det > -EPSILON && det < EPSILON)
      continue;
    inv_det = 1.0f / det;

    tvec = ray.origin - v1;
    const float u = D3DXVec3Dot(&tvec, &pvec)*inv_det;
    if (u<0.0f || u>1.0f)
      continue;

    D3DXVec3Cross(&qvec, &tvec, &edge1);
    const float v = D3DXVec3Dot(&ray.direction, &qvec)*inv_det;
    if (v < 0.0f || u + v > 1.0f)
      continue;

    const float t = D3DXVec3Dot(&edge2, &qvec)*inv_det;
    if (t < 0.00001f)
      continue;

    if (defaultCulling == D3DCULL_CW)
    {
      D3DXVECTOR3 faceNormal;
      D3DXVec3Cross(&faceNormal, &edge1, &edge2);
      if (D3DXVec3Dot(&faceNormal, &ray.direction) < 0)
        continue;
    }
    player->hitFace = &faces[i];
    const D3DXVECTOR3 hitPoint = v1 + (u*(v2 - v1)) + (v*(v3 - v1));
    const float distance = D3DXVec3Length(&(hitPoint - ray.origin));
    if (distance<player->height)
      player->height = distance;
  }
}

__declspec (noalias) bool Collision::AboveSurface(const Ray & __restrict ray, const Mesh* __restrict const mesh) const
{
  const Vertex* __restrict const vertices = this->vertices.size() == 0 ? mesh->GetVertices() : &this->vertices.front();
  const DWORD numFaces = faces.size();
  extern Player* __restrict player;

  for (DWORD i = 0; i < numFaces; i++)
  {
    WORD faceFlag = GetFaceFlag(i);
    if (faceFlag & 0x10)
      continue;

    const SimpleFace & face = faces[i];
    const D3DXVECTOR3 v1 = *(D3DXVECTOR3*)&vertices[face.a];
    const D3DXVECTOR3 v2 = *(D3DXVECTOR3*)&vertices[face.b];
    const D3DXVECTOR3 v3 = *(D3DXVECTOR3*)&vertices[face.c];

    D3DXVECTOR3 edge1, edge2, tvec, pvec, qvec;
    float det, inv_det;

    edge1 = v2 - v1;
    edge2 = v3 - v1;

    D3DXVec3Cross(&pvec, &ray.direction, &edge2);
    det = D3DXVec3Dot(&edge1, &pvec);
    /*if(defaultCulling==D3DCULL_NONE)
    {*/
    if (det > -EPSILON && det < EPSILON)
      continue;
    inv_det = 1.0f / det;

    tvec = ray.origin - v1;
    const float u = D3DXVec3Dot(&tvec, &pvec)*inv_det;
    if (u<0.0f || u>1.0f)
      continue;

    D3DXVec3Cross(&qvec, &tvec, &edge1);
    const float v = D3DXVec3Dot(&ray.direction, &qvec)*inv_det;
    if (v < 0.0f || u + v > 1.0f)
      continue;

    const float t = D3DXVec3Dot(&edge2, &qvec)*inv_det;
    if (t < 0.00001f)
      continue;

    if (defaultCulling == D3DCULL_CW)
    {
      D3DXVECTOR3 faceNormal;
      D3DXVec3Cross(&faceNormal, &edge1, &edge2);
      if (D3DXVec3Dot(&faceNormal, &ray.direction) < 0)
        continue;
    }
    player->hitFace = &faces[i];
    return true;
  }
  return false;
}

__declspec (noalias) void Collision::Intersect(const Ray & ray, CollisionResult & closestHit, const Mesh* __restrict mesh, D3DXVECTOR3 & newPos) const
{
  const Vertex* __restrict const vertices = this->vertices.size() == 0 ? mesh->GetVertices() : &this->vertices.front();
  const DWORD numFaces = faces.size();
  extern Player* __restrict player;

  for (DWORD i = 0; i < numFaces; i++)
  {
    WORD faceFlag = GetFaceFlag(i);
    if (faceFlag & 0x10 && !(faceFlag & 0x40))
      continue;
    const SimpleFace & face = faces[i];
    const D3DXVECTOR3 v1 = *(D3DXVECTOR3*)&vertices[face.a];
    const D3DXVECTOR3 v2 = *(D3DXVECTOR3*)&vertices[face.b];
    const D3DXVECTOR3 v3 = *(D3DXVECTOR3*)&vertices[face.c];

    D3DXVECTOR3 edge1, edge2, tvec, pvec, qvec;
    float det, inv_det;

    edge1 = v2 - v1;
    edge2 = v3 - v1;

    D3DXVec3Cross(&pvec, &ray.direction, &edge2);
    det = D3DXVec3Dot(&edge1, &pvec);
    /*if(defaultCulling==D3DCULL_NONE)
    {*/
    if (det > -EPSILON && det < EPSILON)
      continue;
    inv_det = 1.0f / det;

    tvec = ray.origin - v1;
    const float u = D3DXVec3Dot(&tvec, &pvec)*inv_det;
    if (u<0.0f || u>1.0f)
      continue;

    D3DXVec3Cross(&qvec, &tvec, &edge1);
    const float v = D3DXVec3Dot(&ray.direction, &qvec)*inv_det;
    if (v < 0.0f || u + v > 1.0f)
      continue;

    const float t = D3DXVec3Dot(&edge2, &qvec)*inv_det;
    if (t < 0.00001f)
      continue;

    if (defaultCulling == D3DCULL_CW)
    {
      D3DXVECTOR3 faceNormal;
      D3DXVec3Cross(&faceNormal, &edge1, &edge2);
      if (D3DXVec3Dot(&faceNormal, &ray.direction) < 0)
        continue;
    }
    const D3DXVECTOR3 hitPoint = v1 + (u*(v2 - v1)) + (v*(v3 - v1));
    const float distance = D3DXVec3Length(&(hitPoint - ray.origin));
    const float dist = D3DXVec3Length(&(newPos - ray.origin)) + 16.0f;

    if (distance < closestHit.distance && distance <= dist)
    {
      if (!(faceFlag & 0x10))
      {
        newPos = hitPoint;
        //newPos.x -= ray.direction.x*0.5f;
        //newPos.z -= ray.direction.z*0.5f;

        /*D3DXVECTOR3 V1 = (v2 - v1);
        D3DXVECTOR3 V2 = (v3 - v1);*/
        D3DXVECTOR3 surfaceNormal;
        /*surfaceNormal.x = (V1.y*V2.z) - (V1.z - V2.y);
        surfaceNormal.z = (V1.x - V2.y) - (V1.y - V2.x);*/
        D3DXVec3Cross(&surfaceNormal, &edge1, &edge2);

        D3DXVec3Normalize(&surfaceNormal, &surfaceNormal);
        player->faceNormal = surfaceNormal;
        char msg[256];
        /*sprintf(msg, "x %f y %f z %f", surfaceNormal.x, surfaceNormal.y, surfaceNormal.z);
        MessageBox(0, msg, msg, 0);*/
        if (surfaceNormal.y > -0.65f)
        {
          //newPos = ray.origin;
          newPos -= surfaceNormal*2.0f;
          player->velocity = D3DXVECTOR3(0, 0, 0);
        }
        /*else if(surfaceNormal.y > -0.90f)
        {
          D3DXVECTOR3 direction = newPos - ray.origin;
          D3DXVec3Normalize(&direction, &direction);
          newPos -= direction*30.0f;
          newPos.y += 15.0f;

        }*/
      else
          newPos.y += 10.0f;
        closestHit.distance = distance;
        closestHit.mesh = mesh;
        closestHit.triIndex = i;
        /*}
        else
        {
        newPos.x = ray.origin.x;
        newPos.z = ray.origin.z;
        }*/
      }
      if ((faceFlag & 0x40))
      {
        extern Scene* __restrict scene;

        for (DWORD i = 0; i < scene->nodes.size(); i++)
        {
          if (scene->nodes[i].Name.checksum == mesh->GetName().checksum)
          {
            if (scene->nodes[i].Trigger.checksum == Checksum("sk3_killskater").checksum)
            {
              if (scene->nodes[i].Links.size())
              {
                newPos.x = scene->nodes[scene->nodes[i].Links[0]].Position.x;
                newPos.y = scene->nodes[scene->nodes[i].Links[0]].Position.y+10.0f;
                newPos.z = scene->nodes[scene->nodes[i].Links[0]].Position.z;
                *(D3DXVECTOR3*)&ray.origin = *(D3DXVECTOR3*)&newPos;
              }
              goto done;
            }
            else if (scene->nodes[i].Trigger.checksum)
            {
              for (DWORD j = 0, numScripts = scene->KnownScripts.size(); j < numScripts; j++)
              {
                if (scene->KnownScripts[j].name.checksum == scene->nodes[i].Trigger.checksum)
                {
                  D3DXVECTOR3 oldPos = newPos;
                  scene->KnownScripts[j].Execute(&scene->nodes[i]);
                  if (oldPos != newPos)
                    *(D3DXVECTOR3*)&ray.origin = *(D3DXVECTOR3*)&player->newPos;
                  /*BYTE* param = NULL;
                  redo:
                  BYTE* pFile = scene->KnownScripts[j].func;
                  BYTE* eof = pFile + scene->KnownScripts[j].size;
                  while (pFile < eof)
                  {
                    const BYTE opcode = *pFile;
                    pFile++;
                    switch (opcode)
                    {
                    case 0x1C:
                    case 0x1B:
                      pFile += *(DWORD*)pFile + 4;
                      break;
                    case 0x17:
                    case 0x1A:
                    case 0x2E:
                      pFile += 4;
                      break;
                    case 2:
                      pFile += 4;
                      break;
                    case 1:
                      break;
                    case 0x1F:
                      pFile += 8;
                      break;
                    case 0x1E:
                      pFile += 12;
                      break;
                    case 0x40:
                    case 0x2F:
                    case 0x37:
                      pFile += *(DWORD*)pFile * 4 + 4;
                      break;
                    case 0x16:
                      if (*(DWORD*)pFile == Checksum("Sk3_TeleportToNode").checksum)
                      {
                        pFile += 4;
                        pFile += 6;
                        if (*pFile != 0x2D)
                        {
                          param = NULL;
                        }
                        while (*pFile != 0x16)
                          pFile++;

                        pFile++;
                        DWORD nodeName = *(DWORD*)pFile;
                        if (param)
                        {
                          while (*(DWORD*)param != nodeName)
                            param++;
                          param += 4;
                          while (*param != 0x16 && param < (eof-5))
                            param++;
                          param++;
                          nodeName = *(DWORD*)param;
                        }
                        for (DWORD k = 0; k < scene->nodes.size(); k++)
                        {
                          if (scene->nodes[k].Name.checksum == nodeName)
                          {
                            newPos.x = scene->nodes[k].Position.x;
                            newPos.y = scene->nodes[k].Position.y+10.0f;
                            newPos.z = scene->nodes[k].Position.z;
                            *(D3DXVECTOR3*)&ray.origin = *(D3DXVECTOR3*)&scene->nodes[k].Position;
                            (*(D3DXVECTOR3*)&ray.origin).y += 10.0f;
                            goto done;
                          }
                        }
                        goto done;
                      }
                      else
                      {
                        DWORD crc = *(DWORD*)pFile;
                        for (DWORD k = 0; k < scene->KnownScripts.size(); k++)
                        {
                          if (scene->KnownScripts[k].name.checksum == crc)
                          {
                            pFile += 4;
                            param = pFile;
                            j = k;
                            goto redo;
                          }
                        }
                      }
                      pFile += 4;
                      break;
                    }
                  }*/
                  goto done;
                }
              }
            }
            goto done;
          }
        }
      }

    done:;

    }
  }
}

__declspec (noalias) void Collision::Render(IDirect3DDevice9Ex* const __restrict Device, const bool isSelected, const Mesh* __restrict mesh) const//const Vertex* const vertices, const DWORD numVertices) const
{
  DWORD numVertices = this->vertices.size();
  const Vertex* verts = NULL;

  if (numVertices == 0)
  {
    numVertices = mesh->GetNumVertices();
    if (numVertices == 0)
      return;
    verts = mesh->GetVertices();
  }
  else
    verts = &this->vertices.front();

  IDirect3DDevice9Ex* const __restrict Dev = Device;
  if (isSelected)
  {
    Dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
    Dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    Dev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, numVertices, faces.size(), &faces.front(), D3DFMT_INDEX16, verts, sizeof(Vertex));
    //this->vertices.size() == 0? Dev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,0,numVertices,faces.size(),&faces.front(),D3DFMT_INDEX16,vertices,sizeof(Vertex)) : Dev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,0,this->vertices.size(),faces.size(),&faces.front(),D3DFMT_INDEX16,&this->vertices.front(),sizeof(Vertex));
    Dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    Dev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
  }
  else
    Dev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, numVertices, faces.size(), &faces.front(), D3DFMT_INDEX16, verts, sizeof(Vertex));//this->vertices.size() == 0? Dev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,0,numVertices,faces.size(),&faces.front(),D3DFMT_INDEX16,vertices,sizeof(Vertex)) : Dev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,0,this->vertices.size(),faces.size(),&faces.front(),D3DFMT_INDEX16,&this->vertices.front(),sizeof(Vertex));
}

__declspec (noalias) void Collision::CastRay(const Ray & __restrict ray, const Mesh* __restrict mesh, CollisionResult & __restrict result) const
{
  const DWORD numFaces = faces.size();
  if (numFaces == 0)
    return;
  const Vertex* __restrict const vertices = this->vertices.size() == 0 ? mesh->GetVertices() : &this->vertices.front();

  for (DWORD i = 0; i < numFaces; i++)
  {
    WORD faceFlag = GetFaceFlag(i);
    if (faceFlag & 0x10 && !(faceFlag & 0x40))
      continue;
    const SimpleFace & face = faces[i];
    const D3DXVECTOR3 v1 = *(D3DXVECTOR3*)&vertices[face.a];
    const D3DXVECTOR3 v2 = *(D3DXVECTOR3*)&vertices[face.b];
    const D3DXVECTOR3 v3 = *(D3DXVECTOR3*)&vertices[face.c];

    D3DXVECTOR3 edge1, edge2, tvec, pvec, qvec;
    float det, inv_det;

    edge1 = v2 - v1;
    edge2 = v3 - v1;

    D3DXVec3Cross(&pvec, &ray.direction, &edge2);
    det = D3DXVec3Dot(&edge1, &pvec);
    /*if(defaultCulling==D3DCULL_NONE)
    {*/
    if (det > -EPSILON && det < EPSILON)
      continue;
    inv_det = 1.0f / det;

    tvec = ray.origin - v1;
    const float u = D3DXVec3Dot(&tvec, &pvec)*inv_det;
    if (u<0.0f || u>1.0f)
      continue;

    D3DXVec3Cross(&qvec, &tvec, &edge1);
    const float v = D3DXVec3Dot(&ray.direction, &qvec)*inv_det;
    if (v < 0.0f || u + v > 1.0f)
      continue;

    const float t = D3DXVec3Dot(&edge2, &qvec)*inv_det;
    if (t < 0.00001f)
      continue;

    if (defaultCulling == D3DCULL_CW)
    {
      D3DXVECTOR3 faceNormal;
      D3DXVec3Cross(&faceNormal, &edge1, &edge2);
      if (D3DXVec3Dot(&faceNormal, &ray.direction) < 0)
        continue;
    }
    if (faceFlag & 0x40)
    {
      extern Scene* __restrict scene;

      for (DWORD i = 0; i < scene->nodes.size(); i++)
      {
        if (scene->nodes[i].Name.checksum == mesh->GetName().checksum)
        {
          if (scene->nodes[i].Trigger.checksum)
          {
            for (DWORD j = 0, numScripts = scene->KnownScripts.size(); j < numScripts; j++)
            {
              if (scene->KnownScripts[j].name.checksum == scene->nodes[i].Trigger.checksum)
              {
                scene->KnownScripts[j].Execute();
                //MessageBox(0, mesh->GetName().GetString(), scene->KnownScripts[j].name.GetString(), 0);
                goto done;
              }
            }
          }
          goto done;
        }
      }
    done:;
    }

    if (!(faceFlag & 0x10))
    {
      const D3DXVECTOR3 hitPoint = v1 + (u*(v2 - v1)) + (v*(v3 - v1));

      const float distance = D3DXVec3Length(&(hitPoint - ray.origin));
      if (distance < result.distance)
      {
        result.distance = distance;
        *(D3DXVECTOR3*)&result.hitPoint = hitPoint;
      }
    }
  }
}

__declspec (noalias) void Collision::Intersect(const Ray & ray, CollisionResult & closestHit, const Mesh* __restrict mesh) const
{
  const DWORD numFaces = faces.size();
  if (numFaces == 0)
    return;
  const Vertex* __restrict const vertices = this->vertices.size() == 0 ? mesh->GetVertices() : &this->vertices.front();
 
  for (DWORD i = 0; i < numFaces; i++)
  {
    const SimpleFace & face = faces[i];
    const D3DXVECTOR3 v1 = *(D3DXVECTOR3*)&vertices[face.a];
    const D3DXVECTOR3 v2 = *(D3DXVECTOR3*)&vertices[face.b];
    const D3DXVECTOR3 v3 = *(D3DXVECTOR3*)&vertices[face.c];

    D3DXVECTOR3 edge1, edge2, tvec, pvec, qvec;
    float det, inv_det;

    edge1 = v2 - v1;
    edge2 = v3 - v1;

    D3DXVec3Cross(&pvec, &ray.direction, &edge2);
    det = D3DXVec3Dot(&edge1, &pvec);
    /*if(defaultCulling==D3DCULL_NONE)
    {*/
    if (det > -EPSILON && det < EPSILON)
      continue;
    inv_det = 1.0f / det;

    tvec = ray.origin - v1;
    const float u = D3DXVec3Dot(&tvec, &pvec)*inv_det;
    if (u<0.0f || u>1.0f)
      continue;

    D3DXVec3Cross(&qvec, &tvec, &edge1);
    const float v = D3DXVec3Dot(&ray.direction, &qvec)*inv_det;
    if (v < 0.0f || u + v > 1.0f)
      continue;

    const float t = D3DXVec3Dot(&edge2, &qvec)*inv_det;
    if (t < 0.00001f)
      continue;

    if (defaultCulling == D3DCULL_CW)
    {
      D3DXVECTOR3 faceNormal;
      D3DXVec3Cross(&faceNormal, &edge1, &edge2);
      if (D3DXVec3Dot(&faceNormal, &ray.direction) < 0)
        continue;
    }
    const D3DXVECTOR3 hitPoint = v1 + (u*(v2 - v1)) + (v*(v3 - v1));
    const float distance = D3DXVec3Length(&(hitPoint - ray.origin));
    if (distance < closestHit.distance)
    {
      closestHit.distance = distance;
      closestHit.mesh = mesh;
      closestHit.triIndex = i;
    }
  }
}

__declspec (noalias) bool Collision::Intersect(const D3DXVECTOR3* rayOrigin, const D3DXVECTOR3* rayDirection, float* outDistance, Vertex* meshVertices, DWORD* outTriangleIndex)
{
  if (faces.size() == 0)
    return false;
  float closestIntersect = 0.0f;
  bool intersect = false;
  Vertex* vertices = this->vertices.size() == 0 ? meshVertices : &this->vertices.front();

  for (DWORD i = 0; i<faces.size(); i++)
  {
    if (TriangleIntersection((D3DXVECTOR3*)&vertices[faces[i].a], (D3DXVECTOR3*)&vertices[faces[i].b], (D3DXVECTOR3*)&vertices[faces[i].c], rayOrigin, rayDirection, outDistance))
    {
      if (closestIntersect > *outDistance || !intersect)
      {
        closestIntersect = *outDistance;
        intersect = true;
        if (outTriangleIndex)
          *outTriangleIndex = i;
      }
    }
  }
  *outDistance = closestIntersect;
  return intersect;
}

NSCollisionObject* ProcessNSCollision(DWORD ptr)
{
  NSCollisionHeader* collisionHeader = (NSCollisionHeader*)ptr;
  ptr += sizeof(NSCollisionHeader);

  NSCollisionObject* collision = (NSCollisionObject*)ptr;
  ptr += (sizeof(NSCollisionObject)*collisionHeader->numObjects);

  DWORD baseVertAddr = ptr;
  if (collisionHeader->Version == 8)
    ptr += (collisionHeader->numTotalVerts*sizeof(NSFloatVert));
  else
  {
    ptr += (collisionHeader->numTotalVerts);
    ptr += (collisionHeader->numLargeVerts*sizeof(NSFloatThugVert));
    ptr += (collisionHeader->numSmallVerts*sizeof(NSFixedVert));
    ptr = (DWORD)((DWORD)(ptr + 3) & 0xFFFFFFFC);
  }

  DWORD baseFaceAddr = ptr;

  for (DWORD i = 0; i < collisionHeader->numObjects; ++i)
  {
    if (collisionHeader->Version == 8)
      collision[i].FloatVert = (NSFloatVert*)(baseVertAddr + (DWORD)((DWORD)collision[i].FloatVert << 4));
    else
      collision[i].FloatVert = (NSFloatVert*)(baseVertAddr + (DWORD)((DWORD)collision[i].FloatVert));
    collision[i].Faces = (NSFace*)(baseFaceAddr + (DWORD)collision[i].Faces);
  }

  return collision;
}
