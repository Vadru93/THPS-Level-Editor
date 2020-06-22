#include "StdAfx.h"
#include "Th4Scene.h"
#include <d3d9.h>
#include <d3dx9.H>
#include "ImageHandling.h"
#pragma unmanaged

extern DWORD MAX_TEXCOORDS; extern IDirect3DDevice9Ex* __restrict Device; extern vector <Script> Scripts; extern DWORD MAX_PASSES;
using std::string;
char Name[256];
struct Thug2Vertex
{
  float x;
  float y;
  float z;
  float xNormal;
  float yNormal;
  float zNormal;
  Colour color;
  TexCoord tUV[8];
};
#pragma pack(1)
struct Thug2VertexHeader
{
  BYTE padding;
  BYTE stride;
  WORD numVertices;
  WORD numBuffers;
  DWORD bufferSize;
};
#pragma pop(pack)
void FixBlending(Colour* color, DWORD numPixels, Texture* texture);//fixblendings
void Thug2Scene::ProcessMeshes(DWORD ptr)
{
    DWORD imageSize2 = 0;
  DWORD numObjects = ((DWORD*)ptr)[0];
  ptr += 4; meshes.reserve(numObjects);
  nodes.reserve(numObjects);
  for (DWORD i = 0; i < numObjects; i++)
  {
    Thug2Mesh::Data* meshData = (Thug2Mesh::Data*)ptr;
    Mesh* mesh = new Thug2Mesh(meshData);

    ptr += 56; if (meshData->flags & MESHFLAG_HEADEREXTENSION) ptr += 40;

    MaterialSplit matSplit;
    DWORD vOffset = 0;
    for (DWORD j = 0; j < meshData->numSplits; j++)
    {
      ptr += 44;
      matSplit.matId = ((DWORD*)ptr)[0];
      ptr += 4;
      DWORD lod = *(DWORD*)ptr;
      ptr += 4;
      Material* mat = NULL;
      for (DWORD k = 0; k < lod; k++)
      {
        DWORD numIndices = ((DWORD*)ptr)[0];
       // matSplit.numIndices = (DWORD)numIndices;
        ptr += 4;
        /*if(k==0)
          matSplit.AddIndices((WORD*)ptr, matSplit.numIndices);*/
        ptr += numIndices * 2;
        DWORD numIndices2 = (DWORD)*(WORD*)ptr;
        ptr += 2;
        if (k == 0)
        {
            matSplit.numIndices = numIndices2;
          matSplit.AddIndices((WORD*)ptr, matSplit.numIndices);// , vOffset);
          
          DWORD id = matSplit.matId;
          mat = matList.GetMaterial((Checksum)id);

          if (mat)
          {
            Texture* texture = mat->GetTexture(0);
            if (texture)
            {
              /*char* blendings[] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10" };
              if (texture->GetTexId() == 0x0926312C || texture->GetTexId() == 0x2C312609)
              MessageBox(0, "YE", blendings[texture->blendMode], 0);*/
              if (texture->blendMode == 1 || texture->blendMode == 2 || texture->blendMode == 4 || texture->blendMode == 5 || texture->blendMode == 6)
              {
                static char imageName[MAX_PATH] = "";
                static char tmp[15] = "";
                GetEnvironmentVariable("TEMP", imageName, MAX_PATH);
                DWORD chc = texture->GetTexId();
                char* Name = (*(Checksum*)&(chc)).GetString();
                if (Name == NULL)
                {
                  sprintf(tmp, "\\%08X.dds", texture->GetTexId());
                  strcat(imageName, tmp);
                }
                else
                {
                  DWORD len = strlen(imageName);
                  imageName[len] = '\\';
                  len++;
                  strcpy(&imageName[len], Name);
                  strcat(imageName, ".dds");
                }
                //HRESULT hres = D3DXCreateTextureFromFile(Device, imageName, &imageData);
                FILE* f = fopen(imageName, "rb");
                if (f)
                {
                  extern TexFiles texFiles;
                  char imageName2[MAX_PATH];
                  strcpy(imageName2, imageName);
                  imageName2[strlen(imageName) - 3] = 't';
                  imageName2[strlen(imageName) - 2] = 'g';
                  imageName2[strlen(imageName) - 1] = 'a';
                  for (DWORD l = 0; l < texFiles.numFiles; l++)
                  {
                    if (!stricmp(texFiles.fileNames[l], imageName))
                    {
                      texFiles.fileNames[l][strlen(imageName) - 3] = 't';
                      texFiles.fileNames[l][strlen(imageName) - 2] = 'g';
                      texFiles.fileNames[l][strlen(imageName) - 1] = 'a';
                    }
                  }
                  fseek(f, 0, SEEK_END);
                  DWORD size = ftell(f);
                  BYTE* data = new BYTE[size];
                  fseek(f, 0, SEEK_SET);
                  fread(data, size, 1, f);
                  fclose(f);
                  DeleteFile(imageName);
                  f = fopen(imageName2, "w+b");
                  if (f)
                  {
                    DWORD imageSize;
                    BYTE* imageData = data;
                    DDS_HEADER* header = (DDS_HEADER*)imageData;
                    imageData += sizeof(DDS_HEADER);

                    WORD width = (WORD)header->width;
                    WORD height = (WORD)header->height;

                    if (header->pixelFormat[1] & 0x4)
                    {
                      imageSize = header->width*header->height * 4;
                      //if (imageSize != 64)
                      {
                          imageSize2 = imageSize;
                          BYTE* levelData = new BYTE[imageSize];
                          DecompressDXT(imageData, header->imageSize, header->width, header->height, levelData, (((header->pixelFormat[2] & 0xFF000000) >> 24) - 0x30));
                          fputc(0, f);
                          fputc(0, f);
                          fputc(2, f);
                          DWORD zero = 0;
                          fwrite(&zero, 4, 1, f);
                          fwrite(&zero, 4, 1, f);
                          fputc(0, f);
                          fwrite(&header->width, 2, 1, f);
                          fwrite(&header->height, 2, 1, f);
                          fputc(32, f);
                          fputc(32, f);
                          Colour* color = (Colour*)levelData;
                          FixBlending(color, imageSize / 4, texture);
                          fwrite(levelData, imageSize, 1, f);
                          delete[] levelData;
                      }
                    }
                    else
                    {
                      imageSize = header->width*header->height * 4;
                      DWORD bytesPerPixel = header->imageSize / header->width / header->height;
                      bool usingAlpha = header->pixelFormat[1] & 0x2;

                      if (bytesPerPixel == 4)
                      {
                        fputc(0, f);
                        fputc(0, f);
                        fputc(2, f);
                        DWORD zero = 0;
                        fwrite(&zero, 4, 1, f);
                        fwrite(&zero, 4, 1, f);
                        fputc(0, f);
                        fwrite(&header->width, 2, 1, f);
                        fwrite(&header->height, 2, 1, f);
                        fputc(32, f);
                        fputc(32, f);
                        Colour* color = (Colour*)imageData;
                        FixBlending(color, imageSize / 4, texture);
                        fwrite(imageData, imageSize, 1, f);
                      }
                      else if (bytesPerPixel == 2)
                      {
                        imageSize = header->width*header->height * 4;
                        DWORD numPixels = imageSize / 4;
                        Colour* levelData = new Colour[numPixels];
                        //MessageBox(0,"sure this is 16 bit?",imageName,0);
                        if (usingAlpha)
                        {
                          for (DWORD l = 0; l < numPixels; l++)
                          {
                            BYTE r = ((BYTE)((((WORD*)imageData)[l] & 0x003E) >> 1));
                            BYTE g = ((BYTE)((((WORD*)imageData)[l] & 0x7C0) >> 6));
                            BYTE b = ((BYTE)((((WORD*)imageData)[l] & 0xF800) >> 12));
                            BYTE a = ((BYTE)((((WORD*)imageData)[l] & 0x1)));
                            levelData[l].r = (r << 3) | (r >> 2);
                            levelData[l].g = (g << 3) | (g >> 2);
                            levelData[l].b = (b << 3) | (b >> 2);
                            if (a)
                              levelData[l].a = 255;
                            else
                              levelData[l].a = 0;
                          }
                        }
                        else
                        {
                          for (DWORD l = 0; l < numPixels; l++)
                          {
                            BYTE r = (BYTE)(((WORD*)imageData)[l] & 0x001F);
                            BYTE g = ((BYTE)((((WORD*)imageData)[l] & 0x7E0) >> 5));
                            BYTE b = ((BYTE)((((WORD*)imageData)[l] & 0xF800) >> 11));
                            levelData[l].r = (r << 3) | (r >> 2);
                            levelData[l].g = (g << 2) | (g >> 4);
                            levelData[l].b = (b << 3) | (b >> 2);
                            levelData[l].a = 255;
                          }
                        }
                        fputc(0, f);
                        fputc(0, f);
                        fputc(2, f);
                        DWORD zero = 0;
                        fwrite(&zero, 4, 1, f);
                        fwrite(&zero, 4, 1, f);
                        fputc(0, f);
                        fwrite(&header->width, 2, 1, f);
                        fwrite(&header->height, 2, 1, f);
                        fputc(32, f);
                        fputc(32, f);
                        Colour* color = (Colour*)levelData;
                        FixBlending(color, imageSize / 4, texture);
                        fwrite(levelData, imageSize, 1, f);
                        delete [] levelData;
                        //delete [] levelData;
                      }
                    }
                    fclose(f);
                  }
                  delete [] data;
                }
              }
              if (imageSize2 == 64)
                  MessageBox(0, "WT5?!", "...", MB_OK);
              //mat->transparent = true;
              texture->Release();
              texture->CreateTexture();
              if (imageSize2 == 64)
                  MessageBox(0, "WT6?!", "...", MB_OK);
            }
          }

          mesh->AddMatSplit(&matSplit, &matList);
        }
        ptr += numIndices * 2 + 13;
      }
      //ptr += numIndices * 4 + 15;
      //
      Thug2VertexHeader* vertexHeader = (Thug2VertexHeader*)ptr;

      ptr += sizeof(Thug2VertexHeader);
      //mesh->Reserve(vertexHeader->numVertices+vOffset);
      /*for (DWORD i = 0; i < mesh->GetNumVertices(); i++)
      {
      for (DWORD j = 1; j < mesh->GetNumVertices(); j++)
      {
      if (mesh->GetVertex(i).x == mesh->GetVertex(j).x && mesh->GetVertex(i).y == mesh->GetVertex(j).y && mesh->GetVertex(i).z == mesh->GetVertex(j).z && mesh->GetVertex(i).tUV[0].u == mesh->GetVertex(j).tUV[0].u && mesh->GetVertex(i).tUV[0].v == mesh->GetVertex(j).tUV[0].v)
      {
      mesh->DeleteVertex((WORD)j, (WORD)i);
      vOffset--;
      j--;
      i--;
      }
      }
      }*/
      if (vOffset)
      {
        for (DWORD k = 0; k < mesh->matSplits[j].Indices.size(); k++)
        {
          mesh->matSplits[j].Indices[k] += vOffset;
        }
      }
      vOffset += (DWORD)vertexHeader->numVertices;
      TexCoord* texCoords;
      /*for (DWORD l = 0; l < matSplit.Indices.size(); l++)
      {
        if ((vOffset && matSplit.Indices[l] > vOffset))
          MessageBox(0, "above", "", 0);
        else if ((vOffset && matSplit.Indices[l] < vOffset - (DWORD)vertexHeader->numVertices))
          MessageBox(0, "below", "", 0);
      }*/

      for (DWORD k = 0; k < vertexHeader->numBuffers; k++)
      {
        DWORD numVertices = vertexHeader->numVertices;
        /*if (k == 0)
          vOffset += numVertices;*/
        /*if(numVertices>vertexHeader->numVertices)
        numVertices=vertexHeader->numVertices;*/
        DWORD numUV = 0;
        if (meshData->flags & MESHFLAG_TEXCOORD)
        {
          numUV = vertexHeader->stride - 12;
          if (meshData->flags & MESHFLAG_NORMAL)
            numUV -= 12;
          if (meshData->flags & MESHFLAG_COLOUR)
            numUV -= 4;
          if (meshData->flags & MESHFLAG_SKIN)
            numUV -= 16;


          numUV = numUV / 8;
          if (numUV > 8)
            numUV = 8;
          if (MAX_TEXCOORDS < numUV)
            MAX_TEXCOORDS = numUV;

        }
        Vertex vertex;
        vertex.colour.a = 255;
        vertex.colour.b = 128;
        vertex.colour.r = 128;
        vertex.colour.g = 128;


        if (k == 0)
          texCoords = new TexCoord[numUV*numVertices];

        for (DWORD l = 0; l < numVertices; l++)
        {
          DWORD stride = 0;
          vertex.x = ((SimpleVertex*)ptr)->x;
          vertex.y = ((SimpleVertex*)ptr)->y;
          vertex.z = -((SimpleVertex*)ptr)->z;
          stride += 12;
          if (meshData->flags & MESHFLAG_NORMAL)
          {
            stride += 12;
          }
          if (meshData->flags & MESHFLAG_COLOUR)
          {
            vertex.colour = ((Colour*)(ptr + stride))[0];
            stride += 4;
          }
          if (meshData->flags & MESHFLAG_SKIN)
            stride += 16;

          vertex.tUV[0] = ((TexCoord*)(ptr + stride))[0];

          if (k == 0)
          {
            for (DWORD m = 0; m < numUV; m++)
            {
              texCoords[l*numUV + m] = ((TexCoord*)(ptr + stride))[m];
            }
          }

          ptr += vertexHeader->stride;
          if (k == 0)
          {
            mesh->AddVertex(&vertex);
            if (numUV > 1)
            {
              Material* mat = matList.GetMaterial(matSplit.matId);
              if (mat)
              {
                if (mat->fucked && mat->fucked < numUV)
                {
                  Vertex* verts = (Vertex*)mesh->GetVertices();
                  for (DWORD k = 0; k < matSplit.Indices.size(); k++)
                  {
                    if (matSplit.Indices[k] < mesh->GetNumVertices())
                      verts[matSplit.Indices[k]].tUV[0] = texCoords[matSplit.Indices[k] * numUV + mat->fucked];
                  }
                }
              }
            }
          }
        }
        if (k == 0)
        {
          if (numUV > 1)
          {
            //Material* mat = matList.GetMaterial((Checksum)id);
            if (mat && mat->GetNumTextures() > 1)
            {
              for (DWORD k = 0; k < extraLayerMeshes.size(); k++)
              {
                if (extraLayerMeshes[k].name.checksum == meshData->name.checksum)
                {
                  extraLayerMeshes[k].matSplits.push_back(MaterialSplit());
                  extraLayerMeshes[k].matSplits.back().matId = matSplit.matId;
                  extraLayerMeshes[k].matSplits.back().numIndices = matSplit.numIndices;
                  extraLayerMeshes[k].matSplits.back().Indices.assign(matSplit.Indices.begin(), matSplit.Indices.end());
                  extraLayerMeshes[k].numPasses = (numUV);
                  /*DWORD vertexIndex = extraLayerMeshes[k].vertices.size();
                  extraLayerMeshes[k].vertices.resize(vertexIndex+1);
                  extraLayerMeshes[k].vertices[vertexIndex].resize(meshData->numVertices);
                  for(DWORD l=0; l<meshData->numVertices; l++)
                  {
                  for(DWORD m=1; m<numTexCoords; m++)
                  {
                  extraLayerMeshes[k].vertices[vertexIndex][l].x = vertices[j].x;
                  extraLayerMeshes[k].vertices[vertexIndex][l].y = vertices[j].y;
                  extraLayerMeshes[k].vertices[vertexIndex][l].z = vertices[j].z;
                  extraLayerMeshes[k].vertices[vertexIndex][j].colour=vertices[j].colour;
                  extraLayerMeshes[k].vertices[vertexIndex][l].tUV[m] = meshData->texCoords[l*numTexCoords+m];
                  }
                  }*/
                  goto found;
                }
              }
              extraLayerMeshes.push_back(ExtraLayerMesh(meshData->name, &matSplit, mesh->GetVertices(), mesh->GetNumVertices(), texCoords, numUV));
            }
          }
        found:;

          /*Grass* grass = GetGrass(id);
          if (grass)
          {
            if (grassMesh == NULL)
            {
              grassMesh = new Mesh(*mesh);
              char name[128];
              sprintf(name, "3DGrassMesh%u", numGrassMeshes);
              numGrassMeshes++;
              grassMesh->SetName(name);
              grassMesh->Clear(CLEARFLAGS_KEEPVERTICES);
            }
            DWORD grassLayers = grass->grassLayers;
            if (grassLayers > 5)
              grassLayers = 5;
            float grassHeight = grass->grassHeight;

            float heightOffset = grassHeight / grassLayers;

            for (DWORD layer = 0; layer < grassLayers; layer++)
            {

              DWORD vertCount = grassMesh->GetNumVertices();
              MaterialSplit newSplit = matSplit;
              char material_name[64];
              sprintf(material_name, "Grass-Grass_Layer%u", layer);
              Checksum chc = GenerateCaseSensChecksum(material_name);
              Material* mat = matList.GetMaterial(*(DWORD*)&chc, &id);
              if (mat)
              {
                newSplit.matId = id;
                mat->transparent = true;
                mat->drawOrder = 18010.0f + (float)layer;
              }
              WORD* alreadyExported = new WORD[matSplit.Indices.size()];
              DWORD* newIndx = new DWORD[matSplit.Indices.size()];
              for (DWORD k = 0; k < matSplit.Indices.size(); k++)
              {
                //newSplit.Indices[i]+=vertCount;
                for (DWORD l = 0; l < k; l++)
                {
                  if (alreadyExported[l] == matSplit.Indices[k])
                  {
                    alreadyExported[l] = 0xFFFF;
                    newSplit.Indices[k] = newIndx[l];
                    continue;
                  }
                }
                newIndx[k] = grassMesh->GetNumVertices();
                newSplit.Indices[k] = newIndx[k];
                alreadyExported[k] = matSplit.Indices[k];
                Vertex v = grassMesh->GetVertex(matSplit.Indices[k]);
                v.y += (heightOffset*(float)(layer + 1));//+5.0f);
                v.tUV[0].u = v.x / 48.0f;
                v.tUV[0].v = v.z / 48.0f;
                grassMesh->AddVertex(&v);
                //Vertex v = mesh->GetVertex(matSplit.Indices[i]);
              }
              delete[] newIndx;
              delete[] alreadyExported;
              grassMesh->AddMatSplit(&newSplit, &matList);
              grassMesh->GetSplit(mesh->GetNumSplits() - 1)->numIndices = grassMesh->GetSplit(mesh->GetNumSplits() - 1)->Indices.size() - 2;
            }
            if (grassMesh->GetNumVertices() >= 0xFFFF)
              MessageBox(0, "WTF", "", 0);
          }*/
        }
        if (k + 1 != vertexHeader->numBuffers)//16E00
        {
          //MessageBox(0, "I", "", 0);
          ptr++;
        }
        vertexHeader->bufferSize = ((DWORD*)ptr)[0];
        ptr += 4;
      }
      delete[] texCoords;

      ptr += 4;
      BYTE a = *(BYTE*)ptr;
      ptr++;
      BYTE b = *(BYTE*)ptr;
      ptr++;
      BYTE c = *(BYTE*)ptr;
      ptr++;
      BYTE d = *(BYTE*)ptr;
      ptr++;

      if (d != 0)
      {
        ptr += (DWORD)vertexHeader->numVertices;
      }

      int b2 = *(DWORD*)ptr;
      ptr += 4;
      if (b2 > 1)
      {
        ptr+= (4 * b2);
      }

      c = *(DWORD*)ptr;
      ptr += 4;
      int d2;
      int e;
      if (c == 1)
      {
        d2 = *(DWORD*)ptr;
        ptr += 4;
        e = *(DWORD*)ptr;
        ptr += 4;
        ptr+= (e);
      }
      //ptr += 7;
      /*BYTE a = *(BYTE*)ptr;
      ptr++;
      BYTE b = *(BYTE*)ptr;
      ptr++;
      BYTE c = *(BYTE*)ptr;
      ptr++;
      BYTE d = *(BYTE*)ptr;
      ptr++;
      if (d != 0)
        ptr += vertexHeader->numVertices;
      int unk = *(DWORD*)ptr;
      ptr += 4;
      if (unk > 1)
        ptr += unk * 4;

      unk = *(DWORD*)ptr;
      ptr += 4;
      if (unk == 1)
      {
        ptr += 4;
        ptr += *(DWORD*)ptr;
        ptr += 4;
      }*/
      /*if (*(bool*)ptr++)
      {
        char waaaw[256];
        if ((WORD)vertexHeader->bufferSize != 0x352 &&
          (WORD)vertexHeader->bufferSize != 0x452 &&
          (WORD)vertexHeader->bufferSize != 0x652 &&
          (WORD)vertexHeader->bufferSize != 0x252 &&
          (WORD)vertexHeader->bufferSize != 0x052 &&
          (WORD)vertexHeader->bufferSize != 0x152 &&
          (WORD)vertexHeader->bufferSize != 0x142)
        {
          sprintf(waaaw, "%X", vertexHeader->bufferSize);
          MessageBox(0, "wtf?", waaaw, 0);
        }
        ptr += 4;
        ///*if (*(DWORD*)ptr == 0)
        //{
      retry:
        while (*(DWORD*)ptr != 0x1) ptr++;
        ptr += 4;
        if (*(DWORD*)ptr == 0x0 || *(WORD*)(ptr + 1) != 0x0)
        {
          ptr -= 3;
          goto retry;
        }
        if (*(DWORD*)ptr == 1)
        {
          //MessageBox(0, "1", "", 0);
          ptr += 4;
          ptr += 248;
        }
        else if (*(DWORD*)ptr == 3 || *(DWORD*)ptr == 2 || *(DWORD*)ptr == 4)
        {
          //MessageBox(0, "3rd", "", 0);
          //MessageBox(0, "3", "", 0);
          ptr += 4;
        }
        else
        {
          char wa[256];
          sprintf(wa, "%X %X %X", *(DWORD*)(ptr), *(DWORD*)(ptr + 30), vertexHeader->bufferSize);
          MessageBox(0, wa, "WAAA1", 0);
          ptr += 4;
          //}
        }
        ///*}
        //else
        //  ptr += 8;
        continue;
      }
      ptr += 4;
      if (*(DWORD*)ptr == 1)
      {
        //MessageBox(0, "1", "", 0);
        ptr += 4;
        ptr += 248;
      }
      else if (*(DWORD*)ptr == 3 || *(DWORD*)ptr == 2 || *(DWORD*)ptr == 4)
      {
        //MessageBox(0, "3", "", 0);
        ptr += 4;
      }
      else
      {
        char wa[256];
        sprintf(wa, "%X", *(DWORD*)ptr);
        MessageBox(0, "WAAA2", wa, 0);
        ptr += 4;
      }
      // /*if(*(DWORD*)ptr+=4==1)
      //   ptr+=248;*/
    }
    //free(meshData);
    /*nodes.push_back(EnvironmentObject(mesh->GetName(), 0, 0, true));
    nodes.back().SetPosition(mesh->GetCenter());*/
    //mesh->node = &nodes.back();
    ///mesh->node->SetPosition(mesh->GetCenter());
    /* static char chc[128] = "";
    sprintf(chc, "Mesh%u",i);
    Scripts.push_back(Script(nodes.back().Name.checksum, chc));*/
    AddMesh(mesh);
    delete mesh;
  }
}


