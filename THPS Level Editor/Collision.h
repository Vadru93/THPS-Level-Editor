#ifndef COLLISION_H
#define COLLISION_H
#include "Structs.h"
#include <d3dx9.h>


#define EPSILON 0.000001f

#pragma unmanaged
bool TriangleIntersection(const D3DXVECTOR3* v1, const D3DXVECTOR3* v2, const D3DXVECTOR3* v3, const D3DXVECTOR3* rayPos, const D3DXVECTOR3* rayDir, float* outDistance, Vertex* outVertex);
bool TriangleIntersection(const D3DXVECTOR3* v1, const D3DXVECTOR3* v2, const D3DXVECTOR3* v3, const D3DXVECTOR3* rayPos, const D3DXVECTOR3* rayDir, float* outDistance);
bool TriangleIntersection(const D3DXVECTOR3* v1, const D3DXVECTOR3* v2, const D3DXVECTOR3* v3, const D3DXVECTOR3* rayPos, const D3DXVECTOR3* rayDir, D3DXVECTOR3* outHitPoint);
bool TriangleIntersection(const D3DXVECTOR3* v1, const D3DXVECTOR3* v2, const D3DXVECTOR3* v3, const D3DXVECTOR3* rayPos, const D3DXVECTOR3* rayDir);

struct NSCollisionHeader
{
  unsigned int Version;
  unsigned int numObjects;
  unsigned int numTotalVerts;
  unsigned int numLargeFaces;
  unsigned int numSmallFaces;
  unsigned int numLargeVerts;
  unsigned int numSmallVerts;
  unsigned int padding;
};

struct NSFaceInfo
{
  WORD Flags;//Collision flags
  WORD TerrainType;
};

struct NSFaceSmall
{
  NSFaceInfo Info;
  BYTE a;
  BYTE b;
  BYTE c;
  BYTE pad;
};

struct NSFace
{
  NSFaceInfo Info;
  WORD a;
  WORD b;
  WORD c;
  WORD pad;
};

struct NSFaceThug
{
  NSFaceInfo Info;
  WORD a;
  WORD b;
  WORD c;
};

struct NSFloatVert
{
  float pos[3];
  Colour colour;
};

struct NSFloatThugVert
{
  float pos[3];
};

struct NSFixedVert
{
  WORD pos[3];
};

struct NSCollisionObject
{
  unsigned int Name;
  WORD Flags;
  WORD numVerts;
  WORD numFaces;
  bool LargeFaces;
  bool FixedVerts;
  union {
    NSFace* Faces;
    NSFaceSmall* faces;
    NSFaceThug* FacesThug;
  };
  SimpleVertex bboxMin;
  float scaleMin;
  SimpleVertex bboxMax;
  float scaleMax;
  union {
    NSFloatVert* FloatVert;
    NSFloatThugVert* FloatThugVert;
    NSFixedVert* FixedVert;
  };
  void* Nodes;
  void* IntensityLst;
  int padding;

  SimpleVertex* GetUniqueVertsTHUG(WORD* numVerts)
  {
    SimpleVertex* verts = new SimpleVertex[this->numVerts];
    WORD vertCount=0;

    for(DWORD i=0; i<this->numVerts; i++)
    {
      bool found=false;
      for(DWORD j=0; j<vertCount; j++)
      {
        if(verts[j].x==FixedVerts ? (float)((float)((float)FixedVert[i].pos[0]/16.0f)+bboxMin.x) : FloatThugVert[i].pos[0] && verts[j].y==FixedVerts ? (float)((float)((float)FixedVert[i].pos[1]/16.0f)+bboxMin.y) : FloatThugVert[i].pos[1] && verts[j].z==FixedVerts ? (float)((float)((float)FixedVert[i].pos[2]/16.0f)+bboxMin.z)*-1 : FloatThugVert[i].pos[2]*-1)
        {
          found=true;
          break;
        }
      }
      if(!found)
      {
        verts[vertCount].x = FixedVerts ? (float)((float)((float)FixedVert[i].pos[0]/16.0f)+bboxMin.x) : FloatThugVert[i].pos[0];
        verts[vertCount].y = FixedVerts ? (float)((float)((float)FixedVert[i].pos[1]/16.0f)+bboxMin.y) : FloatThugVert[i].pos[1];
        verts[vertCount].z = FixedVerts ? (float)((float)((float)FixedVert[i].pos[2]/16.0f)+bboxMin.z) : FloatThugVert[i].pos[2]*-1;
        vertCount++;
      }

    }
    *numVerts=vertCount;
    return verts;
  }

