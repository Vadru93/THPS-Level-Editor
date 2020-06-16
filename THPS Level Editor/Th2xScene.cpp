#include "StdAfx.h"
#include "Th2xScene.h"
#include <d3d9.h>
#include <d3dx9.H>
#include "ImageHandling.h"
#include "RevisitedRadix.h"
#include "CustomArray.h"
#include "Adjacency.h"
#include "Striper.h"
#include "zip.h"
#include "unzip.h"


#pragma unmanaged
extern TexFiles texFiles; extern DWORD MAX_TEXCOORDS; extern IDirect3DDevice9Ex* __restrict Device; extern vector <Script> Scripts; extern void IncreasProgressbar(); extern void SetProgressbarMaximum(DWORD maximum);
extern DWORD epicIndex;
extern MaterialList* __restrict globalMaterialList;
extern void ProcessTRG(BYTE* pFile, DWORD size);

struct XboxVertex
{
  float x;
  float y;
  float z;
  float xn;
  float yn;
  float zn;
  DWORD RGBX;
  float u;
  float v;
};

struct Th2xSplit
{
  WORD matIndex;
  WORD offset;
  WORD count;
};


struct Th2xMaterial
{
  char name[64];
  char texture[64];
  DWORD drawOrder;
  DWORD id;
  float unk2[3];
  DWORD unk3;
};
struct Th2xMesh
{
  int index;
  DWORD checksum;
  float animSpeedX;
  float animSpeedY;
  float u3;
  int u4;

  DWORD flags;
  char name[64];
  char unk[32];
  DWORD numVertices;
  DWORD numIndices;
  DWORD numMaterials;
};

struct DDMData
{
  DWORD checksum;
  int index;
  vector<Vertex> vertices;
  vector<MaterialSplit> splits;
  bool imported;
  BYTE grass;

  DDMData()
  {
    imported = false;
  }

  DWORD GetNumIndices()
  {
    DWORD numIndices=0;
    for (DWORD i = 0; i < splits.size(); i++)
    {
      numIndices += splits[i].Indices.size();
    }
    return numIndices;
  }
}; vector<DDMData> ddmObj;