DWORD Thug2Scene::ProcessMaterials(DWORD ptr, bool sky)
{
  DWORD numMaterials = *(DWORD*)ptr;
  ptr += 4;
  for (DWORD i = 0; i<numMaterials; i++)
  {

    ThugMaterial::Data* materialData = (ThugMaterial::Data*)ptr;
    ThugMaterial material = ThugMaterial(materialData);

    ptr += 28; if (materialData->numTextures>MAX_PASSES) MAX_PASSES = materialData->numTextures;
    if (materialData->unkFlag)
      ptr += 8;
    float power = *(float*)ptr;
    ptr += 4;
    if (power > 0.0f)
      ptr += 12;
    /*if(((float*)ptr)[0] > 0.0f)
    ptr+=12;*/

    for (DWORD j = 0; j < materialData->numTextures; j++)
    {
      ThugTexture::Data* textureData = (ThugTexture::Data*)ptr;
      ThugTexture texture = ThugTexture(textureData);

      if (j == 0)
      {
        if (textureData->flags & MATFLAG_PASS_IGNORE_VERTEX_ALPHA)
          material.ignoreAlpha = true;
        if (materialData->doubled || (!materialData->singledSided && (((texture.blendMode | texture.fixedAlpha) & 0x00FFFFFFUL) != 0x00)))
          material.doubled = true;
      }

      //material.AddTexture(&texture);
      if (material.transparent)
      {
        material.doubled = true;
        //material.drawOrder *= 10.0f;
        material.drawOrder++;
      }

      /*if (j == 0)
      {
        if (textureData->flags & MATFLAG_PASS_IGNORE_VERTEX_ALPHA)
          material.ignoreAlpha = true;
        if (materialData->doubled || (!materialData->singledSided && (((texture.blendMode | texture.fixedAlpha) & 0x00FFFFFFUL) != 0x00)))
          material.doubled = true;
      }*/

      if (textureData->flags & (1 << 3))
      {
        if (material.GetNumTextures() == 0 && j + 1 == materialData->numTextures)
        {
          //if(material.GetNumTextures()==0)
          material.fucked = j;
          material.AddTexture(&texture);
          material.invisible = true;
          material.transparent = true;
        }
      }
      else
      {
        if (j != 0 && material.GetNumTextures() == 0)
          material.fucked = j;
        material.AddTexture(&texture);
        if (sky)
          material.drawOrder = 0;
      }
      //delete texture;
      ptr += 49;

      if (textureData->flags & TEXFLAG_UNKNOWN)
        ptr += 32;
      if (j == 0 && textureData->flags & TEXFLAG_EXTENSION)
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
      if (textureData->flags & TEXFLAG_EXTENSION2)
      {
        DWORD *numExtensions = (DWORD*)ptr;
        ptr += 16;

        for (DWORD k = 0; k < *numExtensions; k++)
        {
          if (k == 0)
            material.GetTexture(j)->SetId(((DWORD*)ptr)[1]);
          ptr += 8;
        }
      }
      ptr += 16;
    }
    AddMaterial(&material);
    //delete material;
  }
  return ptr;
}

DWORD GetCombinedVertCount(Mesh* mesh)
{
  /*DWORD vertCount = mesh->GetNumVertices();
  Collision* collision = mesh->GetCollision();
  if(collision)
  {
  WORD numVerts=0;
  SimpleVertex* uniqueVerts = collision->GetUniqueVerts(&numVerts);
  vertCount+=(DWORD)numVerts;
  const Vertex* v = mesh->GetVertices();
  for(DWORD i=0; i<numVerts; i++)
  {
  for(DWORD j=0; j<mesh->GetNumVertices(); j++)
  {
  if(uniqueVerts[i].x==v[j].x && uniqueVerts[i].y==v[j].y && uniqueVerts[i].z==v[j].z)
  vertCount--;
  }
  }
  delete [] uniqueVerts;
  }
  return vertCount;*/
  //DWORD numColVert=0;
  Collision* collision = mesh->GetCollision();
  if (collision)
    return collision->GetNumVertices() + mesh->GetNumVertices();
  return 0;//mesh->GetNumVertices()+numColVert;
}

void FixBlending(Colour* color, DWORD numPixels, Texture* texture)//fixblendings
{
  for (DWORD l = 0; l < numPixels; l++)
  {
    if (texture->blendMode == 4)
    {
      if (true)
      {
        if (texture->fixedAlpha)
        {
          if ((*(Colour*)&texture->fixedAlpha).a)
          {
            color[l].a = (BYTE)((float)(((DWORD)color[l].r + (DWORD)color[l].g + (DWORD)color[l].b) / 6)*((float)(*(Colour*)&texture->fixedAlpha).a / 127.0f));
            DWORD alpha = color[l].a * 3;
            if (alpha > 0xFF)
              alpha = 0xFF;
            color[l].a = (BYTE)alpha;
          }
          else
            color[l].a = (color[l].r + color[l].g + color[l].b) / 6;
          color[l].r = 0xFF - color[l].r;
          color[l].g = 0xFF - color[l].g;
          color[l].b = 0xFF - color[l].b;
          if ((*(Colour*)&texture->fixedAlpha).r || (*(Colour*)&texture->fixedAlpha).g || (*(Colour*)&texture->fixedAlpha).b)
          {
            color[l].r = (BYTE)((float)color[l].r*((float)(*(Colour*)&texture->fixedAlpha).r / 254.0f / 2.0f));
            color[l].g = (BYTE)((float)color[l].g*((float)(*(Colour*)&texture->fixedAlpha).g / 254.0f / 2.0f));
            color[l].b = (BYTE)((float)color[l].b*((float)(*(Colour*)&texture->fixedAlpha).b / 254.0f / 2.0f));
          }
          else
          {
            color[l].r = 0xFF - color[l].r;
            color[l].g = 0xFF - color[l].g;
            color[l].b = 0xFF - color[l].b;
          }
          /*if((*(Colour*)&texture->fixedAlpha).r || (*(Colour*)&texture->fixedAlpha).g || (*(Colour*)&texture->fixedAlpha).b)
          {*/
          /*if(!((*(Colour*)&texture->fixedAlpha).r==254 && (*(Colour*)&texture->fixedAlpha).g==254 && (*(Colour*)&texture->fixedAlpha).b==254))
          {
          color[l].r = (BYTE)((float)color[l].r*((float)(*(Colour*)&texture->fixedAlpha).r/254.0f));
          color[l].g = (BYTE)((float)color[l].g*((float)(*(Colour*)&texture->fixedAlpha).g/254.0f));
          color[l].b = (BYTE)((float)color[l].b*((float)(*(Colour*)&texture->fixedAlpha).b/254.0f));
          /*color[l].r = 0xFF-color[l].r;
          color[l].g = 0xFF-color[l].g;
          color[l].b = 0xFF-color[l].b;
          color[l].r /= 2;
          color[l].g /= 2;
          color[l].b /= 2;*/
          //}
          /*else
          {
          color[l].r = 0xFF-color[l].r;
          color[l].g = 0xFF-color[l].g;
          color[l].b = 0xFF-color[l].b;
          }*/

          /*}
          else
          {
          color[l].r = 0xFF-color[l].r;
          color[l].g = 0xFF-color[l].g;
          color[l].b = 0xFF-color[l].b;
          }*/
        }
      }
    }
    else if (texture->blendMode == 5 || texture->blendMode == 6)
    {
      if ((*(Colour*)&texture->fixedAlpha).a)
      {
        color[l].a = (BYTE)((float)color[l].a*((float)(*(Colour*)&texture->fixedAlpha).a / 127.0f));
      }
      if ((*(Colour*)&texture->fixedAlpha).r || (*(Colour*)&texture->fixedAlpha).g || (*(Colour*)&texture->fixedAlpha).b)
      {
        if ((*(Colour*)&texture->fixedAlpha).r != 254 || (*(Colour*)&texture->fixedAlpha).g != 254 || (*(Colour*)&texture->fixedAlpha).b != 254)//(!((*(Colour*)&texture->fixedAlpha).r == 254 && (*(Colour*)&texture->fixedAlpha).g == 254 && (*(Colour*)&texture->fixedAlpha).b == 254))
        {
          color[l].r = (BYTE)((float)color[l].r*((float)(*(Colour*)&texture->fixedAlpha).r / 254.0f/**((float)color[l].a/254.0f)*/));
          color[l].g = (BYTE)((float)color[l].g*((float)(*(Colour*)&texture->fixedAlpha).g / 254.0f/**((float)color[l].a/254.0f)*/));
          color[l].b = (BYTE)((float)color[l].b*((float)(*(Colour*)&texture->fixedAlpha).b / 254.0f/**((float)color[l].a/254.0f)*/));
          /*color[l].r = 255;
          if (color[l].a)
            color[l].a /= 2;*/
        }
        else if (color[l].a == 0xFC)
          color[l].a = color[l].a = 0xFF;
        /*else if(texture->GetFlags() & 0x40)
        {
        color[l].r/=2;
        color[l].g/=2;
        color[l].b/=2;
        color[l].a/=2;
        }*/
      }
      else
      {
        color[l].r = 0;//(BYTE)((float)color[l].r*((float)(*(Colour*)&texture->fixedAlpha).r/254.0f));
        color[l].g = 0;//(BYTE)((float)color[l].g*((float)(*(Colour*)&texture->fixedAlpha).g/254.0f));
        color[l].b = 0;//(BYTE)((float)color[l].b*((float)(*(Colour*)&texture->fixedAlpha).b/254.0f));
        if (color[l].a)
          color[l].a = 80;
      }
      /*color[l].r = 0xFF-color[l].r;
      color[l].g = 0xFF-color[l].g;
      color[l].b = 0xFF-color[l].b;*/
      //}
      /*else if(!texture->fixedAlpha)
      {
      color[l].r = 0xFF-color[l].r;
      color[l].g = 0xFF-color[l].g;
      color[l].b = 0xFF-color[l].b;
      color[l].a /= 2;
      }
      else
      {
      color[l].r = (BYTE)((float)color[l].r*((float)(*(Colour*)&texture->fixedAlpha).r/255.0f));
      color[l].g = (BYTE)((float)color[l].g*((float)(*(Colour*)&texture->fixedAlpha).g/255.0f));
      color[l].b = (BYTE)((float)color[l].b*((float)(*(Colour*)&texture->fixedAlpha).b/255.0f));

      }*/
    }
    else
    {
      if (texture->fixedAlpha)
      {
        if ((*(Colour*)&texture->fixedAlpha).a)
        {
          color[l].a = (BYTE)((float)(((DWORD)color[l].r + (DWORD)color[l].g + (DWORD)color[l].b) / 3)*((float)(*(Colour*)&texture->fixedAlpha).a / 127.0f));
          if (texture->blendMode == 2)
          {
            DWORD alpha = color[l].a * 4;
            if (alpha > 0xFF)
              alpha = 0xFF;
            color[l].a = (BYTE)alpha;
          }
        }
        else
          color[l].a = (color[l].r + color[l].g + color[l].b) / 10;

        if ((*(Colour*)&texture->fixedAlpha).r != 254 && (*(Colour*)&texture->fixedAlpha).g != 254 && (*(Colour*)&texture->fixedAlpha).b != 254)
        {
          color[l].r = (BYTE)((float)color[l].r*((float)(*(Colour*)&texture->fixedAlpha).r / 254.0f));
          color[l].g = (BYTE)((float)color[l].g*((float)(*(Colour*)&texture->fixedAlpha).g / 254.0f));
          color[l].b = (BYTE)((float)color[l].b*((float)(*(Colour*)&texture->fixedAlpha).b / 254.0f));
        }
        /*if((!((*(Colour*)&texture->fixedAlpha).r || (*(Colour*)&texture->fixedAlpha).g || (*(Colour*)&texture->fixedAlpha).b) || ((*(Colour*)&texture->fixedAlpha).r==254 && (*(Colour*)&texture->fixedAlpha).g==254 && (*(Colour*)&texture->fixedAlpha).b==254)) && texture->blendMode == 2)
        {
        color[l].r = 0xFF-color[l].r;
        color[l].g = 0xFF-color[l].g;
        color[l].b = 0xFF-color[l].b;
        }*/
      }
      else
        color[l].a = (color[l].r + color[l].g + color[l].b) / 10;
    }
  }
}

