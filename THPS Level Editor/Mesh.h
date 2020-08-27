#ifndef MESH_H
#define MESH_H

#include "Structs.h"
#include "Th4Material.h"
#include "Collision.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include "Camera.h"
#include <stdio.h>
#include <stdlib.h>
#include <d3dx9.h>
#include <D3dx9math.h>
#pragma unmanaged
#include <memory>

const enum MeshFlags
{
  visible=1,
  transparent=2,
  movable=4,
  deleted=8,
};




extern DWORD defaultCulling;

struct Node;

struct Mesh;

struct Camera;

static bool ClipSegment(float min, float max, float a, float b, float d, float& t0, float& t1)
{
  const float threshold = 1.0e-6f;

  if (abs(d) < threshold)
  {
    if (d > 0.0f)
    {
      return !(b < min || a > max);
    }
    else
    {
      return !(a < min || b > max);
    }
  }

  float u0, u1;

  u0 = (min - a) / (d);
  u1 = (max - a) / (d);

  if (u0 > u1) 
  {
    float tmp = u0;
    u0 = u1;
    u1= tmp;
  }

  if (u1 < t0 || u0 > t1)
  {
    return false;
  }

  if(t0>u0)
    t0 = u0;
  if(u1<t1)
    t1 = u1;

  if (t1 < t0)
  {
    return false;
  }

  return true; 
}

struct SelectedObject
{
  //Checksum name;
  //DWORD index;
  Mesh* mesh;

  SelectedObject(Mesh* const mesh, bool Event = true);
};

struct SelectedTri
{
  Vertex vertices[3];
  WORD a;
  WORD b;
  WORD c;
  DWORD faceIndex;
  void* face;
  FaceInfo* collision;

  SelectedTri(MaterialSplit* const split, FaceInfo* const faceInfo, const DWORD &faceIndex, const Vertex* const vertices)
  {
    this->vertices[0] = vertices[split->Indices[faceIndex]];
    this->vertices[0].colour = Colour(128,128,128);
    this->vertices[0].colour.a = 175;
    this->vertices[1] = vertices[split->Indices[faceIndex+1]];
    this->vertices[1].colour = Colour(128,128,128);
    this->vertices[1].colour.a = 175;
    this->vertices[2] = vertices[split->Indices[faceIndex+2]];
    this->vertices[2].colour = Colour(128,128,128);
    this->vertices[2].colour.a = 175;
    this->faceIndex = faceIndex;
    collision=faceInfo;
    face = split;

    a=0;
    b=1;
    c=2;
  }

  SelectedTri(SimpleFace* const face, FaceInfo* const faceInfo, const Vertex* const vertices)
  {
    this->vertices[0] = vertices[face->a];
    this->vertices[0].colour = Colour(128,128,128);
    this->vertices[0].colour.a = 175;
    this->vertices[1] = vertices[face->b];
    this->vertices[1].colour = Colour(128,128,128);
    this->vertices[1].colour.a = 175;
    this->vertices[2] = vertices[face->c];
    this->vertices[2].colour = Colour(128,128,128);
    this->vertices[2].colour.a = 175;
    faceIndex = 0xFFFFFFFF;
    collision=faceInfo;
    this->face = face;

    a=0;
    b=1;
    c=2;
  }

};

struct Rail
{
  vector <Node*> nodes;

  Rail()
  {
  }

  Rail(Node* node)
  {
    nodes.push_back(node);
  }

  ~Rail()
  {
    nodes.clear();
  }
};

template <class X> class copied_ptr
{
public:
    typedef X element_type;

    explicit copied_ptr(X* p = 0) throw()       : itsPtr(p) {}
    ~copied_ptr()                               {delete itsPtr;}
    copied_ptr(const copied_ptr& r)             {copy(r.get());}
    copied_ptr& operator=(const copied_ptr& r)
    {
        if (this != &r) {
            delete itsPtr;
            copy(r);
        }
        return *this;
    }

    X& operator*()  const throw()               {return *itsPtr;}
    X* operator->() const throw()               {return itsPtr;}
    X* get()        const throw()               {return itsPtr;}

private:
    X* itsPtr;
    void copy(const copied_ptr& r)  {itsPtr = r.itsPtr ? new X(*r.itsPtr) : 0;}
};

struct Mesh
{

  //protected:
  DWORD flags;
  IDirect3DVertexBuffer9 *verts;
  vector <MaterialSplit> matSplits;
protected:
  vector <Vertex> vertices;
  Checksum name;
  //bool transparent;
  //DWORD numTexCoords;
  //BBox bbox;
  //std::unique_ptr<Collision> collision;//
  std::shared_ptr<Collision> collision;//Collision* collision;//Collision collision;
public:
  //Node* node;
  //vector <Rail> rails;
  __forceinline const bool &GetFlag(const MeshFlags &flag) const
  {
    return flags & flag;
  }

   /*Mesh (const Mesh& cpy) : flags(cpy.flags), verts(cpy.verts), matSplits(cpy.matSplits), vertices(cpy.vertices), 
     name(cpy.name), collision(new Collision(*cpy.collision)), node(cpy.node), bbox(cpy.bbox), center(cpy.center){};*/

protected:
  BBox bbox;
  //vector <Vertex> vertices;
public:
  D3DXVECTOR3 center;

  inline Vertex & GetVertex(const WORD index)
  {
    return vertices[index];
  }

  void CopyVertsToColl()
  {
    if (collision && collision->GetNumVertices() == 0)
    {
      collision->AddVertices(&vertices.front(), vertices.size());
      collision->BlendFaceTypes();
    }

  }

  void ExportObj(FILE* f, DWORD & vcount)
  {
    DWORD numVertices = GetNumVertices();
    if (numVertices == 0 || matSplits.size() == 0)
      return;
    fprintf(f, "g %s\n", GetName().GetString2());
    fprintf(f, "o %s\n", GetName().GetString2());

    for (DWORD j = 0; j<numVertices; j++)
    {
      fprintf(f, "v %.5f %.5f %.5f\n", vertices[j].x, vertices[j].y, vertices[j].z*-1);
      fprintf(f, "vt %.5f %.5f\n", vertices[j].tUV[0].u, vertices[j].tUV[0].v);
    }


    for (DWORD j = 0, numSplits = GetNumSplits(); j<numSplits; j++)
    {
      fprintf(f, "usemtl Material%d\n", matSplits[j].matId);
      WORD v1 = 0, v2 = 0, v3 = 0;
      bool tmp = false;
      for (DWORD k = 0, numIndices = matSplits[j].numIndices; k<numIndices; k++)
      {
        if (!tmp)
        {
          v1 = matSplits[j].Indices[k];
          v2 = matSplits[j].Indices[k + 1];
          v3 = matSplits[j].Indices[k + 2];
          tmp = true;
        }
        else
        {
          v1 = v3;
          v3 = matSplits[j].Indices[k + 2];
          tmp = false;
        }
        if (v1 != v2 && v1 != v3 && v2 != v3)
        {
          fprintf(f, "f %u %u %u\n", v1 + vcount, v2 + vcount, v3 + vcount);
        }
      }
    }

    vcount += numVertices;
  }

  void DeleteVertex(DWORD index)
  {
    vertices.erase(vertices.begin() + index);
    for (DWORD i = 0; i < matSplits.size(); i++)
    {
      for (DWORD j = 0; j < matSplits[i].Indices.size(); j++)
      {
        if (matSplits[i].Indices[j]>index)
          matSplits[i].Indices[j]--;
      }
    }
  }

  void DeleteVertex(const WORD index, const WORD newIndex)
  {
    for (DWORD i = 0; i<matSplits.size(); i++)
    {
      for (DWORD j = 0; j < matSplits[i].Indices.size(); j++)
      {
        if (matSplits[i].Indices[j] == index)
          matSplits[i].Indices[j] = newIndex;
        else if (matSplits[i].Indices[j]>index)
          matSplits[i].Indices[j]--;
      }
    }
    vertices.erase(vertices.begin() + index);
  }

  __declspec (noalias) void GetHitPoint(const Ray & __restrict ray, const WORD* __restrict triangle, CollisionResult & __restrict closestHit) const
  {
    const D3DXVECTOR3 v1 = *(D3DXVECTOR3*)&vertices[*triangle];
    triangle++;
    const D3DXVECTOR3 v2 = *(D3DXVECTOR3*)&vertices[*triangle];
    triangle++;
    const D3DXVECTOR3 v3 = *(D3DXVECTOR3*)&vertices[*triangle];

    D3DXVECTOR3 edge1, edge2, tvec, pvec, qvec;
    float det,inv_det;

    edge1 = v2-v1;
    edge2 = v3-v1;

    D3DXVec3Cross(&pvec, &ray.direction, &edge2);
    det =  D3DXVec3Dot(&edge1, &pvec);
    /*if(defaultCulling==D3DCULL_NONE)
    {*/
    if(det > -EPSILON && det < EPSILON)
      return;
    inv_det = 1.0f / det;

    tvec = ray.origin - v1;
    const float u = D3DXVec3Dot(&tvec,&pvec)*inv_det;
    if(u<0.0f || u>1.0f)
      return;

    D3DXVec3Cross(&qvec,&tvec,&edge1);
    const float v = D3DXVec3Dot(&ray.direction,&qvec)*inv_det;
    if(v < 0.0f || u + v > 1.0f)
      return;

    const float t = D3DXVec3Dot(&edge2,&qvec)*inv_det;
    if(t < 0.00001f)
      return;

    if(defaultCulling==D3DCULL_CW)
    {
      D3DXVECTOR3 faceNormal;
      D3DXVec3Cross(&faceNormal, &edge1,&edge2);
      if(D3DXVec3Dot(&faceNormal, &ray.direction)<0)
        return;
    }
    const D3DXVECTOR3 hitPoint = v1 + (u*(v2-v1)) + (v*(v3-v1));
    const float distance = D3DXVec3Length(&(hitPoint-ray.origin));
    if(distance<closestHit.distance)
    {
      closestHit.distance=distance;
      closestHit.SetHitPoint(hitPoint);
      return;
    }
    return;
  }