void Th2xScene::ProcessMeshes(BYTE* pFile, DWORD size, bool sky)
{
#pragma pack(push)
#pragma pack(1)
  struct Quad
  {
    union
    {
      BYTE flag;
      DWORD flags;
    };
    BYTE a;
    BYTE b;
    BYTE c;
    BYTE d;
    BYTE index[4];
    WORD faceIndex;
    WORD collFlags;
    DWORD matIndex;
    union
    {
      signed short uv[8];
      signed char signedUV[8];
    };
    //signed char u[4];
    //signed char v[4];
    DWORD zero;
  };

  struct Th2Mesh
  {
    WORD type;
    WORD numVertices;
    WORD numCollision;
    WORD numQuads;
    DWORD flags;
    Th2Vertex* vertices;
    DWORD* collisions;
    Quad** tristrip;
    DWORD unknown;
  };
#pragma pack(pop)

  DWORD ptr = (DWORD)pFile;
  ptr += 4;

  DWORD rgbOffset = ((DWORD*)ptr)[0];
  DWORD numObjects = ((DWORD*)ptr)[1];
  ptr += 8;

  Th2FixedVert** positions = new Th2FixedVert*[numObjects + 1];
  positions[numObjects] = &Th2FixedVert();
  //DWORD* meshFlags = new DWORD[numObjects];
  WORD* meshIndices = new WORD[numObjects];

  for (DWORD i = 0; i < numObjects; i++)
  {
    ptr += 4;
    positions[i] = ((Th2FixedVert*)ptr);
    ptr += 18;
    meshIndices[i] = ((WORD*)ptr)[0];
    ptr += 14;
    //ptr+=32;
  }

  DWORD numMeshes = ((DWORD*)ptr)[0];
  this->meshes.reserve(numMeshes);
  if (numMeshes > ddmObj.size())
    SetProgressbarMaximum(ddmObj.size() + numMeshes);

  DWORD* meshNames = new DWORD[numMeshes];
  // if(!sky)
  DWORD pRGB = rgbOffset + (DWORD)pFile;
  pRGB += 4;
  DWORD numRGBs = ((DWORD*)pRGB)[0];
  pRGB += 4;
  pRGB += numRGBs;

  if (pRGB < ((DWORD)pFile + size))
    ProcessMaterials(pRGB, numMeshes, meshNames, sky);
  else
  {
    for (DWORD i = 0; i < numMeshes; i++)
    {
      meshNames[i] = i + 0xABEBCE;
    }
  }

  ptr += 4; Th2Mesh** meshes = new Th2Mesh*[numMeshes];
  for (DWORD i = 0; i < numMeshes; i++)
  {
    DWORD offset = ((DWORD*)ptr)[0];
    ptr += 4;
    meshes[i] = (Th2Mesh*)((DWORD)pFile + offset);
  }

  Colour** colours;
  DWORD numGrassMeshes = 0;
  TexCoord* texCoords;
  FaceInfo faceInfo;
  vector <Face> extraFaces;
  vector <Th2Vertex> vertices;
  DWORD vcount = 1;

  for (DWORD i = 0; i < numMeshes; i++)
  {
    IncreasProgressbar();
    Mesh* mesh = new Mesh();
    mesh->InitCollision();

    Checksum name;
    name.checksum = meshNames[i];
    mesh->SetName(name);
    /*if (!strcmp(name.GetString2(), "mirror_Han_Wind_Tunnel39"))
      MessageBox(0, name.GetString2(), name.GetString2(), 0);*/
    DDMData* ddm = NULL;
    for (DWORD j = 0; j < numObjects; j++)
    {
      if (meshIndices[j] == i)
      {
        for (DWORD k = 0; k < ddmObj.size(); k++)
        {
          if (ddmObj[k].index == j)
          {
            ddm = &ddmObj[k];
            goto found;
          }
        }
      }
    }
    if (ddm == NULL)
    {
      for (DWORD j = 0; j < ddmObj.size(); j++)
      {
        if (ddmObj[j].checksum == name.checksum)
        {
          ddm = &ddmObj[j];
          break;
        }
      }
    }
  found:;

    if (ddm)
    {
      ddm->imported = true;
      //MessageBox(0, mesh->GetName().GetString2(), mesh->GetName().GetString2(), 0);
      for (DWORD j = 0; j < ddm->splits.size(); j++)
        mesh->AddMatSplit(&ddm->splits[j], &matList);

    }

    Th2Vertex* vertex1 = (Th2Vertex*)((DWORD)meshes[i] + 12);
    Th2Vertex* vertex2 = (Th2Vertex*)((DWORD)meshes[i] + 18);
    mesh->center.x = (((float)vertex1->x + (float)vertex1->y) / 2.0f) / 2.833f;
    mesh->center.y = (((float)vertex1->z + (float)vertex2->x) / 2.0f) / 2.833f;
    mesh->center.z = (((float)vertex2->y + (float)vertex2->z) / 2.0f) / 2.833f;
    //Th2Vertex* vertx2 = (Th2Vertex*)((DWORD)meshes[i] + 12);
    meshes[i]->vertices = (Th2Vertex*)((DWORD)meshes[i] + 28);
    vertices.assign(meshes[i]->vertices, meshes[i]->vertices + meshes[i]->numVertices);

    colours = new Colour*[meshes[i]->numVertices + meshes[i]->numVertices*meshes[i]->numQuads];
    //Colour** tmpColours = new Colour*[meshes[i]->numVertices];
    texCoords = new TexCoord[meshes[i]->numVertices + meshes[i]->numVertices*meshes[i]->numQuads];
    ZeroMemory(texCoords, sizeof(TexCoord)*(meshes[i]->numVertices + meshes[i]->numVertices*meshes[i]->numQuads));
    ZeroMemory(colours, 4 * (meshes[i]->numVertices + meshes[i]->numVertices*meshes[i]->numQuads));//zeroymemory here so that if colour is not initialized later can ignore it if(colours[i]==NULL)

    meshes[i]->collisions = (DWORD*)((DWORD)meshes[i]->vertices + sizeof(Th2Vertex)*meshes[i]->numVertices);
    ptr = (DWORD)meshes[i]->collisions + 8 * meshes[i]->numCollision;
    meshes[i]->tristrip = new Quad*[meshes[i]->numQuads];
    MaterialSplit* matSplit = new MaterialSplit[meshes[i]->numQuads];
    DWORD splitCount = 0;
    bool first = true;

    for (DWORD j = 0; j < meshes[i]->numQuads; j++)
    {
      meshes[i]->tristrip[j] = (Quad*)ptr;
      /*char test[256] = "";
      sprintf(test, "%X %X", &meshes[i]->tristrip[j]->matIndex, ptr);
      MessageBox(0, test, test, 0);*/

      /*if(meshes[i]->tristrip[j]->a < meshes[i]->numVertices && meshes[i]->tristrip[j]->b  < meshes[i]->numVertices && meshes[i]->tristrip[j]->c  < meshes[i]->numVertices)
      {*/
      /*if(meshes[i]->tristrip[j]->collFlags & 0x2)
      mesh->SetFlag(MeshFlags::isVisible, false);*/

      if (!((GetBit(meshes[i]->tristrip[j]->flags, 7) && !GetBit(meshes[i]->tristrip[j]->flags, 6))/* ||  meshes[i]->tristrip[j]->collFlags & 0x2*/))
      {
        DWORD a = (DWORD)meshes[i]->tristrip[j]->a;
        DWORD b = (DWORD)meshes[i]->tristrip[j]->b;
        DWORD c = (DWORD)meshes[i]->tristrip[j]->c;
        DWORD d = (DWORD)meshes[i]->tristrip[j]->d;

        Material* tmpMat = NULL;

        if (((GetBit(meshes[i]->tristrip[j]->flag, 0) || GetBit(meshes[i]->tristrip[j]->flag, 3) || GetBit(meshes[i]->tristrip[j]->flag, 1)) && meshes[i]->tristrip[j]->flag != 128 && meshes[i]->tristrip[j]->flag != 0x90 && meshes[i]->tristrip[j]->flag != 0 && meshes[i]->tristrip[j]->flag != 0x10 && (meshes[i]->tristrip[j]->flags & 0xFF0000) != 0x100000))//if(GetBit(meshes[i]->tristrip[j]->flags,0)/* || GetBit(meshes[i]->tristrip[j]->flags,1)*/)//meshes[i]->tristrip[j]->flag != 128 && meshes[i]->tristrip[j]->flag != 0x90 && meshes[i]->tristrip[j]->flag != 0 && meshes[i]->tristrip[j]->flag != 0x10)
        {
          float u1 = (float)texCoords[a].u;
          float v1 = (float)texCoords[a].v;
          float u2 = (float)texCoords[b].u;
          float v2 = (float)texCoords[b].v;
          float u3 = (float)texCoords[c].u;
          float v3 = (float)texCoords[c].v;
          float u4 = (float)texCoords[d].u;
          float v4 = (float)texCoords[d].v;

          float uv[8];
          Colour* colourA = NULL;
          Colour* colourB = NULL;
          Colour* colourC = NULL;
          Colour* colourD = NULL;

          tmpMat = matList.GetMaterial(meshes[i]->tristrip[j]->matIndex, &meshes[i]->tristrip[j]->matIndex);
          if (ddm && tmpMat == NULL)
          {
            tmpMat = matList.GetMaterial(Checksum(ddm->splits[0].matId));
          }

          if (tmpMat == NULL)
          {
            //MessageBox(0, "epic", "", 0);
            if (ddm)
              meshes[i]->tristrip[j]->matIndex = Checksum("NO-MATERIAL").checksum;
            else
              meshes[i]->tristrip[j]->matIndex = epicIndex;
            if (ddm)
              tmpMat = matList.GetMaterial(Checksum("NO-MATERIAL"));
            else
              tmpMat = matList.GetMaterial(epicIndex);
          }
          if (tmpMat)
          {
            uv[0] = (float)(meshes[i]->tristrip[j]->uv[0]) / (512.0f);
            uv[2] = (float)(meshes[i]->tristrip[j]->uv[1]) / (512.0f);
            uv[4] = (float)(meshes[i]->tristrip[j]->uv[2]) / (512.0f);
            uv[6] = (float)(meshes[i]->tristrip[j]->uv[3]) / (512.0f);

            uv[1] = (float)(meshes[i]->tristrip[j]->uv[4]) / (512.0f);
            uv[3] = (float)(meshes[i]->tristrip[j]->uv[5]) / (512.0f);
            uv[5] = (float)(meshes[i]->tristrip[j]->uv[6]) / (512.0f);
            uv[7] = (float)(meshes[i]->tristrip[j]->uv[7]) / (512.0f);

            if (meshes[i]->tristrip[j]->flags & 0x800)
            {
              if ((GetBit(meshes[i]->tristrip[j]->flags, 7) && GetBit(meshes[i]->tristrip[j]->flags, 6)) || (!GetBit(meshes[i]->tristrip[j]->flags, 7) && GetBit(meshes[i]->tristrip[j]->flags, 6)))
              {
                colourA = new Colour((Colour*)((DWORD)pFile + 8 + rgbOffset + (DWORD)meshes[i]->tristrip[j]->index[0] * 4));
                colourA->a = 155;
                colourB = new Colour((Colour*)((DWORD)pFile + 8 + rgbOffset + (DWORD)meshes[i]->tristrip[j]->index[1] * 4));
                colourB->a = 155;
                colourC = new Colour((Colour*)((DWORD)pFile + 8 + rgbOffset + (DWORD)meshes[i]->tristrip[j]->index[2] * 4));
                colourC->a = 155;
              }
              else
              {
                colourA = (Colour*)((DWORD)pFile + 8 + rgbOffset + (DWORD)meshes[i]->tristrip[j]->index[0] * 4);
                colourB = (Colour*)((DWORD)pFile + 8 + rgbOffset + (DWORD)meshes[i]->tristrip[j]->index[1] * 4);
                colourC = (Colour*)((DWORD)pFile + 8 + rgbOffset + (DWORD)meshes[i]->tristrip[j]->index[2] * 4);
              }

              if (!GetBit(meshes[i]->tristrip[j]->flag, 4))
              {
                if ((GetBit(meshes[i]->tristrip[j]->flags, 7) && GetBit(meshes[i]->tristrip[j]->flags, 6)) || (!GetBit(meshes[i]->tristrip[j]->flags, 7) && GetBit(meshes[i]->tristrip[j]->flags, 6)))
                {
                  colourD = new Colour((Colour*)((DWORD)pFile + 8 + rgbOffset + (DWORD)meshes[i]->tristrip[j]->index[3] * 4));
                  colourD->a = 155;
                }
                else
                  colourD = (Colour*)((DWORD)pFile + 8 + rgbOffset + (DWORD)meshes[i]->tristrip[j]->index[3] * 4);
              }
            }
            else
            {
              DWORD* color = (DWORD*)&meshes[i]->tristrip[j]->index;
              if ((GetBit(meshes[i]->tristrip[j]->flags, 7) && GetBit(meshes[i]->tristrip[j]->flags, 6)) || (!GetBit(meshes[i]->tristrip[j]->flags, 7) && GetBit(meshes[i]->tristrip[j]->flags, 6)))
              {
                ((Colour*)color)->a = 154;
              }
              colourA = (Colour*)color;
              colourB = (Colour*)color;
              colourC = (Colour*)color;

              if (!GetBit(meshes[i]->tristrip[j]->flag, 4))
                colourD = (Colour*)color;
            }

            if (((colours[c] && (*colours[c] != *colourC || (texCoords[c].u != uv[4] || texCoords[c].v != uv[5]))) || ((((!GetBit(meshes[i]->tristrip[j]->flags, 4) && colours[d]) && (*colours[d] != *colourD || (texCoords[d].u != uv[6] || texCoords[d].v != uv[7])))))))
            {
              Th2Vertex vertex;

              vertex = meshes[i]->vertices[c];
              c = vertices.size();
              vertices.push_back(vertex);
              if (!GetBit(meshes[i]->tristrip[j]->flags, 4))
              {
                vertex = meshes[i]->vertices[d];
                d = c + 1;
                vertices.push_back(vertex);
              }

              if (GetBit(meshes[i]->tristrip[j]->flags, 1) && ((colours[a] && (*colours[a] != *colourA || (texCoords[a].u != uv[0] || texCoords[a].v != uv[1]))) || ((colours[b] && (*colours[b] != *colourB || (texCoords[b].u != uv[2] || texCoords[b].v != uv[3]))))))//GetBit(meshes[i]->tristrip[j]->flags,1))
              {
                vertex = meshes[i]->vertices[a];
                a = vertices.size();
                vertices.push_back(vertex);
                vertex = meshes[i]->vertices[b];
                b = a + 1;
                vertices.push_back(vertex);
              }
            }
            else if (GetBit(meshes[i]->tristrip[j]->flags, 1) && ((colours[a] && (*colours[a] != *colourA || (texCoords[a].u != uv[0] || texCoords[a].v != uv[1]))) || ((colours[b] && (*colours[b] != *colourB || (texCoords[b].u != uv[2] || texCoords[b].v != uv[3]))))))
            {
              Th2Vertex vertex;

              vertex = meshes[i]->vertices[a];
              a = vertices.size();
              vertices.push_back(vertex);
              vertex = meshes[i]->vertices[b];
              b = a + 1;
              vertices.push_back(vertex);
            }

            if (!GetBit(meshes[i]->tristrip[j]->flags, 4))
            {
              if (colours[d] && colours[d]->a == 155)
                delete colours[d];
              colours[d] = colourD;
              texCoords[d].u = uv[6];
              texCoords[d].v = uv[7];
            }
            if (GetBit(meshes[i]->tristrip[j]->flags, 1))
            {
              if (colours[a] && colours[a]->a == 155)
                delete colours[a];
              if (colours[b] && colours[b]->a == 155)
                delete colours[b];
              if (colours[c] && colours[c]->a == 155)
                delete colours[c];
              colours[a] = colourA;
              colours[b] = colourB;
              colours[c] = colourC;
              texCoords[a].u = uv[0];
              texCoords[a].v = uv[1];
              texCoords[b].u = uv[2];
              texCoords[b].v = uv[3];
              texCoords[c].u = uv[4];
              texCoords[c].v = uv[5];
            }
            else if (colourA)
            {
              colours[a] = colourA;
              colours[b] = colourB;
              colours[c] = colourC;
            }
          }
          else
          {
            MessageBox(0, "no mat", "", 0);
            if (meshes[i]->tristrip[j]->flags & 0x800)
            {
              if ((GetBit(meshes[i]->tristrip[j]->flags, 7) && GetBit(meshes[i]->tristrip[j]->flags, 6)) || (!GetBit(meshes[i]->tristrip[j]->flags, 7) && GetBit(meshes[i]->tristrip[j]->flags, 6)))
              {
                colourA = new Colour((Colour*)((DWORD)pFile + 8 + rgbOffset + (DWORD)meshes[i]->tristrip[j]->index[0] * 4));
                colourA->a = 155;
                colourB = new Colour((Colour*)((DWORD)pFile + 8 + rgbOffset + (DWORD)meshes[i]->tristrip[j]->index[1] * 4));
                colourB->a = 155;
                colourC = new Colour((Colour*)((DWORD)pFile + 8 + rgbOffset + (DWORD)meshes[i]->tristrip[j]->index[2] * 4));
                colourC->a = 155;
              }
              else
              {
                colourA = (Colour*)((DWORD)pFile + 8 + rgbOffset + (DWORD)meshes[i]->tristrip[j]->index[0] * 4);
                colourB = (Colour*)((DWORD)pFile + 8 + rgbOffset + (DWORD)meshes[i]->tristrip[j]->index[1] * 4);
                colourC = (Colour*)((DWORD)pFile + 8 + rgbOffset + (DWORD)meshes[i]->tristrip[j]->index[2] * 4);
              }

              if (colours[a] && *colours[a] != *colourA)
              {
                Th2Vertex vertex;

                vertex = meshes[i]->vertices[a];
                a = vertices.size();
                vertices.push_back(vertex);
              }
              colours[a] = colourA;
              if (colours[b] && *colours[b] != *colourB)
              {
                Th2Vertex vertex;

                vertex = meshes[i]->vertices[b];
                b = vertices.size();
                vertices.push_back(vertex);
              }
              colours[b] = colourB;
              if (colours[c] && *colours[c] != *colourC)
              {
                Th2Vertex vertex;

                vertex = meshes[i]->vertices[c];
                c = vertices.size();
                vertices.push_back(vertex);
              }
              colours[c] = colourC;

              if (!GetBit(meshes[i]->tristrip[j]->flag, 4))
              {
                if ((GetBit(meshes[i]->tristrip[j]->flags, 7) && GetBit(meshes[i]->tristrip[j]->flags, 6)) || (!GetBit(meshes[i]->tristrip[j]->flags, 7) && GetBit(meshes[i]->tristrip[j]->flags, 6)))
                {
                  colourD = new Colour((Colour*)((DWORD)pFile + 8 + rgbOffset + (DWORD)meshes[i]->tristrip[j]->index[3] * 4));
                  colourD->a = 155;
                }
                else
                  colourD = (Colour*)((DWORD)pFile + 8 + rgbOffset + (DWORD)meshes[i]->tristrip[j]->index[3] * 4);

                if (colours[d] && *colours[d] != *colourD)
                {
                  Th2Vertex vertex;

                  vertex = meshes[i]->vertices[d];
                  d = vertices.size();
                  vertices.push_back(vertex);
                }
                colours[d] = colourD;
              }
            }
            else
            {
              DWORD* color = (DWORD*)&meshes[i]->tristrip[j]->index;
              if ((GetBit(meshes[i]->tristrip[j]->flags, 7) && GetBit(meshes[i]->tristrip[j]->flags, 6)) || (!GetBit(meshes[i]->tristrip[j]->flags, 7) && GetBit(meshes[i]->tristrip[j]->flags, 6)))
              {
                ((Colour*)color)->a = 154;
              }
              colourA = (Colour*)color;
              colourB = (Colour*)color;
              colourC = (Colour*)color;

              if (colours[a] && *colours[a] != *colourA)
              {
                Th2Vertex vertex;

                vertex = meshes[i]->vertices[a];
                a = vertices.size();
                vertices.push_back(vertex);
              }
              colours[a] = colourA;
              if (colours[b] && *colours[b] != *colourB)
              {
                Th2Vertex vertex;

                vertex = meshes[i]->vertices[b];
                b = vertices.size();
                vertices.push_back(vertex);
              }
              colours[b] = colourB;
              if (colours[c] && *colours[c] != *colourC)
              {
                Th2Vertex vertex;

                vertex = meshes[i]->vertices[c];
                c = vertices.size();
                vertices.push_back(vertex);
              }
              colours[c] = colourC;

              if (!GetBit(meshes[i]->tristrip[j]->flag, 4))
              {
                colourD = (Colour*)color;
                if (colours[d] && *colours[d] != *colourD)
                {
                  Th2Vertex vertex;

                  vertex = meshes[i]->vertices[d];
                  d = vertices.size();
                  vertices.push_back(vertex);
                }
                colours[d] = colourD;
              }
            }
          }
        }
        else
        {


          Colour* colourA = NULL;
          Colour* colourB = NULL;
          Colour* colourC = NULL;
          Colour* colourD = NULL;

          if (meshes[i]->tristrip[j]->flags & 0x800)
          {
            if ((GetBit(meshes[i]->tristrip[j]->flags, 7) && GetBit(meshes[i]->tristrip[j]->flags, 6)) || (!GetBit(meshes[i]->tristrip[j]->flags, 7) && GetBit(meshes[i]->tristrip[j]->flags, 6)))
            {
              colourA = new Colour((Colour*)((DWORD)pFile + 8 + rgbOffset + (DWORD)meshes[i]->tristrip[j]->index[0] * 4));
              colourA->a = 155;
              colourB = new Colour((Colour*)((DWORD)pFile + 8 + rgbOffset + (DWORD)meshes[i]->tristrip[j]->index[1] * 4));
              colourB->a = 155;
              colourC = new Colour((Colour*)((DWORD)pFile + 8 + rgbOffset + (DWORD)meshes[i]->tristrip[j]->index[2] * 4));
              colourC->a = 155;
            }
            else
            {
              colourA = (Colour*)((DWORD)pFile + 8 + rgbOffset + (DWORD)meshes[i]->tristrip[j]->index[0] * 4);
              colourB = (Colour*)((DWORD)pFile + 8 + rgbOffset + (DWORD)meshes[i]->tristrip[j]->index[1] * 4);
              colourC = (Colour*)((DWORD)pFile + 8 + rgbOffset + (DWORD)meshes[i]->tristrip[j]->index[2] * 4);
            }

            if (!GetBit(meshes[i]->tristrip[j]->flag, 4))
            {
              if ((GetBit(meshes[i]->tristrip[j]->flags, 7) && GetBit(meshes[i]->tristrip[j]->flags, 6)) || (!GetBit(meshes[i]->tristrip[j]->flags, 7) && GetBit(meshes[i]->tristrip[j]->flags, 6)))
              {
                colourD = new Colour((Colour*)((DWORD)pFile + 8 + rgbOffset + (DWORD)meshes[i]->tristrip[j]->index[3] * 4));
                colourD->a = 155;
              }
              else
                colourD = (Colour*)((DWORD)pFile + 8 + rgbOffset + (DWORD)meshes[i]->tristrip[j]->index[3] * 4);
            }
          }
          else
          {
            DWORD* color = (DWORD*)&meshes[i]->tristrip[j]->index;
            if ((GetBit(meshes[i]->tristrip[j]->flags, 7) && GetBit(meshes[i]->tristrip[j]->flags, 6)) || (!GetBit(meshes[i]->tristrip[j]->flags, 7) && GetBit(meshes[i]->tristrip[j]->flags, 6)))
            {
              ((Colour*)color)->a = 154;
            }
            colourA = (Colour*)color;
            colourB = (Colour*)color;
            colourC = (Colour*)color;

            if (!GetBit(meshes[i]->tristrip[j]->flag, 4))
              colourD = (Colour*)color;
          }

          if (((colours[c] && (*colours[c] != *colourC)) || ((((!GetBit(meshes[i]->tristrip[j]->flags, 4) && colours[d]) && (*colours[d] != *colourD))))))
          {
            Th2Vertex vertex;

            vertex = meshes[i]->vertices[c];
            c = vertices.size();
            vertices.push_back(vertex);
            if (!GetBit(meshes[i]->tristrip[j]->flags, 4))
            {
              vertex = meshes[i]->vertices[d];
              d = c + 1;
              vertices.push_back(vertex);
            }

            if (GetBit(meshes[i]->tristrip[j]->flags, 1) && ((colours[a] && (*colours[a] != *colourA)) || ((colours[b] && (*colours[b] != *colourB)))))//GetBit(meshes[i]->tristrip[j]->flags,1))
            {
              vertex = meshes[i]->vertices[a];
              a = vertices.size();
              vertices.push_back(vertex);
              vertex = meshes[i]->vertices[b];
              b = a + 1;
              vertices.push_back(vertex);
            }
          }
          else if (GetBit(meshes[i]->tristrip[j]->flags, 1) && ((colours[a] && (*colours[a] != *colourA)) || ((colours[b] && (*colours[b] != *colourB)))))
          {
            Th2Vertex vertex;

            vertex = meshes[i]->vertices[a];
            a = vertices.size();
            vertices.push_back(vertex);
            vertex = meshes[i]->vertices[b];
            b = a + 1;
            vertices.push_back(vertex);
          }

          if (!GetBit(meshes[i]->tristrip[j]->flags, 4))
          {
            if (colours[d] && colours[d]->a == 155)
              delete colours[d];
            colours[d] = colourD;
          }
          if (GetBit(meshes[i]->tristrip[j]->flags, 1))
          {
            if (colours[a] && colours[a]->a == 155)
              delete colours[a];
            if (colours[b] && colours[b]->a == 155)
              delete colours[b];
            if (colours[c] && colours[c]->a == 155)
              delete colours[c];
            colours[a] = colourA;
            colours[b] = colourB;
            colours[c] = colourC;
          }
          else if (colourA)
          {
            colours[a] = colourA;
            colours[b] = colourB;
            colours[c] = colourC;
          }
        }
        if (first)
        {
          first = false;
          matSplit[0].matId = meshes[i]->tristrip[0]->matIndex;
        }
        else if (matSplit[splitCount].matId == meshes[i]->tristrip[j]->matIndex)
        {
          matSplit[splitCount].Indices.push_back((WORD)b);
        }
        else
        {
          matSplit[splitCount].Indices.pop_back();
          matSplit[splitCount].Indices.pop_back();
          matSplit[splitCount].Indices.pop_back();
          matSplit[splitCount].Indices.pop_back();
          matSplit[splitCount].numIndices = matSplit[splitCount].Indices.size();
          splitCount++;
          matSplit[splitCount].matId = meshes[i]->tristrip[j]->matIndex;
        }

        matSplit[splitCount].Indices.push_back((WORD)b);
        matSplit[splitCount].Indices.push_back((WORD)a);
        matSplit[splitCount].Indices.push_back((WORD)c);

        faceInfo.flags = 0x0001;
        faceInfo.flags |= 0x0200;

        if (GetBit(meshes[i]->tristrip[j]->collFlags, 8))
        {

          if (!GetBit(meshes[i]->tristrip[j]->collFlags, 7))
          {
            faceInfo.flags = 0x0005;
          }
          else
          {
            faceInfo.flags = 0x0003;
          }
          if (meshes[i]->tristrip[j]->collFlags & 0x0400)
            faceInfo.flags |= 0x0400;
          faceInfo.flags |= 0x0100;
        }
        if (GetBit(meshes[i]->tristrip[j]->collFlags, 5))
        {
          faceInfo.flags |= 0x0009;
          faceInfo.flags &= ~0x0002;
        }
        else if (GetBit(meshes[i]->tristrip[j]->collFlags, 6) && !GetBit(meshes[i]->tristrip[j]->collFlags, 8))
        {
          faceInfo.flags |= 0x0018;
          faceInfo.flags &= ~0x0001;
          faceInfo.flags &= ~0x0002;
          //faceInfo.flags = 0x0510;
        }
        else if (GetBit(meshes[i]->tristrip[j]->collFlags, 1))
        {
          faceInfo.flags &= ~0x0001;
          faceInfo.flags |= 0x0050;
        }
        else if (GetBit(meshes[i]->tristrip[j]->collFlags, 0))
        {
          faceInfo.flags = 0x0510;
        }
        if (GetBit(meshes[i]->tristrip[j]->collFlags, 3))
        {
          faceInfo.flags |= 0x0040;
        }
        mesh->AddFace(b, a, c, &faceInfo);

        if (j != meshes[i]->numQuads - 1 || !GetBit(meshes[i]->tristrip[j]->flags, 4))
        {
          matSplit[splitCount].Indices.push_back((WORD)c);
          matSplit[splitCount].Indices.push_back(0);
          matSplit[splitCount].Indices.push_back(0);
          matSplit[splitCount].Indices.push_back(0);
        }

        if (!GetBit(meshes[i]->tristrip[j]->flag, 4))
        {
          mesh->AddFace(b, c, d, &faceInfo);
          matSplit[splitCount].Indices.push_back((WORD)b);
          matSplit[splitCount].Indices.push_back((WORD)b);
          matSplit[splitCount].Indices.push_back((WORD)c);
          matSplit[splitCount].Indices.push_back((WORD)d);
          if (j != meshes[i]->numQuads - 1)
          {
            matSplit[splitCount].Indices.push_back((WORD)d);
            matSplit[splitCount].Indices.push_back(0);
            matSplit[splitCount].Indices.push_back(0);
            matSplit[splitCount].Indices.push_back(0);
          }
        }
        matSplit[splitCount].numIndices = matSplit[splitCount].Indices.size();

        if ((GetBit(meshes[i]->tristrip[j]->flags, 7) && GetBit(meshes[i]->tristrip[j]->flags, 6)) || (!GetBit(meshes[i]->tristrip[j]->flags, 7) && GetBit(meshes[i]->tristrip[j]->flags, 6)))
        {
          mesh->SetFlag(MeshFlags::isTransparent, true);

          if (tmpMat)
          {
            tmpMat->transparent = true;
            tmpMat->drawOrder = 1501.0f;
          }
        }



      }
      else
      {
        DWORD a = (DWORD)meshes[i]->tristrip[j]->a;
        DWORD b = (DWORD)meshes[i]->tristrip[j]->b;
        DWORD c = (DWORD)meshes[i]->tristrip[j]->c;
        DWORD d = (DWORD)meshes[i]->tristrip[j]->d;
        faceInfo.flags = 0x0501;

        if (GetBit(meshes[i]->tristrip[j]->collFlags, 8))
        {

          if (!GetBit(meshes[i]->tristrip[j]->collFlags, 7))
          {
            faceInfo.flags |= 0x0005;
          }
          else
          {
            faceInfo.flags |= 0x0003;
          }
        }
        if (GetBit(meshes[i]->tristrip[j]->collFlags, 5))
        {
          faceInfo.flags |= 0x0009;
          faceInfo.flags &= ~0x0002;
        }
        else if (GetBit(meshes[i]->tristrip[j]->collFlags, 6) && !GetBit(meshes[i]->tristrip[j]->collFlags, 8))
        {
          faceInfo.flags |= 0x0018;
          faceInfo.flags &= ~0x0001;
          faceInfo.flags &= ~0x0002;
        }
        else if (GetBit(meshes[i]->tristrip[j]->collFlags, 1))
        {
          faceInfo.flags &= ~0x0001;
          faceInfo.flags |= 0x0050;
        }
        else if (GetBit(meshes[i]->tristrip[j]->collFlags, 0) && !GetBit(meshes[i]->tristrip[j]->collFlags, 4) && !GetBit(meshes[i]->tristrip[j]->collFlags, 5))
        {
          faceInfo.flags = 0x0510;
        }
        if (GetBit(meshes[i]->tristrip[j]->collFlags, 3))
        {
          faceInfo.flags |= 0x0040;
        }
        extraFaces.push_back((Face(&SimpleFace(b, a, c), &faceInfo)));

        if (!GetBit(meshes[i]->tristrip[j]->flag, 4))
        {
          extraFaces.push_back((Face(&SimpleFace(b, c, d), &faceInfo)));
        }
      }
      ptr += 16;
      if ((GetBit(meshes[i]->tristrip[j]->flag, 0) || GetBit(meshes[i]->tristrip[j]->flag, 3) || GetBit(meshes[i]->tristrip[j]->flag, 1)) && meshes[i]->tristrip[j]->flag != 128 && meshes[i]->tristrip[j]->flag != 0x90 && meshes[i]->tristrip[j]->flag != 0 && meshes[i]->tristrip[j]->flag != 0x10 && (meshes[i]->tristrip[j]->flags & 0xFF0000) != 0x100000)//if(GetBit(meshes[i]->tristrip[j]->flag, 0))//if(meshes[i]->tristrip[j]->flag != 128 && meshes[i]->tristrip[j]->flag != 0x90 && meshes[i]->tristrip[j]->flag != 0 && meshes[i]->tristrip[j]->flag != 0x10 && (meshes[i]->tristrip[j]->flags & 0xFF0000) != 0x100000)
      {
        ptr += 4;
        if (GetBit(meshes[i]->tristrip[j]->flags, 0))
          ptr += 8;
        if (GetBit(meshes[i]->tristrip[j]->flags, 1))
          ptr += 8;
        if (GetBit(meshes[i]->tristrip[j]->flags, 3))
          ptr += 8;
        if (GetBit(meshes[i]->tristrip[j]->flags, 5))
          ptr += 4;
      }
    }

    for (DWORD k = 0; k < numObjects; k++)
    {
      if (meshIndices[k] == i)
      {
        if (ddm != NULL)
        {
          if (vertices.size())
          {
            DWORD numVertices = vertices.size();
            mesh->Reserve(numVertices);
            mesh->AddVertices(positions[k], &vertices.front(), numVertices, colours, texCoords);
            Collision* col = mesh->GetCollision();
            if (col && col->GetNumFaces())
              col->CopyVertices(mesh->GetVertices(), numVertices, &mesh->GetBBoxX());
            mesh->ClearVertices();
          }
          //mesh->Clear(4);
          Vertex bboxMin, bboxMax;
          Vertex vertex;

          for (DWORD j = 0; j < ddm->vertices.size(); j++)
          {
            vertex = ddm->vertices[j];
            vertex.x = -(vertex.x + ((float)positions[k]->x / 4096.0f)) / 2.833f;
            vertex.y = -(vertex.y + ((float)positions[k]->y / 4096.0f)) / 2.833f;
            vertex.z = -(vertex.z + ((float)positions[k]->z / 4096.0f)) / 2.833f;

            vertex.colour.r = (float)(vertex.colour.r)*0.65f;
            vertex.colour.g = (float)(vertex.colour.g)*0.65f;
            vertex.colour.b = (float)(vertex.colour.b)*0.65f;
            //}

            /*vertex.tUV[0].u = ddm->vertices[j].u;
            vertex.tUV[0].v = ddm->vertices[j].v;*/
            if (j == 0)
            {
              bboxMin = vertex;
              bboxMax = vertex;
            }
            else
            {
              if (vertex.x < bboxMin.x)
                bboxMin.x = vertex.x;
              if (vertex.y < bboxMin.y)
                bboxMin.y = vertex.y;
              if (vertex.z < bboxMin.z)
                bboxMin.z = vertex.z;

              if (vertex.x > bboxMax.x)
                bboxMax.x = vertex.x;
              if (vertex.y > bboxMax.y)
                bboxMax.y = vertex.y;
              if (vertex.z > bboxMax.z)
                bboxMax.z = vertex.z;
            }
            mesh->AddVertexSilent(vertex);
          }

          mesh->SetBBox(bboxMin, bboxMax);
          break;
        }
        else
        {
          DWORD numVertices = vertices.size();
          mesh->Reserve(numVertices);
          mesh->AddVertices(positions[k], &vertices.front(), numVertices, colours, texCoords);
          break;
        }
      }
    }
    if (mesh->GetNumVertices() == 0)
    {
      if (ddm != NULL)
      {
        if (vertices.size())
        {
          DWORD numVertices = vertices.size();
          mesh->Reserve(numVertices);
          mesh->AddVertices(positions[numObjects], &vertices.front(), numVertices, colours, texCoords);
          Collision* col = mesh->GetCollision();
          if (col && col->GetNumFaces())
            col->CopyVertices(mesh->GetVertices(), numVertices, &mesh->GetBBoxX());
          mesh->ClearVertices();
        }
        //mesh->Clear(4);
        Vertex bboxMin, bboxMax;
        Vertex vertex;

        for (DWORD j = 0; j < ddm->vertices.size(); j++)
        {
          vertex = ddm->vertices[j];
          vertex.x = -(vertex.x + ((float)positions[numObjects]->x / 4096.0f)) / 2.833f;
          vertex.y = -(vertex.y + ((float)positions[numObjects]->y / 4096.0f)) / 2.833f;
          vertex.z = -(vertex.z + ((float)positions[numObjects]->z / 4096.0f)) / 2.833f;

          vertex.colour.r = (float)(vertex.colour.r)*0.65f;
          vertex.colour.g = (float)(vertex.colour.g)*0.65f;
          vertex.colour.b = (float)(vertex.colour.b)*0.65f;
          //}

          /*vertex.tUV[0].u = ddm->vertices[j].u;
          vertex.tUV[0].v = ddm->vertices[j].v;*/
          if (j == 0)
          {
            bboxMin = vertex;
            bboxMax = vertex;
          }
          else
          {
            if (vertex.x < bboxMin.x)
              bboxMin.x = vertex.x;
            if (vertex.y < bboxMin.y)
              bboxMin.y = vertex.y;
            if (vertex.z < bboxMin.z)
              bboxMin.z = vertex.z;

            if (vertex.x > bboxMax.x)
              bboxMax.x = vertex.x;
            if (vertex.y > bboxMax.y)
              bboxMax.y = vertex.y;
            if (vertex.z > bboxMax.z)
              bboxMax.z = vertex.z;
          }
          mesh->AddVertexSilent(vertex);
        }
        mesh->SetBBox(bboxMin, bboxMax);
      }
      else
      {
        DWORD numVertices = vertices.size();
        mesh->Reserve(numVertices);
        mesh->AddVertices(positions[numObjects], &vertices.front(), numVertices, colours, texCoords);
      }
    }
    if (ddm == NULL)
    {
      if (first)
        mesh->SetFlag(MeshFlags::isVisible, false);
      else
        splitCount++;

      for (DWORD j = 0; j < splitCount; j++)
      {
        mesh->AddMatSplit(&matSplit[j], &matList, true);
      }
    }
    /*else
    {
    FILE* f = fopen("test.obj", "rt+");
    fseek(f, 0, SEEK_END);

    fprintf(f, "g %s\n", mesh->GetName().GetString2());
    fprintf(f, "o %s\n", mesh->GetName().GetString2());

    const Vertex* vertices2 = mesh->GetVertices();
    DWORD numVertices = mesh->GetNumVertices();

    for (DWORD j = 0; j < numVertices; j++)
    {
    fprintf(f, "v %.5f %.5f %.5f\n", vertices2[j].x, vertices2[j].y, vertices2[j].z*-1);
    fprintf(f, "vt %.5f %.5f\n", vertices2[j].tUV[0].u, vertices2[j].tUV[0].v);
    }

    const MaterialSplit* matSplits2 = &mesh->matSplits.front();



    for (DWORD j = 0, numSplits = mesh->GetNumSplits(); j < numSplits; j++)
    {
    Material* mat = matList.GetMaterial(matSplits2[j].matId);
    if (mat)
    {
    DWORD id = mat->GetMatId();
    fprintf(f, "usemtl %s\n", (*(Checksum*)&id).GetString2());
    }
    else
    fprintf(f, "usemtl Material%d\n", matSplits2[j].matId);
    WORD v1 = 0, v2 = 0, v3 = 0;
    bool tmp = false;
    for (DWORD k = 0, numIndices = matSplits2[j].numIndices; k < numIndices; k++)
    {
    if (!tmp)
    {
    v1 = matSplits2[j].Indices[k];
    v2 = matSplits2[j].Indices[k + 1];
    v3 = matSplits2[j].Indices[k + 2];
    tmp = true;
    }
    else
    {
    v1 = v3;
    v3 = matSplits2[j].Indices[k + 2];
    tmp = false;
    }
    if (v1 != v2 && v1 != v3 && v2 != v3)
    {
    fprintf(f, " %u %u %u\n", v1 + vcount, v2 + vcount, v3 + vcount);
    }
    }
    }

    vcount += numVertices;

    fclose(f);
    }*/

    for (DWORD j = 0, numExtra = extraFaces.size(); j < numExtra; j++)
      mesh->AddFace(extraFaces[j]);
    delete[] matSplit;
    delete[] colours;
    delete[] texCoords;
    delete[] meshes[i]->tristrip;
    extraFaces.clear();
    if (mesh)
    {
      if (mesh->GetNumVertices())
      {
        Collision* pCollision = mesh->GetCollision();
        /*if (pCollision->GetNumVertices())
        mesh->Clear(CLEARFLAGS_CLEARCOLLISION);*/
        /*mesh->CopyVertsToColl();
        if (pCollision)
        pCollision->BlendFaceTypes();*/
      }
    }
    AddMesh(mesh);
    Mesh* grassMesh = NULL;
    if (ddm && ddm->grass)
    {

      Texture texture;
      ZeroMemory(&texture, sizeof(Texture));
      if (sky)
      {
        texture.vAddress = D3DTADDRESS_WRAP;
        texture.uAddress = D3DTADDRESS_WRAP;
      }
      else
      {
        texture.vAddress = D3DTADDRESS_WRAP;
        texture.uAddress = D3DTADDRESS_WRAP;
      }
      texture.blendMode = 20;

      char material_name[64];
      sprintf(material_name, "Grass");
      Material* parent = NULL;
      MaterialSplit matSplit;

      for (DWORD j = 0; j < ddm->splits.size(); j++)
      {
        parent = matList.GetMaterial(ddm->splits[j].matId);
        if (strstr(parent->GetTexture(0)->GetName(), "Grass_") || strstr(parent->GetTexture(0)->GetName(), "grass_"))
        {
          matSplit = ddm->splits[j];
          break;
        }
      }

      if (!strstr(parent->GetTexture(0)->GetName(), "Grass") && !strstr(parent->GetTexture(0)->GetName(), "grass"))
        matSplit = ddm->splits[0];//MessageBox(0, material_name, parent->GetTexture(0)->GetName(), 0);
      /*else
      {*/
      parent = matList.GetMaterial(matSplit.matId);
      float drawOrder = parent->drawOrder;


      grassMesh = new Mesh();// (*mesh);
      char name[128];
      sprintf(name, "3DGrassMesh%u", numGrassMeshes);
      numGrassMeshes++;
      grassMesh->SetName(name);
      grassMesh->SetFlags(mesh->GetFlags());
      grassMesh->GenerateSimpleCollision(mesh);
      
      /*grassMesh->Resize(mesh->GetNumVertices());
      std::copy(mesh->Begin(), mesh->End(), grassMesh->Begin());*/
      //grassMesh->Clear(CLEARFLAGS_KEEPVERTICES);

      DWORD grassLayers = 16;
      float grassHeight = 8.0f;

      float heightOffset = grassHeight / grassLayers;

      for (DWORD layer = 0; layer < grassLayers; layer++)
      {
        drawOrder++;
        DWORD vertCount = grassMesh->GetNumVertices();
        MaterialSplit newSplit = matSplit;

        switch (ddm->grass)
        {
        case 2:
          sprintf(material_name, "grassa%02u", layer);
          break;
        case 4:
          sprintf(material_name, "grassb%02u", layer);
          break;
        case 6:
          sprintf(material_name, "grassc%02u", layer);
          break;
        case 8:
          sprintf(material_name, "grassd%02u", layer);
          break;
        case 10:
          sprintf(material_name, "grasse%02u", layer);
          break;
        case 12:
          sprintf(material_name, "grassf%02u", layer);
          break;
        case 14:
          sprintf(material_name, "grassg%02u", layer);
          break;
        default:
          __assume(0);
          sprintf(material_name, "Unknown grasstype:%u", ddm->grass);
          MessageBox(0, material_name, material_name, 0);
          break;
        }

        Checksum chc = Checksum(material_name);
        Material* mat = matList.GetMaterial(chc);
        if (mat == NULL)
        {
          Material material = Material();
          material.SetMatId(chc.checksum);
          material.Reserve(1);
          material.drawOrder = drawOrder;

          material.doubled = parent->doubled;

          texture.SetId(chc.checksum);
          texture.CreateTexture();

          material.transparent = true;

          material.AddTexture2(&texture);
          AddMaterial(&material);
          mat = matList.GetMaterial(chc);
        }

        
        mat->drawOrder = 18010.0f + (float)layer;

        WORD* alreadyExported = new WORD[ddm->GetNumIndices()];
        memset(alreadyExported, 0xFF, 2 * ddm->GetNumIndices());
        DWORD* newIndx = new DWORD[ddm->GetNumIndices()];
        DWORD idx = 0;
        for (DWORD j = 0; j < ddm->splits.size(); j++)
        {
          newSplit = ddm->splits[j];
          newSplit.matId = chc.checksum;
          //WORD* alreadyExported = new WORD[ddm->splits[j].Indices.size()];
          //DWORD* newIndx = new DWORD[ddm->splits[j].Indices.size()];
          for (DWORD k = 0; k < ddm->splits[j].Indices.size(); k++)
          {
            //newSplit.Indices[i]+=vertCount;
            bool exported = false;
            for (DWORD l = 0; l < ddm->GetNumIndices(); l++)
            {
              if (alreadyExported[l] == ddm->splits[j].Indices[k])
              {
                alreadyExported[l] = 0xFFFF;
                newSplit.Indices[k] = newIndx[l];
                exported = true;
                continue;
              }
            }
            //__assume(exported == false);
            if (exported==false)
            {
              newIndx[idx] = grassMesh->GetNumVertices();
              newSplit.Indices[k] = newIndx[idx];
              alreadyExported[idx] = ddm->splits[j].Indices[k];
              idx++;
              Vertex v = mesh->GetVertex(ddm->splits[j].Indices[k]);
              v.y += (heightOffset*(float)(layer + 1));//+5.0f);
              v.tUV[0].u = v.x / 48.0f;
              v.tUV[0].v = v.z / 48.0f;
              grassMesh->AddVertex(&v);
            }

          }
          //delete[] newIndx;
          //delete[] alreadyExported;
          grassMesh->AddMatSplit(&newSplit, &matList);
          grassMesh->GetSplit(grassMesh->GetNumSplits() - 1)->numIndices = grassMesh->GetSplit(grassMesh->GetNumSplits() - 1)->Indices.size() - 2;
        }
        delete[] newIndx;
        delete[] alreadyExported;
      }
      //}
      if (grassMesh)
      {
        Collision* col = grassMesh->GetCollision();
        for (DWORD idx = 0; idx < grassMesh->GetNumVertices(); idx++)
        {
          bool found = false;
          for (DWORD j = 0; j < grassMesh->GetNumSplits(); j++)
          {
            MaterialSplit* split = grassMesh->GetSplit(j);
            if (split)
            {
              for (DWORD k = 0; k < split->Indices.size(); k++)
              {
                if (split->Indices[k] == idx)
                {
                  found = true;
                  break;
                }
              }
            }
          }
          if (!found)
          {
            grassMesh->DeleteVertex(idx);
            idx--;
          }
        }
        col->CopyVertices(grassMesh->GetVertices(), grassMesh->GetNumVertices(), &grassMesh->GetBBoxX());
        /*WORD* alreadyExported = new WORD[ddm->GetNumIndices()];
        memset(alreadyExported, 0xFF, 2 * ddm->GetNumIndices());
        DWORD* newIndx = new DWORD[ddm->GetNumIndices()];
        DWORD idx = 0;
        for (DWORD j = 0; j < ddm->splits.size(); j++)
        {
          for (DWORD k = 0; k < ddm->splits[j].Indices.size(); k++)
          {
            for (DWORD l = 0; l < ddm->GetNumIndices(); l++)
            {
              if (alreadyExported[l] == ddm->splits[j].Indices[k])
              {
                alreadyExported[l] = 0xFFFF;
                grassMesh->DeleteVertex(WORD(ddm->splits[j].Indices[k]), (WORD)newIndx[l]);
                continue;
              }
            }
            newIndx[idx] = ddm->splits[j].Indices[k];
            alreadyExported[idx] = ddm->splits[j].Indices[k];
            idx++;
          }
        }
        delete[] newIndx;
        delete[] alreadyExported;*/
        AddMesh(grassMesh);
        grassMesh = NULL;
        delete grassMesh;
      }
    }
    //mesh->DeinitCollision();
    vertices.clear();
    delete mesh;
  }
  for (DWORD i = 0; i < ddmObj.size(); i++)
  {
    if (!ddmObj[i].imported && ddmObj[i].vertices.size())
    {
      Mesh* mesh = new Mesh();
      mesh->Clear(CLEARFLAGS_CLEARALL);
      mesh->SetName(ddmObj[i].checksum);
      for (DWORD j = 0; j < this->meshes.size(); j++)
      {
        if (this->meshes[j].GetName().checksum == ddmObj[i].checksum)
        {
          EraseMesh(j);
          j--;
        }
      }
      for (DWORD j = 0; j < ddmObj[i].splits.size(); j++)
        mesh->AddMatSplit(&ddmObj[i].splits[j], &matList);

      for (DWORD k = 0; k < numObjects; k++)
      {
        if (meshIndices[k] == ddmObj[i].index)
        {
          //mesh->Clear(4);
          Vertex bboxMin, bboxMax;
          Vertex vertex;
          for (DWORD j = 0; j < ddmObj[i].vertices.size(); j++)
          {
            vertex = ddmObj[i].vertices[j];
            vertex.x = -(vertex.x + ((float)positions[k]->x / 4096.0f)) / 2.833f;
            vertex.y = -(vertex.y + ((float)positions[k]->y / 4096.0f)) / 2.833f;
            vertex.z = -(vertex.z + ((float)positions[k]->z / 4096.0f)) / 2.833f;

            vertex.colour.r = (float)(vertex.colour.r)*0.65f;
            vertex.colour.g = (float)(vertex.colour.g)*0.65f;
            vertex.colour.b = (float)(vertex.colour.b)*0.65f;

            if (j == 0)
            {
              bboxMin = vertex;
              bboxMax = vertex;
            }
            else
            {
              if (vertex.x < bboxMin.x)
                bboxMin.x = vertex.x;
              if (vertex.y < bboxMin.y)
                bboxMin.y = vertex.y;
              if (vertex.z < bboxMin.z)
                bboxMin.z = vertex.z;

              if (vertex.x > bboxMax.x)
                bboxMax.x = vertex.x;
              if (vertex.y > bboxMax.y)
                bboxMax.y = vertex.y;
              if (vertex.z > bboxMax.z)
                bboxMax.z = vertex.z;
            }
            mesh->AddVertexSilent(vertex);
          }
          mesh->SetBBox(bboxMin, bboxMax);
          break;
        }
      }
      if (mesh->GetNumVertices() == 0)
      {
        //mesh->Clear(4);
        Vertex bboxMin, bboxMax;
        Vertex vertex;
        for (DWORD j = 0; j < ddmObj[i].vertices.size(); j++)
        {
          vertex = ddmObj[i].vertices[j];
          vertex.x = -(vertex.x + ((float)positions[numObjects]->x / 4096.0f)) / 2.833f;
          vertex.y = -(vertex.y + ((float)positions[numObjects]->y / 4096.0f)) / 2.833f;
          vertex.z = -(vertex.z + ((float)positions[numObjects]->z / 4096.0f)) / 2.833f;

          vertex.colour.r = (float)(vertex.colour.r)*0.65f;
          vertex.colour.g = (float)(vertex.colour.g)*0.65f;
          vertex.colour.b = (float)(vertex.colour.b)*0.65f;


          if (j == 0)
          {
            bboxMin = vertex;
            bboxMax = vertex;
          }
          else
          {
            if (vertex.x < bboxMin.x)
              bboxMin.x = vertex.x;
            if (vertex.y < bboxMin.y)
              bboxMin.y = vertex.y;
            if (vertex.z < bboxMin.z)
              bboxMin.z = vertex.z;

            if (vertex.x > bboxMax.x)
              bboxMax.x = vertex.x;
            if (vertex.y > bboxMax.y)
              bboxMax.y = vertex.y;
            if (vertex.z > bboxMax.z)
              bboxMax.z = vertex.z;
          }
          mesh->AddVertexSilent(vertex);
        }
        mesh->SetBBox(bboxMin, bboxMax);
      }

      AddMesh(mesh);
      delete mesh;
    }
  }
  delete[] positions;
  delete[] meshIndices;
  //delete [] meshFlags;
  delete[] meshes;
  delete[] meshNames;
}