Thug2Scene::Thug2Scene(char* Path, bool sky)
{
  ZeroMemory(name, MAX_PATH);
  SetName(Path); blendValue = 2;
  cullMode = D3DCULL_NONE;

  BYTE*pFile = NULL;
  DWORD size = 0, ptr = 0;
  char path[256] = "";

  memcpy(path, Path, strlen(Path) + 1);
  DWORD len = strlen(Path);

  if (strstr(Path, ".skin.xbx"))
  {
    path[len - 8] = 't';
    path[len - 7] = 'e';
    path[len - 6] = 'x';
    path[len - 5] = '.';
    path[len - 4] = 'x';
    path[len - 3] = 'b';
    path[len - 2] = 'x';
    path[len - 1] = '\0';
  }
  else
  {
    path[len - 7] = 't';
    path[len - 6] = 'e';
    path[len - 5] = 'x';
  }

  FILE*f = fopen(path, "rb");
  if (f)
  {
    fseek(f, 0, SEEK_END);

    size = ftell(f);

    pFile = new BYTE[size];

    fseek(f, 0, SEEK_SET);
    fread(pFile, size, 1, f);
    fclose(f);

    ptr = (DWORD)(pFile);
    ptr += 4;
    ProcessTexFile(ptr, sky);

    delete[] pFile;
  }
  f = fopen(Path, "rb");
  if (f)
  {
    fseek(f, 0, SEEK_END);

    size = ftell(f);

    pFile = new BYTE[size];

    fseek(f, 0, SEEK_SET);
    fread(pFile, size, 1, f);
    fclose(f);

    ptr = (DWORD)(pFile);
    ptr += 12;

    ptr = ProcessMaterials(ptr, sky);
    ProcessMeshes(ptr);

    delete[] pFile;
  }
  len = strlen(path);
  path[len - 7] = 'c';
  path[len - 6] = 'o';
  path[len - 5] = 'l';
  f = fopen(path, "rb");
  if (f)
  {
    fseek(f, 0, SEEK_END);

    size = ftell(f);

    pFile = new BYTE[size];

    fseek(f, 0, SEEK_SET);
    fread(pFile, size, 1, f);
    fclose(f);

    ptr = (DWORD)(pFile);
    NSCollisionHeader header = ((NSCollisionHeader*)ptr)[0];
    NSCollisionObject* collision = ProcessNSCollision(ptr);


    for (DWORD i = 0; i < header.numObjects; i++)
    {
      if (collision[i].numVerts != 0)
      {
        Mesh* tmpMesh = GetMesh(*(Checksum*)&collision[i].Name);
        if (tmpMesh)
        {
          /*if(GetCombinedVertCountTHUG(&collision[i], tmpMesh)>750)//650?|600+atleast
          {
          MessageBox(0,tmpMesh->GetName().GetString(),"",0);
          const DWORD index = meshes.size();
          meshes.push_back(Mesh());
          meshes[index].AddCollision(collision[i], header.Version);
          meshes[index].UnSetFlag(MeshFlags::isVisible);
          collision[i].Name++;
          meshes[index].SetName(*(Checksum*)&collision[i].Name);
          }
          else*/
          tmpMesh->AddCollision(collision[i], header.Version);
        }
        else
        {
          const DWORD index = meshes.size();
          meshes.push_back(Mesh());
          meshes[index].AddCollision(collision[i], header.Version);
          meshes[index].UnSetFlag(MeshFlags::isVisible);
          meshes[index].SetName(*(Checksum*)&collision[i].Name);
        }
      }
    }
    /*for(DWORD i=0; i<header.numObjects; i++)
    {
    if(collision[i].numVerts!=0)
    {
    Mesh* tmpMesh = GetMesh(*(Checksum*)&collision[i].Name);
    if(tmpMesh)
    tmpMesh->AddCollision(collision[i], header.Version);
    else
    {
    DWORD index = meshes.size();
    meshes.push_back(Mesh());
    meshes[index].InitCollision();
    meshes[index].AddCollision(collision[i], header.Version);
    meshes[index].SetFlag(MeshFlags::isVisible, false);
    meshes[index].SetName(*(Checksum*)&collision[i].Name);
    }
    }
    }*/
    delete[] pFile;
  } if (sky) { MAX_TEXCOORDS *= 0x100; Device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | MAX_TEXCOORDS); }
  else
  {
    memcpy(path, Path, strlen(Path) + 1);
    char tmp[25] = "";
    DWORD pos = ((string)path).find_last_of("\\");
    DWORD endPos = ((string)path).find(".scn.xbx");
    if (endPos == string::npos)
      endPos = ((string)path).find(".mdl.xbx");
    if (endPos == string::npos)
      endPos = ((string)path).find(".skin.xbx");
    if (endPos != string::npos)
    {
      for (DWORD i = pos; i < endPos; i++)
      {
        tmp[i - pos] = path[i];
      }
      char tmp2[100] = "";
      sprintf(tmp2, "%s.qb", tmp);
      memcpy(&path[pos], tmp2, strlen(tmp2) + 1);
      MessageBox(0, path, tmp2, 0);
      f = fopen(path, "rb");
      if (f)
      {
        fseek(f, 0, SEEK_END);
        DWORD size = ftell(f);
        fseek(f, 0, SEEK_SET);
        BYTE* pFile = new BYTE[size];
        fread(pFile, size, 1, f);
        fclose(f);
        vector<KnownScript> scripts;
        sprintf(tmp2, "%s_scripts.qb", tmp);
        memcpy(&path[pos], tmp2, strlen(tmp2) + 1);
        vector<Script> sounds;
        LoadScriptsFile(path, scripts, sounds);
        LoadThugNodeArray(pFile, pFile + size, scripts, sounds, extraLayerMeshes);
        /*KnownScripts.clear();
        triggers.clear();*/
        KnownScripts.push_back(KnownScript(Checksum("LoadSounds"), NULL, 0));
        triggers.push_back(Checksum("LoadSounds"));
        KnownScript& script = KnownScripts.back();
        for (DWORD i = 0, numSounds = sounds.size(); i < numSounds; i++)
        {
          if (sounds[i].name[0] == 0)
          {
            script.Append(0x16);
            script.Append(Checksum("LoadSound"));
            script.Append(0x1B);
            DWORD tmpLen = strlen(tmp);
            char* soundString = (*(Checksum*)&sounds[i].checksum).GetString();
            DWORD soundLen = strlen(soundString) + 1;
            DWORD len = tmpLen + soundLen;
            script.Append(*(Checksum*)&len);
            char *loadsound = new char[len];
            sprintf(loadsound, "%s\\%s", tmp + 1, soundString);
            script.Append((BYTE*)loadsound, len);
            script.EndScript();
            delete[] loadsound;
          }
        }
        scripts.clear();
        sounds.clear();
        delete[] pFile;
        /*for(DWORD i=0, numNodes = nodes.size(); i<numNodes; i++)
        {
        if(nodes[i].Name.checksum==EnvironmentObject::GetClass())
        {
        if(GetMesh(nodes[i].Name)==NULL)
        {
        if(nodes[i].Trigger.checksum)
        {
        for(DWORD j=0; j<numNodes; j++)
        {
        if(nodes[i].Trigger.checksum==nodes[j].Trigger.checksum)
        goto found;
        }
        for(DWORD j=0; j<KnownScripts.size(); j++)
        {
        if(KnownScripts[j].name.checksum==nodes[i].Trigger.checksum)
        {
        KnownScripts.erase(KnownScripts.begin()+j);
        break;
        }
        }
        for(DWORD j=0; j<triggers.size(); j++)
        {
        if(triggers[j].checksum==nodes[i].Trigger.checksum)
        {
        triggers.erase(triggers.begin()+j);
        break;
        }
        }
        }
        found:
        DeleteNode(i);
        i--;
        numNodes--;
        continue;
        }
        }*/

        /*if(nodes[i].Trigger.checksum)
        {
        DWORD size;
        const void* pScript = GetScript(nodes[i].Trigger.checksum, &size);
        if(pScript==NULL || size==0)
        {

        for(DWORD j=0; j<KnownScripts.size(); j++)
        {
        if(KnownScripts[j].name.checksum==nodes[i].Trigger.checksum)
        {
        KnownScripts.erase(KnownScripts.begin()+j);
        break;
        }
        }
        for(DWORD j=0; j<triggers.size(); j++)
        {
        if(triggers[j].checksum==nodes[i].Trigger.checksum)
        {
        triggers.erase(triggers.begin()+j);
        break;
        }
        }
        nodes[i].Trigger=0;
        }
        }*/
        //}
      }
      sprintf(tmp2, "_Sky\%s_Sky.scn.xbx", tmp);
      //path[pos] = '\\';
      memcpy(&path[pos], tmp2, strlen(tmp2) + 1);
      MessageBox(0, path, tmp2, MB_OK);
      f = fopen(path, "rb");
      if (f)
      {
        skyDome = new Thug2Scene(path, true);
        fclose(f);
      }
      else
      {
        MAX_TEXCOORDS *= 0x100; Device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | MAX_TEXCOORDS);
      }
    }
    else
    {
      MAX_TEXCOORDS *= 0x100; Device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | MAX_TEXCOORDS);
    }

    bool found = false;
    char chc[128] = "";

    FixLink();

    for (DWORD i = 0, numMeshes = this->meshes.size(); i < numMeshes; i++)
    {
      found = false;
      for (DWORD j = 0; j < this->nodes.size(); j++)
      {
        if (this->nodes[j].Class.checksum == EnvironmentObject::GetClass() && this->nodes[j].Name.checksum == this->meshes[i].GetName().checksum)
        {
          //meshes[i].node=&this->nodes[j];
          found = true;
          break;
        }
      }
      if (!found)
      {
        sprintf(chc, "Mesh%u", i);
        Checksum Name = this->meshes[i].GetName();
        Scripts.push_back(Script(Name.checksum, chc));
        //DWORD shatter=0;
        /*if(this->meshes[i].shatterObject)
        {
        strcat(chc,"_shatter");

        shatter = Checksum(chc).checksum;
        triggers.push_back(*(Checksum*)&shatter);
        memcpy(&ShatterScript[27],&Name,4);
        memcpy(&ShatterScript[48],&Name,4);
        KnownScripts.push_back(KnownScript(*(Checksum*)&shatter, (void*)ShatterScript, 123));
        }*/

        this->nodes.push_back(EnvironmentObject(Name, 0, 0, true, false));
        //this->meshes[i].node = &this->nodes[this->nodes.size()-1];
      }
      if (meshes[i].GetCollision())
      {
        if (GetCombinedVertCount(&meshes[i]) > 1500)
        {
          sprintf(Name, "%s_Coll", meshes[i].GetName().GetString());
          meshes.push_back(Mesh());
          Mesh& mesh = meshes.back();
          mesh.SetFlag(MeshFlags::isVisible, false);
          meshes[i].MoveCollision(mesh);
          mesh.SetName(Checksum(Name));
          for (DWORD j = 0; j < KnownScripts.size(); j++)
          {
            BYTE* pFile = KnownScripts[j].func;
            BYTE* eof = pFile + KnownScripts[j].size;
            BYTE* offset;
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
                offset = pFile;
                break;
              case 1:
                offset = pFile;
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
                const DWORD qbKey = *(DWORD*)pFile;
                pFile += 4;
                if (qbKey == meshes[i].GetName().checksum)
                {
                  BYTE tmpData[1900];
                  DWORD size = (DWORD)((pFile)-offset);
                  DWORD backupSize = (DWORD)(eof - offset);
                  memcpy(tmpData, offset, backupSize);
                  *(DWORD*)&*(offset + size - 4) = mesh.GetName().checksum;
                  *(offset + size) = 0x2;
                  size++;
                  *(DWORD*)&*(offset + size) = 0x00000001;
                  size += 4;
                  KnownScripts[j].size += size;
                  eof += size;
                  pFile += size;
                  memcpy(offset + size, tmpData, backupSize);
                }
                else
                {
                  switch (qbKey)
                  {
                  case 0xEEC24148:
                  case 0xCB8B5900:
                  case 0x854F17A6:
                  case 0x70291F04:
                  case 0xF8D6A996:
                  case 0x6E77719D:
                    //kill, create, shatter, shatteranddie, visible, invisible
                    if (*pFile == 0x16)
                    {
                      offset = (pFile - 5);
                      pFile++;
                      if (*(DWORD*)pFile == 0xA1DC81F9)
                      {
                        pFile += 4;
                        while (*pFile != 0x16) pFile++;
                        pFile++;
                        if (*(DWORD*)pFile == meshes[i].GetName().checksum)
                        {
                          BYTE tmpData[1900];
                          DWORD size = (DWORD)((pFile + 4) - offset);
                          DWORD backupSize = (DWORD)(eof - offset);
                          memcpy(tmpData, offset, backupSize);
                          *(DWORD*)&*(offset + size - 4) = mesh.GetName().checksum;
                          *(offset + size) = 0x2;
                          size++;
                          *(DWORD*)&*(offset + size) = 0x00000001;
                          size += 4;
                          KnownScripts[j].size += size;
                          eof += size;
                          pFile += size;
                          memcpy(offset + size, tmpData, backupSize);
                        }

                        pFile += 4;
                      }
                      else
                        pFile += 4;
                    }
                    break;
                  }
                }
                break;
              }
            }
          done:;
          }
          Node* node = GetNode(meshes[i].GetName());
          if (node)
          {
            nodes.push_back(*node);
            Node & n = nodes.back();
            n.Name = mesh.GetName();
            if (n.TrickObject)
            {
              if (!n.Cluster.checksum)
              {
                n.Cluster = mesh.GetName();
                node = GetNode(meshes[i].GetName());
                node->Cluster = n.Cluster;
              }
            }
          }
        }
      }
    }



    /*for(DWORD j=0; j<KnownScripts.size(); j++)
    {
    BYTE* pFile = KnownScripts[j].func;
    BYTE* eof = pFile+KnownScripts[j].size;
    BYTE* offset;
    while(pFile<eof)
    {
    const BYTE opcode = *pFile;
    pFile++;
    switch(opcode)
    {
    case 0x1C:
    case 0x1B:
    pFile+=*(DWORD*)pFile+4;
    break;
    case 0x17:
    case 0x1A:
    case 0x2E:
    case 2:
    pFile+=4;
    break;
    case 0x1F:
    pFile+=8;
    break;
    case 0x1E:
    pFile+=12;
    break;
    case 0x2F:
    pFile+=*(DWORD*)pFile*4+4;
    break;
    case 0x16:
    const DWORD qbKey = *(DWORD*)pFile;
    pFile+=4;
    switch(qbKey)
    {
    case 0xEEC24148:
    case 0xCB8B5900:
    case 0x854F17A6:
    case 0x70291F04:
    case 0xF8D6A996:
    case 0x6E77719D:
    //kill, create, shatter, shatteranddie, visible, invisible
    if(*pFile==0x16)
    {
    offset=(pFile-5);
    pFile++;
    if(*(DWORD*)pFile==0xA1DC81F9)
    {
    pFile+=4;
    bool global=false;
    while(*pFile!=0x16) { if(*pFile==0x2D) global=true; pFile++;}
    pFile++;
    if(global)
    {
    pFile+=4;
    goto found;
    }
    DWORD name = *(DWORD*)pFile;
    pFile+=4;
    for(DWORD k=0; k<nodes.size(); k++)
    {
    if(nodes[k].Name.checksum==name)
    {
    if(nodes[k].Class.checksum==EnvironmentObject::GetClass())
    {
    for(DWORD l=0; l<meshes.size(); l++)
    {
    if(meshes[l].GetName().checksum==name)
    goto found;
    }
    }
    else
    goto found;
    break;
    }
    }
    while(pFile<eof)
    {
    const BYTE op = *pFile;
    pFile++;
    switch(op)
    {
    case 0x1C:
    case 0x1B:
    pFile+=*(DWORD*)pFile+4;
    break;

    case 2:
    pFile+=4;
    goto ya;
    break;
    case 1:
    goto ya;
    break;

    case 0x16:
    case 0x17:
    case 0x1A:
    case 0x2E:
    pFile+=4;
    break;
    case 0x1F:
    pFile+=8;
    break;
    case 0x1E:
    pFile+=12;
    break;
    case 0x2F:
    pFile+=*(DWORD*)pFile*4+4;
    break;
    }
    }
    ya:
    //MessageBox(0,(*(Checksum*)&name).GetString(),(*(Checksum*)&name).GetString(),0);
    DWORD backupSize = (DWORD)(eof-pFile);
    memcpy(offset, pFile, backupSize);
    DWORD size = (DWORD)(pFile-offset);
    KnownScripts[j].size-=size;
    eof-=size;
    pFile-=size;
    found:;
    }
    else
    pFile+=4;
    }
    break;
    }
    break;
    }
    }
    }*/
    //FixNodes();
    for (DWORD i = 0; i < extraLayerMeshes.size(); i++)
    {
      meshes.push_back(Mesh());
      Mesh& mesh = meshes.back();
      Node* node = GetNode(extraLayerMeshes[i].name);
      if (node)
      {
        if (!node->CreatedAtStart)
          sprintf(Name, "InvisibleExtraLayer_%s", extraLayerMeshes[i].name.GetString());
        else
          sprintf(Name, "ExtraLayer_%s", extraLayerMeshes[i].name.GetString());
        mesh.SetName(Name);
        nodes.push_back(*node);
        nodes.back().Name = mesh.GetName();
      }
      else
      {
        sprintf(Name, "ExInvisibleExtraLayer_%s", extraLayerMeshes[i].name.GetString());
        mesh.SetName(Name);
        nodes.push_back(EnvironmentObject(mesh.GetName(), 0, 0, false, false));
      }
      vector<DWORD> addedMaterials;
      DWORD vOffset = 0;
      Vertex v;
      for (DWORD j = 0; j < extraLayerMeshes[i].vertices.size(); j++)
      {
        v.x = extraLayerMeshes[i].vertices[j].x;
        v.y = extraLayerMeshes[i].vertices[j].y;
        v.z = extraLayerMeshes[i].vertices[j].z;
        v.colour = extraLayerMeshes[i].vertices[j].colour;
        if (v.colour.a)
          v.colour.a /= 2;
        v.tUV[0] = extraLayerMeshes[i].vertices[j].tUV[0];
        mesh.AddVertexSilent(v);
      }
      for (DWORD j = 0; j < extraLayerMeshes[i].matSplits.size(); j++)
      {
        MaterialSplit split;
        split.type = 0;

        Material* mat = matList.GetMaterial((Checksum)extraLayerMeshes[i].matSplits[j].matId);
        if (mat)
        {
          for (DWORD k = 0; k < addedMaterials.size(); k++)
          {
            if (addedMaterials[k] == mat->GetMatId())
              goto found;
          }
          if (mat)
          {
            Material material;
            material.Reserve(1);
            material.SetMatId(mat->GetMatId() + 1);
            material.doubled = mat->doubled;
            material.drawOrder = mat->drawOrder + 1.0f;
            material.ignoreAlpha = false;
            //material.alphaMap=true;
            material.extraLayer = true;
            Texture* texture = mat->GetTexture(1);
            if (texture->blendMode == 1 || texture->blendMode == 2 || texture->blendMode == 4 || texture->blendMode == 5 || texture->blendMode == 6)
            {
              static char imageName[MAX_PATH] = "";
              static char tmp[15] = "";
              GetEnvironmentVariable("TEMP", imageName, MAX_PATH);
              DWORD chc = texture->GetTexId();
              char* Name = (*(Checksum*)&(chc)).GetString();
              if (Name == NULL)
              {
                sprintf(tmp, "\\%08X.dds", texture->GetTexId());
                strcat(imageName, tmp);
              }
              else
              {
                DWORD len = strlen(imageName);
                imageName[len] = '\\';
                len++;
                strcpy(&imageName[len], name);
                strcat(imageName, ".dds");
              }
              //HRESULT hres = D3DXCreateTextureFromFile(Device, imageName, &imageData);
              FILE* f = fopen(imageName, "rb");
              if (f)
              {
                extern TexFiles texFiles;
                char imageName2[MAX_PATH];
                strcpy(imageName2, imageName);
                imageName2[strlen(imageName) - 3] = 't';
                imageName2[strlen(imageName) - 2] = 'g';
                imageName2[strlen(imageName) - 1] = 'a';
                for (DWORD l = 0; l < texFiles.numFiles; l++)
                {
                  if (!stricmp(texFiles.fileNames[l], imageName))
                  {
                    texFiles.fileNames[l][strlen(imageName) - 3] = 't';
                    texFiles.fileNames[l][strlen(imageName) - 2] = 'g';
                    texFiles.fileNames[l][strlen(imageName) - 1] = 'a';
                  }
                }
                fseek(f, 0, SEEK_END);
                DWORD size = ftell(f);
                BYTE* data = new BYTE[size];
                fseek(f, 0, SEEK_SET);
                fread(data, size, 1, f);
                fclose(f);
                DeleteFile(imageName);
                f = fopen(imageName2, "w+b");
                if (f)
                {
                  DWORD imageSize;
                  BYTE* imageData = data;
                  DDS_HEADER* header = (DDS_HEADER*)imageData;
                  imageData += sizeof(DDS_HEADER);

                  WORD width = (WORD)header->width;
                  WORD height = (WORD)header->height;

                  if (header->pixelFormat[1] & 0x4)
                  {
                    imageSize = header->width*header->height * 4;
                    BYTE* levelData = new BYTE[imageSize];
                    DecompressDXT(imageData, header->imageSize, header->width, header->height, levelData, (((header->pixelFormat[2] & 0xFF000000) >> 24) - 0x30));
                    fputc(0, f);
                    fputc(0, f);
                    fputc(2, f);
                    DWORD zero = 0;
                    fwrite(&zero, 4, 1, f);
                    fwrite(&zero, 4, 1, f);
                    fputc(0, f);
                    fwrite(&header->width, 2, 1, f);
                    fwrite(&header->height, 2, 1, f);
                    fputc(32, f);
                    fputc(32, f);
                    Colour* color = (Colour*)levelData;
                    FixBlending(color, imageSize / 4, texture);
                    fwrite(levelData, imageSize, 1, f);
                    delete[] levelData;
                  }
                  else
                  {
                    imageSize = header->width*header->height * 4;
                    DWORD bytesPerPixel = header->imageSize / header->width / header->height;
                    bool usingAlpha = header->pixelFormat[1] & 0x2;

                    if (bytesPerPixel == 4)
                    {
                      fputc(0, f);
                      fputc(0, f);
                      fputc(2, f);
                      DWORD zero = 0;
                      fwrite(&zero, 4, 1, f);
                      fwrite(&zero, 4, 1, f);
                      fputc(0, f);
                      fwrite(&header->width, 2, 1, f);
                      fwrite(&header->height, 2, 1, f);
                      fputc(32, f);
                      fputc(32, f);
                      Colour* color = (Colour*)imageData;
                      FixBlending(color, imageSize / 4, texture);
                      fwrite(imageData, imageSize, 1, f);
                    }
                    else if (bytesPerPixel == 2)
                    {
                      imageSize = header->width*header->height * 4;
                      DWORD numPixels = imageSize / 4;
                      Colour* levelData = new Colour[numPixels];
                      //MessageBox(0,"sure this is 16 bit?",imageName,0);
                      if (usingAlpha)
                      {
                        for (DWORD l = 0; l < numPixels; l++)
                        {
                          BYTE r = ((BYTE)((((WORD*)imageData)[l] & 0x003E) >> 1));
                          BYTE g = ((BYTE)((((WORD*)imageData)[l] & 0x7C0) >> 6));
                          BYTE b = ((BYTE)((((WORD*)imageData)[l] & 0xF800) >> 12));
                          BYTE a = ((BYTE)((((WORD*)imageData)[l] & 0x1)));
                          levelData[l].r = (r << 3) | (r >> 2);
                          levelData[l].g = (g << 3) | (g >> 2);
                          levelData[l].b = (b << 3) | (b >> 2);
                          if (a)
                            levelData[l].a = 255;
                          else
                            levelData[l].a = 0;
                        }
                      }
                      else
                      {
                        for (DWORD l = 0; l < numPixels; l++)
                        {
                          BYTE r = (BYTE)(((WORD*)imageData)[l] & 0x001F);
                          BYTE g = ((BYTE)((((WORD*)imageData)[l] & 0x7E0) >> 5));
                          BYTE b = ((BYTE)((((WORD*)imageData)[l] & 0xF800) >> 11));
                          levelData[l].r = (r << 3) | (r >> 2);
                          levelData[l].g = (g << 2) | (g >> 4);
                          levelData[l].b = (b << 3) | (b >> 2);
                          levelData[l].a = 255;
                        }
                      }
                      fputc(0, f);
                      fputc(0, f);
                      fputc(2, f);
                      DWORD zero = 0;
                      fwrite(&zero, 4, 1, f);
                      fwrite(&zero, 4, 1, f);
                      fputc(0, f);
                      fwrite(&header->width, 2, 1, f);
                      fwrite(&header->height, 2, 1, f);
                      fputc(32, f);
                      fputc(32, f);
                      Colour* color = (Colour*)levelData;
                      FixBlending(color, imageSize / 4, texture);
                      fwrite(levelData, imageSize, 1, f);
                      delete[] levelData;
                      //delete [] levelData;
                    }
                    else if (bytesPerPixel == 1)
                      MessageBox(0, "1", 0, 0);
                    else
                      MessageBox(0, "unknown", 0, 0);

                    //MessageBox(0,"wtf this texture is not compressed","wawwiee",0);
                    //levelData = (pImageData;
                  }
                  fclose(f);
                }

                delete[] data;
              }
              /*else
              {
              GetEnvironmentVariable("TEMP",imageName, MAX_PATH);
              if(name == NULL)
              {
              sprintf(tmp,"\\%08X.tga\0",texture->GetTexId());
              strcat(imageName,tmp);
              }
              else
              {
              DWORD len = strlen(imageName);
              imageName[len] = '\\';
              len++;
              strcpy(&imageName[len],name);
              strcat(imageName,".tga");
              }
              f = fopen(imageName, "rb");
              if(f)
              {
              //MessageBox(0,imageName,imageName,0);
              }
              }*/
              //material.reverse=true;
            }
            /*else
            {
            static char imageName[MAX_PATH] = "";
            static char tmp[15] = "";
            GetEnvironmentVariable("TEMP",imageName, MAX_PATH);
            DWORD chc = texture->GetTexId();
            char* name=(*(Checksum*)&(chc)).GetString();
            if(name == NULL)
            {
            sprintf(tmp,"\\%08X.dds",texture->GetTexId());
            strcat(imageName,tmp);
            }
            else
            {
            DWORD len = strlen(imageName);
            imageName[len] = '\\';
            len++;
            strcpy(&imageName[len],name);
            strcat(imageName,".dds");
            }
            //HRESULT hres = D3DXCreateTextureFromFile(Device, imageName, &imageData);
            FILE* f = fopen(imageName, "rb");
            if(f)
            {

            }
            else
            {
            GetEnvironmentVariable("TEMP",imageName, MAX_PATH);
            if(name == NULL)
            {
            sprintf(tmp,"\\%08X.tga\0",texture->GetTexId());
            strcat(imageName,tmp);
            }
            else
            {
            DWORD len = strlen(imageName);
            imageName[len] = '\\';
            len++;
            strcpy(&imageName[len],name);
            strcat(imageName,".tga");
            }
            f = fopen(imageName, "rb");
            if(f)
            {

            }
            }
            }*/
            //material.transparent = true;
            material.AddTexture2(texture);
            material.GetTexture(0)->CreateTexture();
            matList.AddMaterial(&material);
            addedMaterials.push_back(mat->GetMatId());
          }
        found:
          split.matId = mat->GetMatId() + 1;
          split.numIndices = extraLayerMeshes[i].matSplits[j].Indices.size();
          if (split.numIndices)
          {
            split.AddIndices(&extraLayerMeshes[i].matSplits[j].Indices.front(), split.numIndices, vOffset);
            mesh.AddMatSplit(&split, &matList);
          }
          else
            MessageBox(0, "no indices3?", "", 0);
        }
        //vOffset+=extraLayerMeshes[i].vertices[j].size();
      }
      for (DWORD j = 1; j < extraLayerMeshes[i].numPasses && j < 3; j++)
      {
        meshes.push_back(Mesh());
        Mesh & mesh2 = meshes.back();//th4
        Node* node2 = GetNode(extraLayerMeshes[i].name);
        if (node2)
        {
          if (!node2->CreatedAtStart)
            sprintf(Name, "InvisibleExtraLayer%u_%s", j, extraLayerMeshes[i].name.GetString());
          else
            sprintf(Name, "ExtraLayer%u_%s", j, extraLayerMeshes[i].name.GetString());
          mesh2.SetName(Name);
          nodes.push_back(*node2);
          nodes.back().Name = mesh2.GetName();
        }
        else
        {
          sprintf(Name, "ExInvisibleExtraLayer%u_%s", j, extraLayerMeshes[i].name.GetString());
          mesh2.SetName(Name);
          nodes.push_back(EnvironmentObject(mesh2.GetName(), 0, 0, false, false));
        }
        Vertex v2;
        for (DWORD k = 0; k < extraLayerMeshes[i].vertices.size(); k++)
        {
          v2.x = extraLayerMeshes[i].vertices[k].x;
          v2.y = extraLayerMeshes[i].vertices[k].y;
          v2.z = extraLayerMeshes[i].vertices[k].z;
          v2.colour = extraLayerMeshes[i].vertices[k].colour;
          if (v2.colour.a)
            v2.colour.a /= 2;
          v2.tUV[0] = extraLayerMeshes[i].vertices[k].tUV[j];
          mesh2.AddVertexSilent(v2);
        }
        for (DWORD k = 0; k<extraLayerMeshes[i].matSplits.size(); k++)
        {
          MaterialSplit split;
          split.type = 0;


          Material* mat = matList.GetMaterial((Checksum)extraLayerMeshes[i].matSplits[k].matId);
          if (mat && mat->GetNumTextures()>j + 1)
          {
            if (mat)
            {
              Material material;
              material.Reserve(1);
              material.SetMatId(mat->GetMatId() + j + 1);
              material.doubled = mat->doubled;
              material.drawOrder = mat->drawOrder + (float)j + 1.0f;
              material.ignoreAlpha = false;
              //material.alphaMap=true;
              material.extraLayer = true;
              Texture* texture = mat->GetTexture(j + 1);
              if (texture->blendMode == 1 || texture->blendMode == 2 || texture->blendMode == 4 || texture->blendMode == 5 || texture->blendMode == 6)
              {
                static char imageName[MAX_PATH] = "";
                static char tmp[15] = "";
                GetEnvironmentVariable("TEMP", imageName, MAX_PATH);
                DWORD chc = texture->GetTexId();
                char* Name = (*(Checksum*)&(chc)).GetString();
                if (Name == NULL)
                {
                  sprintf(tmp, "\\%08X.dds", texture->GetTexId());
                  strcat(imageName, tmp);
                }
                else
                {
                  DWORD len = strlen(imageName);
                  imageName[len] = '\\';
                  len++;
                  strcpy(&imageName[len], name);
                  strcat(imageName, ".dds");
                }
                //HRESULT hres = D3DXCreateTextureFromFile(Device, imageName, &imageData);
                FILE* f = fopen(imageName, "rb");
                if (f)
                {
                  extern TexFiles texFiles;
                  char imageName2[MAX_PATH];
                  strcpy(imageName2, imageName);
                  imageName2[strlen(imageName) - 3] = 't';
                  imageName2[strlen(imageName) - 2] = 'g';
                  imageName2[strlen(imageName) - 1] = 'a';
                  for (DWORD l = 0; l < texFiles.numFiles; l++)
                  {
                    if (!stricmp(texFiles.fileNames[l], imageName))
                    {
                      texFiles.fileNames[l][strlen(imageName) - 3] = 't';
                      texFiles.fileNames[l][strlen(imageName) - 2] = 'g';
                      texFiles.fileNames[l][strlen(imageName) - 1] = 'a';
                    }
                  }
                  fseek(f, 0, SEEK_END);
                  DWORD size = ftell(f);
                  BYTE* data = new BYTE[size];
                  fseek(f, 0, SEEK_SET);
                  fread(data, size, 1, f);
                  fclose(f);
                  DeleteFile(imageName);
                  f = fopen(imageName2, "w+b");
                  if (f)
                  {
                    DWORD imageSize;
                    BYTE* imageData = data;
                    DDS_HEADER* header = (DDS_HEADER*)imageData;
                    imageData += sizeof(DDS_HEADER);

                    WORD width = (WORD)header->width;
                    WORD height = (WORD)header->height;

                    if (header->pixelFormat[1] & 0x4)
                    {
                      imageSize = header->width*header->height * 4;
                      BYTE* levelData = new BYTE[imageSize];
                      DecompressDXT(imageData, header->imageSize, header->width, header->height, levelData, (((header->pixelFormat[2] & 0xFF000000) >> 24) - 0x30));
                      fputc(0, f);
                      fputc(0, f);
                      fputc(2, f);
                      DWORD zero = 0;
                      fwrite(&zero, 4, 1, f);
                      fwrite(&zero, 4, 1, f);
                      fputc(0, f);
                      fwrite(&header->width, 2, 1, f);
                      fwrite(&header->height, 2, 1, f);
                      fputc(32, f);
                      fputc(32, f);
                      Colour* color = (Colour*)levelData;
                      FixBlending(color, imageSize / 4, texture);
                      fwrite(levelData, imageSize, 1, f);
                      delete[] levelData;
                    }
                    else
                    {
                      imageSize = header->width*header->height * 4;
                      DWORD bytesPerPixel = header->imageSize / header->width / header->height;
                      bool usingAlpha = header->pixelFormat[1] & 0x2;

                      if (bytesPerPixel == 4)
                      {
                        fputc(0, f);
                        fputc(0, f);
                        fputc(2, f);
                        DWORD zero = 0;
                        fwrite(&zero, 4, 1, f);
                        fwrite(&zero, 4, 1, f);
                        fputc(0, f);
                        fwrite(&header->width, 2, 1, f);
                        fwrite(&header->height, 2, 1, f);
                        fputc(32, f);
                        fputc(32, f);
                        Colour* color = (Colour*)imageData;
                        FixBlending(color, imageSize / 4, texture);
                        fwrite(imageData, imageSize, 1, f);
                      }
                      else if (bytesPerPixel == 2)
                      {
                        imageSize = header->width*header->height * 4;
                        DWORD numPixels = imageSize / 4;
                        Colour* levelData = new Colour[numPixels];
                        //MessageBox(0,"sure this is 16 bit?",imageName,0);
                        if (usingAlpha)
                        {
                          for (DWORD l = 0; l < numPixels; l++)
                          {
                            BYTE r = ((BYTE)((((WORD*)imageData)[l] & 0x003E) >> 1));
                            BYTE g = ((BYTE)((((WORD*)imageData)[l] & 0x7C0) >> 6));
                            BYTE b = ((BYTE)((((WORD*)imageData)[l] & 0xF800) >> 12));
                            BYTE a = ((BYTE)((((WORD*)imageData)[l] & 0x1)));
                            levelData[l].r = (r << 3) | (r >> 2);
                            levelData[l].g = (g << 3) | (g >> 2);
                            levelData[l].b = (b << 3) | (b >> 2);
                            if (a)
                              levelData[l].a = 255;
                            else
                              levelData[l].a = 0;
                          }
                        }
                        else
                        {
                          for (DWORD l = 0; l < numPixels; l++)
                          {
                            BYTE r = (BYTE)(((WORD*)imageData)[l] & 0x001F);
                            BYTE g = ((BYTE)((((WORD*)imageData)[l] & 0x7E0) >> 5));
                            BYTE b = ((BYTE)((((WORD*)imageData)[l] & 0xF800) >> 11));
                            levelData[l].r = (r << 3) | (r >> 2);
                            levelData[l].g = (g << 2) | (g >> 4);
                            levelData[l].b = (b << 3) | (b >> 2);
                            levelData[l].a = 255;
                          }
                        }
                        fputc(0, f);
                        fputc(0, f);
                        fputc(2, f);
                        DWORD zero = 0;
                        fwrite(&zero, 4, 1, f);
                        fwrite(&zero, 4, 1, f);
                        fputc(0, f);
                        fwrite(&header->width, 2, 1, f);
                        fwrite(&header->height, 2, 1, f);
                        fputc(32, f);
                        fputc(32, f);
                        Colour* color = (Colour*)levelData;
                        FixBlending(color, imageSize / 4, texture);
                        fwrite(levelData, imageSize, 1, f);
                        delete[] levelData;
                        //delete [] levelData;
                      }
                      else if (bytesPerPixel == 1)
                        MessageBox(0, "1", 0, 0);
                      else
                        MessageBox(0, "unknown", 0, 0);

                      //MessageBox(0,"wtf this texture is not compressed","wawwiee",0);
                      //levelData = (pImageData;
                    }
                    fclose(f);
                  }

                  delete[] data;
                }
                /*else
                {
                GetEnvironmentVariable("TEMP",imageName, MAX_PATH);
                if(name == NULL)
                {
                sprintf(tmp,"\\%08X.tga\0",texture->GetTexId());
                strcat(imageName,tmp);
                }
                else
                {
                DWORD len = strlen(imageName);
                imageName[len] = '\\';
                len++;
                strcpy(&imageName[len],name);
                strcat(imageName,".tga");
                }
                f = fopen(imageName, "rb");
                if(f)
                {
                //MessageBox(0,imageName,imageName,0);
                }
                //material.reverse=true;
                }*/
              }
              _fcloseall();
              // material.transparent = true;
              material.AddTexture2(texture);
              material.GetTexture(0)->CreateTexture();
              matList.AddMaterial(&material);
            }
            split.matId = mat->GetMatId() + j + 1;
            split.numIndices = extraLayerMeshes[i].matSplits[k].Indices.size();
            if (split.numIndices)
            {
              split.AddIndices(&extraLayerMeshes[i].matSplits[k].Indices.front(), split.numIndices, vOffset);
              mesh2.AddMatSplit(&split, &matList);
            }
            else
              MessageBox(0, "no indices4?", "", 0);
          }
        }
      }
    }
    extraLayerMeshes.clear();
  }
}