  SimpleVertex* GetUniqueVerts(WORD* numVerts)
  {
    SimpleVertex* verts = new SimpleVertex[this->numVerts];
    WORD vertCount=0;

    for(DWORD i=0; i<this->numVerts; i++)
    {
      bool found=false;
      for(DWORD j=0; j<vertCount; j++)
      {
        if(verts[j].x==FloatVert[i].pos[0] && verts[j].y==FloatVert[i].pos[1] && verts[j].z==FloatVert[i].pos[2]*-1)
        {
          found=true;
          break;
        }
      }
      if(!found)
      {
        verts[vertCount].x = FloatVert[i].pos[0];
        verts[vertCount].y = FloatVert[i].pos[1];
        verts[vertCount].z = FloatVert[i].pos[2]*-1;
        vertCount++;
      }

    }
    *numVerts=vertCount;
    return verts;
  }

  NSCollisionObject()
  {
    Name =0;
    Flags=0;
    numVerts=0;
    numFaces=0;
    LargeFaces=0;
    FixedVerts=0;
    Faces=NULL;
    faces=NULL;
    scaleMin = 0.0f;
    scaleMax = 0.0f;
    FloatVert=NULL;
    FixedVert=NULL;
    Nodes=NULL;
    IntensityLst=NULL;
    padding=0;

  }

};

struct Ray
{
  D3DXVECTOR3 origin;
  D3DXVECTOR3 direction;

  Ray(const D3DXVECTOR3 pos, const D3DXVECTOR3 dir)
  {
    origin=pos;
    direction=dir;
  }
};

struct Mesh;

struct CollisionResult
{
  float distance;
  union
  {
    const Mesh* mesh;
    float hitPoint;
  };
  DWORD matIndex;
  DWORD triIndex;

  CollisionResult()
  {
    distance = FLT_MAX;
    mesh=NULL;
  }
  
  inline D3DXVECTOR3 GetHitPoint()
  {
    return *(D3DXVECTOR3*)&hitPoint;
  }

  inline void SetHitPoint(const D3DXVECTOR3 point)
  {
    *(D3DXVECTOR3*)&hitPoint = point;
  }

  DWORD GetCollType();
};

struct Collision
{
private:
  std::vector <Vertex> vertices;
  std::vector <SimpleFace> faces;
  std::vector <FaceInfo> facesInfo;
  BBox bbox;

  D3DXVECTOR3 normalize(SimpleFace* face)
  {
    D3DXVECTOR3 normal;
    D3DXVec3Cross(&normal, &(*(D3DXVECTOR3*)&vertices[face->b]-*(D3DXVECTOR3*)&vertices[face->a]),&(*(D3DXVECTOR3*)&vertices[face->c]-*(D3DXVECTOR3*)&vertices[face->a]));
    D3DXVECTOR3 normalized;
    D3DXVec3Normalize(&normalized,&normal);
    return normalized;
  }

  bool CheckFlagsAndNormal(DWORD numShared, DWORD* shared, WORD collFlags, D3DXVECTOR3 normal)
  {
    for(DWORD i=1; i<numShared; i++)
    {
      if(collFlags != facesInfo[shared[i]].flags || normalize(&faces[shared[i]]) != normal/*D3DXVec3Length(&(normalize(&faces[shared[i]]) - normal))>0.01*/)
        return false;
    }

    return true;
  }