void Th2xScene::ProcessMaterials(DWORD ptr, DWORD numMeshes, DWORD* meshNames, bool sky)
{
  /*ptr+=4;
  DWORD numRGBs = ((DWORD*)ptr)[0];
  ptr+=4;
  ptr+= numRGBs;*/

  DWORD tmpByte = ((DWORD*)ptr)[0];
  while (tmpByte == 6 || tmpByte == 7 || tmpByte == 10)
  {
    printf("tmpByte1 %08X\n", tmpByte);
    ptr += 4;
    tmpByte = ((DWORD*)ptr)[0];
    printf("tmpByte2 %08X\n", tmpByte);
    ptr += 4;
    ptr += tmpByte;
    tmpByte = ((DWORD*)ptr)[0];
    printf("tmpByte3 %08X\n", tmpByte);
  }

  /*while(((DWORD*)ptr)[0]==0xFFFFFFFF)
  {*/
  printf("tmpByteCore %08X\n", tmpByte);
  if (tmpByte == 0xFFFFFFFF)
    ptr += 4;
  //}
  for (DWORD i = 0; i < numMeshes; i++)
  {
    meshNames[i] = ((DWORD*)ptr)[0];
    ptr += 4;
  }
  for (DWORD i = 0; i < numMeshes; i++)
  {
    DWORD numCopies = 0;
    for (DWORD j = i + 1; j < numMeshes; j++)
    {
      if (meshNames[i] == meshNames[j])
      {
        char temp[256] = "";
        sprintf(temp, "%s_%u", (*(Checksum*)&meshNames[i]).GetString2(), numCopies);
        meshNames[j] = Checksum(temp).checksum;
        numCopies++;
      }
    }
  }
  printf("numMatPtr %08X\n", ptr);
  /*DWORD numMaterials = ((DWORD*)ptr)[0];
  printf("numMat %d\n",numMaterials);
  ptr+=4;

  //Material** materials = new Material*[numMaterials];
  Texture texture;
  ZeroMemory(&texture,sizeof(Texture));
  if(sky)
  {
  texture.vAddress = D3DTADDRESS_WRAP;
  texture.uAddress = D3DTADDRESS_WRAP;
  }
  else
  {
  texture.vAddress = D3DTADDRESS_WRAP;
  texture.uAddress = D3DTADDRESS_WRAP;
  }
  texture.blendMode = 20;
  for(DWORD i=0; i<numMaterials; i++)
  {
  Material* material = new Material();
  //materials[i] = (Material*)malloc(sizeof(Material));
  //materials[i]->SetMatId(((DWORD*)ptr)[0]);
  //material->drawOrder = (float)i;
  material->drawOrder = 0.0f;
  material->transparent[0] = false;
  material->SetMatId(((DWORD*)ptr)[0]);
  /*Texture* texture = new Texture();
  ZeroMemory(texture,sizeof(Texture));
  texture->vAddress = D3DTADDRESS_WRAP;
  texture->uAddress = D3DTADDRESS_WRAP;
  texture->blendMode = 20;*/

  /*texture.SetId(((DWORD*)ptr)[0]);
  ptr+=4;
  texture.CreateTexture();
  //texture->CreateTexture();
  if(texture.IsTransparent())
  material->transparent[0]=true;
  else
  material->transparent[0]=false;
  material->AddTexture(&texture);
  AddMaterial(material);
  //free(materials[i]);
  delete material;
  }^*/
}