DWORD ThugScene::ProcessMaterials(DWORD ptr, bool sky)
{
  DWORD *numMaterials = (DWORD*)ptr;
  ptr += 4;
  for (DWORD i = 0; i<*numMaterials; i++)
  {

    ThugMaterial::Data* materialData = (ThugMaterial::Data*)ptr;
    ThugMaterial material = ThugMaterial(materialData);

    ptr += 28; if (materialData->numTextures>MAX_PASSES) MAX_PASSES = materialData->numTextures;
    if (materialData->unkFlag)
      ptr += 8;
    //ptr+=4;
    /*if(materialData->flags & 4 || materialData->flags & 128)
      ptr+=12;*/
    float power = *(float*)ptr;
    ptr += 4;
    if (power > 0.0f)
      ptr += 12;

    for (DWORD j = 0; j < materialData->numTextures; j++)
    {
      ThugTexture::Data* textureData = (ThugTexture::Data*)ptr;
      ThugTexture texture = ThugTexture(textureData);
      /*if (j == 0)
      {
        if (textureData->flags & MATFLAG_PASS_IGNORE_VERTEX_ALPHA)
          material.ignoreAlpha = true;
        if (materialData->doubled || (!materialData->singledSided && (((texture.blendMode | texture.fixedAlpha) & 0x00FFFFFFUL) != 0x00)))
          material.doubled = true;
      }*/
      //material.AddTexture(&texture);
      if (material.transparent)
      {
        material.doubled = true;
        //material.drawOrder *= 10.0f;
        material.drawOrder++;
        /*if(material->drawOrder == 0.0f)
        material->drawOrder=1.0f;*/
      }
      /*if(!(textureData->flags & (1<<3)))
      {
      material.AddTexture(&texture);
      if(material.transparent)
      {
      material.drawOrder++;
      }
      }
      else if(j==0 && materialData->numTextures==1)
      {
      material.AddTexture(&texture);
      material.invisible=true;
      material.transparent=true;

      }*/
      /*if (textureData->flags & 8)
      {
        material.invisible = true;
      }*/
      /*if (textureData->flags & (1 << 3))
      {
        if (material.GetNumTextures() == 1 && j + 1 == materialData->numTextures)
        {
          //if(material.GetNumTextures()==0)
          material.fucked = j;
          material.invisible = true;
          material.transparent = true;
        }
      }
      else
      {
        if (j != 0 && material.GetNumTextures() == 1)
          material.fucked = j;
        if (sky)
          material.drawOrder = 0;
      }*/
      if (textureData->flags & (1 << 3))
      {
        if (material.GetNumTextures() == 0 && j + 1 == materialData->numTextures)
        {
          //if(material.GetNumTextures()==0)
          material.fucked = j;
          material.AddTexture(&texture);
          material.invisible = true;
          material.transparent = true;
        }
      }
      else
      {
        if (j != 0 && material.GetNumTextures() == 0)
          material.fucked = j;
        material.AddTexture(&texture);
        if (sky)
          material.drawOrder = 0;
      }
      //delete texture;
      ptr += 49;

      if (textureData->flags & TEXFLAG_UNKNOWN)
      {
        if (j == 0)
        {
          this->animations.push_back(UVAnim(material.GetMatId(), (Animation*)ptr));
          material.animated = true;
        }
        ptr += 32;
      }
      if (j == 0 && textureData->flags & TEXFLAG_EXTENSION)
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
      /*if(textureData->flags & TEXFLAG_EXTENSION2)
      {
      DWORD *numExtensions = (DWORD*)ptr;
      ptr+=16;

      for(DWORD k=0; k<*numExtensions; k++)
      {
      if(k==0)
      material->GetTexture(j)->SetId(((DWORD*)ptr)[0]);
      ptr+=8;
      }
      }
      if(material->GetTexture(j)->GetTexId())*/
      ptr += 16;
    }
    AddMaterial(&material);
    //delete material;
  }
  return ptr;
}

void ThugScene::ProcessMeshes(DWORD ptr)
{
  DWORD numObjects = ((DWORD*)ptr)[0];
  ptr += 4; meshes.reserve(numObjects);
  nodes.reserve(numObjects);
  for (DWORD i = 0; i < numObjects; i++)
  {

    ThugMesh::Data* meshData = (ThugMesh::Data*)malloc(sizeof(ThugMesh::Data));
    memcpy(meshData, (ThugMesh::Data*)ptr, 56);
    Mesh* mesh = new ThugMesh(meshData);

    ptr += 56; if (meshData->flags & MESHFLAG_HEADEREXTENSION) ptr += 40;

    meshData->numVertices = ((DWORD*)ptr)[0]; ptr += 8;
    meshData->vertices = (SimpleVertex*)ptr;
    ptr += 12 * meshData->numVertices;

    if (meshData->flags & MESHFLAG_NORMAL)
    {
      meshData->normals = (SimpleVertex*)ptr;
      ptr += 12 * meshData->numVertices;
    }
    else
      meshData->normals = NULL;

    if (meshData->flags & MESHFLAG_SKIN)
    {
      ptr += 32 * meshData->numVertices;
    }
    DWORD numTexCoords = 0;
    if (meshData->flags & MESHFLAG_TEXCOORD)
    {
      //meshData->texCoords = new TexCoord[meshData->numVertices];

      numTexCoords = ((DWORD*)ptr)[0]; meshData->texCoords = new TexCoord[meshData->numVertices*numTexCoords]; if (numTexCoords > MAX_TEXCOORDS) MAX_TEXCOORDS = numTexCoords;
      ptr += 4;
      for (DWORD j = 0; j < meshData->numVertices*numTexCoords; j++)
      {
        meshData->texCoords[j] = ((TexCoord*)ptr)[0];
        ptr += 8;
      }

    }
    else
      meshData->texCoords = NULL;

    if (meshData->flags & MESHFLAG_COLOUR)
    {
      meshData->colours = (Colour*)ptr;
      ptr += 4 * meshData->numVertices;
    }
    else
      meshData->colours = NULL;

    if (meshData->flags & MESHFLAG_EXTENSION)
    {
      ptr += meshData->numVertices;//colour wibble data?
    }


    Vertex* vertices = new Vertex[meshData->numVertices];

    for (int j = 0; j < meshData->numVertices; j++)
    {
      meshData->vertices[j].z *= -1;
      memcpy(&vertices[j], &meshData->vertices[j], 12);

      if (meshData->texCoords)
      {
        /*for(DWORD k=0; k<numTexCoords; k++)
        {
        vertices[j].uv[k*2] = meshData->texCoords[j+k].u;
        vertices[j].uv[k*2+1] = meshData->texCoords[j+k].v;
        }*/
        /*for(DWORD k=0; k<numTexCoords; k++)
        {
        vertices[j].tUV[k].u = meshData->texCoords[j*numTexCoords+k].u;
        vertices[j].tUV[k].v = meshData->texCoords[j*numTexCoords+k].v;
        }

        for(DWORD k=numTexCoords; k<4; k++)
        {
        vertices[j].tUV[k].u = meshData->texCoords[j*numTexCoords+numTexCoords].u;
        vertices[j].tUV[k].v = meshData->texCoords[j*numTexCoords+numTexCoords].v;
        }*/

        vertices[j].tUV[0] = meshData->texCoords[j*numTexCoords];

        /*vertices[j].u = meshData->texCoords[j].u;
        vertices[j].v = meshData->texCoords[j].v;*/
      }

      if (meshData->colours)
      {
        //meshData->colours[j].a = 0xFF;
        vertices[j].colour = meshData->colours[j]; //if(vertices[j].colour.a<128) vertices[j].colour.a *=2; else if(vertices[j].colour.a==128) vertices[j].colour.a = 255;
      }

    }

    mesh->Reserve(meshData->numVertices);
    mesh->AddVertices(vertices, meshData->numVertices, true);

    delete[] vertices;
    //delete [] meshData->texCoords;

    for (DWORD j = 0; j < meshData->numSplits; j++)
    {
      ptr += 44;
      MaterialSplit matSplit;
      matSplit.matId = ((DWORD*)ptr)[0];
      matSplit.type = 0;//((DWORD*)ptr)[1];
      matSplit.numIndices = ((DWORD*)ptr)[2];
      ptr += 12;

      WORD* Indices = (WORD*)ptr;
      matSplit.AddIndices(Indices, matSplit.numIndices);

      DWORD id = matSplit.matId;
      Material* mat = matList.GetMaterial((Checksum)id);
      if (mat)
      {
        Texture* texture = mat->GetTexture(0);
        if (texture)
        {
          if (texture->blendMode == 1 || texture->blendMode == 2 || texture->blendMode == 4 || texture->blendMode == 5 || texture->blendMode == 6)
          {
            static char imageName[MAX_PATH] = "";
            static char tmp[15] = "";
            GetEnvironmentVariable("TEMP", imageName, MAX_PATH);
            DWORD chc = texture->GetTexId();
            char* Name = (*(Checksum*)&(chc)).GetString();
            if (Name == NULL)
            {
              sprintf(tmp, "\\%08X.dds", texture->GetTexId());
              strcat(imageName, tmp);
            }
            else
            {
              DWORD len = strlen(imageName);
              imageName[len] = '\\';
              len++;
              strcpy(&imageName[len], Name);
              strcat(imageName, ".dds");
            }
            //HRESULT hres = D3DXCreateTextureFromFile(Device, imageName, &imageData);
            FILE* f = fopen(imageName, "rb");
            if (f)
            {
              extern TexFiles texFiles;
              char imageName2[MAX_PATH];
              strcpy(imageName2, imageName);
              imageName2[strlen(imageName) - 3] = 't';
              imageName2[strlen(imageName) - 2] = 'g';
              imageName2[strlen(imageName) - 1] = 'a';
              for (DWORD l = 0; l < texFiles.numFiles; l++)
              {
                if (!stricmp(texFiles.fileNames[l], imageName))
                {
                  texFiles.fileNames[l][strlen(imageName) - 3] = 't';
                  texFiles.fileNames[l][strlen(imageName) - 2] = 'g';
                  texFiles.fileNames[l][strlen(imageName) - 1] = 'a';
                }
              }
              fseek(f, 0, SEEK_END);
              DWORD size = ftell(f);
              BYTE* data = new BYTE[size];
              fseek(f, 0, SEEK_SET);
              fread(data, size, 1, f);
              fclose(f);
              DeleteFile(imageName);
              f = fopen(imageName2, "w+b");
              if (f)
              {
                DWORD imageSize;
                BYTE* imageData = data;
                DDS_HEADER* header = (DDS_HEADER*)imageData;
                imageData += sizeof(DDS_HEADER);

                WORD width = (WORD)header->width;
                WORD height = (WORD)header->height;

                if (header->pixelFormat[1] & 0x4)
                {
                  imageSize = header->width*header->height * 4;
                  BYTE* levelData = new BYTE[imageSize];
                  DecompressDXT(imageData, header->imageSize, header->width, header->height, levelData, (((header->pixelFormat[2] & 0xFF000000) >> 24) - 0x30));
                  fputc(0, f);
                  fputc(0, f);
                  fputc(2, f);
                  DWORD zero = 0;
                  fwrite(&zero, 4, 1, f);
                  fwrite(&zero, 4, 1, f);
                  fputc(0, f);
                  fwrite(&header->width, 2, 1, f);
                  fwrite(&header->height, 2, 1, f);
                  fputc(32, f);
                  fputc(32, f);
                  Colour* color = (Colour*)levelData;
                  FixBlending(color, imageSize / 4, texture);
                  fwrite(levelData, imageSize, 1, f);
                  delete[] levelData;
                }
                else
                {
                  imageSize = header->width*header->height * 4;
                  DWORD bytesPerPixel = header->imageSize / header->width / header->height;
                  bool usingAlpha = header->pixelFormat[1] & 0x2;

                  if (bytesPerPixel == 4)
                  {
                    fputc(0, f);
                    fputc(0, f);
                    fputc(2, f);
                    DWORD zero = 0;
                    fwrite(&zero, 4, 1, f);
                    fwrite(&zero, 4, 1, f);
                    fputc(0, f);
                    fwrite(&header->width, 2, 1, f);
                    fwrite(&header->height, 2, 1, f);
                    fputc(32, f);
                    fputc(32, f);
                    Colour* color = (Colour*)imageData;
                    FixBlending(color, imageSize / 4, texture);
                    fwrite(imageData, imageSize, 1, f);
                  }
                  else if (bytesPerPixel == 2)
                  {
                    imageSize = header->width*header->height * 4;
                    DWORD numPixels = imageSize / 4;
                    Colour* levelData = new Colour[numPixels];
                    //MessageBox(0,"sure this is 16 bit?",imageName,0);
                    if (usingAlpha)
                    {
                      for (DWORD l = 0; l < numPixels; l++)
                      {
                        BYTE r = ((BYTE)((((WORD*)imageData)[l] & 0x003E) >> 1));
                        BYTE g = ((BYTE)((((WORD*)imageData)[l] & 0x7C0) >> 6));
                        BYTE b = ((BYTE)((((WORD*)imageData)[l] & 0xF800) >> 12));
                        BYTE a = ((BYTE)((((WORD*)imageData)[l] & 0x1)));
                        levelData[l].r = (r << 3) | (r >> 2);
                        levelData[l].g = (g << 3) | (g >> 2);
                        levelData[l].b = (b << 3) | (b >> 2);
                        if (a)
                          levelData[l].a = 255;
                        else
                          levelData[l].a = 0;
                      }
                    }
                    else
                    {
                      for (DWORD l = 0; l < numPixels; l++)
                      {
                        BYTE r = (BYTE)(((WORD*)imageData)[l] & 0x001F);
                        BYTE g = ((BYTE)((((WORD*)imageData)[l] & 0x7E0) >> 5));
                        BYTE b = ((BYTE)((((WORD*)imageData)[l] & 0xF800) >> 11));
                        levelData[l].r = (r << 3) | (r >> 2);
                        levelData[l].g = (g << 2) | (g >> 4);
                        levelData[l].b = (b << 3) | (b >> 2);
                        levelData[l].a = 255;
                      }
                    }
                    fputc(0, f);
                    fputc(0, f);
                    fputc(2, f);
                    DWORD zero = 0;
                    fwrite(&zero, 4, 1, f);
                    fwrite(&zero, 4, 1, f);
                    fputc(0, f);
                    fwrite(&header->width, 2, 1, f);
                    fwrite(&header->height, 2, 1, f);
                    fputc(32, f);
                    fputc(32, f);
                    Colour* color = (Colour*)levelData;
                    FixBlending(color, imageSize / 4, texture);
                    fwrite(levelData, imageSize, 1, f);
                    delete[] levelData;
                    //delete [] levelData;
                  }
                }
                fclose(f);
              }
              delete[] data;
            }
          }
          //mat->transparent = true;
          texture->Release();
          texture->CreateTexture();
        }
      }
      if (numTexCoords > 1)
      {
        //Material* mat = matList.GetMaterial((Checksum)id);
        if (mat && mat->GetNumTextures() > 1)
        {
          for (DWORD k = 0; k < extraLayerMeshes.size(); k++)
          {
            if (extraLayerMeshes[k].name.checksum == meshData->name.checksum)
            {
              extraLayerMeshes[k].matSplits.push_back(MaterialSplit());
              extraLayerMeshes[k].matSplits.back().matId = matSplit.matId;
              extraLayerMeshes[k].matSplits.back().numIndices = matSplit.numIndices;
              extraLayerMeshes[k].matSplits.back().Indices.assign(matSplit.Indices.begin(), matSplit.Indices.end());
              extraLayerMeshes[k].numPasses = (numTexCoords - 1);
              //extraLayerMeshes[k].pass = mat->fucked;
              goto found;
            }
          }
          extraLayerMeshes.push_back(ExtraLayerMesh(meshData->name, &matSplit, mesh->GetVertices(), mesh->GetNumVertices(), meshData->texCoords, numTexCoords));
        }
      }
    found://*/
      if (numTexCoords > 1)
      {
        Material* mat = matList.GetMaterial(matSplit.matId);
        if (mat)
        {
          if (mat->fucked && mat->fucked < numTexCoords)
          {
            Vertex* verts = (Vertex*)mesh->GetVertices();
            for (DWORD k = 0; k < matSplit.Indices.size(); k++)
            {
              if (matSplit.Indices[k] < mesh->GetNumVertices())
                verts[matSplit.Indices[k]].tUV[0] = meshData->texCoords[matSplit.Indices[k] * numTexCoords + mat->fucked];
            }
          }
        }
      }
      mesh->AddMatSplit(&matSplit, &matList);

      //mesh->AddMatSplit(&matSplit, &matList);
      ptr += 2 * matSplit.numIndices;
      //delete matSplit;
    }
    delete[] meshData->texCoords;
    free(meshData);
    /*nodes.push_back(EnvironmentObject(mesh->GetName(), 0, 0, true));
    mesh->node = &nodes.back();
    mesh->node->SetPosition(mesh->GetCenter());*/
    AddMesh(mesh);
    delete mesh;
  }

}

DWORD GetCombinedVertCountTHUG(NSCollisionObject* collision, Mesh* mesh)
{
  DWORD vertCount = mesh->GetNumVertices();

  WORD numVerts = 0;
  SimpleVertex* uniqueVerts = collision->GetUniqueVertsTHUG(&numVerts);
  vertCount += (DWORD)numVerts;
  const Vertex* v = mesh->GetVertices();
  for (DWORD i = 0; i < numVerts; i++)
  {
    for (DWORD j = 0; j < mesh->GetNumVertices(); j++)
    {
      if (uniqueVerts[i].x == v[j].x && uniqueVerts[i].y == v[j].y && uniqueVerts[i].z == v[j].z)
        vertCount--;
    }
  }
  delete[] uniqueVerts;
  return vertCount;
}