  WORD GetOptimalShared(DWORD numShared, DWORD* shared, DWORD collapse)
  {
    WORD optimal=0;
    WORD best=0;
    WORD current;
    for(DWORD i=0; i<numShared; i++)
    {
      if(faces[shared[i]].a!=collapse)
      {
        current=0;
        for(DWORD j=0; j<numShared; j++)
        {
          if(faces[shared[j]].a == faces[shared[i]].a || faces[shared[j]].b == faces[shared[i]].a || faces[shared[j]].c == faces[shared[i]].a)
          {
            current++;
          }
        }
        if(current>=2)
          return faces[shared[i]].a;
        else if(best<current)
        {
          best=current;
          optimal=faces[shared[i]].a;
        }
      }

      if(faces[shared[i]].b!=collapse)
      {
        current=0;
        for(DWORD j=0; j<numShared; j++)
        {
          if(faces[shared[j]].a == faces[shared[i]].b || faces[shared[j]].b == faces[shared[i]].b || faces[shared[j]].c == faces[shared[i]].b)
          {
            current++;
          }
        }
        if(current>=2)
          return faces[shared[i]].b;
        else if(best<current)
        {
          best=current;
          optimal=faces[shared[i]].b;
        }
      }

      if(faces[shared[i]].c!=collapse)
      {
        current=0;
        for(DWORD j=0; j<numShared; j++)
        {
          if(faces[shared[j]].a == faces[shared[i]].c || faces[shared[j]].b == faces[shared[i]].c || faces[shared[j]].c == faces[shared[i]].c)
          {
            current++;
          }
        }
        if(current>=2)
          return faces[shared[i]].c;
        else if(best<current)
        {
          best=current;
          optimal=faces[shared[i]].c;
        }
      }


    }
    return optimal;
  }

  void Opt()
  {
    for(DWORD i=0; i<vertices.size(); i++)
    {
      for(DWORD j=i+1; j<vertices.size(); j++)
      {
        if(vertices[j].x==vertices[i].x && vertices[j].y==vertices[i].y && vertices[j].z==vertices[i].z)
        {
          for(DWORD k=0; k<faces.size(); k++)
          {
            if(faces[k].a==j)
              faces[k].a=i;
            else if(faces[k].a>j)
              faces[k].a--;

            if(faces[k].b==j)
              faces[k].b=i;
            else if(faces[k].b>j)
              faces[k].b--;

            if(faces[k].c==j)
              faces[k].c=i;
            else if(faces[k].c>j)
              faces[k].c--;
          }
          vertices.erase(vertices.begin()+j);
          j--;
        }
      }
    }

    /*std::vector <DWORD> shared;
    for(DWORD i=0; i<vertices.size(); i++)
    {
    for(DWORD j=0; j<faces.size(); j++)
    {
    if(faces[j].a==i || faces[j].b==i || faces[j].c==i)
    shared.push_back(j);
    }

    DWORD numShared = shared.size();

    if(numShared>2)
    {
    WORD collFlags  = facesInfo[shared[0]].flags;

    D3DXVECTOR3 normal;
    D3DXVec3Cross(&normal, &(*(D3DXVECTOR3*)&vertices[faces[shared[0]].b]-*(D3DXVECTOR3*)&vertices[faces[shared[0]].a]),&(*(D3DXVECTOR3*)&vertices[faces[shared[0]].c]-*(D3DXVECTOR3*)&vertices[faces[shared[0]].a]));
    D3DXVECTOR3 normalized;
    D3DXVec3Normalize(&normalized,&normal);

    if(CheckFlagsAndNormal(numShared, &shared.front(), collFlags, normalized))
    {
    WORD optimal = GetOptimalShared(numShared, &shared.front(), i);

    for(DWORD j=0; j<numShared; j++)
    {
    if(faces[shared[j]].a==i)
    faces[shared[j]].a=optimal;
    if(faces[shared[j]].b==i)
    faces[shared[j]].b=optimal;
    if(faces[shared[j]].c==i)
    faces[shared[j]].c=optimal;
    }
    }
    }
    shared.clear();
    }*/
    for(DWORD i=0; i<faces.size(); i++)
    {
      if(faces[i].a == faces[i].b || faces[i].a == faces[i].c || faces[i].b == faces[i].c)
      {
        faces.erase(faces.begin()+i);
        facesInfo.erase(facesInfo.begin()+i);
        i--;
      }
    }
    for(DWORD i=0; i<vertices.size(); i++)
    {
      bool found = false;
      for(DWORD j=0; j<faces.size(); j++)
      {
        if(faces[j].a==i || faces[j].b==i || faces[j].c==i)
          found = true;
      }
      if(!found)
      {
        for(DWORD j=0; j<faces.size(); j++)
        {
          if(faces[j].a>i)
            faces[j].a--;
          if(faces[j].b>i)
            faces[j].b--;
          if(faces[j].c>i)
            faces[j].c--;
        }
        vertices.erase(vertices.begin()+i);
        i--;
      }
    }
  }