  __declspec (noalias) bool IntersectTriangle(const Ray & __restrict ray, const WORD* __restrict triangle, CollisionResult & __restrict closestHit) const
  {
    const D3DXVECTOR3 v1 = *(D3DXVECTOR3*)&vertices[*triangle];
    triangle++;
    const D3DXVECTOR3 v2 = *(D3DXVECTOR3*)&vertices[*triangle];
    triangle++;
    const D3DXVECTOR3 v3 = *(D3DXVECTOR3*)&vertices[*triangle];

    D3DXVECTOR3 edge1, edge2, tvec, pvec, qvec;
    float det,inv_det;

    edge1 = v2-v1;
    edge2 = v3-v1;

    D3DXVec3Cross(&pvec, &ray.direction, &edge2);
    det =  D3DXVec3Dot(&edge1, &pvec);
    /*if(defaultCulling==D3DCULL_NONE)
    {*/
    if(det > -EPSILON && det < EPSILON)
      return false;
    inv_det = 1.0f / det;

    tvec = ray.origin - v1;
    const float u = D3DXVec3Dot(&tvec,&pvec)*inv_det;
    if(u<0.0f || u>1.0f)
      return false;

    D3DXVec3Cross(&qvec,&tvec,&edge1);
    const float v = D3DXVec3Dot(&ray.direction,&qvec)*inv_det;
    if(v < 0.0f || u + v > 1.0f)
      return false;

    const float t = D3DXVec3Dot(&edge2,&qvec)*inv_det;
    if(t < 0.00001f)
      return false;

    if(defaultCulling==D3DCULL_CW)
    {
      D3DXVECTOR3 faceNormal;
      D3DXVec3Cross(&faceNormal, &edge1,&edge2);
      if(D3DXVec3Dot(&faceNormal, &ray.direction)<0)
        return false;
    }
    const D3DXVECTOR3 hitPoint = v1 + (u*(v2-v1)) + (v*(v3-v1));
    const float distance = D3DXVec3Length(&(hitPoint-ray.origin));
    if(distance<closestHit.distance)
    {
      closestHit.distance=distance;
      closestHit.mesh=this;
      return true;
    }
    return false;
  }

  void GetHitPoint(const Ray & __restrict ray, CollisionResult & __restrict closestHit) const
  {
    const DWORD numSplits = matSplits.size();

    for(DWORD i=0; i<numSplits; i++)
    {
      const MaterialSplit & matSplit = matSplits[i];

      for(DWORD j=0, numIndices = matSplit.numIndices; j<numIndices; j++)
      {
        GetHitPoint(ray, &matSplit.Indices[j], closestHit);
      }
    }
  }

  void Intersect(const Ray & __restrict ray, CollisionResult & __restrict closestHit) const
  {
    const DWORD numSplits = matSplits.size();

    for(DWORD i=0; i<numSplits; i++)
    {
      const MaterialSplit & matSplit = matSplits[i];

      for(DWORD j=0, numIndices = matSplit.numIndices; j<numIndices; j++)
      {
        if(IntersectTriangle(ray, &matSplit.Indices[j], closestHit))
        {
          closestHit.matIndex=i;
          closestHit.triIndex=j;
        }
      }
    }
  }

  __declspec (noalias) bool Intersect(const D3DXVECTOR3 *rayOrigin, const D3DXVECTOR3 *rayDirection, const POINT* mouse, float* outDistance, D3DXVECTOR3* outHitPoint)
  {
    extern Camera* __restrict camera;
    static D3DXVECTOR3 proj;
    extern D3DVIEWPORT9 port;
    static D3DXMATRIX identity;
    D3DXMatrixIdentity(&identity);
    static float closest = 0.0f;
    bool intersected = false;
    D3DXVECTOR3 hitPoint, closestHitPoint;

    for(DWORD i=0, numSplits = matSplits.size(); i<numSplits; i++)
    {
      for(DWORD j=0; j<matSplits[i].numIndices; j++)
      {
        if(TriangleIntersection((D3DXVECTOR3*)&vertices[matSplits[i].Indices[j]],(D3DXVECTOR3*)&vertices[matSplits[i].Indices[j+1]],(D3DXVECTOR3*)&vertices[matSplits[i].Indices[j+2]],rayOrigin,rayDirection/*,NULL,NULL*/,outHitPoint))
        {
          D3DXVec3Project(&proj, outHitPoint, &port, &camera->proj(), &camera->view(), &identity);
          *outDistance = D3DXVec3Length(&(D3DXVECTOR3(mouse->x, mouse->y, 0.0f)-proj));
          if(closest>*outDistance || !intersected)
          {
            intersected=true;
            closest=*outDistance;
            closestHitPoint = *outHitPoint;
          }
        }
      }
    }
    *outHitPoint = closestHitPoint;
    *outDistance = closest;
    return intersected;
  }

  __declspec (noalias) bool Intersect(const D3DXVECTOR3 *const rayOrigin, const D3DXVECTOR3 *const rayDirection)
  {

    if(TriangleIntersection(&D3DXVECTOR3(bbox.Min.x,bbox.Max.y,bbox.Min.z), &D3DXVECTOR3(bbox.Max.x,bbox.Max.y,bbox.Min.z), &D3DXVECTOR3(bbox.Min.x, bbox.Min.y, bbox.Min.z), rayOrigin, rayDirection))
    {
      return true;
    }

    else if(TriangleIntersection(&D3DXVECTOR3(bbox.Min.x,bbox.Min.y,bbox.Min.z), &D3DXVECTOR3(bbox.Max.x,bbox.Max.y,bbox.Min.z), &D3DXVECTOR3(bbox.Max.x, bbox.Min.y, bbox.Min.z), rayOrigin, rayDirection))
    {
      return true;
    }

    else if(TriangleIntersection(&D3DXVECTOR3(bbox.Max.x,bbox.Max.y,bbox.Min.z), &D3DXVECTOR3(bbox.Max.x,bbox.Min.y,bbox.Min.z), &D3DXVECTOR3(bbox.Max.x, bbox.Max.y, bbox.Max.z), rayOrigin, rayDirection))
    {
      return true;
    }

    else if(TriangleIntersection(&D3DXVECTOR3(bbox.Max.x,bbox.Min.y,bbox.Min.z), &D3DXVECTOR3(bbox.Max.x,bbox.Min.y,bbox.Max.z), &D3DXVECTOR3(bbox.Max.x, bbox.Max.y, bbox.Max.z), rayOrigin, rayDirection))
    {
      return true;
    }

    else if(TriangleIntersection(&D3DXVECTOR3(bbox.Min.x,bbox.Max.y,bbox.Min.z), &D3DXVECTOR3(bbox.Min.x,bbox.Max.y,bbox.Max.z), &D3DXVECTOR3(bbox.Max.x, bbox.Max.y, bbox.Min.z), rayOrigin, rayDirection))
    {
      return true;
    }

    else if(TriangleIntersection(&D3DXVECTOR3(bbox.Min.x,bbox.Max.y,bbox.Max.z), &D3DXVECTOR3(bbox.Max.x,bbox.Max.y,bbox.Min.z), &D3DXVECTOR3(bbox.Max.x, bbox.Max.y, bbox.Max.z), rayOrigin, rayDirection))
    {
      return true;
    }

    else if(TriangleIntersection(&D3DXVECTOR3(bbox.Min.x,bbox.Min.y,bbox.Min.z), &D3DXVECTOR3(bbox.Max.x,bbox.Min.y,bbox.Min.z), &D3DXVECTOR3(bbox.Min.x, bbox.Min.y, bbox.Max.z), rayOrigin, rayDirection))
    {
      return true;
    }

    else if(TriangleIntersection(&D3DXVECTOR3(bbox.Max.x,bbox.Min.y,bbox.Min.z), &D3DXVECTOR3(bbox.Max.x,bbox.Min.y,bbox.Max.z), &D3DXVECTOR3(bbox.Min.x, bbox.Min.y, bbox.Max.z), rayOrigin, rayDirection))
    {
      return true;
    }

    else
      return false;
  }