ThugScene::ThugScene(char* Path, bool sky)
{
  ZeroMemory(name, MAX_PATH);
  SetName(Path); blendValue = 2;
  cullMode = D3DCULL_NONE;

  BYTE*pFile = NULL;
  DWORD size = 0, ptr = 0;
  char path[256] = "";

  memcpy(path, Path, strlen(Path) + 1);
  DWORD len = strlen(Path);

  if (strstr(Path, ".skin.xbx"))
  {
    path[len - 8] = 't';
    path[len - 7] = 'e';
    path[len - 6] = 'x';
    path[len - 5] = '.';
    path[len - 4] = 'x';
    path[len - 3] = 'b';
    path[len - 2] = 'x';
    path[len - 1] = '\0';
  }
  else
  {
    path[len - 7] = 't';
    path[len - 6] = 'e';
    path[len - 5] = 'x';
  }

  FILE*f = fopen(path, "rb");
  if (f)
  {
    fseek(f, 0, SEEK_END);

    size = ftell(f);

    pFile = new BYTE[size];

    fseek(f, 0, SEEK_SET);
    fread(pFile, size, 1, f);
    fclose(f);

    ptr = (DWORD)(pFile);
    ptr += 4;
    ProcessTexFile(ptr, sky);

    delete[] pFile;
  }
  f = fopen(Path, "rb");
  if (f)
  {
    fseek(f, 0, SEEK_END);

    size = ftell(f);

    pFile = new BYTE[size];

    fseek(f, 0, SEEK_SET);
    fread(pFile, size, 1, f);
    fclose(f);

    ptr = (DWORD)(pFile);
    ptr += 12;

    ptr = ProcessMaterials(ptr, sky);
    ProcessMeshes(ptr);

    delete[] pFile;
  }
  len = strlen(path);
  path[len - 7] = 'c';
  path[len - 6] = 'o';
  path[len - 5] = 'l';
  f = fopen(path, "rb");
  if (f)
  {
    fseek(f, 0, SEEK_END);

    size = ftell(f);

    pFile = new BYTE[size];

    fseek(f, 0, SEEK_SET);
    fread(pFile, size, 1, f);
    fclose(f);

    ptr = (DWORD)(pFile);
    NSCollisionHeader header = ((NSCollisionHeader*)ptr)[0];
    NSCollisionObject* collision = ProcessNSCollision(ptr);

    for (DWORD i = 0; i < header.numObjects; i++)
    {
      if (collision[i].numVerts != 0)
      {
        Mesh* tmpMesh = GetMesh(*(Checksum*)&collision[i].Name);
        if (tmpMesh)
        {
          /*if(GetCombinedVertCountTHUG(&collision[i], tmpMesh)>750)//650?|600+atleast
          {
          MessageBox(0,tmpMesh->GetName().GetString(),"",0);
          const DWORD index = meshes.size();
          meshes.push_back(Mesh());
          meshes[index].AddCollision(collision[i], header.Version);
          meshes[index].UnSetFlag(MeshFlags::isVisible);
          collision[i].Name++;
          meshes[index].SetName(*(Checksum*)&collision[i].Name);
          }
          else*/
          tmpMesh->AddCollision(collision[i], header.Version);
        }
        else
        {
          const DWORD index = meshes.size();
          meshes.push_back(Mesh());
          meshes[index].AddCollision(collision[i], header.Version);
          meshes[index].UnSetFlag(MeshFlags::isVisible);
          meshes[index].SetName(*(Checksum*)&collision[i].Name);
        }
      }
    }

    /*currentfor(DWORD i=0; i<header.numObjects; i++)
    {
    if(collision[i].numVerts!=0)
    {
    Mesh* tmpMesh = GetMesh(*(Checksum*)&collision[i].Name);
    if(tmpMesh)
    {
    /*if(GetCombinedVertCountTHUG(&collision[i], tmpMesh)>300)//650?|600+atleast
    {*/
    //MessageBox(0,tmpMesh->GetName().GetString(),"",0);
    /*current DWORD index = meshes.size();
    meshes.push_back(Mesh());
    meshes[index].InitCollision();
    meshes[index].AddCollision(collision[i], header.Version);
    meshes[index].SetFlag(MeshFlags::isVisible, false);
    collision[i].Name++;
    meshes[index].SetName(*(Checksum*)&collision[i].Name);current*/
    /*}
    else
    tmpMesh->AddCollision(collision[i], header.Version);*/
    //current}
    /*else
    {
    DWORD index = meshes.size();
    meshes.push_back(Mesh());
    meshes[index].AddCollision(collision[i], header.Version);
    meshes[index].SetFlag(MeshFlags::isVisible, false);
    meshes[index].SetName(*(Checksum*)&collision[i].Name);
    }*/
    //current}
    //current}
    delete[] pFile;
  } if (sky) { MAX_TEXCOORDS *= 0x100; Device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | MAX_TEXCOORDS); }
  else
  {
    memcpy(path, Path, strlen(Path) + 1);
    char tmp[25] = "";
    DWORD pos = ((string)path).find_last_of("\\");
    DWORD endPos = ((string)path).find(".scn.xbx");
    if (endPos == string::npos)
      endPos = ((string)path).find(".mdl.xbx");
    if (endPos == string::npos)
      endPos = ((string)path).find(".skin.xbx");
    if (endPos != string::npos)
    {
      for (DWORD i = pos; i < endPos; i++)
      {
        tmp[i - pos] = path[i];
      }
      char tmp2[100] = "";
      sprintf(tmp2, "%s.qb", tmp);
      memcpy(&path[pos], tmp2, strlen(tmp2) + 1);
      MessageBox(0, path, tmp2, 0);
      f = fopen(path, "rb");
      if (f)
      {
        fseek(f, 0, SEEK_END);
        DWORD size = ftell(f);
        fseek(f, 0, SEEK_SET);
        BYTE* pFile = new BYTE[size];
        fread(pFile, size, 1, f);
        fclose(f);
        vector<KnownScript> scripts;
        sprintf(tmp2, "%s_scripts.qb", tmp);
        memcpy(&path[pos], tmp2, strlen(tmp2) + 1);
        vector<Script> sounds;
        LoadScriptsFile(path, scripts, sounds);
        LoadThugNodeArray(pFile, pFile + size, scripts, sounds, extraLayerMeshes);
        KnownScripts.push_back(KnownScript(Checksum("LoadSounds"), NULL, 0));
        triggers.push_back(Checksum("LoadSounds"));
        KnownScript& script = KnownScripts.back();
        for (DWORD i = 0, numSounds = sounds.size(); i < numSounds; i++)
        {
          if (sounds[i].name[0] == 0)
          {
            script.Append(0x16);
            script.Append(Checksum("LoadSound"));
            script.Append(0x1B);
            DWORD tmpLen = strlen(tmp);
            char* soundString = (*(Checksum*)&sounds[i].checksum).GetString();
            DWORD soundLen = strlen(soundString) + 1;
            DWORD len = tmpLen + soundLen;
            script.Append(*(Checksum*)&len);
            char *loadsound = new char[len];
            sprintf(loadsound, "%s\\%s", tmp + 1, soundString);
            script.Append((BYTE*)loadsound, len);
            script.EndScript();
            delete[] loadsound;
          }
        }
        scripts.clear();
        sounds.clear();
        delete[] pFile;
        /*for(DWORD i=0, numNodes = nodes.size(); i<numNodes; i++)
        {
        if(nodes[i].Name.checksum==EnvironmentObject::GetClass())
        {
        if(GetMesh(nodes[i].Name)==NULL)
        {
        if(nodes[i].Trigger.checksum)
        {
        for(DWORD j=0; j<numNodes; j++)
        {
        if(nodes[i].Trigger.checksum==nodes[j].Trigger.checksum)
        goto found;
        }
        for(DWORD j=0; j<KnownScripts.size(); j++)
        {
        if(KnownScripts[j].name.checksum==nodes[i].Trigger.checksum)
        {
        KnownScripts.erase(KnownScripts.begin()+j);
        break;
        }
        }
        for(DWORD j=0; j<triggers.size(); j++)
        {
        if(triggers[j].checksum==nodes[i].Trigger.checksum)
        {
        triggers.erase(triggers.begin()+j);
        break;
        }
        }
        }
        found:
        DeleteNode(i);
        i--;
        numNodes--;
        continue;
        }
        }*/

        /*if(nodes[i].Trigger.checksum)
        {
        DWORD size;
        const void* pScript = GetScript(nodes[i].Trigger.checksum, &size);
        if(pScript==NULL || size==0)
        {

        for(DWORD j=0; j<KnownScripts.size(); j++)
        {
        if(KnownScripts[j].name.checksum==nodes[i].Trigger.checksum)
        {
        KnownScripts.erase(KnownScripts.begin()+j);
        break;
        }
        }
        for(DWORD j=0; j<triggers.size(); j++)
        {
        if(triggers[j].checksum==nodes[i].Trigger.checksum)
        {
        triggers.erase(triggers.begin()+j);
        break;
        }
        }
        nodes[i].Trigger=0;
        }
        }*/
        //}
      }
      sprintf(tmp2, "_Sky\\%s_Sky.scn.xbx", tmp);
      //path[pos] = '\\';
      memcpy(&path[pos], tmp2, strlen(tmp2) + 1);
      f = fopen(path, "rb");
      if (f)
      {
        skyDome = new ThugScene(path, true);
        fclose(f);
      }
      else
      {
        MAX_TEXCOORDS *= 0x100; Device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | MAX_TEXCOORDS);
      }
    }
    else
    {
      MAX_TEXCOORDS *= 0x100; Device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | MAX_TEXCOORDS);
    }

    bool found = false;
    char chc[128] = "";

    FixLink();

    for (DWORD i = 0, numMeshes = this->meshes.size(); i < numMeshes; i++)
    {
      found = false;
      for (DWORD j = 0; j < this->nodes.size(); j++)
      {
        if (this->nodes[j].Class.checksum == EnvironmentObject::GetClass() && this->nodes[j].Name.checksum == this->meshes[i].GetName().checksum)
        {
          //meshes[i].node=&this->nodes[j];
          found = true;
          break;
        }
      }
      if (!found)
      {
        sprintf(chc, "DummyMesh%u", i);
        Checksum name = this->meshes[i].GetName();
        Scripts.push_back(Script(name.checksum, chc));
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

        //this->nodes.push_back(EnvironmentObject(name, 0, 0, true, false));
        //this->meshes[i].node = &this->nodes[this->nodes.size()-1];
      }
      if (meshes[i].GetCollision())
      {
        if (GetCombinedVertCount(&meshes[i]) > 2500)
        {
          sprintf(Name, "%s_Coll", meshes[i].GetName().GetString());
          meshes.push_back(Mesh());
          Mesh& mesh = meshes.back();
          mesh.SetFlag(MeshFlags::isVisible, false);
          meshes[i].MoveCollision(mesh);
          mesh.SetName(Checksum(Name));
          for (DWORD j = 0; j < KnownScripts.size(); j++)
          {
            BYTE* pFile = KnownScripts[j].func;
            BYTE* eof = pFile + KnownScripts[j].size;
            BYTE* offset;
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
                offset = pFile;
                break;
              case 1:
                offset = pFile;
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
                const DWORD qbKey = *(DWORD*)pFile;
                pFile += 4;
                if (qbKey == meshes[i].GetName().checksum)
                {
                  BYTE tmpData[1900];
                  DWORD size = (DWORD)((pFile)-offset);
                  DWORD backupSize = (DWORD)(eof - offset);
                  memcpy(tmpData, offset, backupSize);
                  *(DWORD*)&*(offset + size - 4) = mesh.GetName().checksum;
                  *(offset + size) = 0x2;
                  size++;
                  *(DWORD*)&*(offset + size) = 0x00000001;
                  size += 4;
                  KnownScripts[j].size += size;
                  eof += size;
                  pFile += size;
                  memcpy(offset + size, tmpData, backupSize);
                }
                else
                {
                  switch (qbKey)
                  {
                  case 0xEEC24148:
                  case 0xCB8B5900:
                  case 0x854F17A6:
                  case 0x70291F04:
                  case 0xF8D6A996:
                  case 0x6E77719D:
                    //kill, create, shatter, shatteranddie, visible, invisible
                    if (*pFile == 0x16)
                    {
                      offset = (pFile - 5);
                      pFile++;
                      if (*(DWORD*)pFile == 0xA1DC81F9)
                      {
                        pFile += 4;
                        while (*pFile != 0x16) pFile++;
                        pFile++;
                        if (*(DWORD*)pFile == meshes[i].GetName().checksum)
                        {
                          BYTE tmpData[1900];
                          DWORD size = (DWORD)((pFile + 4) - offset);
                          DWORD backupSize = (DWORD)(eof - offset);
                          memcpy(tmpData, offset, backupSize);
                          *(DWORD*)&*(offset + size - 4) = mesh.GetName().checksum;
                          *(offset + size) = 0x2;
                          size++;
                          *(DWORD*)&*(offset + size) = 0x00000001;
                          size += 4;
                          KnownScripts[j].size += size;
                          eof += size;
                          pFile += size;
                          memcpy(offset + size, tmpData, backupSize);
                        }

                        pFile += 4;
                      }
                      else
                        pFile += 4;
                    }
                    break;
                  }
                }
                break;
              }
            }
          done:;
          }
          Node* node = GetNode(meshes[i].GetName());
          if (node)
          {
            nodes.push_back(*node);
            Node & n = nodes.back();
            n.Name = mesh.GetName();
            if (n.TrickObject)
            {
              if (!n.Cluster.checksum)
              {
                n.Cluster = mesh.GetName();
                node = GetNode(meshes[i].GetName());
                node->Cluster = n.Cluster;
              }
            }
          }
        }
      }
    }

    for (DWORD j = 0; j < this->meshes.size(); j++)
    {
      Node* node2 = GetNode(this->meshes[j].GetName());
      if (strstr(this->meshes[j].GetName().GetString2(), "occlusion") || strstr(this->meshes[j].GetName().GetString2(), "Occlusion") || strstr(this->meshes[j].GetName().GetString2(), "Occluder"))
      {
        this->meshes[j].SetFlag(MeshFlags::isDeleted);
        this->meshes[j].UnSetFlag(MeshFlags::isVisible);
        sprintf(Name, "%s_Coll", meshes[j].GetName().GetString());
        Mesh* tmp = GetMesh(Checksum(Name));
        if (tmp)
        {
          tmp->SetFlag(MeshFlags::isDeleted);
          tmp->UnSetFlag(MeshFlags::isVisible);
        }
        for (DWORD k = 0; k < extraLayerMeshes.size(); k++)
        {
          if (extraLayerMeshes[k].name.checksum == meshes[j].GetName().checksum)
          {
            extraLayerMeshes.erase(extraLayerMeshes.begin() + k);
          }
        }
        DeleteNode(this->meshes[j].GetName());
      }
      else if (node2)
      {
        if (node2->AbsentInNetGames)
        {
          this->meshes[j].SetFlag(MeshFlags::isDeleted);
          this->meshes[j].UnSetFlag(MeshFlags::isVisible);
          sprintf(Name, "%s_Coll", meshes[j].GetName().GetString());
          Mesh* tmp = GetMesh(Checksum(Name));
          if (tmp)
          {
            tmp->SetFlag(MeshFlags::isDeleted);
            tmp->UnSetFlag(MeshFlags::isVisible);
          }
          for (DWORD k = 0; k < extraLayerMeshes.size(); k++)
          {
            if (extraLayerMeshes[k].name.checksum == meshes[j].GetName().checksum)
            {
              extraLayerMeshes.erase(extraLayerMeshes.begin() + k);
            }
          }
          DeleteNode(this->meshes[j].GetName());
        }
      }
    }

    /*for (DWORD j = 0; j < this->meshes.size(); j++)
    {
      Node* node2 = GetNode(this->meshes[j].GetName());
      if (node2)
      {
        if (node2->CreatedAtStart==false)
          this->meshes[j].SetFlag(MeshFlags::isDeleted);
      }
    }*/

    /*for(DWORD j=0; j<KnownScripts.size(); j++)
    {
    BYTE* pFile = KnownScripts[j].func;
    BYTE* eof = pFile+KnownScripts[j].size;
    BYTE* offset;
    while(pFile<eof)
    {
    const BYTE opcode = *pFile;
    pFile++;
    switch(opcode)
    {
    case 0x1C:
    case 0x1B:
    pFile+=*(DWORD*)pFile+4;
    break;
    case 0x17:
    case 0x1A:
    case 0x2E:
    case 2:
    pFile+=4;
    break;
    case 0x1F:
    pFile+=8;
    break;
    case 0x1E:
    pFile+=12;
    break;
    case 0x2F:
    pFile+=*(DWORD*)pFile*4+4;
    break;
    case 0x16:
    const DWORD qbKey = *(DWORD*)pFile;
    pFile+=4;
    switch(qbKey)
    {
    case 0xEEC24148:
    case 0xCB8B5900:
    case 0x854F17A6:
    case 0x70291F04:
    case 0xF8D6A996:
    case 0x6E77719D:
    //kill, create, shatter, shatteranddie, visible, invisible
    if(*pFile==0x16)
    {
    offset=(pFile-5);
    pFile++;
    if(*(DWORD*)pFile==0xA1DC81F9)
    {
    pFile+=4;
    bool global=false;
    while(*pFile!=0x16) { if(*pFile==0x2D) global=true; pFile++;}
    pFile++;
    if(global)
    {
    pFile+=4;
    goto found;
    }
    DWORD name = *(DWORD*)pFile;
    pFile+=4;
    for(DWORD k=0; k<nodes.size(); k++)
    {
    if(nodes[k].Name.checksum==name)
    {
    if(nodes[k].Class.checksum==EnvironmentObject::GetClass())
    {
    for(DWORD l=0; l<meshes.size(); l++)
    {
    if(meshes[l].GetName().checksum==name)
    goto found;
    }
    }
    else
    goto found;
    break;
    }
    }
    while(pFile<eof)
    {
    const BYTE op = *pFile;
    pFile++;
    switch(op)
    {
    case 0x1C:
    case 0x1B:
    pFile+=*(DWORD*)pFile+4;
    break;

    case 2:
    pFile+=4;
    goto ya;
    break;
    case 1:
    goto ya;
    break;

    case 0x16:
    case 0x17:
    case 0x1A:
    case 0x2E:
    pFile+=4;
    break;
    case 0x1F:
    pFile+=8;
    break;
    case 0x1E:
    pFile+=12;
    break;
    case 0x2F:
    pFile+=*(DWORD*)pFile*4+4;
    break;
    }
    }
    ya:
    //MessageBox(0,(*(Checksum*)&name).GetString(),(*(Checksum*)&name).GetString(),0);
    DWORD backupSize = (DWORD)(eof-pFile);
    memcpy(offset, pFile, backupSize);
    DWORD size = (DWORD)(pFile-offset);
    KnownScripts[j].size-=size;
    eof-=size;
    pFile-=size;
    found:;
    }
    else
    pFile+=4;
    }
    break;
    }
    break;
    }
    }
    }*/
    //FixNodes();
    for (DWORD i = 0; i < extraLayerMeshes.size(); i++)
    {
      meshes.push_back(Mesh());
      Mesh& mesh = meshes.back();
      Node* node = GetNode(extraLayerMeshes[i].name);
      if (node)
      {
        if (!node->CreatedAtStart)
          sprintf(Name, "InvisibleExtraLayer_%s", extraLayerMeshes[i].name.GetString());
        else
          sprintf(Name, "ExtraLayer_%s", extraLayerMeshes[i].name.GetString());
        mesh.SetName(Name);
        nodes.push_back(*node);
        nodes.back().Name = mesh.GetName();
      }
      else
      {
        sprintf(Name, "ExInvisibleExtraLayer_%s", extraLayerMeshes[i].name.GetString());
        mesh.SetName(Name);
        nodes.push_back(EnvironmentObject(mesh.GetName(), 0, 0, false, false));
      }
      vector<DWORD> addedMaterials;
      DWORD vOffset = 0;
      Vertex v;
      for (DWORD j = 0; j < extraLayerMeshes[i].vertices.size(); j++)
      {
        v.x = extraLayerMeshes[i].vertices[j].x;
        v.y = extraLayerMeshes[i].vertices[j].y;
        v.z = extraLayerMeshes[i].vertices[j].z;
        v.colour = extraLayerMeshes[i].vertices[j].colour;
        if (v.colour.a)
          v.colour.a /= 2;
        /*if (mat->fucked && mat->fucked < mat->GetNumTexCoords()))
          v.tUV[0] = extraLayerMeshes[i].vertices[j].tUV[0 + mat->fucked];
        else*/
        v.tUV[0] = extraLayerMeshes[i].vertices[j].tUV[0];
        mesh.AddVertexSilent(v);
      }
      for (DWORD j = 0; j < extraLayerMeshes[i].matSplits.size(); j++)
      {
        MaterialSplit split;
        split.type = 0;

        Material* mat = matList.GetMaterial((Checksum)extraLayerMeshes[i].matSplits[j].matId);
        if (mat)
        {
          if (mat->fucked && mat->fucked < mat->GetNumTextures())
          {
            Vertex* verts = (Vertex*)mesh.GetVertices();
            for (DWORD e = 0; e < extraLayerMeshes[i].matSplits[j].Indices.size(); e++)
            {
              if (extraLayerMeshes[i].matSplits[j].Indices[e] < mesh.GetNumVertices())
                verts[extraLayerMeshes[i].matSplits[j].Indices[e]].tUV[0] = extraLayerMeshes[i].vertices[extraLayerMeshes[i].matSplits[j].Indices[e]].tUV[0 + mat->fucked];
            }
          }
          for (DWORD k = 0; k < addedMaterials.size(); k++)
          {
            if (addedMaterials[k] == mat->GetMatId())
              goto found;
          }
          if (mat)
          {
            Material material;
            material.Reserve(1);
            material.SetMatId(mat->GetMatId() + 1);
            material.doubled = mat->doubled;
            material.drawOrder = mat->drawOrder + 1.0f;
            material.ignoreAlpha = false;
            //material.alphaMap=true;
            material.extraLayer = true;
            Texture* texture = mat->GetTexture(1);
            if (texture->blendMode == 1 || texture->blendMode == 2 || texture->blendMode == 4 || texture->blendMode == 5 || texture->blendMode == 6)
            {
              static char imageName[MAX_PATH] = "";
              static char tmp[15] = "";
              GetEnvironmentVariable("TEMP", imageName, MAX_PATH);
              DWORD chc = texture->GetTexId();
              char* Name = (*(Checksum*)&(chc)).GetString();
              if (Name == NULL)
              {
                sprintf(tmp, "\\%08X.dds", texture->GetTexId());
                strcat(imageName, tmp);
              }
              else
              {
                DWORD len = strlen(imageName);
                imageName[len] = '\\';
                len++;
                strcpy(&imageName[len], Name);
                strcat(imageName, ".dds");
              }
              //HRESULT hres = D3DXCreateTextureFromFile(Device, imageName, &imageData);
              FILE* f = fopen(imageName, "rb");
              if (f)
              {
                extern TexFiles texFiles;
                char imageName2[MAX_PATH];
                strcpy(imageName2, imageName);
                imageName2[strlen(imageName) - 3] = 't';
                imageName2[strlen(imageName) - 2] = 'g';
                imageName2[strlen(imageName) - 1] = 'a';
                for (DWORD l = 0; l < texFiles.numFiles; l++)
                {
                  if (!stricmp(texFiles.fileNames[l], imageName))
                  {
                    texFiles.fileNames[l][strlen(imageName) - 3] = 't';
                    texFiles.fileNames[l][strlen(imageName) - 2] = 'g';
                    texFiles.fileNames[l][strlen(imageName) - 1] = 'a';
                  }
                }
                fseek(f, 0, SEEK_END);
                DWORD size = ftell(f);
                BYTE* data = new BYTE[size];
                fseek(f, 0, SEEK_SET);
                fread(data, size, 1, f);
                fclose(f);
                DeleteFile(imageName);
                f = fopen(imageName2, "w+b");
                if (f)
                {
                  DWORD imageSize;
                  BYTE* imageData = data;
                  DDS_HEADER* header = (DDS_HEADER*)imageData;
                  imageData += sizeof(DDS_HEADER);

                  WORD width = (WORD)header->width;
                  WORD height = (WORD)header->height;

                  if (header->pixelFormat[1] & 0x4)
                  {
                    imageSize = header->width*header->height * 4;
                    BYTE* levelData = new BYTE[imageSize];
                    DecompressDXT(imageData, header->imageSize, header->width, header->height, levelData, (((header->pixelFormat[2] & 0xFF000000) >> 24) - 0x30));
                    fputc(0, f);
                    fputc(0, f);
                    fputc(2, f);
                    DWORD zero = 0;
                    fwrite(&zero, 4, 1, f);
                    fwrite(&zero, 4, 1, f);
                    fputc(0, f);
                    fwrite(&header->width, 2, 1, f);
                    fwrite(&header->height, 2, 1, f);
                    fputc(32, f);
                    fputc(32, f);
                    Colour* color = (Colour*)levelData;
                    FixBlending(color, imageSize / 4, texture);
                    fwrite(levelData, imageSize, 1, f);
                    delete[] levelData;
                  }
                  else
                  {
                    imageSize = header->width*header->height * 4;
                    DWORD bytesPerPixel = header->imageSize / header->width / header->height;
                    bool usingAlpha = header->pixelFormat[1] & 0x2;

                    if (bytesPerPixel == 4)
                    {
                      fputc(0, f);
                      fputc(0, f);
                      fputc(2, f);
                      DWORD zero = 0;
                      fwrite(&zero, 4, 1, f);
                      fwrite(&zero, 4, 1, f);
                      fputc(0, f);
                      fwrite(&header->width, 2, 1, f);
                      fwrite(&header->height, 2, 1, f);
                      fputc(32, f);
                      fputc(32, f);
                      Colour* color = (Colour*)imageData;
                      FixBlending(color, imageSize / 4, texture);
                      fwrite(imageData, imageSize, 1, f);
                    }
                    else if (bytesPerPixel == 2)
                    {
                      imageSize = header->width*header->height * 4;
                      DWORD numPixels = imageSize / 4;
                      Colour* levelData = new Colour[numPixels];
                      //MessageBox(0,"sure this is 16 bit?",imageName,0);
                      if (usingAlpha)
                      {
                        for (DWORD l = 0; l < numPixels; l++)
                        {
                          BYTE r = ((BYTE)((((WORD*)imageData)[l] & 0x003E) >> 1));
                          BYTE g = ((BYTE)((((WORD*)imageData)[l] & 0x7C0) >> 6));
                          BYTE b = ((BYTE)((((WORD*)imageData)[l] & 0xF800) >> 12));
                          BYTE a = ((BYTE)((((WORD*)imageData)[l] & 0x1)));
                          levelData[l].r = (r << 3) | (r >> 2);
                          levelData[l].g = (g << 3) | (g >> 2);
                          levelData[l].b = (b << 3) | (b >> 2);
                          if (a)
                            levelData[l].a = 255;
                          else
                            levelData[l].a = 0;
                        }
                      }
                      else
                      {
                        for (DWORD l = 0; l < numPixels; l++)
                        {
                          BYTE r = (BYTE)(((WORD*)imageData)[l] & 0x001F);
                          BYTE g = ((BYTE)((((WORD*)imageData)[l] & 0x7E0) >> 5));
                          BYTE b = ((BYTE)((((WORD*)imageData)[l] & 0xF800) >> 11));
                          levelData[l].r = (r << 3) | (r >> 2);
                          levelData[l].g = (g << 2) | (g >> 4);
                          levelData[l].b = (b << 3) | (b >> 2);
                          levelData[l].a = 255;
                        }
                      }
                      fputc(0, f);
                      fputc(0, f);
                      fputc(2, f);
                      DWORD zero = 0;
                      fwrite(&zero, 4, 1, f);
                      fwrite(&zero, 4, 1, f);
                      fputc(0, f);
                      fwrite(&header->width, 2, 1, f);
                      fwrite(&header->height, 2, 1, f);
                      fputc(32, f);
                      fputc(32, f);
                      Colour* color = (Colour*)levelData;
                      FixBlending(color, imageSize / 4, texture);
                      fwrite(levelData, imageSize, 1, f);
                      delete[] levelData;
                      //delete [] levelData;
                    }
                    else if (bytesPerPixel == 1)
                      MessageBox(0, "1", 0, 0);
                    else
                      MessageBox(0, "unknown", 0, 0);

                    //MessageBox(0,"wtf this texture is not compressed","wawwiee",0);
                    //levelData = (pImageData;
                  }
                  fclose(f);
                }

                delete[] data;
              }
              /*else
              {
              GetEnvironmentVariable("TEMP",imageName, MAX_PATH);
              if(name == NULL)
              {
              sprintf(tmp,"\\%08X.tga\0",texture->GetTexId());
              strcat(imageName,tmp);
              }
              else
              {
              DWORD len = strlen(imageName);
              imageName[len] = '\\';
              len++;
              strcpy(&imageName[len],name);
              strcat(imageName,".tga");
              }
              f = fopen(imageName, "rb");
              if(f)
              {
              //MessageBox(0,imageName,imageName,0);
              }
              }*/
              //material.reverse=true;
            }
            /*else
            {
            static char imageName[MAX_PATH] = "";
            static char tmp[15] = "";
            GetEnvironmentVariable("TEMP",imageName, MAX_PATH);
            DWORD chc = texture->GetTexId();
            char* name=(*(Checksum*)&(chc)).GetString();
            if(name == NULL)
            {
            sprintf(tmp,"\\%08X.dds",texture->GetTexId());
            strcat(imageName,tmp);
            }
            else
            {
            DWORD len = strlen(imageName);
            imageName[len] = '\\';
            len++;
            strcpy(&imageName[len],name);
            strcat(imageName,".dds");
            }
            //HRESULT hres = D3DXCreateTextureFromFile(Device, imageName, &imageData);
            FILE* f = fopen(imageName, "rb");
            if(f)
            {

            }
            else
            {
            GetEnvironmentVariable("TEMP",imageName, MAX_PATH);
            if(name == NULL)
            {
            sprintf(tmp,"\\%08X.tga\0",texture->GetTexId());
            strcat(imageName,tmp);
            }
            else
            {
            DWORD len = strlen(imageName);
            imageName[len] = '\\';
            len++;
            strcpy(&imageName[len],name);
            strcat(imageName,".tga");
            }
            f = fopen(imageName, "rb");
            if(f)
            {

            }
            }
            }*/
            material.transparent = true;
            material.AddTexture2(texture);
            material.GetTexture(0)->CreateTexture();
            matList.AddMaterial(&material);
            addedMaterials.push_back(mat->GetMatId());
          }
        found:
          split.matId = mat->GetMatId() + 1;
          split.numIndices = extraLayerMeshes[i].matSplits[j].Indices.size();
          if (split.numIndices)
          {
            split.AddIndices(&extraLayerMeshes[i].matSplits[j].Indices.front(), split.numIndices, vOffset);
            mesh.AddMatSplit(&split, &matList);
          }
          else
            MessageBox(0, "no indices5", "", 0);
        }
        //vOffset+=extraLayerMeshes[i].vertices[j].size();
      }
      for (DWORD j = 1; j < extraLayerMeshes[i].numPasses; j++)// && j < 2; j++)
      {
        meshes.push_back(Mesh());
        Mesh & mesh2 = meshes.back();//th4
        Node* node2 = GetNode(extraLayerMeshes[i].name);
        if (node2)
        {
          if (!node2->CreatedAtStart)
            sprintf(Name, "InvisibleExtraLayer%u_%s", j, extraLayerMeshes[i].name.GetString());
          else
            sprintf(Name, "ExtraLayer%u_%s", j, extraLayerMeshes[i].name.GetString());
          mesh2.SetName(Name);
          nodes.push_back(*node2);
          nodes.back().Name = mesh2.GetName();
        }
        else
        {
          sprintf(Name, "ExInvisibleExtraLayer%u_%s", j, extraLayerMeshes[i].name.GetString());
          mesh2.SetName(Name);
          nodes.push_back(EnvironmentObject(mesh2.GetName(), 0, 0, false, false));
        }
        Vertex v2;
        for (DWORD k = 0; k < extraLayerMeshes[i].vertices.size(); k++)
        {
          v2.x = extraLayerMeshes[i].vertices[k].x;
          v2.y = extraLayerMeshes[i].vertices[k].y;
          v2.z = extraLayerMeshes[i].vertices[k].z;
          v2.colour = extraLayerMeshes[i].vertices[k].colour;
          if (v2.colour.a)
            v2.colour.a /= 2;
          /*if (mat->fucked && mat->fucked < mat->GetNumTexCoords())
            v2.tUV[0] = extraLayerMeshes[i].vertices[k].tUV[j + mat->fucked];
          else*/
          v2.tUV[0] = extraLayerMeshes[i].vertices[k].tUV[j];
          mesh2.AddVertexSilent(v2);
        }
        for (DWORD k = 0; k<extraLayerMeshes[i].matSplits.size(); k++)
        {
          MaterialSplit split;
          split.type = 0;


          Material* mat = matList.GetMaterial((Checksum)extraLayerMeshes[i].matSplits[k].matId);
          if (mat && mat->GetNumTextures()>j + 1)
          {
            if (mat)
            {
              if (mat->fucked && mat->fucked < mat->GetNumTextures())
              {
                Vertex* verts = (Vertex*)mesh.GetVertices();
                for (DWORD e = 0; e < extraLayerMeshes[i].matSplits[k].Indices.size(); e++)
                {
                  if (extraLayerMeshes[i].matSplits[k].Indices[e] < mesh.GetNumVertices())
                    verts[extraLayerMeshes[i].matSplits[k].Indices[e]].tUV[0] = extraLayerMeshes[i].vertices[extraLayerMeshes[i].matSplits[k].Indices[e]].tUV[j + mat->fucked];
                }
              }
              Material material;
              material.Reserve(1);
              material.SetMatId(mat->GetMatId() + j + 1);
              material.doubled = mat->doubled;
              material.drawOrder = mat->drawOrder + (float)j + 1.0f;
              material.ignoreAlpha = false;
              //material.alphaMap=true;
              material.extraLayer = true;
              Texture* texture = mat->GetTexture(j + 1);
              if (texture->blendMode == 1 || texture->blendMode == 2 || texture->blendMode == 4 || texture->blendMode == 5 || texture->blendMode == 6)
              {
                static char imageName[MAX_PATH] = "";
                static char tmp[15] = "";
                GetEnvironmentVariable("TEMP", imageName, MAX_PATH);
                DWORD chc = texture->GetTexId();
                char* Name = (*(Checksum*)&(chc)).GetString();
                if (Name == NULL)
                {
                  sprintf(tmp, "\\%08X.dds", texture->GetTexId());
                  strcat(imageName, tmp);
                }
                else
                {
                  DWORD len = strlen(imageName);
                  imageName[len] = '\\';
                  len++;
                  strcpy(&imageName[len], Name);
                  strcat(imageName, ".dds");
                }
                //HRESULT hres = D3DXCreateTextureFromFile(Device, imageName, &imageData);
                FILE* f = fopen(imageName, "rb");
                if (f)
                {
                  extern TexFiles texFiles;
                  char imageName2[MAX_PATH];
                  strcpy(imageName2, imageName);
                  imageName2[strlen(imageName) - 3] = 't';
                  imageName2[strlen(imageName) - 2] = 'g';
                  imageName2[strlen(imageName) - 1] = 'a';
                  for (DWORD l = 0; l < texFiles.numFiles; l++)
                  {
                    if (!stricmp(texFiles.fileNames[l], imageName))
                    {
                      texFiles.fileNames[l][strlen(imageName) - 3] = 't';
                      texFiles.fileNames[l][strlen(imageName) - 2] = 'g';
                      texFiles.fileNames[l][strlen(imageName) - 1] = 'a';
                    }
                  }
                  fseek(f, 0, SEEK_END);
                  DWORD size = ftell(f);
                  BYTE* data = new BYTE[size];
                  fseek(f, 0, SEEK_SET);
                  fread(data, size, 1, f);
                  fclose(f);
                  DeleteFile(imageName);
                  f = fopen(imageName2, "w+b");
                  if (f)
                  {
                    DWORD imageSize;
                    BYTE* imageData = data;
                    DDS_HEADER* header = (DDS_HEADER*)imageData;
                    imageData += sizeof(DDS_HEADER);

                    WORD width = (WORD)header->width;
                    WORD height = (WORD)header->height;

                    if (header->pixelFormat[1] & 0x4)
                    {
                      imageSize = header->width*header->height * 4;
                      BYTE* levelData = new BYTE[imageSize];
                      DecompressDXT(imageData, header->imageSize, header->width, header->height, levelData, (((header->pixelFormat[2] & 0xFF000000) >> 24) - 0x30));
                      fputc(0, f);
                      fputc(0, f);
                      fputc(2, f);
                      DWORD zero = 0;
                      fwrite(&zero, 4, 1, f);
                      fwrite(&zero, 4, 1, f);
                      fputc(0, f);
                      fwrite(&header->width, 2, 1, f);
                      fwrite(&header->height, 2, 1, f);
                      fputc(32, f);
                      fputc(32, f);
                      Colour* color = (Colour*)levelData;
                      FixBlending(color, imageSize / 4, texture);
                      fwrite(levelData, imageSize, 1, f);
                      delete[] levelData;
                    }
                    else
                    {
                      imageSize = header->width*header->height * 4;
                      DWORD bytesPerPixel = header->imageSize / header->width / header->height;
                      bool usingAlpha = header->pixelFormat[1] & 0x2;

                      if (bytesPerPixel == 4)
                      {
                        fputc(0, f);
                        fputc(0, f);
                        fputc(2, f);
                        DWORD zero = 0;
                        fwrite(&zero, 4, 1, f);
                        fwrite(&zero, 4, 1, f);
                        fputc(0, f);
                        fwrite(&header->width, 2, 1, f);
                        fwrite(&header->height, 2, 1, f);
                        fputc(32, f);
                        fputc(32, f);
                        Colour* color = (Colour*)imageData;
                        FixBlending(color, imageSize / 4, texture);
                        fwrite(imageData, imageSize, 1, f);
                      }
                      else if (bytesPerPixel == 2)
                      {
                        imageSize = header->width*header->height * 4;
                        DWORD numPixels = imageSize / 4;
                        Colour* levelData = new Colour[numPixels];
                        //MessageBox(0,"sure this is 16 bit?",imageName,0);
                        if (usingAlpha)
                        {
                          for (DWORD l = 0; l < numPixels; l++)
                          {
                            BYTE r = ((BYTE)((((WORD*)imageData)[l] & 0x003E) >> 1));
                            BYTE g = ((BYTE)((((WORD*)imageData)[l] & 0x7C0) >> 6));
                            BYTE b = ((BYTE)((((WORD*)imageData)[l] & 0xF800) >> 12));
                            BYTE a = ((BYTE)((((WORD*)imageData)[l] & 0x1)));
                            levelData[l].r = (r << 3) | (r >> 2);
                            levelData[l].g = (g << 3) | (g >> 2);
                            levelData[l].b = (b << 3) | (b >> 2);
                            if (a)
                              levelData[l].a = 255;
                            else
                              levelData[l].a = 0;
                          }
                        }
                        else
                        {
                          for (DWORD l = 0; l < numPixels; l++)
                          {
                            BYTE r = (BYTE)(((WORD*)imageData)[l] & 0x001F);
                            BYTE g = ((BYTE)((((WORD*)imageData)[l] & 0x7E0) >> 5));
                            BYTE b = ((BYTE)((((WORD*)imageData)[l] & 0xF800) >> 11));
                            levelData[l].r = (r << 3) | (r >> 2);
                            levelData[l].g = (g << 2) | (g >> 4);
                            levelData[l].b = (b << 3) | (b >> 2);
                            levelData[l].a = 255;
                          }
                        }
                        fputc(0, f);
                        fputc(0, f);
                        fputc(2, f);
                        DWORD zero = 0;
                        fwrite(&zero, 4, 1, f);
                        fwrite(&zero, 4, 1, f);
                        fputc(0, f);
                        fwrite(&header->width, 2, 1, f);
                        fwrite(&header->height, 2, 1, f);
                        fputc(32, f);
                        fputc(32, f);
                        Colour* color = (Colour*)levelData;
                        FixBlending(color, imageSize / 4, texture);
                        fwrite(levelData, imageSize, 1, f);
                        delete[] levelData;
                        //delete [] levelData;
                      }
                      else if (bytesPerPixel == 1)
                        MessageBox(0, "1", 0, 0);
                      else
                        MessageBox(0, "unknown", 0, 0);

                      //MessageBox(0,"wtf this texture is not compressed","wawwiee",0);
                      //levelData = (pImageData;
                    }
                    fclose(f);
                  }

                  delete[] data;
                }
                /*else
                {
                GetEnvironmentVariable("TEMP",imageName, MAX_PATH);
                if(name == NULL)
                {
                sprintf(tmp,"\\%08X.tga\0",texture->GetTexId());
                strcat(imageName,tmp);
                }
                else
                {
                DWORD len = strlen(imageName);
                imageName[len] = '\\';
                len++;
                strcpy(&imageName[len],name);
                strcat(imageName,".tga");
                }
                f = fopen(imageName, "rb");
                if(f)
                {
                //MessageBox(0,imageName,imageName,0);
                }
                //material.reverse=true;
                }*/
              }
              _fcloseall();
              material.transparent = true;
              material.AddTexture2(texture);
              material.GetTexture(0)->CreateTexture();
              matList.AddMaterial(&material);
            }
            split.matId = mat->GetMatId() + j + 1;
            split.numIndices = extraLayerMeshes[i].matSplits[k].Indices.size();
            if (split.numIndices)
            {
              split.AddIndices(&extraLayerMeshes[i].matSplits[k].Indices.front(), split.numIndices, vOffset);
              mesh2.AddMatSplit(&split, &matList);
            }
            else
              MessageBox(0, "no indices6", "", 0);
          }
        }
      }
    }
    extraLayerMeshes.clear();

  }
}