  void Optimize()
  {
    const DWORD numFaces = GetNumFaces();
    for(WORD i=0; i<vertices.size(); i++)
    {
      for(DWORD j=i+1; j<this->vertices.size(); j++)
      {
        if(this->vertices[i].x == this->vertices[j].x && this->vertices[i].y == this->vertices[j].y && this->vertices[i].z == this->vertices[j].z)
        {
          for(DWORD k=0; k<numFaces; k++)
          {
            if(faces[k].a==i)
              faces[k].a=j;
            else if(faces[k].a>i)
              faces[k].a--;

            if(faces[k].b==i)
              faces[k].b=j;
            else if(faces[k].b>i)
              faces[k].b--;

            if(faces[k].c==i)
              faces[k].c=j;
            else if(faces[k].c>i)
              faces[k].c--;
          }
          this->vertices.erase(this->vertices.begin()+i);
          i--;
          break;
        }
      }
    }
  }

  void SetBBox(const SimpleVertex &bboxMin, const SimpleVertex &bboxMax)
  {
    bbox.Min = bboxMin;
    bbox.Max = bboxMax;
  }

  void AddVertices(NSFloatVert* vertices, const WORD &numVertices)
  {
    Vertex vertex;
    this->vertices.reserve(numVertices);

    for(WORD i=0; i<numVertices; i++)
    {
      vertex.x = vertices[i].pos[0];
      vertex.y = vertices[i].pos[1];
      vertex.z = vertices[i].pos[2]*-1;
      vertex.colour.r = vertices[i].colour.r/2;
      vertex.colour.g = vertices[i].colour.g/2;
      vertex.colour.b = vertices[i].colour.b/2;
      //vertex.colour.a = 255;
      this->vertices.push_back(vertex);
    }
  }

  void AddVertices(NSFixedVert* vertices, const WORD &numVertices, Colour* colours, SimpleVertex bboxMin)
  {
    Vertex vertex;
    for(WORD i=0; i<numVertices; i++)
    {
      vertex.x = (float)((float)((float)vertices[i].pos[0]/16.0f)+bboxMin.x);
      vertex.y = (float)((float)((float)vertices[i].pos[1]/16.0f)+bboxMin.y);
      vertex.z = (float)((float)((float)vertices[i].pos[2]/16.0f)+bboxMin.z)*-1;
      if(colours!=NULL)
        vertex.colour = colours[i];
      else
        vertex.colour = Colour(128,128,128);
      vertex.colour.a = 255;

      this->vertices.push_back(vertex);
    }
  }

  void AddVertices(NSFloatThugVert* vertices, const WORD &numVertices, Colour* colours)
  {
    Vertex vertex;
    for(WORD i=0; i<numVertices; i++)
    {
      vertex.x = vertices[i].pos[0];
      vertex.y = vertices[i].pos[1];
      vertex.z = vertices[i].pos[2]*-1;
      if(colours!=NULL)
        vertex.colour = colours[i];
      else
        vertex.colour = Colour(128,128,128);
      vertex.colour.a = 255;

      this->vertices.push_back(vertex);
    }
  }

  void AddFaces(NSFaceThug* faces, const WORD &numFaces)
  {
    SimpleFace face;
    FaceInfo faceInfo;
    this->faces.reserve(numFaces);
    facesInfo.reserve(numFaces);
    for(WORD i=0; i<numFaces; i++)
    {
      face.a = faces[i].a;
      face.b = faces[i].b;
      face.c = faces[i].c;
      faceInfo.flags = faces[i].Info.Flags;
      faceInfo.sound = faces[i].Info.TerrainType;

      this->faces.push_back(face);
      this->facesInfo.push_back(faceInfo);
    }
  }