  const D3DXVECTOR3 &CheckNormal() const
  {
    D3DXVECTOR3 faceNormal;

    const WORD* indices = &matSplits[0].Indices.front();

    bool checked=false;
    D3DXVECTOR3 edge1, edge2, faceNormal2;
    WORD v1, v2, v3;

    for(DWORD j=0; j<matSplits[0].numIndices; j++)
    {
      if(j&1)
      {
        v1 = v3;
        v3 = indices[j+2];
      }
      else
      {
        v1 = indices[j];
        v2 = indices[j+1];
        v3 = indices[j+2];
      }
      if(v1 != v2 && v1 != v3 && v2 != v3)
      {
        D3DXVec3Cross(&faceNormal, &(*(D3DXVECTOR3*)&vertices[v2]-*(D3DXVECTOR3*)&vertices[v1]),&(*(D3DXVECTOR3*)&vertices[v3]-*(D3DXVECTOR3*)&vertices[v1]));
        return faceNormal;

      }
    }
  }

  const float CheckNormal(const Mesh* const mesh) const
  {
    D3DXVECTOR3 faceNormal;
    const WORD* indices = &matSplits[0].Indices.front();

    bool checked=false;
    D3DXVECTOR3 edge1, edge2, faceNormal2;
    WORD v1, v2, v3;

    for(DWORD j=0; j<matSplits[0].numIndices; j++)
    {
      if(j&1)
      {
        v1 = v3;
        v3 = indices[j+2];
      }
      else
      {
        v1 = indices[j];
        v2 = indices[j+1];
        v3 = indices[j+2];
      }
      if(v1 != v2 && v1 != v3 && v2 != v3)
      {
        D3DXVec3Cross(&faceNormal, &(*(D3DXVECTOR3*)&vertices[v2]-*(D3DXVECTOR3*)&vertices[v1]),&(*(D3DXVECTOR3*)&vertices[v3]-*(D3DXVECTOR3*)&vertices[v1]));

        if(faceNormal==mesh->CheckNormal())
        {
          if(faceNormal.x<0)
            faceNormal.x*=-1;
          if(faceNormal.y<0)
            faceNormal.y*=-1;
          if(faceNormal.z<0)
            faceNormal.z*=-1;

          float highest = faceNormal.x;
          BYTE axis = 0;

          if(faceNormal.y>highest)
          {
            axis=1;
            highest = faceNormal.y;
          }
          if(faceNormal.z>highest)
          {
            axis=2;
            highest = faceNormal.z;
          }

          if(axis==1)
          {
            float dist = GetCenter().y-mesh->GetCenter().y;
            if(dist<0)
              dist*=-1;
            return dist;
          }
          else if(axis==2)
          {
            float dist = GetCenter().z-mesh->GetCenter().z;
            if(dist<0)
              dist*=-1;
            return dist;
          }
          else
          {
            float dist = GetCenter().x-mesh->GetCenter().x;
            if(dist<0)
              dist*=-1;
            return dist;
          }
        }
        return 1000.0f;
      }
    }
    return 1000.0f;
  }

  inline const float GetNormalDistanceTo(const Mesh* const mesh) const
  {
    return CheckNormal(mesh);
  }

  bool inside(Mesh* mesh, DWORD axis)
  {
    //return true;
    /*if (name.checksum == 0x2109F811 || name.checksum == 0x11F80921)
      MessageBox(0, name.GetString(), "", 0);*/
    if (axis == 0)
    {
      /*if (name.checksum == 0x2109F811 || name.checksum == 0x11F80921)
        MessageBox(0, name.GetString(), "axis X", 0);*/
      if (bbox.Min.y >= mesh->bbox.Max.y || bbox.Max.y <= mesh->bbox.Min.y || bbox.Min.z >= mesh->bbox.Max.z || bbox.Max.z <= mesh->bbox.Min.z)
        return false;
    }
    else if (axis == 1)
    {
      /*if (name.checksum == 0x2109F811 || name.checksum == 0x11F80921)
      {
        char waaa[256];
        sprintf(waaa, "%f %f %f %f %f %f %f %f", bbox.Min.x, mesh->bbox.Max.x, bbox.Max.x, mesh->bbox.Min.x, bbox.Min.z, mesh->bbox.Max.z, bbox.Max.z, mesh->bbox.Min.z);
        MessageBox(0, waaa, "axis Y", 0);
      }*/
      if (bbox.Min.x >= mesh->bbox.Max.x && bbox.Max.x <= mesh->bbox.Min.x && bbox.Min.z >= mesh->bbox.Max.z && bbox.Max.z <= mesh->bbox.Min.z)
        return false;
    }
    else
    {
      /*if (name.checksum == 0x2109F811 || name.checksum == 0x11F80921)
        MessageBox(0, name.GetString(), "axis Z", 0);*/
      if (bbox.Min.y >= mesh->bbox.Max.y || bbox.Max.y <= mesh->bbox.Min.y || bbox.Min.x >= mesh->bbox.Max.x || bbox.Max.x <= mesh->bbox.Min.x)
        return false;
    }
    /*if (name.checksum == 0x2109F811 || name.checksum == 0x11F80921)
      MessageBox(0, name.GetString(), "true", 0);*/
    return true;
    if (axis == 0)
    {

      if (((bbox.Max.z+1.0f < mesh->bbox.Max.z && bbox.Max.z-1.0f > mesh->bbox.Min.z) || (bbox.Min.z+1.0f < mesh->bbox.Max.z && bbox.Min.z-1.0f > mesh->bbox.Min.z)) && ((bbox.Max.y+1.0f < mesh->bbox.Max.y && bbox.Max.y-1.0f > mesh->bbox.Min.y) || (bbox.Min.y+1.0f < mesh->bbox.Max.y && bbox.Min.y-1.0f > mesh->bbox.Min.y)))
        return true;
    }
    else if (axis == 1)
    {
      if (((bbox.Max.z+1.0f < mesh->bbox.Max.z && bbox.Max.z-1.0f > mesh->bbox.Min.z) || (bbox.Min.z+1.0f < mesh->bbox.Max.z && bbox.Min.z-1.0f > mesh->bbox.Min.z)) && ((bbox.Max.x+1.0f < mesh->bbox.Max.x && bbox.Max.x-1.0f > mesh->bbox.Min.x) || (bbox.Min.x+1.0f < mesh->bbox.Max.x && bbox.Min.x-1.0f > mesh->bbox.Min.x)))
        return true;
    }
    else if (axis == 2)
    {
      if (((bbox.Max.x+1.0f < mesh->bbox.Max.x && bbox.Max.x-1.0f > mesh->bbox.Min.x) || (bbox.Min.x+1.0f < mesh->bbox.Max.x && bbox.Min.x-1.0f > mesh->bbox.Min.x)) && ((bbox.Max.y+1.0f < mesh->bbox.Max.y && bbox.Max.y-1.0f > mesh->bbox.Min.y) || (bbox.Min.y+1.0f < mesh->bbox.Max.y && bbox.Min.y-1.0f > mesh->bbox.Min.y)))
        return true;
    }
    return false;
  }

  bool ReallyFlat()
  {
    if (bbox.Max.x <= bbox.Min.x + 0.5f)
      return true;
    if (bbox.Max.y <= bbox.Min.y + 0.5f)
      return true;
    if (bbox.Max.z <= bbox.Min.z + 0.5f)
      return true;
    return false;
  }

  bool IsFlat(D3DXVECTOR3* const faceNormal)
  {
    if(matSplits.size()>1)
      return false;
    const WORD* indices = &matSplits[0].Indices.front();

    bool checked=false;
    D3DXVECTOR3 edge1, edge2, faceNormal2;
    WORD v1, v2, v3;

    for(DWORD j=0; j<matSplits[0].numIndices; j++)
    {
      if(j&1)
      {
        v1 = v3;
        v3 = indices[j+2];
      }
      else
      {
        v1 = indices[j];
        v2 = indices[j+1];
        v3 = indices[j+2];
      }

      if(v1 != v2 && v1 != v3 && v2 != v3)
      {
        if(!checked)
        {
          checked=true;
          D3DXVec3Cross(faceNormal, &(*(D3DXVECTOR3*)&vertices[v2]-*(D3DXVECTOR3*)&vertices[v1]),&(*(D3DXVECTOR3*)&vertices[v3]-*(D3DXVECTOR3*)&vertices[v1]));
        }
        else
        {
          D3DXVec3Cross(&faceNormal2, &(*(D3DXVECTOR3*)&vertices[v2]-*(D3DXVECTOR3*)&vertices[v1]),&(*(D3DXVECTOR3*)&vertices[v3]-*(D3DXVECTOR3*)&vertices[v1]));
          if (*faceNormal != faceNormal2)
          {
            if (bbox.Max.x <= bbox.Min.x + 0.5f)
              return true;
            if (bbox.Max.y <= bbox.Min.y + 0.5f)
              return true;
            if (bbox.Max.z <= bbox.Min.z + 0.5f)
              return true;
            return false;
          }
        }
      }
    }

    return true;//ReallyFlat();
  }