static Colour LevelData[167772160];

void Th2xScene::ProcessXBOXTextures(DWORD ptr, bool sky)
{
  bool merging = false;
  DWORD pFile = ptr;
  ptr += 8;
  DWORD pImage = pFile + ((DWORD*)ptr)[0];
  ptr += 4;
  DWORD numTextures = ((DWORD*)ptr)[0];
  if (!texFiles.numFiles)
  {
    texFiles.numFiles = numTextures + 6;
    texFiles.fileNames = new char*[numTextures + 6];
    static char imageName[MAX_PATH] = "";
    GetEnvironmentVariable("TEMP", imageName, MAX_PATH);
    DWORD pName = strlen(imageName);
    imageName[pName] = '\\';
    pName++;
    imageName[pName] = 'D';
    pName++;
    imageName[pName] = '_';
    pName++;
    imageName[pName] = 'l';
    pName++;
    imageName[pName] = 'o';
    pName++;
    imageName[pName] = 'g';
    pName++;
    imageName[pName] = 'o';
    pName++;
    imageName[pName] = '0';
    imageName[pName + 1] = '1';
    imageName[pName + 2] = '.';
    imageName[pName + 3] = 't';
    imageName[pName + 4] = 'g';
    imageName[pName + 5] = 'a';
    texFiles.fileNames[numTextures] = new char[strlen(imageName) + 1];
    memcpy(texFiles.fileNames[numTextures], imageName, strlen(imageName) + 1);
    imageName[pName] = '1';
    texFiles.fileNames[numTextures + 1] = new char[strlen(imageName) + 1];
    memcpy(texFiles.fileNames[numTextures + 1], imageName, strlen(imageName) + 1);
    imageName[pName] = '2';
    texFiles.fileNames[numTextures + 2] = new char[strlen(imageName) + 1];
    memcpy(texFiles.fileNames[numTextures + 2], imageName, strlen(imageName) + 1);
    imageName[pName + 1] = '2';
    imageName[pName] = '0';
    texFiles.fileNames[numTextures + 3] = new char[strlen(imageName) + 1];
    memcpy(texFiles.fileNames[numTextures + 3], imageName, strlen(imageName) + 1);
    imageName[pName] = '1';
    texFiles.fileNames[numTextures + 4] = new char[strlen(imageName) + 1];
    memcpy(texFiles.fileNames[numTextures + 4], imageName, strlen(imageName) + 1);
    imageName[pName] = '2';
    texFiles.fileNames[numTextures + 5] = new char[strlen(imageName) + 1];
    memcpy(texFiles.fileNames[numTextures + 5], imageName, strlen(imageName) + 1);
  }
  else
  {
    merging = true;
    char** temp = (char**)realloc(texFiles.fileNames, (texFiles.numFiles + numTextures) * 4);
    if (temp)
      texFiles.fileNames = temp;
    else
      return;
  }
  ptr += 4;
  static char imageName[MAX_PATH] = "";
  GetEnvironmentVariable("TEMP", imageName, MAX_PATH);
  DWORD pName = strlen(imageName);
  imageName[pName] = '\\';
  pName++;


  TGAHEADER tgaHeader = { 0 };
  tgaHeader.bitsPerPixel = 32;
  tgaHeader.imageDescriptor = 32;
  tgaHeader.dataTypeCode = 2;
  unsigned char tgaFooterExt[8] = { 0 };
  char tgaFooterTag[17] = "TRUEVISION-XFILE";

  HRSRC resource = FindResource(NULL, MAKEINTRESOURCE(2053), RT_RCDATA);
  if (resource != NULL)
  {
    HGLOBAL resourceData = LoadResource(NULL, resource);
    if (resourceData != NULL)
    {
      void* pZip = LockResource(resourceData);
      if (pZip != NULL)
      {
        HZIP hz = OpenZip(pZip, SizeofResource(NULL, resource), 0);

        ZIPENTRY ze;
        if (GetZipItem(hz, -1, &ze) != ZR_OK)
        {
          MessageBox(0, "Empty?!", "", 0);
          CloseZip(hz);
          return;
        }
        const DWORD numItems = ze.index - 1;

        for (DWORD i = 0; i < numItems; i++)
        {
          BYTE* pFile = new BYTE[2600000];

          if (pFile != NULL)
          {

            if (GetZipItem(hz, i, &ze) != ZR_OK)
            {
              MessageBox(0, "trashed zip", "", 0);
              CloseZip(hz);
              return;
            }

            if (ze.attr & FILE_ATTRIBUTE_DIRECTORY)
            {
              CreateDirectory(ze.name, NULL);
            }
            else
            {
              UnzipItem(hz, i, pFile, 2600000);
              DWORD p = (DWORD)pFile;

              char* name = ze.name;
              DWORD len = strlen(name);
              /*name[len - 1] = 'a';
              name[len - 2] = 'g';
              name[len - 3] = 't';*/
              strcpy(&imageName[pName], name);
              if (!merging)
                  texFiles.fileNames[i] = new char[strlen(imageName) + 1];
              else
                  texFiles.fileNames[i + texFiles.numFiles] = new char[strlen(imageName) + 1];

              if (!merging)
                  memcpy(texFiles.fileNames[i], imageName, strlen(imageName) + 1);
              else
                  memcpy(texFiles.fileNames[i + texFiles.numFiles], imageName, strlen(imageName) + 1);

              FILE* f = fopen(imageName, "wb");
              fwrite(pFile, ze.unc_size, 1, f);
              
              fclose(f);//
            }

          }
          delete[] pFile;
        }
        UnlockResource(pZip);
        CloseZip(hz);
      }

    }
  }

  for (DWORD i = 0; i < numTextures; i++)
  {
    DWORD pImageData = pImage + ((DWORD*)ptr)[0];
    BYTE* imageData = (BYTE*)(pImageData);
    pImageData += sizeof(DDS_HEADER);
    ptr += 4;
    DWORD size = ((DWORD*)ptr)[0];
    ptr += 4;
    char* name = (char*)ptr;
    DWORD len = strlen(name);
    /*if(!merging && len && strstr(name, "D_logo"))
    {
    texFiles.fileNames[i] = NULL;
    continue;
    }*/
    name[len - 1] = 'a';
    name[len - 2] = 'g';
    name[len - 3] = 't';
    strcpy(&imageName[pName], name);
    if (!merging)
      texFiles.fileNames[i] = new char[strlen(imageName) + 1];
    else
      texFiles.fileNames[i + texFiles.numFiles] = new char[strlen(imageName) + 1];

    if (!merging)
      memcpy(texFiles.fileNames[i], imageName, strlen(imageName) + 1);
    else
      memcpy(texFiles.fileNames[i + texFiles.numFiles], imageName, strlen(imageName) + 1);
    ptr += 256;

    DDS_HEADER* header = (DDS_HEADER*)imageData;

    tgaHeader.width = header->width;
    tgaHeader.height = header->height;

    FILE*f = fopen(imageName, "wb");
    fwrite(&tgaHeader, sizeof(TGAHEADER), 1, f);
    DWORD imageSize = header->width*header->height * 4;//*/

    if (header->pixelFormat[1] & 0x4)
    {
      /*FILE*f = fopen(imageName,"wb");
      fwrite(&tgaHeader,sizeof(TGAHEADER),1,f);*/
      DWORD imageSize = header->width*header->height * 4;
      //BYTE* levelData = new BYTE[imageSize];
      DecompressDXT((BYTE*)pImageData, header->imageSize, header->width, header->height, LevelData, (((header->pixelFormat[2] & 0xFF000000) >> 24) - 0x30));
      /*if (sky)
      {
        for (DWORD j = 0; j < header->width*header->height; j++)
        {
          if (LevelData[j].a == 0xFF)
            LevelData[j].a = 0xFE;
        }
      }*/
      fwrite(LevelData, imageSize, 1, f);
      /*fwrite(&tgaFooterExt,sizeof(tgaFooterExt),1,f);
      fwrite(&tgaFooterTag,sizeof(tgaFooterTag),1,f);
      fputc(0x2E, f);
      fputc(0x00, f);
      fclose(f);*/
      //delete [] levelData;
    }
    else
    {
      DWORD bytesPerPixel = header->imageSize / header->width / header->height;
      bool usingAlpha = header->pixelFormat[1] & 0x2;

      if (bytesPerPixel == 4)
      {
        //MessageBox(0,"sure this is 32 bit?",imageName,0);
        fwrite((void*)pImageData, imageSize, 1, f);
      }
      else if (bytesPerPixel == 2)
      {
        DWORD numPixels = imageSize / 4;
        //Colour* levelData = new Colour[numPixels];
        //MessageBox(0,"sure this is 16 bit?",imageName,0);
        if (usingAlpha)
        {
          for (DWORD j = 0; j < numPixels; j++)
          {
            BYTE r = ((BYTE)((((WORD*)pImageData)[j] & 0x003E) >> 1));
            BYTE g = ((BYTE)((((WORD*)pImageData)[j] & 0x7C0) >> 6));
            BYTE b = ((BYTE)((((WORD*)pImageData)[j] & 0xF800) >> 12));
            BYTE a = ((BYTE)((((WORD*)pImageData)[j] & 0x1)));
            LevelData[j].r = (r << 3) | (r >> 2);
            LevelData[j].g = (g << 3) | (g >> 2);
            LevelData[j].b = (b << 3) | (b >> 2);
            /*if (a && sky)
              LevelData[j].a = 254;
            else */if (a)
              LevelData[j].a = 255;
            else
              LevelData[j].a = 0;
          }
        }
        else
        {
          for (DWORD j = 0; j < numPixels; j++)
          {
            BYTE r = (BYTE)(((WORD*)pImageData)[j] & 0x001F);
            BYTE g = ((BYTE)((((WORD*)pImageData)[j] & 0x7E0) >> 5));
            BYTE b = ((BYTE)((((WORD*)pImageData)[j] & 0xF800) >> 11));
            LevelData[j].r = (r << 3) | (r >> 2);
            LevelData[j].g = (g << 2) | (g >> 4);
            LevelData[j].b = (b << 3) | (b >> 2);
            /*if (sky)
              LevelData[j].a = 254;
            else*/
            LevelData[j].a = 255;
          }
        }
        fwrite(LevelData, imageSize, 1, f);
        //delete [] levelData;
      }
      else if (bytesPerPixel == 1)
        MessageBox(0, "1", 0, 0);
      else
        MessageBox(0, "unknown", 0, 0);

      //MessageBox(0,"wtf this texture is not compressed","wawwiee",0);
      //levelData = (pImageData;
    }//*/


    fwrite(&tgaFooterExt, sizeof(tgaFooterExt), 1, f);
    fwrite(&tgaFooterTag, sizeof(tgaFooterTag), 1, f);
    fputc(0x2E, f);
    fputc(0x00, f);
    fclose(f);//*/
    /*name[len-1] = 's';
    name[len-2] = 'd';
    name[len-3] = 'd';
    strcpy(&imageName[pName],name);
    f = fopen(imageName,"wb");
    fwrite(imageData,size,1,f);
    fclose(f);*/

  }
  if (merging)
    texFiles.numFiles += numTextures;
  else
  {
    HRSRC resource = FindResource(NULL, MAKEINTRESOURCE(2055), RT_RCDATA);
    if (resource != NULL)
    {
      HGLOBAL resourceData = LoadResource(NULL, resource);
      if (resourceData != NULL)
      {
        void* pFile = LockResource(resourceData);
        if (pFile != NULL)
        {
          FILE* f = fopen(texFiles.fileNames[texFiles.numFiles - 3], "w+b");
          if (f)
          {
            fwrite(pFile, SizeofResource(NULL, resource), 1, f);
            fclose(f);
          }
          f = fopen(texFiles.fileNames[texFiles.numFiles - 6], "w+b");
          if (f)
          {
            fwrite(pFile, SizeofResource(NULL, resource), 1, f);
            fclose(f);
          }
        }
      }
      //CloseHandle(resource);
    }
    resource = FindResource(NULL, MAKEINTRESOURCE(2056), RT_RCDATA);
    if (resource != NULL)
    {
      HGLOBAL resourceData = LoadResource(NULL, resource);
      if (resourceData != NULL)
      {
        void* pFile = LockResource(resourceData);
        if (pFile != NULL)
        {
          FILE* f = fopen(texFiles.fileNames[texFiles.numFiles - 2], "w+b");
          if (f)
          {
            fwrite(pFile, SizeofResource(NULL, resource), 1, f);
            fclose(f);
          }
          f = fopen(texFiles.fileNames[texFiles.numFiles - 5], "w+b");
          if (f)
          {
            fwrite(pFile, SizeofResource(NULL, resource), 1, f);
            fclose(f);
          }
        }
      }
      //CloseHandle(resource);
    }
    resource = FindResource(NULL, MAKEINTRESOURCE(2057), RT_RCDATA);
    if (resource != NULL)
    {
      HGLOBAL resourceData = LoadResource(NULL, resource);
      if (resourceData != NULL)
      {
        void* pFile = LockResource(resourceData);
        if (pFile != NULL)
        {
          FILE* f = fopen(texFiles.fileNames[texFiles.numFiles - 1], "w+b");
          if (f)
          {
            fwrite(pFile, SizeofResource(NULL, resource), 1, f);
            fclose(f);
          }
          f = fopen(texFiles.fileNames[texFiles.numFiles - 4], "w+b");
          if (f)
          {
            fwrite(pFile, SizeofResource(NULL, resource), 1, f);
            fclose(f);
          }
        }
      }
      //CloseHandle(resource);
    }
  }
}