  void AddFaces(const NSFaceSmall* faces, const WORD &numFaces)
  {
    SimpleFace face;
    FaceInfo faceInfo;
    this->faces.reserve(numFaces);
    facesInfo.reserve(numFaces);
    for(WORD i=0; i<numFaces; i++)
    {
      face.a = (WORD)faces[i].a;
      face.b = (WORD)faces[i].b;
      face.c = (WORD)faces[i].c;
      faceInfo.flags = faces[i].Info.Flags;
      faceInfo.sound = faces[i].Info.TerrainType;

      this->faces.push_back(face);
      this->facesInfo.push_back(faceInfo);
    }
  }

  void AddFaces(const NSFace* faces, const WORD &numFaces)
  {
    SimpleFace face;
    FaceInfo faceInfo;
    this->faces.reserve(numFaces);
    facesInfo.reserve(numFaces);
    for(WORD i=0; i<numFaces; i++)
    {
      face.a = faces[i].a;
      face.b = faces[i].b;
      face.c = faces[i].c;
      faceInfo.flags = faces[i].Info.Flags;
      faceInfo.sound = faces[i].Info.TerrainType;

      this->faces.push_back(face);
      this->facesInfo.push_back(faceInfo);
    }
  }

public:

  bool NonCollidable()
  {
    for (DWORD i = 0; i < GetNumFaces(); i++)
    {
      if (GetFaceInfo(i)->flags & 0x1 || !(GetFaceInfo(i)->flags & 0x10) || GetFaceInfo(i)->flags & 0x4 || GetFaceInfo(i)->flags & 8 || GetFaceInfo(i)->flags & 40)
        return false;
    }
    return true;
  }

  void SetBBox(const BBox& bbox)
  {
    this->bbox.Min = bbox.Min;
    this->bbox.Max = bbox.Max;
  }

  bool SameAs(Vertex* other, DWORD numVertices)
  {
    if (numVertices != vertices.size())
      return false;
    for (DWORD i = 0; i < numVertices; i++)
    {
      if (vertices[i].x != other[i].x || vertices[i].y != other[i].y || vertices[i].z != other[i].z)
        return false;
    }
    return true;
  }

  void BlendFaceTypes()
  {
    Colour cWall(0, 0, 255);
    Colour cRamp(255, 0, 0);
    Colour cNoColl(0, 255, 0);

    for (DWORD i = 0; i<faces.size(); i++)
    {
      if (facesInfo[i].flags & 0x4)
      {
        this->vertices[faces[i].a].colour.blend(cRamp);
        this->vertices[faces[i].b].colour.blend(cRamp);
        this->vertices[faces[i].c].colour.blend(cRamp);
      }
      else if (facesInfo[i].flags & 0x8)
      {
        this->vertices[faces[i].a].colour.blend(cWall);
        this->vertices[faces[i].b].colour.blend(cWall);
        this->vertices[faces[i].c].colour.blend(cWall);
      }
      else if (facesInfo[i].flags & 0x10)
      {
        this->vertices[faces[i].a].colour.blend(cNoColl);
        this->vertices[faces[i].b].colour.blend(cNoColl);
        this->vertices[faces[i].c].colour.blend(cNoColl);
      }
    }
  }


  void ClearVertices()
  {
    vertices.clear();
  }

  inline const D3DXVECTOR3 GetCenter() const
  {
    return (*(D3DXVECTOR3*)&bbox.Max+*(D3DXVECTOR3*)&bbox.Min)/2.0f;
  }

  SimpleVertex* GetUniqueVerts(WORD* numVerts)
  {
    const DWORD numVertices = vertices.size();
    SimpleVertex* verts = new SimpleVertex[numVertices];
    WORD vertCount=0;

    for(DWORD i=0; i<numVertices; i++)
    {
      bool found=false;
      for(DWORD j=0; j<vertCount; j++)
      {
        if(verts[j].x==vertices[i].x && verts[j].y==vertices[i].y && verts[j].z==vertices[i].z)
        {
          found=true;
          break;
        }
      }
      if(!found)
      {
        verts[vertCount].x = vertices[i].x;
        verts[vertCount].y = vertices[i].y;
        verts[vertCount].z = vertices[i].z;
        vertCount++;
      }

    }
    *numVerts=vertCount;
    return verts;
  }