  float GetSize(DWORD axis)
  {
    //return fabsf(bbox.Max.x - bbox.Min.x)*fabsf(bbox.Max.y - bbox.Min.y)*fabsf(bbox.Max.z - bbox.Min.z);
    if (axis == 0)
      return fabsf(bbox.Max.y - bbox.Min.y)*fabsf(bbox.Max.z - bbox.Min.z);
    else if (axis==1)
      return fabsf(bbox.Max.x - bbox.Min.x)*fabsf(bbox.Max.z - bbox.Min.z);
    else
      return fabsf(bbox.Max.x - bbox.Min.x)*fabsf(bbox.Max.y - bbox.Min.y);
  }

  bool Intersect(const D3DXVECTOR3 *rayOrigin, const D3DXVECTOR3 *rayDirection, float *outDistance)
  {
    /*DWORD tmp = defaultCulling;
    defaultCulling = D3DCULL_NONE;*/
    D3DXVECTOR3 target;
    D3DXVECTOR3 realTarget;
    bool intersected = false;
    float closest= FLT_MAX;

    if(TriangleIntersection(&D3DXVECTOR3(bbox.Min.x,bbox.Max.y,bbox.Min.z), &D3DXVECTOR3(bbox.Max.x,bbox.Max.y,bbox.Min.z), &D3DXVECTOR3(bbox.Min.x, bbox.Min.y, bbox.Min.z), rayOrigin, rayDirection, &target))
    {
      closest = D3DXVec3Length(&(target-*rayOrigin));
      realTarget = target;
      intersected = true;
    }

    if(TriangleIntersection(&D3DXVECTOR3(bbox.Min.x,bbox.Min.y,bbox.Min.z), &D3DXVECTOR3(bbox.Max.x,bbox.Max.y,bbox.Min.z), &D3DXVECTOR3(bbox.Max.x, bbox.Min.y, bbox.Min.z), rayOrigin, rayDirection, &target))
    {
      *outDistance = D3DXVec3Length(&(target-*rayOrigin));

      if(*outDistance <= closest || !intersected)
      {
        closest=*outDistance;
        realTarget = target;
      }
      intersected=true;
    }

    if(TriangleIntersection(&D3DXVECTOR3(bbox.Max.x,bbox.Max.y,bbox.Min.z), &D3DXVECTOR3(bbox.Max.x,bbox.Min.y,bbox.Min.z), &D3DXVECTOR3(bbox.Max.x, bbox.Max.y, bbox.Max.z), rayOrigin, rayDirection, &target))
    {
      *outDistance = D3DXVec3Length(&(target-*rayOrigin));

      if(*outDistance <= closest || !intersected)
      {
        closest=*outDistance;
        realTarget = target;
      }
      intersected=true;
    }

    if(TriangleIntersection(&D3DXVECTOR3(bbox.Max.x,bbox.Min.y,bbox.Min.z), &D3DXVECTOR3(bbox.Max.x,bbox.Min.y,bbox.Max.z), &D3DXVECTOR3(bbox.Max.x, bbox.Max.y, bbox.Max.z), rayOrigin, rayDirection, &target))
    {
      *outDistance = D3DXVec3Length(&(target-*rayOrigin));

      if(*outDistance <= closest || !intersected)
      {
        closest=*outDistance;
        realTarget = target;
      }
      intersected=true;
    }

    if(TriangleIntersection(&D3DXVECTOR3(bbox.Min.x,bbox.Max.y,bbox.Min.z), &D3DXVECTOR3(bbox.Min.x,bbox.Max.y,bbox.Max.z), &D3DXVECTOR3(bbox.Max.x, bbox.Max.y, bbox.Min.z), rayOrigin, rayDirection, &target))
    {
      *outDistance = D3DXVec3Length(&(target-*rayOrigin));

      if(*outDistance <= closest || !intersected)
      {
        closest=*outDistance;
        realTarget = target;
      }
      intersected=true;
    }

    if(TriangleIntersection(&D3DXVECTOR3(bbox.Min.x,bbox.Max.y,bbox.Max.z), &D3DXVECTOR3(bbox.Max.x,bbox.Max.y,bbox.Min.z), &D3DXVECTOR3(bbox.Max.x, bbox.Max.y, bbox.Max.z), rayOrigin, rayDirection, &target))
    {
      *outDistance = D3DXVec3Length(&(target-*rayOrigin));

      if(*outDistance <= closest || !intersected)
      {
        closest=*outDistance;
        realTarget = target;
      }
      intersected=true;
    }

    if(TriangleIntersection(&D3DXVECTOR3(bbox.Min.x,bbox.Min.y,bbox.Min.z), &D3DXVECTOR3(bbox.Max.x,bbox.Min.y,bbox.Min.z), &D3DXVECTOR3(bbox.Min.x, bbox.Min.y, bbox.Max.z), rayOrigin, rayDirection, &target))
    {
      *outDistance = D3DXVec3Length(&(target-*rayOrigin));

      if(*outDistance <= closest || !intersected)
      {
        closest=*outDistance;
        realTarget = target;
      }
      intersected=true;
    }

    if(TriangleIntersection(&D3DXVECTOR3(bbox.Max.x,bbox.Min.y,bbox.Min.z), &D3DXVECTOR3(bbox.Max.x,bbox.Min.y,bbox.Max.z), &D3DXVECTOR3(bbox.Min.x, bbox.Min.y, bbox.Max.z), rayOrigin, rayDirection, &target))
    {
      *outDistance = D3DXVec3Length(&(target-*rayOrigin));

      if(*outDistance <= closest || !intersected)
      {
        closest=*outDistance;
        realTarget = target;
      }
      intersected=true;
    }

    *outDistance = D3DXVec3Length(&(center-realTarget));
    //defaultCulling = tmp;
    return intersected;
  }

  /*bool Intersect(D3DXVECTOR3 &rayOrigin, D3DXVECTOR3 &rayDirection, float *outDistance)
  {
  float t0 = 0.0f, t1 = 1.0f;
  D3DXVECTOR3 origin = rayOrigin;
  D3DXVECTOR3 distance = rayDirection-rayOrigin;

  if (!ClipSegment(bbox.Min.x, bbox.Max.x, rayOrigin.x, rayDirection.x, distance.x, t0, t1)) 
  {
  return false;
  }

  if (!ClipSegment(bbox.Min.y, bbox.Max.y, rayOrigin.y, rayDirection.y, distance.y, t0, t1)) 
  {
  return false;
  }

  if (!ClipSegment(bbox.Min.z, bbox.Max.z, rayOrigin.z, rayDirection.z, distance.z, t0, t1)) 
  {
  return false;
  }

  rayOrigin = origin + distance * t0;
  rayDirection = origin + distance * t1;

  if(outDistance)
  *outDistance = D3DXVec3Length(&(origin-rayOrigin));

  return true;
  }*/

  /*void SetCollision(std::shared_ptr<Collision> coll)
  {
    collision = std::move(coll);
  }*/

  void SwapCollision(std::shared_ptr<Collision> & coll)
  {
    collision.swap(coll);
  }

  void MoveCollision(Mesh& mesh)
  {
    //mesh.InitCollision();
    mesh.SwapCollision(collision);
    /*memcpy(mesh.GetCollision(), GetCollision(), sizeof(Collision));
    ZeroMemory(GetCollision(), sizeof(Collision));
    collision.reset(NULL);///collision.~shared_ptr();//collision = std::shared_ptr<Collision>();
    //collision.swap(std::shared_ptr<Collision>(NULL));
    //mesh.SetCollision(collision);
    //collision.~shared_ptr();
    //DeinitCollision();
    /*mesh.InitCollision();
    *mesh.GetCollision() = std::move(*this->GetCollision());
    //collision.~shared_ptr();
    DeinitCollision();*/
    //DeinitCollision();
  }

  void MoveCollision(Mesh* mesh)
  {
    mesh->InitCollision();
    *mesh->GetCollision() = std::move(*this->GetCollision());
    collision.~shared_ptr();
  }


  void DeinitCollision()
  {
    collision = std::shared_ptr<Collision>();
  }

  void GenerateSimpleCollision(Mesh* mesh)
  {
    this->collision = collision = std::shared_ptr< Collision>(new Collision(mesh->GetCollision()));
  }

  void InitCollision()
  {
    collision = std::shared_ptr<Collision>(new Collision());
  }

  Mesh()
  {
    verts=NULL;
    //node=NULL;
    flags = MeshFlags::visible;
    //collision = NULL;
    /*flags[MeshFlags::transparent]=false;
    flags[MeshFlags::deleted]=false;
    flags[MeshFlags::undefined]=false;
    flags[MeshFlags::visible]=true;*/
    //flags[MeshFlags::bPicked]=false;
  }

  inline Collision* const GetCollision() const
  {
    return collision.get();
  }

  void SetBBox(const SimpleVertex &bboxMin, const SimpleVertex &bboxMax)
  {
    this->bbox.Min = bboxMin;
    this->bbox.Max = bboxMax;
  }
  
  void SetBBox(const BBox& _bbox)
  {
      this->bbox.Min = _bbox.Min;
      this->bbox.Max = _bbox.Max;
  }