DWORD GetCombinedVertCount(NSCollisionObject* collision, Mesh* mesh)
{
  DWORD vertCount = mesh->GetNumVertices();

  WORD numVerts = 0;
  SimpleVertex* uniqueVerts = collision->GetUniqueVerts(&numVerts);
  vertCount += (DWORD)numVerts;
  const Vertex* v = mesh->GetVertices();
  for (DWORD i = 0; i < numVerts; i++)
  {
    for (DWORD j = 0; j < mesh->GetNumVertices(); j++)
    {
      if (uniqueVerts[i].x == v[j].x && uniqueVerts[i].y == v[j].y && uniqueVerts[i].z == v[j].z)
        vertCount--;
    }
  }
  delete[] uniqueVerts;
  return vertCount;
}

Th4Scene::Th4Scene(char* Path, bool sky)
{
  ZeroMemory(name, MAX_PATH);
  SetName(Path); blendValue = 1;
  cullMode = D3DCULL_NONE;

  BYTE*pFile = NULL;
  DWORD size = 0, ptr = 0;
  char path[256] = "";

  memcpy(path, Path, strlen(Path) + 1);
  DWORD len = strlen(Path);

  if (strstr(path, "skin.dat"))
  {
    path[len - 8] = 't';
    path[len - 7] = 'e';
    path[len - 6] = 'x';
    path[len - 5] = '.';
    path[len - 4] = 'd';
    path[len - 3] = 'a';
    path[len - 2] = 't';
    path[len - 1] = '\0';
  }
  else
  {
    path[len - 7] = 't';
    path[len - 6] = 'e';
    path[len - 5] = 'x';
  }

  FILE*f = fopen(path, "rb");
  if (f)
  {
    fseek(f, 0, SEEK_END);

    size = ftell(f);

    pFile = new BYTE[size];

    fseek(f, 0, SEEK_SET);
    fread(pFile, size, 1, f);
    fclose(f);


    ptr = (DWORD)(pFile);
    ptr += 4;
    ProcessTexFile(ptr, sky);

    delete[] pFile;
  }
  f = fopen(Path, "rb");
  if (f)
  {
    fseek(f, 0, SEEK_END);

    size = ftell(f);

    pFile = new BYTE[size];

    fseek(f, 0, SEEK_SET);
    fread(pFile, size, 1, f);
    fclose(f);

    ptr = (DWORD)(pFile);
    ptr += 12;


    ptr = ProcessMaterials(ptr);
    ProcessMeshes(ptr);

    delete[] pFile;
  }

  len = strlen(path);

  path[len - 7] = 'c';
  path[len - 6] = 'o';
  path[len - 5] = 'l';
  f = fopen(path, "rb");
  if (f)
  {
    fseek(f, 0, SEEK_END);

    size = ftell(f);

    pFile = new BYTE[size];

    fseek(f, 0, SEEK_SET);
    fread(pFile, size, 1, f);
    fclose(f);

    ptr = (DWORD)(pFile);
    NSCollisionHeader header = ((NSCollisionHeader*)ptr)[0];
    NSCollisionObject* collision = ProcessNSCollision(ptr);

    for (DWORD i = 0; i < header.numObjects; i++)
    {
      if (collision[i].numVerts != 0)
      {
        Mesh* tmpMesh = GetMesh(*(Checksum*)&collision[i].Name);
        if (tmpMesh)
        {
          /*if(GetCombinedVertCount(&collision[i], tmpMesh)>750)//650?|600+atleast
          {
          MessageBox(0,tmpMesh->GetName().GetString(),"",0);
          DWORD index = meshes.size();
          meshes.push_back(Mesh());
          meshes[index].AddCollision(collision[i], header.Version);
          meshes[index].SetFlag(MeshFlags::isVisible, false);
          collision[i].Name++;
          meshes[index].SetName(*(Checksum*)&collision[i].Name);
          }
          else*/
          tmpMesh->AddCollision(collision[i], header.Version);
        }
        else
        {
          DWORD index = meshes.size();
          meshes.push_back(Mesh());
          meshes[index].AddCollision(collision[i], header.Version);
          meshes[index].SetFlag(MeshFlags::isVisible, false);
          meshes[index].SetName(*(Checksum*)&collision[i].Name);
        }
      }
    }

    delete[] pFile;
  } if (sky) { MAX_TEXCOORDS *= 0x100; Device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | MAX_TEXCOORDS); }
  else
  {
    memcpy(path, Path, strlen(Path) + 1);
    char tmp[25] = "";
    DWORD pos = ((string)path).find_last_of("\\");
    DWORD endPos = ((string)path).find("scn.dat");
    if (endPos == string::npos)
      endPos = ((string)path).find("mdl.dat");
    if (endPos == string::npos)
      endPos = ((string)path).find("skin.dat");
    if (endPos != string::npos)
    {
      for (DWORD i = pos; i < endPos; i++)
      {
        tmp[i - pos] = path[i];
      }
      char tmp2[100] = "";
      sprintf(tmp2, "%s.qb", tmp);
      memcpy(&path[pos], tmp2, strlen(tmp2) + 1);
      MessageBox(0, path, tmp2, 0);
      f = fopen(path, "rb");
      if (f)
      {
        fseek(f, 0, SEEK_END);
        DWORD size = ftell(f);
        fseek(f, 0, SEEK_SET);
        BYTE* pFile = new BYTE[size];
        fread(pFile, size, 1, f);
        fclose(f);
        vector<KnownScript> scripts;
        sprintf(tmp2, "%s_scripts.qb", tmp);
        memcpy(&path[pos], tmp2, strlen(tmp2) + 1);
        vector<Script> sounds;
        LoadScriptsFile(path, scripts, sounds, false);
        LoadProSkaterNodeArray(pFile, pFile + size, scripts, sounds);
        KnownScripts.push_back(KnownScript(Checksum("LoadSounds"), NULL, 0));
        triggers.push_back(Checksum("LoadSounds"));
        KnownScript& script = KnownScripts.back();
        for (DWORD i = 0, numSounds = sounds.size(); i < numSounds; i++)
        {
          if (sounds[i].name[0] == 0)
          {
            char* soundString = (*(Checksum*)&sounds[i].checksum).GetString();
            if (soundString)
            {
              script.Append(0x16);
              script.Append(Checksum("LoadSound"));
              script.Append(0x1B);
              DWORD tmpLen = strlen(tmp);

              DWORD soundLen = strlen(soundString) + 1;
              DWORD len = tmpLen + soundLen;
              script.Append(*(Checksum*)&len);
              char *loadsound = new char[len];
              sprintf(loadsound, "%s\\%s", tmp + 1, soundString);
              script.Append((BYTE*)loadsound, len);
              script.EndScript();
              delete[] loadsound;
            }
          }
        }
        /*KnownScripts.clear();
        triggers.clear();*/
        scripts.clear();
        sounds.clear();
        delete[] pFile;
      }
      sprintf(tmp2, "_Sky\\%s_Skyscn.dat", tmp);
      //path[pos] = '\\';
      memcpy(&path[pos], tmp2, strlen(tmp2) + 1);
      f = fopen(path, "rb");
      if (f)
      {
        skyDome = new Th4Scene(path, true);
        fclose(f);
      }
      else
      {
        MAX_TEXCOORDS *= 0x100; Device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | MAX_TEXCOORDS);
      }
    }
    else
    {
      MAX_TEXCOORDS *= 0x100; Device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | MAX_TEXCOORDS);
    }

    bool found = false;
    char chc[128] = "";

    FixLink();

    for (DWORD i = 0, numMeshes = this->meshes.size(); i < numMeshes; i++)
    {
      found = false;
      for (DWORD j = 0; j < this->nodes.size(); j++)
      {
        if (this->nodes[j].Class.checksum == EnvironmentObject::GetClass() && this->nodes[j].Name.checksum == this->meshes[i].GetName().checksum)
        {
          //meshes[i].node=&this->nodes[j];
          found = true;
          break;
        }
      }
      if (!found)
      {
        sprintf(chc, "Mesh%u", i);
        Checksum name = this->meshes[i].GetName();
        Scripts.push_back(Script(name.checksum, chc));
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

        this->nodes.push_back(EnvironmentObject(name, 0, 0, true, false));
        //this->meshes[i].node = &this->nodes[this->nodes.size()-1];
      }
      if (meshes[i].GetCollision())
      {
        if (GetCombinedVertCount(&meshes[i]) > 2500)
        {
          sprintf(Name, "%s_Coll", meshes[i].GetName().GetString());
          meshes.push_back(Mesh());
          Mesh& mesh = meshes.back();
          mesh.SetFlag(MeshFlags::isVisible, false);
          meshes[i].MoveCollision(mesh);
          mesh.SetName(Checksum(Name));
          for (DWORD j = 0; j < KnownScripts.size(); j++)
          {
            BYTE* pFile = KnownScripts[j].func;
            BYTE* eof = pFile + KnownScripts[j].size;
            BYTE* offset;
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
                offset = pFile;
                break;
              case 1:
                offset = pFile;
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
                const DWORD qbKey = *(DWORD*)pFile;
                pFile += 4;
                if (qbKey == meshes[i].GetName().checksum)
                {
                  BYTE tmpData[1900];
                  DWORD size = (DWORD)((pFile)-offset);
                  DWORD backupSize = (DWORD)(eof - offset);
                  memcpy(tmpData, offset, backupSize);
                  *(DWORD*)&*(offset + size - 4) = mesh.GetName().checksum;
                  *(offset + size) = 0x2;
                  size++;
                  *(DWORD*)&*(offset + size) = 0x00000001;
                  size += 4;
                  KnownScripts[j].size += size;
                  eof += size;
                  pFile += size;
                  memcpy(offset + size, tmpData, backupSize);
                }
                else
                {
                  switch (qbKey)
                  {
                  case 0xEEC24148:
                  case 0xCB8B5900:
                  case 0x854F17A6:
                  case 0x70291F04:
                  case 0xF8D6A996:
                  case 0x6E77719D:
                    //kill, create, shatter, shatteranddie, visible, invisible
                    if (*pFile == 0x16)
                    {
                      offset = (pFile - 5);
                      pFile++;
                      if (*(DWORD*)pFile == 0xA1DC81F9)
                      {
                        pFile += 4;
                        while (*pFile != 0x16) pFile++;
                        pFile++;
                        if (*(DWORD*)pFile == meshes[i].GetName().checksum)
                        {
                          BYTE tmpData[1900];
                          DWORD size = (DWORD)((pFile + 4) - offset);
                          DWORD backupSize = (DWORD)(eof - offset);
                          memcpy(tmpData, offset, backupSize);
                          *(DWORD*)&*(offset + size - 4) = mesh.GetName().checksum;
                          *(offset + size) = 0x2;
                          size++;
                          *(DWORD*)&*(offset + size) = 0x00000001;
                          size += 4;
                          KnownScripts[j].size += size;
                          eof += size;
                          pFile += size;
                          memcpy(offset + size, tmpData, backupSize);
                        }

                        pFile += 4;
                      }
                      else
                        pFile += 4;
                    }
                    break;
                  }
                }
                break;
              }
            }
          done:;
          }
          Node* node = GetNode(meshes[i].GetName());
          if (node)
          {
            nodes.push_back(*node);
            Node & n = nodes.back();
            n.Name = mesh.GetName();
            if (n.TrickObject)
            {
              if (!n.Cluster.checksum)
              {
                n.Cluster = mesh.GetName();
                node = GetNode(meshes[i].GetName());
                node->Cluster = n.Cluster;
              }
            }
          }
        }
      }
    }



    /*for(DWORD j=0; j<KnownScripts.size(); j++)
    {
    BYTE* pFile = KnownScripts[j].func;
    BYTE* eof = pFile+KnownScripts[j].size;
    BYTE* offset;
    while(pFile<eof)
    {
    const BYTE opcode = *pFile;
    pFile++;
    switch(opcode)
    {
    case 0x1C:
    case 0x1B:
    pFile+=*(DWORD*)pFile+4;
    break;
    case 0x17:
    case 0x1A:
    case 0x2E:
    case 2:
    pFile+=4;
    break;
    case 0x1F:
    pFile+=8;
    break;
    case 0x1E:
    pFile+=12;
    break;
    case 0x2F:
    pFile+=*(DWORD*)pFile*4+4;
    break;
    case 0x16:
    const DWORD qbKey = *(DWORD*)pFile;
    pFile+=4;
    switch(qbKey)
    {
    case 0xEEC24148:
    case 0xCB8B5900:
    case 0x854F17A6:
    case 0x70291F04:
    case 0xF8D6A996:
    case 0x6E77719D:
    //kill, create, shatter, shatteranddie, visible, invisible
    if(*pFile==0x16)
    {
    offset=(pFile-5);
    pFile++;
    if(*(DWORD*)pFile==0xA1DC81F9)
    {
    pFile+=4;
    bool global=false;
    while(*pFile!=0x16) { if(*pFile==0x2D) global=true; pFile++;}
    pFile++;
    if(global)
    {
    pFile+=4;
    goto found;
    }
    DWORD name = *(DWORD*)pFile;
    pFile+=4;
    for(DWORD k=0; k<nodes.size(); k++)
    {
    if(nodes[k].Name.checksum==name)
    {
    if(nodes[k].Class.checksum==EnvironmentObject::GetClass())
    {
    for(DWORD l=0; l<meshes.size(); l++)
    {
    if(meshes[l].GetName().checksum==name)
    goto found;
    }
    }
    else
    goto found;
    break;
    }
    }
    while(pFile<eof)
    {
    const BYTE op = *pFile;
    pFile++;
    switch(op)
    {
    case 0x1C:
    case 0x1B:
    pFile+=*(DWORD*)pFile+4;
    break;

    case 2:
    pFile+=4;
    goto ya;
    break;
    case 1:
    goto ya;
    break;

    case 0x16:
    case 0x17:
    case 0x1A:
    case 0x2E:
    pFile+=4;
    break;
    case 0x1F:
    pFile+=8;
    break;
    case 0x1E:
    pFile+=12;
    break;
    case 0x2F:
    pFile+=*(DWORD*)pFile*4+4;
    break;
    }
    }
    ya:
    //MessageBox(0,(*(Checksum*)&name).GetString(),(*(Checksum*)&name).GetString(),0);
    DWORD backupSize = (DWORD)(eof-pFile);
    memcpy(offset, pFile, backupSize);
    DWORD size = (DWORD)(pFile-offset);
    KnownScripts[j].size-=size;
    eof-=size;
    pFile-=size;
    found:;
    }
    else
    pFile+=4;
    }
    break;
    }
    break;
    }
    }
    }*/
    //FixNodes();
    for (DWORD i = 0; i < extraLayerMeshes.size(); i++)
    {
      meshes.push_back(Mesh());
      Mesh& mesh = meshes.back();
      Node* node = GetNode(extraLayerMeshes[i].name);
      if (node)
      {
        if (!node->CreatedAtStart)
          sprintf(Name, "InvisibleExtraLayer_%s", extraLayerMeshes[i].name.GetString());
        else
          sprintf(Name, "ExtraLayer_%s", extraLayerMeshes[i].name.GetString());
        mesh.SetName(Name);
        nodes.push_back(*node);
        nodes.back().Name = mesh.GetName();
      }
      else
      {
        sprintf(Name, "ExInvisibleExtraLayer_%s", extraLayerMeshes[i].name.GetString());
        mesh.SetName(Name);
        nodes.push_back(EnvironmentObject(mesh.GetName(), 0, 0, false, false));
      }
      vector<DWORD> addedMaterials;
      DWORD vOffset = 0;
      Vertex v;
      for (DWORD j = 0; j < extraLayerMeshes[i].vertices.size(); j++)
      {
        v.x = extraLayerMeshes[i].vertices[j].x;
        v.y = extraLayerMeshes[i].vertices[j].y;
        v.z = extraLayerMeshes[i].vertices[j].z;
        v.colour = extraLayerMeshes[i].vertices[j].colour;
        if (v.colour.a)
          v.colour.a /= 2;
        v.tUV[0] = extraLayerMeshes[i].vertices[j].tUV[0];
        mesh.AddVertexSilent(v);
      }
      for (DWORD j = 0; j < extraLayerMeshes[i].matSplits.size(); j++)
      {
        MaterialSplit split;
        split.type = 0;

        Material* mat = matList.GetMaterial((Checksum)extraLayerMeshes[i].matSplits[j].matId);
        if (mat)
        {
          for (DWORD k = 0; k < addedMaterials.size(); k++)
          {
            if (addedMaterials[k] == mat->GetMatId())
              goto found;
          }
          if (mat)
          {
            Material material;
            material.Reserve(1);
            material.SetMatId(mat->GetMatId() + 1);
            material.doubled = mat->doubled;
            material.drawOrder = mat->drawOrder + 1.0f;
            material.ignoreAlpha = false;
            //material.alphaMap=true;
            material.extraLayer = true;
            Texture* texture = mat->GetTexture(1);
            if (texture->blendMode == 1 || texture->blendMode == 2 || texture->blendMode == 4 || texture->blendMode == 5 || texture->blendMode == 6)
            {
              static char imageName[MAX_PATH] = "";
              static char tmp[15] = "";
              GetEnvironmentVariable("TEMP", imageName, MAX_PATH);
              DWORD chc = texture->GetTexId();
              char* Name = (*(Checksum*)&(chc)).GetString();
              if (Name == NULL)
              {
                sprintf(tmp, "\\%08X.dds", texture->GetTexId());
                strcat(imageName, tmp);
              }
              else
              {
                DWORD len = strlen(imageName);
                imageName[len] = '\\';
                len++;
                strcpy(&imageName[len], Name);
                strcat(imageName, ".dds");
              }
              //HRESULT hres = D3DXCreateTextureFromFile(Device, imageName, &imageData);
              FILE* f = fopen(imageName, "rb");
              if (f)
              {
                extern TexFiles texFiles;
                char imageName2[MAX_PATH];
                strcpy(imageName2, imageName);
                imageName2[strlen(imageName) - 3] = 't';
                imageName2[strlen(imageName) - 2] = 'g';
                imageName2[strlen(imageName) - 1] = 'a';
                for (DWORD l = 0; l < texFiles.numFiles; l++)
                {
                  if (!stricmp(texFiles.fileNames[l], imageName))
                  {
                    texFiles.fileNames[l][strlen(imageName) - 3] = 't';
                    texFiles.fileNames[l][strlen(imageName) - 2] = 'g';
                    texFiles.fileNames[l][strlen(imageName) - 1] = 'a';
                  }
                }
                fseek(f, 0, SEEK_END);
                DWORD size = ftell(f);
                BYTE* data = new BYTE[size];
                fseek(f, 0, SEEK_SET);
                fread(data, size, 1, f);
                fclose(f);
                DeleteFile(imageName);
                f = fopen(imageName2, "w+b");
                if (f)
                {
                  DWORD imageSize;
                  BYTE* imageData = data;
                  DDS_HEADER* header = (DDS_HEADER*)imageData;
                  imageData += sizeof(DDS_HEADER);

                  WORD width = (WORD)header->width;
                  WORD height = (WORD)header->height;

                  if (header->pixelFormat[1] & 0x4)
                  {
                    imageSize = header->width*header->height * 4;
                    BYTE* levelData = new BYTE[imageSize];
                    DecompressDXT(imageData, header->imageSize, header->width, header->height, levelData, (((header->pixelFormat[2] & 0xFF000000) >> 24) - 0x30));
                    fputc(0, f);
                    fputc(0, f);
                    fputc(2, f);
                    DWORD zero = 0;
                    fwrite(&zero, 4, 1, f);
                    fwrite(&zero, 4, 1, f);
                    fputc(0, f);
                    fwrite(&header->width, 2, 1, f);
                    fwrite(&header->height, 2, 1, f);
                    fputc(32, f);
                    fputc(32, f);
                    Colour* color = (Colour*)levelData;
                    FixBlending(color, imageSize / 4, texture);
                    fwrite(levelData, imageSize, 1, f);
                    delete[] levelData;
                  }
                  else
                  {
                    imageSize = header->width*header->height * 4;
                    DWORD bytesPerPixel = header->imageSize / header->width / header->height;
                    bool usingAlpha = header->pixelFormat[1] & 0x2;

                    if (bytesPerPixel == 4)
                    {
                      fputc(0, f);
                      fputc(0, f);
                      fputc(2, f);
                      DWORD zero = 0;
                      fwrite(&zero, 4, 1, f);
                      fwrite(&zero, 4, 1, f);
                      fputc(0, f);
                      fwrite(&header->width, 2, 1, f);
                      fwrite(&header->height, 2, 1, f);
                      fputc(32, f);
                      fputc(32, f);
                      Colour* color = (Colour*)imageData;
                      FixBlending(color, imageSize / 4, texture);
                      fwrite(imageData, imageSize, 1, f);
                    }
                    else if (bytesPerPixel == 2)
                    {
                      imageSize = header->width*header->height * 4;
                      DWORD numPixels = imageSize / 4;
                      Colour* levelData = new Colour[numPixels];
                      //MessageBox(0,"sure this is 16 bit?",imageName,0);
                      if (usingAlpha)
                      {
                        for (DWORD l = 0; l < numPixels; l++)
                        {
                          BYTE r = ((BYTE)((((WORD*)imageData)[l] & 0x003E) >> 1));
                          BYTE g = ((BYTE)((((WORD*)imageData)[l] & 0x7C0) >> 6));
                          BYTE b = ((BYTE)((((WORD*)imageData)[l] & 0xF800) >> 12));
                          BYTE a = ((BYTE)((((WORD*)imageData)[l] & 0x1)));
                          levelData[l].r = (r << 3) | (r >> 2);
                          levelData[l].g = (g << 3) | (g >> 2);
                          levelData[l].b = (b << 3) | (b >> 2);
                          if (a)
                            levelData[l].a = 255;
                          else
                            levelData[l].a = 0;
                        }
                      }
                      else
                      {
                        for (DWORD l = 0; l < numPixels; l++)
                        {
                          BYTE r = (BYTE)(((WORD*)imageData)[l] & 0x001F);
                          BYTE g = ((BYTE)((((WORD*)imageData)[l] & 0x7E0) >> 5));
                          BYTE b = ((BYTE)((((WORD*)imageData)[l] & 0xF800) >> 11));
                          levelData[l].r = (r << 3) | (r >> 2);
                          levelData[l].g = (g << 2) | (g >> 4);
                          levelData[l].b = (b << 3) | (b >> 2);
                          levelData[l].a = 255;
                        }
                      }
                      fputc(0, f);
                      fputc(0, f);
                      fputc(2, f);
                      DWORD zero = 0;
                      fwrite(&zero, 4, 1, f);
                      fwrite(&zero, 4, 1, f);
                      fputc(0, f);
                      fwrite(&header->width, 2, 1, f);
                      fwrite(&header->height, 2, 1, f);
                      fputc(32, f);
                      fputc(32, f);
                      Colour* color = (Colour*)levelData;
                      FixBlending(color, imageSize / 4, texture);
                      fwrite(levelData, imageSize, 1, f);
                      delete[] levelData;
                      //delete [] levelData;
                    }
                    else if (bytesPerPixel == 1)
                      MessageBox(0, "1", 0, 0);
                    else
                      MessageBox(0, "unknown", 0, 0);

                    //MessageBox(0,"wtf this texture is not compressed","wawwiee",0);
                    //levelData = (pImageData;
                  }
                  fclose(f);
                }

                delete[] data;
              }
              /*else
              {
              GetEnvironmentVariable("TEMP",imageName, MAX_PATH);
              if(name == NULL)
              {
              sprintf(tmp,"\\%08X.tga\0",texture->GetTexId());
              strcat(imageName,tmp);
              }
              else
              {
              DWORD len = strlen(imageName);
              imageName[len] = '\\';
              len++;
              strcpy(&imageName[len],name);
              strcat(imageName,".tga");
              }
              f = fopen(imageName, "rb");
              if(f)
              {
              //MessageBox(0,imageName,imageName,0);
              }
              }*/
              //material.reverse=true;
            }
            /*else
            {
            static char imageName[MAX_PATH] = "";
            static char tmp[15] = "";
            GetEnvironmentVariable("TEMP",imageName, MAX_PATH);
            DWORD chc = texture->GetTexId();
            char* name=(*(Checksum*)&(chc)).GetString();
            if(name == NULL)
            {
            sprintf(tmp,"\\%08X.dds",texture->GetTexId());
            strcat(imageName,tmp);
            }
            else
            {
            DWORD len = strlen(imageName);
            imageName[len] = '\\';
            len++;
            strcpy(&imageName[len],name);
            strcat(imageName,".dds");
            }
            //HRESULT hres = D3DXCreateTextureFromFile(Device, imageName, &imageData);
            FILE* f = fopen(imageName, "rb");
            if(f)
            {

            }
            else
            {
            GetEnvironmentVariable("TEMP",imageName, MAX_PATH);
            if(name == NULL)
            {
            sprintf(tmp,"\\%08X.tga\0",texture->GetTexId());
            strcat(imageName,tmp);
            }
            else
            {
            DWORD len = strlen(imageName);
            imageName[len] = '\\';
            len++;
            strcpy(&imageName[len],name);
            strcat(imageName,".tga");
            }
            f = fopen(imageName, "rb");
            if(f)
            {

            }
            }
            }*/
            material.transparent = true;
            material.AddTexture2(texture);
            material.GetTexture(0)->CreateTexture();
            matList.AddMaterial(&material);
            addedMaterials.push_back(mat->GetMatId());
          }
        found:
          split.matId = mat->GetMatId() + 1;
          split.numIndices = extraLayerMeshes[i].matSplits[j].Indices.size();
          if (split.numIndices)
          {
            split.AddIndices(&extraLayerMeshes[i].matSplits[j].Indices.front(), split.numIndices, vOffset);
            mesh.AddMatSplit(&split, &matList);
          }
          else
            MessageBox(0, "no indices?", "", 0);
        }
        //vOffset+=extraLayerMeshes[i].vertices[j].size();
      }
      for (DWORD j = 1; j < extraLayerMeshes[i].numPasses && j < 3; j++)
      {
        meshes.push_back(Mesh());
        Mesh & mesh2 = meshes.back();//th4
        Node* node2 = GetNode(extraLayerMeshes[i].name);
        if (node2)
        {
          if (!node2->CreatedAtStart)
            sprintf(Name, "InvisibleExtraLayer%u_%s", j, extraLayerMeshes[i].name.GetString());
          else
            sprintf(Name, "ExtraLayer%u_%s", j, extraLayerMeshes[i].name.GetString());
          mesh2.SetName(Name);
          nodes.push_back(*node2);
          nodes.back().Name = mesh2.GetName();
        }
        else
        {
          sprintf(Name, "ExInvisibleExtraLayer%u_%s", j, extraLayerMeshes[i].name.GetString());
          mesh2.SetName(Name);
          nodes.push_back(EnvironmentObject(mesh2.GetName(), 0, 0, false, false));
        }
        Vertex v2;
        for (DWORD k = 0; k < extraLayerMeshes[i].vertices.size(); k++)
        {
          v2.x = extraLayerMeshes[i].vertices[k].x;
          v2.y = extraLayerMeshes[i].vertices[k].y;
          v2.z = extraLayerMeshes[i].vertices[k].z;
          v2.colour = extraLayerMeshes[i].vertices[k].colour;
          if (v2.colour.a)
            v2.colour.a /= 2;
          v2.tUV[0] = extraLayerMeshes[i].vertices[k].tUV[j];
          mesh2.AddVertexSilent(v2);
        }
        for (DWORD k = 0; k<extraLayerMeshes[i].matSplits.size(); k++)
        {
          MaterialSplit split;
          split.type = 0;


          Material* mat = matList.GetMaterial((Checksum)extraLayerMeshes[i].matSplits[k].matId);
          if (mat && mat->GetNumTextures()>j + 1)
          {
            if (mat)
            {
              Material material;
              material.Reserve(1);
              material.SetMatId(mat->GetMatId() + j + 1);
              material.doubled = mat->doubled;
              material.drawOrder = mat->drawOrder + (float)j + 1.0f;
              material.ignoreAlpha = false;
              //material.alphaMap=true;
              material.extraLayer = true;
              Texture* texture = mat->GetTexture(j + 1);
              if (texture->blendMode == 1 || texture->blendMode == 2 || texture->blendMode == 4 || texture->blendMode == 5 || texture->blendMode == 6)
              {
                static char imageName[MAX_PATH] = "";
                static char tmp[15] = "";
                GetEnvironmentVariable("TEMP", imageName, MAX_PATH);
                DWORD chc = texture->GetTexId();
                char* Name = (*(Checksum*)&(chc)).GetString();
                if (Name == NULL)
                {
                  sprintf(tmp, "\\%08X.dds", texture->GetTexId());
                  strcat(imageName, tmp);
                }
                else
                {
                  DWORD len = strlen(imageName);
                  imageName[len] = '\\';
                  len++;
                  strcpy(&imageName[len], Name);
                  strcat(imageName, ".dds");
                }
                //HRESULT hres = D3DXCreateTextureFromFile(Device, imageName, &imageData);
                FILE* f = fopen(imageName, "rb");
                if (f)
                {
                  extern TexFiles texFiles;
                  char imageName2[MAX_PATH];
                  strcpy(imageName2, imageName);
                  imageName2[strlen(imageName) - 3] = 't';
                  imageName2[strlen(imageName) - 2] = 'g';
                  imageName2[strlen(imageName) - 1] = 'a';
                  for (DWORD l = 0; l < texFiles.numFiles; l++)
                  {
                    if (!stricmp(texFiles.fileNames[l], imageName))
                    {
                      texFiles.fileNames[l][strlen(imageName) - 3] = 't';
                      texFiles.fileNames[l][strlen(imageName) - 2] = 'g';
                      texFiles.fileNames[l][strlen(imageName) - 1] = 'a';
                    }
                  }
                  fseek(f, 0, SEEK_END);
                  DWORD size = ftell(f);
                  BYTE* data = new BYTE[size];
                  fseek(f, 0, SEEK_SET);
                  fread(data, size, 1, f);
                  fclose(f);
                  DeleteFile(imageName);
                  f = fopen(imageName2, "w+b");
                  if (f)
                  {
                    DWORD imageSize;
                    BYTE* imageData = data;
                    DDS_HEADER* header = (DDS_HEADER*)imageData;
                    imageData += sizeof(DDS_HEADER);

                    WORD width = (WORD)header->width;
                    WORD height = (WORD)header->height;

                    if (header->pixelFormat[1] & 0x4)
                    {
                      imageSize = header->width*header->height * 4;
                      BYTE* levelData = new BYTE[imageSize];
                      DecompressDXT(imageData, header->imageSize, header->width, header->height, levelData, (((header->pixelFormat[2] & 0xFF000000) >> 24) - 0x30));
                      fputc(0, f);
                      fputc(0, f);
                      fputc(2, f);
                      DWORD zero = 0;
                      fwrite(&zero, 4, 1, f);
                      fwrite(&zero, 4, 1, f);
                      fputc(0, f);
                      fwrite(&header->width, 2, 1, f);
                      fwrite(&header->height, 2, 1, f);
                      fputc(32, f);
                      fputc(32, f);
                      Colour* color = (Colour*)levelData;
                      FixBlending(color, imageSize / 4, texture);
                      fwrite(levelData, imageSize, 1, f);
                      delete[] levelData;
                    }
                    else
                    {
                      imageSize = header->width*header->height * 4;
                      DWORD bytesPerPixel = header->imageSize / header->width / header->height;
                      bool usingAlpha = header->pixelFormat[1] & 0x2;

                      if (bytesPerPixel == 4)
                      {
                        fputc(0, f);
                        fputc(0, f);
                        fputc(2, f);
                        DWORD zero = 0;
                        fwrite(&zero, 4, 1, f);
                        fwrite(&zero, 4, 1, f);
                        fputc(0, f);
                        fwrite(&header->width, 2, 1, f);
                        fwrite(&header->height, 2, 1, f);
                        fputc(32, f);
                        fputc(32, f);
                        Colour* color = (Colour*)imageData;
                        FixBlending(color, imageSize / 4, texture);
                        fwrite(imageData, imageSize, 1, f);
                      }
                      else if (bytesPerPixel == 2)
                      {
                        imageSize = header->width*header->height * 4;
                        DWORD numPixels = imageSize / 4;
                        Colour* levelData = new Colour[numPixels];
                        //MessageBox(0,"sure this is 16 bit?",imageName,0);
                        if (usingAlpha)
                        {
                          for (DWORD l = 0; l < numPixels; l++)
                          {
                            BYTE r = ((BYTE)((((WORD*)imageData)[l] & 0x003E) >> 1));
                            BYTE g = ((BYTE)((((WORD*)imageData)[l] & 0x7C0) >> 6));
                            BYTE b = ((BYTE)((((WORD*)imageData)[l] & 0xF800) >> 12));
                            BYTE a = ((BYTE)((((WORD*)imageData)[l] & 0x1)));
                            levelData[l].r = (r << 3) | (r >> 2);
                            levelData[l].g = (g << 3) | (g >> 2);
                            levelData[l].b = (b << 3) | (b >> 2);
                            if (a)
                              levelData[l].a = 255;
                            else
                              levelData[l].a = 0;
                          }
                        }
                        else
                        {
                          for (DWORD l = 0; l < numPixels; l++)
                          {
                            BYTE r = (BYTE)(((WORD*)imageData)[l] & 0x001F);
                            BYTE g = ((BYTE)((((WORD*)imageData)[l] & 0x7E0) >> 5));
                            BYTE b = ((BYTE)((((WORD*)imageData)[l] & 0xF800) >> 11));
                            levelData[l].r = (r << 3) | (r >> 2);
                            levelData[l].g = (g << 2) | (g >> 4);
                            levelData[l].b = (b << 3) | (b >> 2);
                            levelData[l].a = 255;
                          }
                        }
                        fputc(0, f);
                        fputc(0, f);
                        fputc(2, f);
                        DWORD zero = 0;
                        fwrite(&zero, 4, 1, f);
                        fwrite(&zero, 4, 1, f);
                        fputc(0, f);
                        fwrite(&header->width, 2, 1, f);
                        fwrite(&header->height, 2, 1, f);
                        fputc(32, f);
                        fputc(32, f);
                        Colour* color = (Colour*)levelData;
                        FixBlending(color, imageSize / 4, texture);
                        fwrite(levelData, imageSize, 1, f);
                        delete[] levelData;
                        //delete [] levelData;
                      }
                      else if (bytesPerPixel == 1)
                        MessageBox(0, "1", 0, 0);
                      else
                        MessageBox(0, "unknown", 0, 0);

                      //MessageBox(0,"wtf this texture is not compressed","wawwiee",0);
                      //levelData = (pImageData;
                    }
                    fclose(f);
                  }

                  delete[] data;
                }
                /*else
                {
                GetEnvironmentVariable("TEMP",imageName, MAX_PATH);
                if(name == NULL)
                {
                sprintf(tmp,"\\%08X.tga\0",texture->GetTexId());
                strcat(imageName,tmp);
                }
                else
                {
                DWORD len = strlen(imageName);
                imageName[len] = '\\';
                len++;
                strcpy(&imageName[len],name);
                strcat(imageName,".tga");
                }
                f = fopen(imageName, "rb");
                if(f)
                {
                //MessageBox(0,imageName,imageName,0);
                }
                //material.reverse=true;
                }*/
              }
              _fcloseall();
              material.transparent = true;
              material.AddTexture2(texture);
              material.GetTexture(0)->CreateTexture();
              matList.AddMaterial(&material);
            }
            split.matId = mat->GetMatId() + j + 1;
            split.numIndices = extraLayerMeshes[i].matSplits[k].Indices.size();
            if (split.numIndices)
            {
              split.AddIndices(&extraLayerMeshes[i].matSplits[k].Indices.front(), split.numIndices, vOffset);
              mesh2.AddMatSplit(&split, &matList);
            }
            else
              MessageBox(0, "no indices2?", "", 0);
          }
        }
      }
    }
    extraLayerMeshes.clear();
  }

};