  void SetPosition(const D3DXVECTOR3 const*__restrict position, const D3DXVECTOR3 const*__restrict angle, const D3DXVECTOR3 & center)
  {
      
      
    //D3DXVECTOR3 center = GetCenter();
    char text[256] = "";
    sprintf(text, "Collision: OldPos %f %f %f new %f %f %f", center.x, center.y, center.z, position->x, position->y, position->z);
       // MessageBox(0, text, "SETPOS", 0);
    *(D3DXVECTOR3*)&bbox.Max = *position+(*(D3DXVECTOR3*)&bbox.Max-center);
    *(D3DXVECTOR3*)&bbox.Min = *position-(*(D3DXVECTOR3*)&bbox.Min+center);
    D3DXMATRIX rotation;
    D3DXMATRIX translation;
    D3DXMATRIX result;
    D3DXMatrixRotationYawPitchRoll(&rotation, -angle->y + D3DX_PI, angle->x, angle->z);
    D3DXMatrixTranslation(&translation,position->x,position->y,position->z);
    D3DXMatrixMultiply(&result, &rotation, &translation);
    D3DXVec3TransformCoord((D3DXVECTOR3*)&bbox.Max, (D3DXVECTOR3*)&bbox.Max, &result);
    D3DXVec3TransformCoord((D3DXVECTOR3*)&bbox.Min, (D3DXVECTOR3*)&bbox.Min, &result);
    if(bbox.Min.x > bbox.Max.x)
    {
      const float temp = bbox.Max.x;
      bbox.Max.x = bbox.Min.x;
      bbox.Min.x = temp;
    }
    if(bbox.Min.y > bbox.Max.y)
    {
      const float temp = bbox.Max.y;
      bbox.Max.y = bbox.Min.y;
      bbox.Min.y = temp;
    }
    if(bbox.Min.x > bbox.Max.x)
    {
      const float temp = bbox.Max.z;
      bbox.Max.z = bbox.Min.z;
      bbox.Min.z = temp;
    }

    for(DWORD i=0, numVertices = vertices.size(); i<numVertices; i++)
    {
      //*(D3DXVECTOR3*)&vertices[i]-=center;
      D3DXVec3TransformCoord((D3DXVECTOR3*)&vertices[i], (D3DXVECTOR3*)&vertices[i], &result);
    }
  }

  __declspec (noalias) void AddVertices(const Th2FixedVert *const __restrict position, const Th2Vertex* const __restrict vertices, const DWORD numVertices, Colour** const __restrict colours)
  {
    Vertex vertex;
    BBox bbox;
    ZeroMemory(&vertex,sizeof(Vertex));
    vertex.colour = Colour(128,128,128);
    this->vertices.clear();
    this->vertices.reserve(numVertices);
    for(DWORD i=0; i<numVertices; i++)
    {
      vertex.x = -((float)vertices[i].x+((float)position->x/4096.0f))/2.833f;
      vertex.y = -((float)vertices[i].y+((float)position->y/4096.0f))/2.833f;
      vertex.z = -((float)vertices[i].z+((float)position->z/4096.0f))/2.833f;

      if(colours[i])//check if pointer is initialized it should be but on some levels it crash here if not check...
      {
        vertex.colour.r = (float)colours[i]->b*0.65f;
        vertex.colour.g = (float)colours[i]->g*0.65f;
        vertex.colour.b = (float)colours[i]->r*0.65f;

        if(colours[i]->a == 155)
        {
          vertex.colour.a = colours[i]->a;
          //delete colours[i];
        }
        else if(colours[i]->a == 154)
        {
          vertex.colour.a = colours[i]->a;
        }
        else
          vertex.colour.a = 255;
        //colours[i]=NULL;
      }
      else
        vertex.colour.a = 255;

      if(i==0)
      {
        bbox.Min.x = vertex.x;
        bbox.Min.y = vertex.y;
        bbox.Min.z = vertex.z*=-1;
        bbox.Max.x = vertex.x;
        bbox.Max.y = vertex.y;
        bbox.Max.z = vertex.x*=-1;
      }
      else
      {
        if(vertex.x<bbox.Min.x)
          bbox.Min.x=vertex.x;
        if(vertex.y<bbox.Min.y)
          bbox.Min.y=vertex.y;
        if(vertex.z*=-1<bbox.Min.z)
          bbox.Min.z=vertex.z*=-1;

        if(vertex.x>bbox.Max.x)
          bbox.Max.x=vertex.x;
        if(vertex.y>bbox.Max.y)
          bbox.Max.y=vertex.y;
        if(vertex.z*=-1>bbox.Max.z)
          bbox.Max.z=vertex.z*=-1;
      }

      this->vertices.push_back(vertex);
    }
    this->SetBBox(bbox);
  }