  void SetBBox(const Vertex &bboxMin, const Vertex &bboxMax)
  {
    this->bbox.Min.x = bboxMin.x;
    this->bbox.Min.y = bboxMin.y;
    this->bbox.Min.z = bboxMin.z;
    this->bbox.Max.x = bboxMax.x;
    this->bbox.Max.y = bboxMax.y;
    this->bbox.Max.z = bboxMax.z;
  }

  const BBox & __restrict const GetBBox() const
  {
    return bbox;
  }

  const BBox GetBBoxX() const
  {
    return bbox;
  }

  bool NonCollidable()
  {
    if (collision)
      return collision->NonCollidable();
    return true;
  }

  __declspec (noalias) void ReloadVertexBuffer(IDirect3DDevice9Ex* const dev, const DWORD &fvf)
  {
    if (verts)
      verts->Release();
    const DWORD numVerts = vertices.size();

    if (numVerts != 0)
    {
      if (FAILED(dev->CreateVertexBuffer(numVerts*sizeof(Vertex), D3DUSAGE_WRITEONLY, fvf, D3DPOOL_DEFAULT, &verts, NULL)))
        MessageBox(0, "failed to create vertex buffer", "", 0);


      Vertex* pVertices;
      verts->Lock(0, 0, (void**)&pVertices, 0);
      memcpy(pVertices, &vertices.front(), numVerts*sizeof(Vertex));
      verts->Unlock();
    }
  }

  __declspec (noalias) void CreateBuffers(IDirect3DDevice9Ex* const Device, const DWORD &fvf);
  __declspec (noalias) void ReleaseBuffers();

  inline const DWORD GetNumTriangles()
  {
    DWORD numTriangles = 0;
    const DWORD numSplits = matSplits.size();
    if (numSplits == 0)
      return numTriangles;
    MaterialSplit* splits = &matSplits.front();
    WORD v1, v2, v3;

    for(DWORD i=0; i<numSplits; i++)
    {
      DWORD tmp = 0;
      if (splits[i].Indices.size() < 3)
        MessageBox(0, GetName().GetString2(), "", 0);
      //splits[i].numIndices = splits[i].Indices.size() - 2;*/
      for(DWORD j=0, numIndices = splits[i].numIndices; j<numIndices; j++)
      {
        if(tmp==0)
        {
          v1 = splits[i].Indices[j];
          v2 = splits[i].Indices[j+1];
          v3 = splits[i].Indices[j+2];
          tmp=1;
        }
        else
        {
          v1 = v3;
          v3 = splits[i].Indices[j+2];
          tmp=0;
        }
        if(v1 != v2 && v1 != v3 && v2 != v3)
        {
          numTriangles++;
        }
      }
    }
    return numTriangles;
    //return collision.GetNumFaces();
  }

  __forceinline bool visible() const
  {
    return flags & MeshFlags::visible;//GetFlag(MeshFlags::visible);
  }

  __forceinline bool deleted() const
  {
    return flags & MeshFlags::deleted;//GetFlag(MeshFlags::deleted);
  }

  __forceinline bool transparent() const
  {
    return flags & MeshFlags::transparent;//GetFlag(MeshFlags::transparent);
  }

  void SetVisible(bool state)
  {
    if (state)
      flags |= MeshFlags::visible;
    else
      flags &= ~MeshFlags::visible;
  }

  void Resize(DWORD size)
  {
    vertices.resize(size);
  }

  void OptimizeVerts()
  {
    /*for (DWORD i = 1; i < vertices.size(); i++)
    {
      for (DWORD j = 0; j < i; j++)
      {
        while(vertices.size()>1 && vertices[i].x == vertices[j].x && vertices[i].y == vertices[j].y && vertices[i].z == vertices[j].z && vertices[i].tUV[0].u == vertices[j].tUV[0].u && vertices[i].tUV[0].v == vertices[j].tUV[0].v)
        {
          DeleteVertex((WORD)i, (WORD)j);
        }
      }
    }*/

    for (DWORD i = 0; i < vertices.size(); i++)
    {
      bool found = false;
      for (DWORD j = 0; j < matSplits.size(); j++)
      {
        for (DWORD k = 0; k < matSplits[j].Indices.size(); k++)
        {
          found = true;
          break;
        }
      }
      if (!found)
      {
        DeleteVertex(i);
        i--;
      }
    }
  }

  vector <Vertex>::iterator Begin()
  {
    return vertices.begin();
  }

  vector <Vertex>::iterator End()
  {
    return vertices.end();
  }

  void SetFlags(DWORD flags)
  {
    this->flags = flags;
  }

  inline void SetFlag(const MeshFlags flag, const bool value)
  {
    if(value)
      flags |= flag;
    else
      flags &= ~flag;//flags[flag] = value;
  }

  inline void SetFlag(const MeshFlags flag)
  {
    flags |= flag;
  }

  inline void UnSetFlag(const MeshFlags flag)
  {
    flags &= ~flag;
  }

  inline void ToggleFlag(const MeshFlags flag)
  {
    flags ^= flag;//flags[flag] = !flags[flag];
  }

  inline void AddFaceInfo(const FaceInfo* const faceInfo)
  {
    collision->AddFaceInfo(faceInfo);
  }

  inline void AddFace(const Face &face)
  {
    collision->AddFace(face);
  }

  inline  void AddFace(WORD a, WORD b, WORD c, FaceInfo* faceInfo)
  {
    collision->AddFace(a,b,c,faceInfo);
  }

  float GetDrawOrder() const
  {
    extern MaterialList* __restrict globalMaterialList;
    float order = 0.0f;
    for (DWORD i = 0; i < matSplits.size(); i++)
    {
      Material* mat = globalMaterialList->GetMaterial(matSplits[i].matId);
      if (mat && mat->drawOrder > order)
        order = mat->drawOrder;
    }
    return order;
  }

  bool operator <(const Mesh & other) const
  {
    if (!this->transparent())
      return true;
    if (!this->visible())
      return true;
    if (!other.visible())
      return true;
    return this->GetDrawOrder() < other.GetDrawOrder();
  /*if(!flags[MeshFlags::transparent])
  return false;

  D3DXVECTOR3 cameraPos = camera->pos();
  D3DXVECTOR3 distance = center-cameraPos;//center-cameraPos;
  D3DXVECTOR3 distanceOther = other.center-cameraPos;//other.center-cameraPos;

  distance.x = abs(distance.x); distance.y = abs(distance.y); distance.z = abs(distance.z);
  distanceOther.x = abs(distanceOther.x); distanceOther.y = abs(distanceOther.y); distanceOther.z = abs(distanceOther.z);
  return (distanceOther.x+distanceOther.y+distanceOther.z)<(distance.x+distance.y+distance.z);*/
  }
#define CLEARFLAGS_CLEARVERTICES 1
#define CLEARFLAGS_CLEARMATSPLITS 2
#define CLEARFLAGS_CLEARCOLLISION 4
#define CLEARFLAGS_CLEARALL 7
#define CLEARFLAGS_KEEPVERTICES 6

  void Clear(BYTE flags)
  {
    if(flags & CLEARFLAGS_CLEARVERTICES)
      vertices.clear();
    if(flags & CLEARFLAGS_CLEARMATSPLITS)
      matSplits.clear();
    if(flags & CLEARFLAGS_CLEARCOLLISION)
    {
      if(collision.get())
      {
        collision.~shared_ptr();
        collision = std::shared_ptr<Collision>(NULL);
      }
    }
  }

  inline const bool IsSelected() const
  {
    extern vector <SelectedObject> selectedObjects;
    DWORD numSelected = selectedObjects.size();
    if (numSelected == 0)
      return false;
    const SelectedObject* __restrict const selectedMeshes = &selectedObjects.front();

    for (DWORD i = 0;  i < numSelected; i++)
    {
      if(selectedMeshes[i].mesh == this)
        return true;
    }
    return false;
  }

  __declspec (noalias) void AddCollision(const NSCollisionObject collision, const DWORD version)
  {
    if(!this->collision.get())
      this->collision = std::shared_ptr<Collision>(new Collision());
    this->collision->SetData(collision, version);
    /*if(collision.numVerts==vertices.size())
      this->collision->ClearVertices();*/
  }

  __declspec (noalias) void SetCenter(const D3DXVECTOR3 & center)
  {
    this->center = center;
  }


  __declspec (noalias) bool PickMesh(const D3DXVECTOR3* rayOrigin, const D3DXVECTOR3* rayDirection, float* outDistance, DWORD* outMatSplitIndex, DWORD* outTriangleIndex);
  __declspec (noalias) bool PickCollision(const D3DXVECTOR3* rayOrigin, const D3DXVECTOR3* rayDirection, float* outDistance, DWORD* outTriangleIndex);

  ~Mesh()
  {
    if(verts)
      verts->Release();
    vertices.clear();
    matSplits.clear();
    /*extern bool loading;
    if(collision && !loading)
    {
      delete collision;
      collision=NULL;
    }*/
    //rails.clear();
    //collision.~Collision();
    //collObjects.clear();
  }

  inline const DWORD GetNumVertices() const
  {
    return vertices.size();
  }