/*
void Th4Scene::ProcessTextures(DWORD ptr)
{

}*/
/*
#define TEXFLAG_UNKNOWN 1
#define TEXFLAG_EXTENSION 2

#define MESHFLAG_TEXCOORD 1
#define MESHFLAG_COLOUR 2
#define MESHFLAG_NORMAL 4
#define MESHFLAG_SKIN 16
#define MESHFLAG_EXTENSION 2048*/


void Th4Scene::ProcessMeshes(DWORD ptr)
{
  DWORD numGrassMeshes = 0;
  DWORD numObjects = ((DWORD*)ptr)[0];
  ptr += 4; meshes.reserve(numObjects);
  for (DWORD i = 0; i < numObjects; i++)
  {

    Th4Mesh::Data* meshData = (Th4Mesh::Data*)malloc(sizeof(Th4Mesh::Data));
    memcpy(meshData, (Th4Mesh::Data*)ptr, 64);
    Mesh* mesh = new Th4Mesh(meshData);

    ptr += 64;

    meshData->vertices = (SimpleVertex*)ptr;
    ptr += 12 * meshData->numVertices;

    if (meshData->flags & MESHFLAG_NORMAL)
    {
      meshData->normals = (SimpleVertex*)ptr;
      ptr += 12 * meshData->numVertices;
    }
    else
      meshData->normals = NULL;

    if (meshData->flags & MESHFLAG_SKIN)
    {
      ptr += 24 * meshData->numVertices;
    }
    DWORD numTexCoords = 0;
    if (meshData->flags & MESHFLAG_TEXCOORD)
    {
      // meshData->texCoords = new TexCoord[meshData->numVertices];


      numTexCoords = ((DWORD*)ptr)[0]; meshData->texCoords = new TexCoord[meshData->numVertices*numTexCoords]; if (numTexCoords > MAX_TEXCOORDS) MAX_TEXCOORDS = numTexCoords;//mesh->SetNumTexCoords(numTexCoords);
      ptr += 4;
      for (DWORD j = 0; j < meshData->numVertices*numTexCoords; j++)
      {
        meshData->texCoords[j] = ((TexCoord*)ptr)[0];
        ptr += 8;//*numTexCoords;
      }

    }
    else
      meshData->texCoords = NULL;

    if (meshData->flags & MESHFLAG_COLOUR)
    {
      meshData->colours = (Colour*)ptr;
      ptr += 4 * meshData->numVertices;
    }
    else
      meshData->colours = NULL;

    if (meshData->flags & MESHFLAG_EXTENSION)
    {
      ptr += meshData->numVertices;//colour wibble data?
    }


    Vertex* vertices = new Vertex[meshData->numVertices];

    for (int j = 0; j < meshData->numVertices; j++)
    {
      meshData->vertices[j].z *= -1;
      memcpy(&vertices[j], &meshData->vertices[j], 12);

      if (meshData->texCoords)
      {
        /*for(DWORD k=0; k<numTexCoords; k++)
        {
        vertices[j].tUV[k].u = meshData->texCoords[j*numTexCoords+k].u;
        vertices[j].tUV[k].v = meshData->texCoords[j*numTexCoords+k].v;
        }

        for(DWORD k=numTexCoords; k<4; k++)
        {
        vertices[j].tUV[k].u = meshData->texCoords[j*numTexCoords+numTexCoords].u;
        vertices[j].tUV[k].u = meshData->texCoords[j*numTexCoords+numTexCoords].v;
        }
        */

        vertices[j].tUV[0] = meshData->texCoords[j*numTexCoords];

        /*for(DWORD k=0; k<numTexCoords; k++)
        {
        vertices[j].uv[k*2] = meshData->texCoords[j+k].u;
        vertices[j].uv[k*2+1] = meshData->texCoords[j+k].v;
        }*/
        /*vertices[j].u = meshData->texCoords[j].u;
        vertices[j].v = meshData->texCoords[j].v;*/
      }

      if (meshData->colours)
      {
        //meshData->colours[j].a = 0xFF;
        vertices[j].colour = meshData->colours[j];
        /*if (meshData->name.checksum == 0xE18484C7 || meshData->name.checksum == 0xC78484E1)
        {
          if (vertices[j].colour.a != 0xFF)
            MessageBox(0, "", "", 0);
        }*/
      }

    }

    mesh->Reserve(meshData->numVertices);
    mesh->AddVertices(vertices, meshData->numVertices);
    /*if (meshData->name.checksum == 0xE18484C7 || meshData->name.checksum == 0xC78484E1)
    {
      if (mesh->GetVertex(2).colour.a != 0xFF)
        MessageBox(0, "Yaw", "Yaw", 0);
    }*/

    delete[] vertices;
    delete[] meshData->texCoords;
    Mesh* grassMesh = NULL;

    for (DWORD j = 0; j < meshData->numSplits; j++)
    {
      MaterialSplit matSplit;// = new MaterialSplit;
      memcpy(&matSplit, (MaterialSplit*)ptr, 12);
      matSplit.type = 0;
      ptr += 12;

      WORD* Indices = (WORD*)ptr;
      matSplit.AddIndices(Indices, matSplit.numIndices);

      DWORD id = matSplit.matId;
      Material* mat = matList.GetMaterial((Checksum)id);
      if (mat)
      {
        /*if (id == 0xE6007EC7 || id == 0xC77E00E6)
          MessageBox(0, "WTF", "", 0);*/
        Texture* texture = mat->GetTexture(0);
        if (texture)
        {
          /*char* blendings[] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10" };
          if (texture->GetTexId() == 0x0926312C || texture->GetTexId() == 0x2C312609)
          MessageBox(0, "YE", blendings[texture->blendMode], 0);*/
          if (texture->blendMode == 1 || texture->blendMode == 2 || texture->blendMode == 4 || texture->blendMode == 5 || texture->blendMode == 6)
          {
            static char imageName[MAX_PATH] = "";
            static char tmp[15] = "";
            GetEnvironmentVariable("TEMP", imageName, MAX_PATH);
            DWORD chc = texture->GetTexId();
            char* Name = (*(Checksum*)&(chc)).GetString();
            if (Name == NULL)
            {
              sprintf(tmp, "\\%08X.dds", texture->GetTexId());
              strcat(imageName, tmp);
            }
            else
            {
              DWORD len = strlen(imageName);
              imageName[len] = '\\';
              len++;
              strcpy(&imageName[len], Name);
              strcat(imageName, ".dds");
            }
            //HRESULT hres = D3DXCreateTextureFromFile(Device, imageName, &imageData);
            FILE* f = fopen(imageName, "rb");
            if (f)
            {
              extern TexFiles texFiles;
              char imageName2[MAX_PATH];
              strcpy(imageName2, imageName);
              imageName2[strlen(imageName) - 3] = 't';
              imageName2[strlen(imageName) - 2] = 'g';
              imageName2[strlen(imageName) - 1] = 'a';
              for (DWORD l = 0; l < texFiles.numFiles; l++)
              {
                if (!stricmp(texFiles.fileNames[l], imageName))
                {
                  texFiles.fileNames[l][strlen(imageName) - 3] = 't';
                  texFiles.fileNames[l][strlen(imageName) - 2] = 'g';
                  texFiles.fileNames[l][strlen(imageName) - 1] = 'a';
                }
              }
              fseek(f, 0, SEEK_END);
              DWORD size = ftell(f);
              BYTE* data = new BYTE[size];
              fseek(f, 0, SEEK_SET);
              fread(data, size, 1, f);
              fclose(f);
              DeleteFile(imageName);
              f = fopen(imageName2, "w+b");
              if (f)
              {
                DWORD imageSize;
                BYTE* imageData = data;
                DDS_HEADER* header = (DDS_HEADER*)imageData;
                imageData += sizeof(DDS_HEADER);

                WORD width = (WORD)header->width;
                WORD height = (WORD)header->height;

                if (header->pixelFormat[1] & 0x4)
                {
                  imageSize = header->width*header->height * 4;
                  BYTE* levelData = new BYTE[imageSize];
                  DecompressDXT(imageData, header->imageSize, header->width, header->height, levelData, (((header->pixelFormat[2] & 0xFF000000) >> 24) - 0x30));
                  fputc(0, f);
                  fputc(0, f);
                  fputc(2, f);
                  DWORD zero = 0;
                  fwrite(&zero, 4, 1, f);
                  fwrite(&zero, 4, 1, f);
                  fputc(0, f);
                  fwrite(&header->width, 2, 1, f);
                  fwrite(&header->height, 2, 1, f);
                  fputc(32, f);
                  fputc(32, f);
                  Colour* color = (Colour*)levelData;
                  FixBlending(color, imageSize / 4, texture);
                  fwrite(levelData, imageSize, 1, f);
                  delete[] levelData;
                }
                else
                {
                  imageSize = header->width*header->height * 4;
                  DWORD bytesPerPixel = header->imageSize / header->width / header->height;
                  bool usingAlpha = header->pixelFormat[1] & 0x2;

                  if (bytesPerPixel == 4)
                  {
                    fputc(0, f);
                    fputc(0, f);
                    fputc(2, f);
                    DWORD zero = 0;
                    fwrite(&zero, 4, 1, f);
                    fwrite(&zero, 4, 1, f);
                    fputc(0, f);
                    fwrite(&header->width, 2, 1, f);
                    fwrite(&header->height, 2, 1, f);
                    fputc(32, f);
                    fputc(32, f);
                    Colour* color = (Colour*)imageData;
                    FixBlending(color, imageSize / 4, texture);
                    fwrite(imageData, imageSize, 1, f);
                  }
                  else if (bytesPerPixel == 2)
                  {
                    imageSize = header->width*header->height * 4;
                    DWORD numPixels = imageSize / 4;
                    Colour* levelData = new Colour[numPixels];
                    //MessageBox(0,"sure this is 16 bit?",imageName,0);
                    if (usingAlpha)
                    {
                      for (DWORD l = 0; l < numPixels; l++)
                      {
                        BYTE r = ((BYTE)((((WORD*)imageData)[l] & 0x003E) >> 1));
                        BYTE g = ((BYTE)((((WORD*)imageData)[l] & 0x7C0) >> 6));
                        BYTE b = ((BYTE)((((WORD*)imageData)[l] & 0xF800) >> 12));
                        BYTE a = ((BYTE)((((WORD*)imageData)[l] & 0x1)));
                        levelData[l].r = (r << 3) | (r >> 2);
                        levelData[l].g = (g << 3) | (g >> 2);
                        levelData[l].b = (b << 3) | (b >> 2);
                        if (a)
                          levelData[l].a = 255;
                        else
                          levelData[l].a = 0;
                      }
                    }
                    else
                    {
                      for (DWORD l = 0; l < numPixels; l++)
                      {
                        BYTE r = (BYTE)(((WORD*)imageData)[l] & 0x001F);
                        BYTE g = ((BYTE)((((WORD*)imageData)[l] & 0x7E0) >> 5));
                        BYTE b = ((BYTE)((((WORD*)imageData)[l] & 0xF800) >> 11));
                        levelData[l].r = (r << 3) | (r >> 2);
                        levelData[l].g = (g << 2) | (g >> 4);
                        levelData[l].b = (b << 3) | (b >> 2);
                        levelData[l].a = 255;
                      }
                    }
                    fputc(0, f);
                    fputc(0, f);
                    fputc(2, f);
                    DWORD zero = 0;
                    fwrite(&zero, 4, 1, f);
                    fwrite(&zero, 4, 1, f);
                    fputc(0, f);
                    fwrite(&header->width, 2, 1, f);
                    fwrite(&header->height, 2, 1, f);
                    fputc(32, f);
                    fputc(32, f);
                    Colour* color = (Colour*)levelData;
                    FixBlending(color, imageSize / 4, texture);
                    fwrite(levelData, imageSize, 1, f);
                    delete[] levelData;
                    //delete [] levelData;
                  }
                }
                fclose(f);
              }
              delete[] data;
            }
          }
          //mat->transparent = true;
          texture->Release();
          texture->CreateTexture();
        }
      }
      if (numTexCoords > 1)
      {
        //Material* mat = matList.GetMaterial((Checksum)id);
        if (mat && mat->GetNumTextures() > 1)
        {
          for (DWORD k = 0; k < extraLayerMeshes.size(); k++)
          {
            if (extraLayerMeshes[k].name.checksum == meshData->name.checksum)
            {
              extraLayerMeshes[k].matSplits.push_back(MaterialSplit());
              extraLayerMeshes[k].matSplits.back().matId = matSplit.matId;
              extraLayerMeshes[k].matSplits.back().numIndices = matSplit.numIndices;
              extraLayerMeshes[k].matSplits.back().Indices.assign(matSplit.Indices.begin(), matSplit.Indices.end());
              extraLayerMeshes[k].numPasses = (numTexCoords - 1);
              /*DWORD vertexIndex = extraLayerMeshes[k].vertices.size();
              extraLayerMeshes[k].vertices.resize(vertexIndex+1);
              extraLayerMeshes[k].vertices[vertexIndex].resize(meshData->numVertices);
              for(DWORD l=0; l<meshData->numVertices; l++)
              {
              for(DWORD m=1; m<numTexCoords; m++)
              {
              extraLayerMeshes[k].vertices[vertexIndex][l].x = vertices[j].x;
              extraLayerMeshes[k].vertices[vertexIndex][l].y = vertices[j].y;
              extraLayerMeshes[k].vertices[vertexIndex][l].z = vertices[j].z;
              extraLayerMeshes[k].vertices[vertexIndex][j].colour=vertices[j].colour;
              extraLayerMeshes[k].vertices[vertexIndex][l].tUV[m] = meshData->texCoords[l*numTexCoords+m];
              }
              }*/
              goto found;
            }
          }
          extraLayerMeshes.push_back(ExtraLayerMesh(meshData->name, &matSplit, mesh->GetVertices(), mesh->GetNumVertices(), meshData->texCoords, numTexCoords));
        }
      }
    found:

      mesh->AddMatSplit(&matSplit, &matList);
      Grass* grass = GetGrass(id);
      if (grass)
      {
        if (grassMesh == NULL)
        {
          grassMesh = new Mesh(*mesh);
          char name[128];
          sprintf(name, "3DGrassMesh%u", numGrassMeshes);
          numGrassMeshes++;
          grassMesh->SetName(name);
          grassMesh->Clear(CLEARFLAGS_KEEPVERTICES);
        }
        DWORD grassLayers = grass->grassLayers;
        if (grassLayers > 5)
          grassLayers = 5;
        float grassHeight = grass->grassHeight;

        float heightOffset = grassHeight / grassLayers;

        for (DWORD layer = 0; layer < grassLayers; layer++)
        {

          DWORD vertCount = grassMesh->GetNumVertices();
          MaterialSplit newSplit = matSplit;
          char material_name[64];
          sprintf(material_name, "Grass-Grass_Layer%u", layer);
          Checksum chc = GenerateCaseSensChecksum(material_name);
          Material* mat = matList.GetMaterial(*(DWORD*)&chc, &id);
          if (mat)
          {
            newSplit.matId = id;
            mat->transparent = true;
            mat->drawOrder = 18010.0f + (float)layer;
          }
          WORD* alreadyExported = new WORD[matSplit.Indices.size()];
          DWORD* newIndx = new DWORD[matSplit.Indices.size()];
          for (DWORD k = 0; k < matSplit.Indices.size(); k++)
          {
            //newSplit.Indices[i]+=vertCount;
            for (DWORD l = 0; l < k; l++)
            {
              if (alreadyExported[l] == matSplit.Indices[k])
              {
                alreadyExported[l] = 0xFFFF;
                newSplit.Indices[k] = newIndx[l];
                continue;
              }
            }
            newIndx[k] = grassMesh->GetNumVertices();
            newSplit.Indices[k] = newIndx[k];
            alreadyExported[k] = matSplit.Indices[k];
            Vertex v = grassMesh->GetVertex(matSplit.Indices[k]);
            v.y += (heightOffset*(float)(layer + 1));//+5.0f);
            v.tUV[0].u = v.x / 48.0f;
            v.tUV[0].v = v.z / 48.0f;
            grassMesh->AddVertex(&v);
            //Vertex v = mesh->GetVertex(matSplit.Indices[i]);
            /*v.y+=heightOffset*(layer+1);
            v.tUV[0].u = v.x/48.0f;
            v.tUV[0].v = v.z/48.0f;*/
            /*mesh->AddVertex(&mesh->GetVertex(matSplit.Indices[k]));
            Vertex & v = mesh->GetVertex(newIndx[k]);
            v.y+=heightOffset*(float)(layer+1);
            v.tUV[0].u = v.x/48.0f;
            v.tUV[0].v = v.z/48.0f;*/
          }
          delete[] newIndx;
          delete[] alreadyExported;
          grassMesh->AddMatSplit(&newSplit, &matList);
          grassMesh->GetSplit(grassMesh->GetNumSplits() - 1)->numIndices = grassMesh->GetSplit(grassMesh->GetNumSplits() - 1)->Indices.size() - 2;
        }
        if (grassMesh->GetNumVertices() >= 0xFFFF)
          MessageBox(0, "WTF", "", 0);
      }
      ptr += 2 * matSplit.numIndices;
      //delete matSplit;
    }
    free(meshData);
    AddMesh(mesh);
    /*if (mesh->GetName().checksum == 0xE18484C7 || mesh->GetName().checksum == 0xC78484E1)
    {
      if (this->meshes.back().GetVertex(2).colour.a != 0xFF)
        MessageBox(0, "Yaw", "Yaw", 0);
    }*/
    delete mesh;
    if (grassMesh)
    {
      AddMesh(grassMesh);
      delete grassMesh;
    }
  }
  grass.clear();
  //extraLayerMeshes.clear();
}