void Th2xScene::ProcessTextures(DWORD ptr, bool sky)
{
  bool merging = false;
  DWORD pFile = ptr;
  ptr += 20;
  TGAHEADER tgaHeader = { 0 };
  char imageName[MAX_PATH] = "";
  char tmp[25] = "";
  tgaHeader.bitsPerPixel = 32;
  tgaHeader.imageDescriptor = 32;
  tgaHeader.dataTypeCode = 2;
  unsigned char tgaFooterExt[8] = { 0 };
  char tgaFooterTag[17] = "TRUEVISION-XFILE";
  struct Palette
  {
    DWORD checksum;
    WORD* colours;
  };

  struct Th2Texture
  {
    DWORD checksum;
    DWORD pImage;
  };

  DWORD numTextures = ((DWORD*)ptr)[0];
  ptr += 4;
  Th2Texture* textures = new Th2Texture[numTextures];

  for (DWORD i = 0; i < numTextures; i++)
  {
    textures[i].checksum = ((DWORD*)ptr)[0];
    textures[i].pImage = 0;
    ptr += 4;
  }

  DWORD numSmallPalettes = ((DWORD*)ptr)[0];
  ptr += 4;
  Palette* smallPalettes = new Palette[numSmallPalettes];

  for (DWORD i = 0; i < numSmallPalettes; i++)
  {
    smallPalettes[i].checksum = ((DWORD*)ptr)[0];
    ptr += 4;
    smallPalettes[i].colours = (WORD*)ptr;
    ptr += 32;
  }

  DWORD numBigPalettes = ((DWORD*)ptr)[0];
  ptr += 4;
  Palette* bigPalettes = new Palette[numBigPalettes];
  for (DWORD i = 0; i < numBigPalettes; i++)
  {
    bigPalettes[i].checksum = ((DWORD*)ptr)[0];
    ptr += 4;
    bigPalettes[i].colours = (WORD*)ptr;
    ptr += 512;
  }

  DWORD numImages = ((DWORD*)ptr)[0];
  ptr += 4;
  if (!sky)
  {
    if (!texFiles.numFiles)
    {
      texFiles.numFiles = numImages;
      texFiles.fileNames = new char*[numImages];
    }
    else
    {
      merging = true;
      char** temp = (char**)realloc(texFiles.fileNames, (texFiles.numFiles + numImages) * 4);
      if (temp)
        texFiles.fileNames = temp;
      else
        return;
    }
  }
  for (DWORD i = 0; i < numImages; i++)
  {
    DWORD pImage = ((DWORD*)ptr)[0] + pFile;
    ptr += 4;

    DWORD imageSize = ((DWORD*)pImage)[1];
    DWORD paletteCRC = ((DWORD*)pImage)[2];
    DWORD index = ((DWORD*)pImage)[3];
    textures[index].pImage = pImage;
    DWORD paletteIndex = 0;
    BYTE paletteType = 0;
    for (DWORD j = 0; j < numSmallPalettes; j++)
    {
      if (paletteCRC == smallPalettes[j].checksum)
      {
        paletteType = 0;
        paletteIndex = j;
        break;
      }
    }
    for (DWORD j = 0; j < numBigPalettes; j++)
    {
      if (paletteCRC == bigPalettes[j].checksum)
      {
        paletteType = 1;
        paletteIndex = j;
        break;
      }
    }
    textures[index].pImage += 16;
    DWORD width = ((WORD*)textures[index].pImage)[0];
    DWORD height = ((WORD*)textures[index].pImage)[1];
    textures[index].pImage += 4;
    BYTE* levelData = (BYTE*)textures[index].pImage;
    DWORD dataSize = width*height;
    Colour* colourData = new Colour[width*height];
    if (imageSize == 16)
    {
      DWORD counter = 0;
      for (DWORD j = 0; j < dataSize; j += 2)
      {
        if (paletteType)
        {
          colourData[j + 1].r = (bigPalettes[paletteIndex].colours[(levelData[counter] & 0xF0) >> 4] & 0x7C00) >> 10;
          colourData[j + 1].r *= 8;
          colourData[j + 1].g = (bigPalettes[paletteIndex].colours[(levelData[counter] & 0xF0) >> 4] & 0x3E0) >> 5;
          colourData[j + 1].g *= 8;
          colourData[j + 1].b = (bigPalettes[paletteIndex].colours[(levelData[counter] & 0xF0) >> 4] & 0x1F);
          colourData[j + 1].b *= 8;
          if ((colourData[j + 1].r >= 248 && colourData[j + 1].r <= 255) && colourData[j + 1].g == 0 && (colourData[j + 1].b >= 248 && colourData[j + 1].b <= 255))//248 for truncation errors
          {
            colourData[j + 1].a = 0;
            colourData[j + 1].r = 0;
            colourData[j + 1].g = 0;
            colourData[j + 1].b = 0;
          }
          /*else if((colourData[j+1].r+colourData[j+1].g+colourData[j+1].b)<135)
          colourData[j+1].a = (colourData[j+1].r + colourData[j+1].g + colourData[j+1].b)/3;*/
          else
            colourData[j + 1].a = 255;
          colourData[j].r = (bigPalettes[paletteIndex].colours[levelData[counter] & 0x0F] & 0x7C00) >> 10;
          colourData[j].r *= 8;
          colourData[j].g = (bigPalettes[paletteIndex].colours[levelData[counter] & 0x0F] & 0x3E0) >> 5;
          colourData[j].g *= 8;
          colourData[j].b = bigPalettes[paletteIndex].colours[levelData[counter] & 0x0F] & 0x1F;
          colourData[j].b *= 8;
          if ((colourData[j].r >= 248 && colourData[j].r <= 255) && colourData[j].g == 0 && (colourData[j].b >= 248 && colourData[j].b <= 255))//248 for truncation errors
          {
            colourData[j].a = 0;
            colourData[j].r = 0;
            colourData[j].g = 0;
            colourData[j].b = 0;
          }
          /*else if((colourData[j].r+colourData[j].g+colourData[j].b)<135)
          colourData[j].a = (colourData[j].r + colourData[j].g + colourData[j].b)/3;*/
          else
            colourData[j].a = 255;
        }
        else
        {
          colourData[j + 1].r = (smallPalettes[paletteIndex].colours[(levelData[counter] & 0xF0) >> 4] & 0x7C00) >> 10;
          colourData[j + 1].r *= 8;
          colourData[j + 1].g = (smallPalettes[paletteIndex].colours[(levelData[counter] & 0xF0) >> 4] & 0x3E0) >> 5;
          colourData[j + 1].g *= 8;
          colourData[j + 1].b = smallPalettes[paletteIndex].colours[(levelData[counter] & 0xF0) >> 4] & 0x1F;
          colourData[j + 1].b *= 8;
          if ((colourData[j + 1].r >= 248 && colourData[j + 1].r <= 255) && colourData[j + 1].g == 0 && (colourData[j + 1].b >= 248 && colourData[j + 1].b <= 255))//248 for truncation errors
          {
            colourData[j + 1].a = 0;
            colourData[j + 1].r = 0;
            colourData[j + 1].g = 0;
            colourData[j + 1].b = 0;
          }
          /*else if((colourData[j+1].r+colourData[j+1].g+colourData[j+1].b)<135)
          colourData[j+1].a = (colourData[j+1].r + colourData[j+1].g + colourData[j+1].b)/3;*/
          else
            colourData[j + 1].a = 255;
          colourData[j].r = (smallPalettes[paletteIndex].colours[levelData[counter] & 0x0F] & 0x7C00) >> 10;
          colourData[j].r *= 8;
          colourData[j].g = (smallPalettes[paletteIndex].colours[levelData[counter] & 0x0F] & 0x3E0) >> 5;
          colourData[j].g *= 8;
          colourData[j].b = smallPalettes[paletteIndex].colours[levelData[counter] & 0x0F] & 0x1F;
          colourData[j].b *= 8;
          if ((colourData[j].r >= 248 && colourData[j].r <= 255) && colourData[j].g == 0 && (colourData[j].b >= 248 && colourData[j].b <= 255))//248 for truncation errors
          {
            colourData[j].a = 0;
            colourData[j].r = 0;
            colourData[j].g = 0;
            colourData[j].b = 0;
          }
          /*else if((colourData[j].r+colourData[j].g+colourData[j].b)<135)
          colourData[j].a = (colourData[j].r + colourData[j].g + colourData[j].b)/3;*/
          else
            colourData[j].a = 255;
        }
        counter++;
      }
    }
    else
    {
      for (DWORD j = 0; j < dataSize; j++)
      {
        if (paletteType)
        {
          colourData[j].r = (bigPalettes[paletteIndex].colours[levelData[j]] & 0x7C00) >> 10;
          colourData[j].r *= 8;
          colourData[j].g = (bigPalettes[paletteIndex].colours[levelData[j]] & 0x3E0) >> 5;
          colourData[j].g *= 8;
          colourData[j].b = bigPalettes[paletteIndex].colours[levelData[j]] & 0x1F;
          colourData[j].b *= 8;
          if ((colourData[j].r >= 248 && colourData[j].r <= 255) && colourData[j].g == 0 && (colourData[j].b >= 248 && colourData[j].b <= 255))//248 for truncation errors
          {
            colourData[j].a = 0;
            colourData[j].r = 0;
            colourData[j].g = 0;
            colourData[j].b = 0;
          }
          /*else if((colourData[j].r+colourData[j].g+colourData[j].b)<135)
          colourData[j].a = (colourData[j].r + colourData[j].g + colourData[j].b)/3;*/
          else
            colourData[j].a = 255;
        }
        else
        {
          colourData[j].r = (smallPalettes[paletteIndex].colours[levelData[j]] & 0x7C00) >> 10;
          colourData[j].r *= 8;
          colourData[j].g = (smallPalettes[paletteIndex].colours[levelData[j]] & 0x3E0) >> 5;
          colourData[j].g *= 8;
          colourData[j].b = smallPalettes[paletteIndex].colours[levelData[j]] & 0x1F;
          colourData[j].b *= 8;
          if ((colourData[j].r >= 248 && colourData[j].r <= 255) && colourData[j].g == 0 && (colourData[j].b >= 248 && colourData[j].b <= 255))//248 for truncation errors
          {
            colourData[j].a = 0;
            colourData[j].r = 0;
            colourData[j].g = 0;
            colourData[j].b = 0;
          }
          /*else if((colourData[j].r+colourData[j].g+colourData[j].b)<135)
          colourData[j].a = (colourData[j].r + colourData[j].g + colourData[j].b)/3;*/
          else
            colourData[j].a = 255;
        }
      }
    }
    tgaHeader.width = (short)width;
    tgaHeader.height = (short)height;
    GetEnvironmentVariable("TEMP", imageName, MAX_PATH);
    sprintf(tmp, "\\%08X.tga", textures[index].checksum);
    strcat(imageName, tmp);
    if (!sky)
    {
      if (!merging)
        texFiles.fileNames[i] = new char[strlen(imageName) + 1];
      else
        texFiles.fileNames[i + texFiles.numFiles] = new char[strlen(imageName) + 1];

      if (!merging)
        memcpy(texFiles.fileNames[i], imageName, strlen(imageName) + 1);
      else
        memcpy(texFiles.fileNames[i + texFiles.numFiles], imageName, strlen(imageName) + 1);
    }

    FILE*f = fopen(imageName, "wb");
    fwrite(&tgaHeader, sizeof(TGAHEADER), 1, f);
    fwrite(colourData, dataSize * 4, 1, f);

    fwrite(&tgaFooterExt, sizeof(tgaFooterExt), 1, f);
    fwrite(&tgaFooterTag, sizeof(tgaFooterTag), 1, f);
    fputc(0x2E, f);
    fputc(0x00, f);

    fclose(f);
    delete[] colourData;
  }
  delete[] textures;
  delete[] bigPalettes;
  delete[] smallPalettes;

  if (merging)
    texFiles.numFiles += numImages;
}