  __declspec (noalias) void Render(MaterialList* __restrict const matLits);
  __declspec (noalias) void Render0(MaterialList* __restrict const matList) const;//render opaque
  __declspec (noalias) void Render1(MaterialList* __restrict const matList);//render alpha 1st order
  __declspec (noalias) void Render2(MaterialList* __restrict const matList) const;//render alpha 2nd order
  __declspec (noalias) void Render3(MaterialList* __restrict const matList);//th2
  __declspec (noalias) void RenderSelected(MaterialList* __restrict const matList) const;//Render Selected
  __declspec (noalias) void RenderShaded();
  __declspec (noalias) void RenderShadedWithCurrentVBuffer();
  __declspec (noalias) void RenderShadedWithCurrentBuffers();
  __declspec (noalias) void RenderCollision(const bool isSelected = false) const;

  inline void AddVertexSilent(Vertex vertex)
  {
    vertices.push_back(vertex);
  }

  __declspec (noalias) void AddVertices(const Th2FixedVert *const __restrict position, const Th2Vertex* const __restrict vertices, const DWORD numVertices, const Th2Data* const __restrict verts)
  {
    Vertex vertex;
    ZeroMemory(&vertex,sizeof(Vertex));
    vertex.colour = Colour(128,128,128);
    Vertex bboxMin, bboxMax;

    for(DWORD i=0; i<numVertices; ++i)
    {
      //level seems to be fliped on the x and y axis /2.833f is to get correct scale
      vertex.x = -((float)vertices[i].x+(float)position->x/4096.0f)/2.833f;
      vertex.y = -((float)vertices[i].y+(float)position->y/4096.0f)/2.833f;
      vertex.z = -((float)vertices[i].z+(float)position->z/4096.0f)/2.833f;
      vertex.tUV[0].u = verts[i].u;
      vertex.tUV[0].v = verts[i].v;
      if(verts[i].colour)//check if pointer is initialized it should be but on some levels it crash here if not check...
      {
        vertex.colour.r = verts[i].colour->b;
        vertex.colour.g = verts[i].colour->g;
        vertex.colour.b = verts[i].colour->r;

        if(verts[i].colour->a == 155)
        {
          vertex.colour.a = verts[i].colour->a;
          delete verts[i].colour;
        }
        else if(verts[i].colour->a == 154)
        {
          vertex.colour.a = verts[i].colour->a;
        }
        else
          vertex.colour.a = 255;
      }
      else
        vertex.colour.a = 255;
      if(i==0)
      {
        bboxMin = vertex;
        bboxMax = vertex;
      }
      else
      {
        if(vertex.x<bboxMin.x)
          bboxMin.x=vertex.x;
        if(vertex.y<bboxMin.y)
          bboxMin.y=vertex.y;
        if(vertex.z<bboxMin.z)
          bboxMin.z=vertex.z;

        if(vertex.x>bboxMax.x)
          bboxMax.x=vertex.x;
        if(vertex.y>bboxMax.y)
          bboxMax.y=vertex.y;
        if(vertex.z>bboxMax.z)
          bboxMax.z=vertex.z;
      }
      this->vertices.push_back(vertex);
    }
    SetBBox(bboxMin,bboxMax);
  }

  __declspec (noalias) void Center()
  {
    D3DXVECTOR3 middle;// = (*(D3DXVECTOR3*)&bbox.Max+*(D3DXVECTOR3*)&bbox.Min)/2.0f;
    middle.x = (bbox.Max.x+bbox.Min.x)/2.0f;
    middle.y = bbox.Min.y+500.0f;
    middle.z = (bbox.Max.z+bbox.Min.z)/2.0f;
    for(DWORD i=0, numVertices = vertices.size(); i<numVertices; i++)
    {
      *(D3DXVECTOR3*)&vertices[i]-=middle;
    }
  }

  __declspec (noalias) inline const D3DXVECTOR3 GetCenter() const
  {
    return (*(D3DXVECTOR3*)&bbox.Max+*(D3DXVECTOR3*)&bbox.Min)/2.0f;
  }

  __declspec (noalias) bool GetClosestVertex(const D3DXVECTOR3 * const __restrict __restrict rayOrigin, const D3DXVECTOR3 * const __restrict __restrict rayDirection, Vertex* outVertex, float *outDistance)
  {
    bool hasHit=false;
    float closestIntersect = 0;

    for(DWORD i=0, numSplits = matSplits.size(); i<numSplits; i++)
    {
      for(DWORD j=0; j<matSplits[i].numIndices; j++)
      {
        if(TriangleIntersection((D3DXVECTOR3*)&vertices[matSplits[i].Indices[j]],(D3DXVECTOR3*)&vertices[matSplits[i].Indices[j+1]],(D3DXVECTOR3*)&vertices[matSplits[i].Indices[j+2]],rayOrigin,rayDirection/*,NULL,NULL*/,outDistance, outVertex))
        {
          if(closestIntersect>*outDistance || !hasHit)
          {
            hasHit=true;
            closestIntersect=*outDistance;
          }
        }
      }
    }
    *outDistance = closestIntersect;
    return hasHit;
  }

  __declspec (noalias)void Mesh::Scale(float scale);

  __declspec (noalias) void MoveRelativeNeg(const D3DXVECTOR3* const __restrict relDist)
  {
    SetCenter(GetCenter()-*relDist);

    for(DWORD i=0, numVertices=vertices.size(); i<numVertices; i++)
    {
      *(D3DXVECTOR3*)&vertices[i]-=*relDist;
    }
  }

  __declspec (noalias) void MoveRelative(const D3DXVECTOR3* const __restrict relDist);

  bool FlickersWith(Mesh* mesh)
  {
    /*WORD* indices = &matSplits[0].Indices.front();

    D3DXVECTOR3 edge1, edge2, faceNormal2, faceNormal;
    WORD v1, v2, v3;

    for(DWORD j=0; j<matSplits[0].numIndices; j++)
    {
    if(j&1)
    {
    v1 = v3;
    v3 = indices[j+2];
    }
    else
    {
    v1 = indices[j];
    v2 = indices[j+1];
    v3 = indices[j+2];
    }
    if(v1 != v2 && v1 != v3 && v2 != v3)
    {
    D3DXVec3Cross(&faceNormal, &(*(D3DXVECTOR3*)&vertices[v2]-*(D3DXVECTOR3*)&vertices[v1]),&(*(D3DXVECTOR3*)&vertices[v3]-*(D3DXVECTOR3*)&vertices[v1]));
    break;
    }
    }


    indices = &mesh->matSplits[0].Indices.front();

    for(DWORD j=0; j<mesh->matSplits[0].numIndices; j++)
    {
    if(j&1)
    {
    v1 = v3;
    v3 = indices[j+2];
    }
    else
    {
    v1 = indices[j];
    v2 = indices[j+1];
    v3 = indices[j+2];
    }
    if(v1 != v2 && v1 != v3 && v2 != v3)
    {
    D3DXVec3Cross(&faceNormal2, &(*(D3DXVECTOR3*)&mesh->vertices[v2]-*(D3DXVECTOR3*)&mesh->vertices[v1]),&(*(D3DXVECTOR3*)&mesh->vertices[v3]-*(D3DXVECTOR3*)&mesh->vertices[v1]));
    break;
    }
    }

    BYTE axis=0, axis2=0;
    float highest = faceNormal.x;

    if(highest<faceNormal.y)
    axis=1;
    if(highest<faceNormal.z)
    axis=2;

    highest = faceNormal2.x;

    if(highest<faceNormal2.y)
    axis2=1;
    if(highest<faceNormal2.z)
    axis2=2;

    if(axis!=axis2)
    return false;
    */
    static const float minDist = 0.05f;
    BBox box = mesh->GetBBox();

    if(((this->GetCenter()>*(D3DXVECTOR3*)&box.Max) || this->GetCenter()<*(D3DXVECTOR3*)&box.Min) && (mesh->GetCenter()>*(D3DXVECTOR3*)&this->bbox.Max || mesh->GetCenter()<*(D3DXVECTOR3*)&this->bbox.Min))
      return false;

    if(/*axis == 0 && */(this->bbox.Max.x + minDist > box.Min.x || box.Max.x + minDist > this->bbox.Min.x))
      return true;
    else if(/*axis == 1 && */(this->bbox.Max.y + minDist > box.Min.y || box.Max.y + minDist > this->bbox.Min.y))
      return true;
    else if(/*axis == 2 && */(this->bbox.Max.z + minDist > box.Min.z || box.Max.y + minDist > this->bbox.Min.z))
      return true;

    return false;
  }