DWORD Th4Scene::ProcessMaterials(DWORD ptr)
{
  DWORD *numMaterials = (DWORD*)ptr;
  ptr += 4;
  for (DWORD i = 0; i<*numMaterials; i++)
  {

    Th4Material::Data* materialData = (Th4Material::Data*)ptr;
    Th4Material material = Th4Material(materialData);

    ptr += 19; if (materialData->numTextures>MAX_PASSES) MAX_PASSES = materialData->numTextures;
    if (materialData->unkFlag)
    {
      grass.push_back(Grass(materialData->matId, materialData->grassLayers, materialData->grassHeight));
      ptr += 8;
    }

    for (DWORD j = 0; j < materialData->numTextures; j++)
    {
      Th4Texture::Data* textureData = (Th4Texture::Data*)ptr;
      Th4Texture texture = Th4Texture(textureData);

      /*if(!(textureData->flags & (1<<3)))
      {*/
      material.AddTexture(&texture);
      if (material.transparent)
      {
        material.doubled = true;
        material.drawOrder *= 10.0f;
        material.drawOrder++;
        /*if(material->drawOrder == 0.0f)
        material->drawOrder=1.0f;*/
      }
      /* }
      else if(j==0 && materialData->numTextures==1)
      {
      material.AddTexture(&texture);
      material.invisible=true;
      material.transparent=true;

      }*/
      //delete texture;
      ptr += 77;

      if (textureData->flags & TEXFLAG_UNKNOWN)
      {
        if (j == 0)
        {
          this->animations.push_back(UVAnim(material.GetMatId(), (Animation*)ptr));
          material.animated = true;
        }
        ptr += 32;
      }
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
    AddMaterial(&material);
    //delete material;
  }
  return ptr;
}

ThugMaterial::ThugMaterial(Data* data)
{
  this->doubled = data->doubled;
  SetMatId(data->matId);
  drawOrder = data->drawOrder;
  transparent = false;
  Reserve(data->numTextures);
}

Th4Material::Th4Material(Data* data)
{
  this->doubled = data->doubled;
  SetMatId(data->matId);
  drawOrder = data->drawOrder;
  transparent = false;
  Reserve(data->numTextures);

}