void Th2xScene::LoadDDM(DWORD ptr, bool sky)
{
  DWORD index;
  DWORD checksum;
  DWORD pFile = ptr;
  ptr += 8;

  DWORD numObjects = ((DWORD*)ptr)[0];
  SetProgressbarMaximum(numObjects * 2);
  ptr += 4;

  Texture texture;
  ZeroMemory(&texture, sizeof(Texture));
  if (sky)
  {
    texture.vAddress = D3DTADDRESS_WRAP;
    texture.uAddress = D3DTADDRESS_WRAP;
  }
  else
  {
    texture.vAddress = D3DTADDRESS_WRAP;
    texture.uAddress = D3DTADDRESS_WRAP;
  }
  texture.blendMode = 20;

  ddmObj.resize(numObjects);
  for (DWORD i = 0; i < numObjects; i++)
  {
    IncreasProgressbar();
    DWORD pObject = pFile + ((DWORD*)ptr)[0];
    ptr += 4;
    DWORD size = ((DWORD*)ptr)[0];
    ptr += 4;
    Th2xMesh* mesh = (Th2xMesh*)pObject;
    ddmObj[i].grass = (BYTE)(mesh->flags & 0xE);
    ddmObj[i].checksum = mesh->checksum;
    ddmObj[i].index = mesh->index;
    Th2xMaterial* materials = (Th2xMaterial*)(pObject + sizeof(Th2xMesh));
    XboxVertex* vertices = (XboxVertex*)(materials + mesh->numMaterials);
    WORD* indices = (WORD*)(vertices + mesh->numVertices);
    Th2xSplit* splits = (Th2xSplit*)(indices + mesh->numIndices);

    if (!(*(Checksum*)&mesh->checksum).GetString())
      Scripts.push_back((Script(mesh->checksum, mesh->name)));

    for (DWORD j = 0; j < mesh->numMaterials; j++)
    {
      char chc[256] = "";
      sprintf(chc, "%s_%s_%d", (*(Checksum*)&mesh->checksum).GetString(), materials[j].name, j);
      materials[j].id = Checksum(chc).checksum;

      //matList.GetMaterial(materials[j].id, &materials[j].id);
      if (!(*(Checksum*)&materials[j].id).GetString())
        Scripts.push_back((Script(materials[j].id, materials[j].name)));

      if (!matList.GetMaterial(Checksum(materials[j].id)))
      {
        Material material = Material();
        material.SetMatId(materials[j].id);
        material.Reserve(1);
        material.drawOrder = (float)materials[j].drawOrder;

        material.doubled = (mesh->flags & 0x20);

        texture.SetId(Checksum(materials[j].texture));
        texture.CreateTexture();
        if (texture.IsTransparent())
        {
          material.transparent = true;
          material.drawOrder += 1501.0f;
        }
        else
          material.transparent = false;
        if (materials[j].unk3 & 1)
        {
          material.alphaMap = true;
          material.reverse = true;
          material.drawOrder += 1501.0f;
        }
        material.AddTexture2(&texture);
        if (mesh->animSpeedX || mesh->animSpeedY)
        {
          material.animated = true;
          AddAnimation(materials[j].id, mesh->animSpeedX, mesh->animSpeedY);
        }
        AddMaterial(&material);
      }
    }

    ddmObj[i].vertices.resize(mesh->numVertices);
    for (DWORD j = 0; j < mesh->numVertices; j++)
    {
      //Vertex v;
      ddmObj[i].vertices[j].x = vertices[j].x;
      ddmObj[i].vertices[j].y = vertices[j].y;
      ddmObj[i].vertices[j].z = vertices[j].z;

      ddmObj[i].vertices[j].colour = (*(Colour*)&vertices[j].RGBX);
      //std::swap(ddmObj[i].vertices[j].colour.r, ddmObj[i].vertices[j].colour.b);
      ddmObj[i].vertices[j].tUV[0].u = vertices[j].u;
      ddmObj[i].vertices[j].tUV[0].v = vertices[j].v;

      //ddmObj[i].vertices.push_back(v);
    }

    ddmObj[i].splits.resize(mesh->numMaterials);
    if (ddmObj[i].splits.size() == 0)
      MessageBox(0, "01?", "", 0);
    for (DWORD j = 0; j < mesh->numMaterials; j++)
    {
      ddmObj[i].splits[j].matId = materials[splits[j].matIndex].id;
      char testmsg[256] = "";
      /*sprintf(testmsg, "id %X numindices %d index1 %d texname %s", materials[splits[j].matIndex].id, splits[j].count, indices[0 + splits[j].offset], materials[splits[j].matIndex].name);
      MessageBox(0, testmsg, testmsg, 0);*/
      if (splits[j].count < 3)
        MessageBox(0, "02?", "", 0);
      ddmObj[i].splits[j].numIndices = splits[j].count + 1;
      ddmObj[i].splits[j].Indices.reserve(ddmObj[i].splits[j].numIndices);
      ddmObj[i].splits[j].Indices.push_back(indices[splits[j].offset]);
      for (DWORD k = 0; k < splits[j].count; k++)
      {
        ddmObj[i].splits[j].Indices.push_back(indices[k + splits[j].offset]);
      }
      /*sprintf(testmsg, "index1 %d index2 %d index3 %d", ddmObj[i].splits[j].Indices[0], ddmObj[i].splits[j].Indices[1], ddmObj[i].splits[j].Indices[2]);
      MessageBox(0, testmsg, testmsg, 0);*/
    }
  }

  Material material;
  material.transparent = true;
  material.SetMatId(Checksum("NO-MATERIAL").checksum);
  material.Reserve(1);
  texture.SetId(Checksum("NO-TEXTURE").checksum);
  material.AddTexture2(&texture);
  epicIndex = matList.GetNumMaterials();
  matList.AddMaterial(&material);
}