  float GetClosestDistance(Mesh* mesh, DWORD* axis)
  {
    float sqrDist = 0.0f;

    BBox box = mesh->GetBBox();

    if (this->bbox.Max.x < box.Min.x)
    {
      float d = this->bbox.Max.x - box.Min.x;
      sqrDist = d*d;
      *axis = 0;
    }
    else if (this->bbox.Min.x > box.Max.x);
    {
      float d = this->bbox.Min.x - box.Max.x;
      sqrDist = d*d;
      *axis = 0;
    }

    if (this->bbox.Max.y < box.Min.y)
    {
      float d = this->bbox.Max.y - box.Min.y;
      d *= d;
      if (d<sqrDist)
      {
        sqrDist = d;
        *axis = 1;
      }
    }
    else if (this->bbox.Min.y > box.Max.y);
    {
      float d = this->bbox.Min.y - box.Max.y;
      d *= d;
      if (d<sqrDist)
      {
        sqrDist = d;
        *axis = 1;
      }
    }

    if (this->bbox.Max.z < box.Min.z)
    {
      float d = this->bbox.Max.z - box.Min.z;
      d *= d;
      if (d<sqrDist)
      {
        sqrDist = d;
        *axis = 2;
      }
    }
    else if (this->bbox.Min.z > box.Max.z);
    {
      float d = this->bbox.Min.z- box.Max.z;
      d *= d;
      if (d<sqrDist)
      {
        sqrDist = d;
        *axis = 2;
      }
    }
    return sqrtf(sqrDist);
  }

  float GetDistanceTo(Mesh* mesh)
  {
    float sqrDist = 0;

    BBox bbox = mesh->GetBBox();

    if(this->bbox.Max.x < bbox.Min.x)
    {
      float d = this->bbox.Max.x - bbox.Min.x;
      sqrDist = d*d;
    }
    else if(this->bbox.Min.x > bbox.Max.x)
    {
      float d = this->bbox.Min.x - bbox.Max.x;
      sqrDist = d*d;
    }

    if(this->bbox.Max.y < bbox.Min.y)
    {
      float d = this->bbox.Max.y - bbox.Min.y;
      sqrDist += d*d;
    }
    else if(this->bbox.Min.y > bbox.Max.y)
    {
      float d = this->bbox.Min.y - bbox.Max.y;
      sqrDist += d*d;
    }

    if(this->bbox.Max.z < bbox.Min.z)
    {
      float d = this->bbox.Max.z - bbox.Min.z;
      sqrDist += d*d;
    }
    else if(this->bbox.Min.z > bbox.Max.z)
    {
      float d = this->bbox.Min.z - bbox.Max.z;
      sqrDist += d*d;
    }

    return sqrtf(sqrDist);
  }

  void SetPosition(const D3DXVECTOR3 &position);
  void SetPosition(const D3DXVECTOR3 const*__restrict position, const D3DXVECTOR3 const*__restrict angle);
  void SetPositionMeshOnly(const D3DXVECTOR3 const* __restrict position, const D3DXVECTOR3 const* __restrict angle);
  void SetAngles(const D3DXVECTOR3 const* __restrict angle);

  void Move(const D3DXVECTOR3 *direction);
  void Move(const D3DXVECTOR3 *direction, const D3DXVECTOR3 * __restrict angle);
  void MoveWithColl(const D3DXVECTOR3 const* __restrict direction, const D3DXVECTOR3 const* __restrict angle);

  void Move(const SimpleVertex &position, const SimpleVertex &angle)
  {
    Vertex bboxMin, bboxMax;
    D3DXMATRIX transform;
    D3DXMATRIX translation;
    D3DXMATRIX rotation;

    D3DXMatrixRotationYawPitchRoll(&rotation,-angle.y+D3DX_PI,angle.x,angle.z);
    D3DXMatrixTranslation(&translation,position.x,position.y,position.z);
    D3DXMatrixMultiply(&transform, &rotation, &translation);

    D3DXVECTOR3 middle = (*(D3DXVECTOR3*)&bbox.Max+*(D3DXVECTOR3*)&bbox.Min)/2.0f + center;
    //D3DXVECTOR3 middle = D3DXVECTOR3(bbox.Max.x-bbox.Max.y,bbox.Max.z-bbox.Min.x, bbox.Min.y-bbox.Min.z)/2.0f;

    for(DWORD i=0, numVertices = vertices.size(); i<numVertices; i++)
    {
      Vertex *vertex = (Vertex*)&vertices[i];
      *(D3DXVECTOR3*)&vertices[i]-=middle;
      D3DXVec3TransformCoord((D3DXVECTOR3*)vertex,(D3DXVECTOR3*)vertex,&transform);
      if(i==0)
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
      }
    }
    SetBBox(bboxMin,bboxMax);
  }

  __declspec (noalias) void AddVertex(const Vertex* const __restrict vertex)
  {
    vertices.push_back(*vertex);
  }

  inline void Reserve(const DWORD &numVertices)
  {
    vertices.reserve(numVertices);
  }

  __declspec (noalias) void AddVertices(const Th2FixedVert *const __restrict position, const Th2Vertex* const __restrict vertices, const DWORD numVertices, Colour** const __restrict colours, const TexCoord* const __restrict texCoords)
  {
    Vertex vertex;
    ZeroMemory(&vertex,sizeof(Vertex));
    vertex.colour = Colour(128,128,128);
    Vertex bboxMin, bboxMax;

    for(DWORD i=0; i<numVertices; ++i)
    {
      //level seems to be fliped on the x and y axis /2.833f is to get correct scale
      vertex.x = -((float)vertices[i].x+(float)position->x/4096.0f)/2.833f;
      vertex.y = -((float)vertices[i].y+(float)position->y/4096.0f)/2.833f;
      vertex.z = -((float)vertices[i].z+(float)position->z/4096.0f)/2.833f;
      vertex.tUV[0].u = texCoords[i].u;
      vertex.tUV[0].v = texCoords[i].v;
      if(colours[i])//check if pointer is initialized it should be but on some levels it crash here if not check...
      {
        vertex.colour.r = (float)colours[i]->b*0.65f;
        vertex.colour.g = (float)colours[i]->g*0.65f;
        vertex.colour.b = (float)colours[i]->r*0.65f;

        if(colours[i]->a == 155)
        {
          vertex.colour.a = colours[i]->a;
          delete colours[i];
        }
        else if(colours[i]->a == 154)
        {
          vertex.colour.a = colours[i]->a;
        }
        else
          vertex.colour.a = 255;
        colours[i]=NULL;
      }
      else
        vertex.colour.a = 255;
      if(i==0)
      {
        bboxMin = vertex;
        bboxMax = vertex;
      }
      else
      {
        if(vertex.x<bboxMin.x)
          bboxMin.x=vertex.x;
        if(vertex.y<bboxMin.y)
          bboxMin.y=vertex.y;
        if(vertex.z<bboxMin.z)
          bboxMin.z=vertex.z;

        if(vertex.x>bboxMax.x)
          bboxMax.x=vertex.x;
        if(vertex.y>bboxMax.y)
          bboxMax.y=vertex.y;
        if(vertex.z>bboxMax.z)
          bboxMax.z=vertex.z;
      }
      this->vertices.push_back(vertex);
    }
    SetBBox(bboxMin,bboxMax);
  }

  void ClearVertices()
  {
    vertices.clear();
  }

  __declspec (noalias) void AddVertices(const Vertex* const __restrict vertices, const DWORD &numVertices, const bool blendX2 = false, const bool exporting = false, const BYTE blendValue = 0)
  {
    extern MaterialList* __restrict globalMaterialList;
    Vertex bboxMin;
    Vertex bboxMax;

    DWORD appending = this->vertices.size();
    for(DWORD i=0; i<numVertices; ++i)
    {
      this->vertices.push_back(vertices[i]);
      if(!appending)
      {
        if(blendValue==1)
        {
          if (vertices[i].colour.a<128)
            flags |= MeshFlags::transparent;
          this->vertices[i].colour.r *=0.5f;///= 2.5f;
          this->vertices[i].colour.g *=0.5f;///= 2.5f;
          this->vertices[i].colour.b *=0.5f;///= 2.5f;
          extern MaterialSplit *lastSplit;
          extern DWORD numLastSplit;
          if (lastSplit)
          {
            for (DWORD j = 0; j < numLastSplit; j++)
            {
              Material* mat = globalMaterialList->GetMaterial((*(lastSplit + j*sizeof(MaterialSplit))).matId);
              if (mat && mat->transparent)
              {
                for (DWORD k = 0; k < (*(lastSplit + j*sizeof(MaterialSplit))).Indices.size(); k++)
                {
                  if ((*(lastSplit + j*sizeof(MaterialSplit))).Indices[k] == i)
                  {
                    if (this->vertices[i].colour.a < 128) this->vertices[i].colour.a *= 2; else if (this->vertices[i].colour.a >= 128) this->vertices[i].colour.a = 255;
                    goto done;
                  }
                }
              }
            }
          }
          this->vertices[i].colour.a = 0xFF;
        done:;
          //this->vertices[i].colour.a = 0xFF;
        }
        else if(blendX2) {if(this->vertices[i].colour.a<128) this->vertices[i].colour.a *=2; else if(this->vertices[i].colour.a==128) this->vertices[i].colour.a = 255; }
        if(exporting)
        {
          this->vertices[i].z*=-1;
          /*this->vertices[i].x *= 0.2f;
          this->vertices[i].y *= 0.2f;
          this->vertices[i].z *= 0.2f;*/
        }
        if(i==0)
        {
          bboxMin = this->vertices[i];
          bboxMax = this->vertices[i];
        }
        else
        {
          if(this->vertices[i].x<bboxMin.x)
            bboxMin.x=this->vertices[i].x;
          if(this->vertices[i].y<bboxMin.y)
            bboxMin.y=this->vertices[i].y;
          if(this->vertices[i].z<bboxMin.z)
            bboxMin.z=this->vertices[i].z;

          if(this->vertices[i].x>bboxMax.x)
            bboxMax.x=this->vertices[i].x;
          if(this->vertices[i].y>bboxMax.y)
            bboxMax.y=this->vertices[i].y;
          if(this->vertices[i].z>bboxMax.z)
            bboxMax.z=this->vertices[i].z;
        }
      }
      else if(exporting)
      {
        this->vertices[i+appending].z*=-1;

        if(blendValue==1)
        {
         // v.colour.a = 0xFF;

          this->vertices[i+appending].colour.b /= 2.5f;
          this->vertices[i+appending].colour.r /= 2.5f;
          this->vertices[i+appending].colour.g /= 2.5f;
        }
      }
    }
    if(!appending)
      SetBBox(bboxMin,bboxMax);
  }

  void sort()
  {
    if(!this->deleted() && matSplits.size())
      std::sort(matSplits.begin(), matSplits.end());
  }

  bool movable()
  {
      return flags & MeshFlags::movable;
  }

  /*__declspec (noalias) void AddModelSplit(MaterialSplit* const __restrict split, MaterialList* const __restrict matList)
  {
      if (matList)
      {
          Material* tmpMat = NULL;

          tmpMat = matList->GetMaterial(split->matId);
          if (tmpMat)
          {
              if (tmpMat->transparent)
                  flags |= MeshFlags::transparent;
          }
      }
  }*/

  __declspec (noalias) void AddMatSplit(MaterialSplit* const __restrict split, MaterialList* const __restrict matList,const bool indexed = false, const bool exporting = false, bool th3=false)
  {
    if(matList)
    {
      Material* tmpMat = NULL;

      if(!indexed)
        tmpMat = matList->GetMaterial(split->matId, &split->matId);
      else
        tmpMat = matList->GetMaterial(split->matId);
      if(tmpMat)
      {
        //for(DWORD i=0; i<tmpMat->GetNumTextures(); i++)
        if(tmpMat->transparent)
          flags |= MeshFlags::transparent;
      }
    }

    matSplits.push_back(*split);
    if(!exporting)
      matSplits.back().numIndices -= 2;
    else
    {
      if (th3 && matList)
      {
        Material* tmpMat = NULL;

        if (!indexed)
          tmpMat = matList->GetMaterial(split->matId, &split->matId);
        else
          tmpMat = matList->GetMaterial(split->matId);
        if (tmpMat)
        {
          if (tmpMat && tmpMat->doubled)
          {
            for (DWORD i = 1; i < matSplits.back().Indices.size(); i++)
            {
              matSplits.back().Indices.push_back(matSplits.back().Indices[i]);
            }
          }
        }
      }
      if (matList)
      {
          matSplits.back().numIndices = split->Indices.size();
          if (split->matId > matList->GetNumMaterials())
          {
              matSplits.back().matId = matList->GetNumMaterials();
              if (indexed)
                  matSplits.back().matId--;
          }
      }
    }
  }

  inline MaterialSplit* const GetSplit(const DWORD &index)
  {
    return &matSplits[index];
  }
  inline const DWORD GetNumSplits() const
  {
    return matSplits.size();
  }

  inline const Vertex* const GetVertices() const
  {
    if(vertices.size())
      return &vertices.front();
    MessageBox(0, "tried vertices", "", 0);
    return NULL;
  }

  void SetName(const Checksum &name)
  {
    this->name = name;
  }

  void SetName(const char* const __restrict name)
  {
    this->name = Checksum(name);
  }

  inline const Checksum &GetName() const
  {
    return name;
  }

  inline const DWORD &GetFlags() const
  {
    return flags;
  }

};