  void AddVertex(Vertex v)
  {
    v.colour = Colour(128, 128, 128);

    if (v.x < bbox.Min.x)
      bbox.Min.x = v.x;
    if (v.y < bbox.Min.y)
      bbox.Min.y = v.y;
    if (v.z < bbox.Min.z)
      bbox.Min.z = v.z;

    if (v.x > bbox.Max.x)
      bbox.Max.x = v.x;
    if (v.y > bbox.Max.y)
      bbox.Max.y = v.y;
    if (v.z > bbox.Max.z)
      bbox.Max.z = v.z;

    this->vertices.push_back(v);
  }

  void Move(const D3DXVECTOR3 const* __restrict position, const D3DXVECTOR3 const* __restrict angle)
  {
    D3DXMATRIX rotation;
    D3DXMatrixRotationYawPitchRoll(&rotation, -angle->y+D3DX_PI,angle->x,angle->z);
    D3DXVECTOR3 translation;
    D3DXVec3TransformCoord(&translation, position, &rotation);

    for(DWORD i=0; i<vertices.size(); i++)
    {
      *(D3DXVECTOR3*)&vertices[i]+=translation;
    }
    *(D3DXVECTOR3*)&bbox.Min+=translation;
    *(D3DXVECTOR3*)&bbox.Max+=translation;
    if(bbox.Min.x>bbox.Max.x)
    {
      float temp = bbox.Min.x;
      bbox.Min.x = bbox.Max.x;
      bbox.Max.x = temp;
      //std::swap(bbox.Min.x,bbox.Max.x);
    }
    if(bbox.Min.y>bbox.Max.y)
    {
      float temp = bbox.Min.y;
      bbox.Min.y = bbox.Max.y;
      bbox.Max.y = temp;
    }
    if(bbox.Min.z>bbox.Max.z)
    {
      float temp = bbox.Min.z;
      bbox.Min.z = bbox.Max.z;
      bbox.Max.z = temp;
    }
  }

  void AddVertices(Vertex* vertices, DWORD numVertices)
  {
    this->vertices.assign(vertices, vertices+numVertices);
  }

  void AddFaces(SimpleFace* faces, DWORD numFaces)
  {
    this->faces.reserve(numFaces);
    this->faces.assign(faces, faces+numFaces);
  }

  void CopyVertices(const Vertex* vertices, DWORD numVertices, const BBox* bbox)
  {
    this->vertices.resize(numVertices);
    for (DWORD i = 0; i < numVertices; i++)
    {
      this->vertices[i].x = vertices[i].x;
      this->vertices[i].y = vertices[i].y;
      this->vertices[i].z = vertices[i].z;

      this->vertices[i].colour = vertices[i].colour;
    }
    this->bbox = *bbox;
  }

  void AddFaces(FaceInfo* faces, DWORD numFaces)
  {
    this->faces.reserve(numFaces);
    this->facesInfo.assign(faces, faces+numFaces);
  }

  Collision(Collision* collis)
  {
    FaceInfo Simple;
    Simple.flags = 0x10;
    Simple.sound = 0;
    this->bbox = collis->GetBBox();
    if (collis)
    {
      for (DWORD i = 0; i < collis->GetNumFaces(); i++)
        this->AddFace(collis->GetFace(i)->a, collis->GetFace(i)->b, collis->GetFace(i)->c, &Simple);
    }
  }

  Collision()
  {
    ZeroMemory(&this->bbox, sizeof(BBox));//ZeroMemory(this, sizeof(Collision));
  }


  ~Collision()
  {
    faces.clear();
    vertices.clear();
  }

  inline const DWORD GetNumFaces() const
  {
    return faces.size();
  }

  inline const DWORD GetNumVertices() const
  {
    return vertices.size();
  }

  inline const Vertex* GetVertices() const
  {
    if(vertices.size())
      return &vertices.front();
    MessageBox(0, "tried collvert", "", 0);
    return NULL;
  }