extern DWORD skyCRC;
extern DWORD crc;
extern DWORD numPowerups;
extern DWORD SizeDiff;
extern char levelName[25];

Th2xScene::Th2xScene(char* Path, bool sky)
{
#pragma pack(push)
#pragma pack(1)

  ZeroMemory(name, MAX_PATH);
  SetName(Path); blendValue = 2;
  cullMode = D3DCULL_CW;
  Checksum("change");
  Checksum("Wall_Non_Skatable_Angle");

  BYTE*pFile = NULL;
  DWORD size = 0, ptr = 0;

  char path[256] = "";
  memcpy(path, Path, strlen(Path) + 1);
  DWORD len = strlen(Path);

  FILE*f;
  path[len - 3] = 'D';
  path[len - 2] = 'D';
  path[len - 1] = 'X';

  f = fopen(path, "rb");
  if (f)
  {
    fseek(f, 0, SEEK_END);

    size = ftell(f);

    pFile = new BYTE[size];

    fseek(f, 0, SEEK_SET);
    fread(pFile, size, 1, f);
    fclose(f);

    ProcessXBOXTextures((DWORD)(pFile), sky);

    delete[] pFile;
  }

  path[len - 1] = 'M';

  f = fopen(path, "rb");
  BYTE* xbox = NULL;
  if (f)
  {
    fseek(f, 0, SEEK_END);

    size = ftell(f);

    pFile = new BYTE[size];

    fseek(f, 0, SEEK_SET);
    fread(pFile, size, 1, f);
    fclose(f);

    LoadDDM((DWORD)(pFile), sky);
    xbox = pFile;
    //delete [] pFile;
  }
  // }
  path[len - 4] = '_';
  path[len - 3] = 'L';
  path[len - 2] = '.';
  path[len - 1] = 'p';
  path[len + 0] = 's';
  path[len + 1] = 'x';

  f = fopen(Path, "rb");
  if (f)
  {
    fseek(f, 0, SEEK_END);

    size = ftell(f);

    pFile = new BYTE[size];

    fseek(f, 0, SEEK_SET);
    fread(pFile, size, 1, f);
    fclose(f);
    _fcloseall();
    ProcessMeshes(pFile, size, sky);
    delete[] pFile;
  }
  else
    MessageBox(0, Path, strerror(errno), 0);
  delete[] xbox;
  ddmObj.clear();

  if (sky) { MAX_TEXCOORDS; MAX_TEXCOORDS *= 0x100; Device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | MAX_TEXCOORDS); }
  else
  {
    path[strlen(Path) - 3] = 'O';

    f = fopen(path, "rb");
    if (f)
    {
      fclose(f);
      globalMaterialList = &this->matList;
      skyDome = new Th2xScene(path, true);
    }
    else
    {
      MAX_TEXCOORDS *= 0x100; if (Device) Device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | MAX_TEXCOORDS);
    }
    path[strlen(Path) - 3] = 'T';
    path[strlen(Path) - 2] = '.';
    path[strlen(Path) - 1] = 'T';
    path[strlen(Path) + 0] = 'R';
    path[strlen(Path) + 1] = 'G';

    f = fopen(path, "rb");
    if (f)
    {
      skyCRC = 0;
      crc = 0;
      numPowerups = 0;
      SizeDiff = 0;
      sprintf(levelName, "");

      DWORD pos = ((std::string)Path).find_last_of("\\");
      pos++;
      DWORD endPos = ((std::string)Path).find(".psx");
      for (DWORD i = pos; i < endPos; i++)
      {
        levelName[i - pos] = Path[i];
      }
      fseek(f, 0, SEEK_END);
      size = ftell(f);
      pFile = new BYTE[size];
      fseek(f, 0, SEEK_SET);
      fread(pFile, size, 1, f);
      fclose(f);

      ProcessTRG(pFile, size);

      if (skyDome)
      {
        static char chc[128] = "";
        bool found = false;
        for (int i = 0; i < skyDome->GetNumMeshes(); i++)
        {
          Mesh* mesh = skyDome->GetMesh(i);
          if (mesh->IsDeleted())
          {
            //mesh->DeinitCollision();
            /*Collision* collision = mesh->GetCollision();
            if(collision)
            {
            delete collision;
            mesh->DeinitCollision();
            }*/

            skyDome->EraseMesh(i);
            i--;
          }
          else if (mesh->GetName().checksum == skyCRC)
          {
            if (!mesh->GetName().GetString())
            {
              sprintf(chc, "SkyMesh%d", i);
              Scripts.push_back(Script(mesh->GetName().checksum, chc));
            }
            found = true;
            mesh->Center();
          }
          else
          {
            if (!mesh->GetName().GetString())
            {
              sprintf(chc, "SkyMesh%d", i);
              Scripts.push_back(Script(mesh->GetName().checksum, chc));
            }
          }
        }
        if (!found && skyDome->GetNumMeshes() == 1)
          skyDome->GetMesh(0)->Center();
        else if (skyDome->GetNumMeshes() == 0)
        {
          skyDome->matList.ReleaseTextures();
          delete skyDome;
          skyDome = NULL;
        }
        //skyDome->MoveObjects(this, skyCRC);
      }
    }

    //bool found = false;
    char chc[128] = "";

    for (DWORD i = 0, numMeshes = this->meshes.size(); i < numMeshes; i++)
    {
      /*found = false;
      for(DWORD j=0; j<this->nodes.size(); j++)
      {
      if(this->nodes[j].Class.checksum == EnvironmentObject::GetClass() && this->nodes[j].Name.checksum == this->meshes[i].GetName().checksum)
      {
      found = true;
      break;
      }
      }
      if(!found)
      {
      Checksum name = this->meshes[i].GetName();
      if(!name.GetString())
      {
      sprintf(chc,"Mesh%d",i);
      Scripts.push_back(Script(name.checksum, chc));
      }*/
      //DWORD shatter=0;
      /*if(this->meshes[i].shatterObject)
      {
      strcat(chc,"_shatter");

      shatter = Checksum(chc).checksum;
      triggers.push_back(*(Checksum*)&shatter);
      memcpy(&ShatterScript[27],&name,4);
      memcpy(&ShatterScript[48],&name,4);
      KnownScripts.push_back(KnownScript(*(Checksum*)&shatter, (void*)ShatterScript, 123));
      }*/
      if (GetNode(meshes[i].GetName()) == NULL)
      {
        Checksum name = this->meshes[i].GetName();
        if (!name.GetString())
        {
          sprintf(chc, "Mesh%d", i);
          Scripts.push_back(Script(name.checksum, chc));
        }
        this->nodes.push_back(EnvironmentObject(name, 0, 0, true));
        this->nodes.back().SetPosition(this->meshes[i].GetCenter());
        //this->meshes[i].node = &this->nodes.back();
        //this->meshes[i].node->SetPosition(this->meshes[i].GetCenter());
      }
      else if (!this->meshes[i].GetName().GetString())
      {
        sprintf(chc, "Mesh%d", i);
        Scripts.push_back(Script(this->meshes[i].GetName().checksum, chc));
      }
      //this->meshes[i].node->SetPosition(this->meshes[i].GetCenter());
    }

    for (DWORD i = 0, numNodes = this->nodes.size(); i < numNodes; i++)
    {
      for (DWORD j = 0; j < this->nodes[i].Links.size(); j++)
      {
        if (this->nodes[i].Links[j] >= nodes.size())
        {
          MessageBox(0, "??", "??", 0);
          this->nodes[i].Links.erase(this->nodes[i].Links.begin() + j);
          j--;
        }
      }
    }

    //FixNodes();

    for (DWORD i = 0, numNodes = this->nodes.size(); i < numNodes; i++)
    {
      if (this->nodes[i].TrickObject)
      {
        if (!this->nodes[i].Cluster.checksum)
        {
          this->nodes[i].Cluster = this->nodes[i].Name;
          this->nodes[i].ApplyCluster(&this->nodes[0]);
          //}

          /*for (DWORD j = 0; j < nodes[i].Links.size(); j++)
          {
          Node & link = this->nodes[this->nodes[i].Links[j]];
          if (!link.Cluster.checksum && (link.Class.checksum == EnvironmentObject::GetClass() || link.Class.checksum == RailNode::GetClass()))
          {
          link.TrickObject = true;
          link.Cluster = this->nodes[i].Cluster;
          if (link.Links.size())
          {
          DWORD X = 0;
          Node & link2 = nodes[link.Links[X]];
          while (!link2.Cluster.checksum && (link2.Class.checksum == EnvironmentObject::GetClass() || link2.Class.checksum == RailNode::GetClass()))
          {
          if (X<link.Links.size())
          X++;
          else

          }
          }
          for (DWORD k = 0; k < link.Links.size(); k++)
          {
          if (!nodes[link.Links[k]].Cluster.checksum && (nodes[link.Links[k]].Class.checksum == EnvironmentObject::GetClass() || nodes[link.Links[k]].Class.checksum == RailNode::GetClass()))
          nodes[link.Links[k]].Cluster = link.Cluster;
          }
          }
          }*/
        }
      }
      /*else if(this->nodes[i].Class.checksum==EnvironmentObject::GetClass())
      {
      if(!this->nodes[i].Trigger.checksum && !GetMesh(this->nodes[i].Name))
      {
      DeleteNode(i);
      i--;
      numNodes--;
      }
      }*/
    }
    FixLink();

    for (DWORD i = 0, numNodes = this->nodes.size(); i < numNodes; i++)
    {
      if (!this->nodes[i].Cluster.checksum && this->nodes[i].Class.checksum == RailNode::GetClass())
      {

        for (DWORD j = 0; j < this->nodes[i].Links.size(); j++)
        {

          Node & link = this->nodes[this->nodes[i].Links[j]];

          if (link.TrickObject && link.Class.checksum == EnvironmentObject::GetClass())
          {
            this->nodes[i].TrickObject = true;
            if (link.Cluster.checksum)
            {
              this->nodes[i].Cluster = link.Cluster;
              this->nodes[i].ApplyCluster(&nodes[0]);
            }
            else if (this->nodes[i].Cluster.checksum)
            {
              MessageBox(0, "impossible", "", 0);
              link.Cluster = this->nodes[i].Cluster;


              Mesh* tmpMesh = GetMesh(this->nodes[i].Cluster);

              if (tmpMesh)
              {
                //tmpMesh->rails.push_back(Rail(&this->nodes[i]));

                this->nodes[i].ApplyCluster(&nodes[0]);
                for (DWORD k = 0; k < this->nodes[i].Links.size(); k++)
                {
                  Node & meshLink = this->nodes[this->nodes[i].Links[k]];
                  if (meshLink.Class.checksum == RailNode::GetClass())
                  {
                    meshLink.Cluster = this->nodes[i].Cluster;
                    // tmpMesh->rails.back().nodes.push_back(&this->nodes[this->nodes[i].Links[k]]);
                  }
                }
              }
            }
            else
            {
              MessageBox(0, "impossible2", "", 0);
              this->nodes[i].Cluster = link.Name;
              Mesh* tmpMesh = GetMesh(this->nodes[i].Cluster);
              link.Cluster = this->nodes[i].Cluster;
              if (tmpMesh)
              {
                //tmpMesh->rails.push_back(Rail(&this->nodes[i]));

                this->nodes[i].ApplyCluster(&nodes[0]);
                for (DWORD k = 0; k < this->nodes[i].Links.size(); k++)
                {
                  Node & meshLink = this->nodes[this->nodes[i].Links[k]];
                  if (meshLink.Class.checksum == RailNode::GetClass())
                  {
                    meshLink.Cluster = this->nodes[i].Cluster;
                    //tmpMesh->rails.back().nodes.push_back(&this->nodes[this->nodes[i].Links[k]]);
                  }
                }
              }
            }
            //break;
          }
          else if (this->nodes[i].LinksToTrickOb(&nodes[0], &this->nodes[i].Cluster.checksum))
          {
            this->nodes[i].FixObCheck(&this->nodes[i]);
            this->nodes[i].TrickObject = true;
            this->nodes[i].ApplyCluster(&nodes[0]);
          }
          else
            this->nodes[i].FixObCheck(&this->nodes[i]);
        }
      }
      else if (this->nodes[i].Class.checksum == RailNode::GetClass())
      {
        this->nodes[i].ApplyCluster(&nodes[0]);
      }
      else if (this->nodes[i].Class.checksum == EnvironmentObject::GetClass() && this->nodes[i].CreatedAtStart)
      {
        if (this->nodes[i].Links.size() && this->nodes[i].LinksTo(0x1806DDF8, &this->nodes[i].Links.front(), &this->nodes.front()))
          this->nodes[i].Trigger = Checksum("sk3_killskater");
        else if (this->nodes[i].Trigger.checksum)
        {
          this->nodes[i].NetEnabled = true;
          this->nodes[i].Permanent = true;
        }
        this->nodes[i].FixCheck(0x1806DDF8, &nodes.front());
      }
    }

    for (DWORD i = 0, numNodes = this->nodes.size(); i < numNodes; i++)
    {
      if (this->nodes[i].Cluster.checksum && this->nodes[i].Class.checksum == RailNode::GetClass())
      {
        for (DWORD j = 0; j < this->nodes[i].Links.size(); j++)
        {
          Node & link = this->nodes[this->nodes[i].Links[j]];
          //extern const char Env [] = "EnvironmentObject";


          if (!link.Cluster.checksum && link.Class.checksum == RailNode::GetClass())
          {
            link.Cluster = this->nodes[i].Cluster;
            link.TrickObject = true;
          }
        }
      }
    }
    //CenterScene();
  }
}