struct Thug2Mesh : Mesh
{
  struct Data
  {
    Checksum name;
    DWORD transformIndex;
    DWORD flags;
    DWORD numSplits;
    SimpleVertex bboxMin;
    SimpleVertex bboxMax;
    D3DXVECTOR3 sphereCenter;
    float sphereRadius;
  };
  Data* data;

  Thug2Mesh(const Data* const __restrict data)
  {
    SetName(data->name);
    SetCenter(data->sphereCenter);
    SetBBox(data->bboxMin,data->bboxMax);
  }
};

struct RenderableSplit
{
  IDirect3DIndexBuffer9* indices;
  Material* material;
  DWORD numTriangles;
};

struct RenderableMesh
{
  IDirect3DVertexBuffer9* vertices;
  DWORD numSplits;
  DWORD numVerts;
  RenderableSplit* splits;
  Mesh* mesh;

  __declspec(noalias) void Render0()
  {
    extern IDirect3DDevice9Ex* __restrict Device;
    register IDirect3DDevice9Ex* const __restrict Dev = Device;
    Dev->SetStreamSource(0, vertices, 0, sizeof(Vertex));
    register const DWORD numVertices = numVerts;
    for(register DWORD i=0; i<numSplits; i++)
    {
      const RenderableSplit & __restrict split = splits[i];
      Dev->SetIndices(split.indices);

      if(split.material)
        split.material->Submit();
      else
      {
        Dev->SetTexture(0, NULL);
        Dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        Dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
      }

      Dev->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, numVertices, 0, split.numTriangles);
    }
  }

  __declspec(noalias) void Render2()
  {
    extern IDirect3DDevice9Ex* __restrict Device;
    register IDirect3DDevice9Ex* const __restrict Dev = Device;
    Dev->SetStreamSource(0, vertices, 0, sizeof(Vertex));
    register const DWORD numVertices = numVerts;
    for(register DWORD i=0; i<numSplits; i++)
    {
      const RenderableSplit & __restrict split = splits[i];
      Dev->SetIndices(split.indices);

      if(split.material)
      {
        split.material->Submit();

        Dev->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, numVertices, 0, split.numTriangles);
      }
    }
  }

  RenderableMesh()
  {
  }

  RenderableMesh(Mesh* const __restrict mesh, MaterialList* const __restrict matList)
  {
    this->mesh=mesh;
    extern IDirect3DDevice9Ex* __restrict Device;
    IDirect3DDevice9Ex* __restrict  dev = Device;
    numVerts = mesh->GetNumVertices();

    if(numVerts != 0)
    {
      if(FAILED(dev->CreateVertexBuffer(numVerts*sizeof(Vertex), D3DUSAGE_WRITEONLY, D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX0, D3DPOOL_DEFAULT, &vertices, NULL)))
        MessageBox(0,"failed to create vertex buffer","",0);


      Vertex* pVertices;
      vertices->Lock(0,0,(void**)&pVertices, 0);
      memcpy(pVertices, mesh->GetVertices(),numVerts*sizeof(Vertex));
      vertices->Unlock();


      numSplits = mesh->GetNumSplits();
      splits = new RenderableSplit[numSplits];

      for(DWORD i=0; i<numSplits; i++)
      {
        const DWORD numIndices = mesh->matSplits[i].Indices.size();
        dev->CreateIndexBuffer(numIndices*2, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &splits[i].indices, NULL);
        splits[i].numTriangles = numIndices-2;
        splits[i].material=matList->GetMaterial(mesh->matSplits[i].matId);


        WORD* indices;
        splits[i].indices->Lock(0,0,(void**)&indices, 0);
        memcpy(indices, &mesh->matSplits[i].Indices.front(), numIndices*2);
        splits[i].indices->Unlock();
      }
    }

  }
};

struct ThugMesh : Mesh
{
  struct Data
  {
    Checksum name;
    DWORD transformIndex;
    DWORD flags;
    DWORD numSplits;
    SimpleVertex bboxMin;
    SimpleVertex bboxMax;
    D3DXVECTOR3 sphereCenter;
    float sphereRadius;
    DWORD numVertices;
    DWORD vertexFlags;
    SimpleVertex* vertices;
    SimpleVertex* normals;
    TexCoord* texCoords;
    Colour* colours;
  };
  Data* data;

  ThugMesh(const Data* const __restrict data)
  {
    SetName(data->name);
    SetCenter(data->sphereCenter);
    SetBBox(data->bboxMin,data->bboxMax);
  }
};

struct Th4Mesh : Mesh
{
  struct Data
  {
    Checksum name;
    DWORD transformIndex;
    DWORD flags;
    DWORD numSplits;
    SimpleVertex bboxMin;
    SimpleVertex bboxMax;
    D3DXVECTOR3 sphereCenter;
    float sphereRadius;
    DWORD numVertices;
    DWORD vertexFlags;
    SimpleVertex* vertices;
    SimpleVertex* normals;
    TexCoord* texCoords;
    Colour* colours;
  };
  Data* data;

  Th4Mesh(const Data* const __restrict data)
  {
    SetName(data->name);
    SetCenter(data->sphereCenter);
    SetBBox(data->bboxMin,data->bboxMax);
  }

  Th4Mesh(const char* const __restrict path, const DWORD index = 0, const bool materials = false);
  Th4Mesh(DWORD pFile, const DWORD index = 0, const bool materials = false);
};

#endif