  inline SimpleFace* const GetFace(const DWORD &index)
  {
    if(faces.size()>index)
    return &faces[index];
    MessageBox(0, "tried simpleface idx", "", 0);
  }

  inline const SimpleFace* GetFaces() const
  {
    if (faces.size())
      return &faces.front();
    MessageBox(0, "tried simpleface", "", 0);
  }

  inline const FaceInfo* const GetFacesInfo() const
  {

    if(facesInfo.size())
      return &facesInfo.front();
    MessageBox(0, "tried facesInfo", "", 0);
  }

  inline FaceInfo* const GetFaceInfo(const DWORD &index)
  {
    if (index < facesInfo.size())
      return &facesInfo[index];
    MessageBox(0, "tried facesinfo idxd", "", 0);
    return NULL;
  }

  bool Intersect(const D3DXVECTOR3* rayOrigin,const D3DXVECTOR3* rayDirection,float* outDistance, Vertex* meshVertices, DWORD* outTriangleIndex);

  inline const Vertex* GetVertex(const DWORD &index) const
  {
    return &vertices[index];
  }

  void Intersect(const Ray & __restrict ray, CollisionResult & __restrict closestHit, const Mesh* __restrict mesh) const;
  __declspec (noalias) void CastRay(const Ray & __restrict ray, const Mesh* __restrict mesh, CollisionResult & __restrict result) const;
  void GetHitPoint(const Ray & __restrict ray, CollisionResult & __restrict closestHit, const Mesh* __restrict mesh) const;
  __declspec (noalias) bool Collision::AboveSurface(const Ray & __restrict ray, const Mesh* __restrict const mesh) const;
  __declspec (noalias) void Collision::FindGround(const Ray & __restrict ray, const Mesh* __restrict const mesh, D3DXVECTOR3 & __restrict newPos) const;
  void Intersect(const Ray & __restrict ray, CollisionResult & __restrict closestHit, const Mesh* __restrict mesh, D3DXVECTOR3 & __restrict newPos) const;
  
  void AddFace(const Face &face)
  {
    SimpleFace sFace;
    sFace.a=face.a;
    sFace.b=face.b;
    sFace.c=face.c;
    faces.push_back(sFace);
    FaceInfo faceInfo;
    faceInfo.flags = face.flags;
    faceInfo.sound = face.sound;
    facesInfo.push_back(faceInfo);
  }

  _declspec (noalias) WORD GetFaceFlag(DWORD index) const
  {
    return facesInfo[index].flags;
  }

  void AddFaceInfo(const FaceInfo* const faceInfo)
  {
    facesInfo.push_back(faceInfo[0]);
  }

  void AddFace(WORD &a, WORD &b, WORD &c, FaceInfo* faceInfo)
  {
    facesInfo.push_back(faceInfo[0]);
    faces.push_back(SimpleFace(a,b,c));
  }

  inline const DWORD GetNumFaceInfo() const
  {
    return facesInfo.size();
  }

  __declspec (noalias) void Render(IDirect3DDevice9Ex* const __restrict Device, const bool isSelected, const Mesh* __restrict mesh) const;//const Vertex* const vertices, const DWORD numVertices) const
  
  inline const BBox &GetBBox() const
  {
    return bbox;
  }

  __declspec (noalias) void SetData(const NSCollisionObject collisionData, const DWORD version)
  {
    SetBBox(collisionData.bboxMin, collisionData.bboxMax);
    if(version==8)
    {
      collisionData.LargeFaces? AddFaces(collisionData.faces, collisionData.numFaces) : AddFaces(collisionData.Faces, collisionData.numFaces);
      AddVertices(collisionData.FloatVert, collisionData.numVerts);
    }
    else
    {
      collisionData.LargeFaces? AddFaces(collisionData.faces, collisionData.numFaces) : AddFaces(collisionData.FacesThug, collisionData.numFaces);
      if(collisionData.FixedVerts)
        AddVertices(collisionData.FixedVert, collisionData.numVerts, NULL, collisionData.bboxMin);
      else
        AddVertices(collisionData.FloatThugVert, collisionData.numVerts, NULL);
    }
    Opt();
    //Optimize();
    BlendFaceTypes();
  }
};

NSCollisionObject* ProcessNSCollision(DWORD ptr);

#endif