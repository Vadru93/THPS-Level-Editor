#include "stdafx.h"
#include "Scene.h"

#include "Debug.h"
#include "ImageHandling.h"
#include "player.h"
//#include <ppl.h>
#ifndef _DEBUG
//#include <boost\filesystem.hpp>
#endif
#pragma unmanaged

extern MaterialList* __restrict globalMaterialList;
extern vector <Script> Scripts;
extern TexFiles texFiles;
extern bool renderCollision;
static const float bias = -1.0f;
extern DWORD MAX_TEXCOORDS;
extern DWORD alphaRef;
extern void IncreasProgressbar(); extern void SetProgressbarMaximum(DWORD maximum); extern void SetProgressbarValue(DWORD value);

//static const float bias = -1.0f;//-0.6f;
extern vector <SelectedTri> selectedTriangles;
//extern DWORD numMeshes;
const static BYTE init[] = { 0x07, 0x17, 0x00, 0x00, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x00 };



//TexFiles texFiles;
using std::string;
void Unswizzle(const void* src, unsigned int depth, unsigned int width, unsigned int height, void* dest)
{
	for (UINT y = 0; y < height; y++)
	{
		UINT sy = 0;
		if (y < width)
		{
			for (int bit = 0; bit < 16; bit++)
				sy |= ((y >> bit) & 1) << (2 * bit);
			sy <<= 1; // y counts twice
		}
		else
		{
			UINT y_mask = y % width;
			for (int bit = 0; bit < 16; bit++)
				sy |= ((y_mask >> bit) & 1) << (2 * bit);
			sy <<= 1; // y counts twice
			sy += (y / width) * width * width;
		}
		BYTE* d = (BYTE*)dest + y * width * depth;
		for (UINT x = 0; x < width; x++)
		{
			UINT sx = 0;
			if (x < height * 2)
			{
				for (int bit = 0; bit < 16; bit++)
					sx |= ((x >> bit) & 1) << (2 * bit);
			}
			else
			{
				int x_mask = x % (2 * height);
				for (int bit = 0; bit < 16; bit++)
					sx |= ((x_mask >> bit) & 1) << (2 * bit);
				sx += (x / (2 * height)) * 2 * height * height;
			}
			BYTE* s = (BYTE*)src + (sx + sy) * depth;
			for (unsigned int i = 0; i < depth; ++i)
				*d++ = *s++;
		}
	}
}

char* Scene::GetName()
{
	return name;
}

inline void WriteData(FILE* const __restrict f, const void* const __restrict data, const DWORD& size)
{
	fwrite(data, size, 1, f);
}

inline void WriteDWORD(FILE* const __restrict f, const DWORD& dword, const DWORD count = 1)
{
	for (DWORD i = 0; i < count; i++)
	{
		fwrite(&dword, 4, 1, f);
	}
}

inline void WriteWORD(FILE* const __restrict f, const WORD& word, const DWORD count = 1)
{
	for (DWORD i = 0; i < count; i++)
	{
		fwrite(&word, 2, 1, f);
	}
}

inline void WriteBYTE(FILE* const __restrict f, const BYTE& byte, const DWORD count = 1)
{
	for (DWORD i = 0; i < count; i++)
	{
		fputc(byte, f);
	}
}

void WriteChunkHeader(FILE* const __restrict f, const DWORD& id, const DWORD& size)
{
	static const DWORD version = 0x310;
	WriteDWORD(f, id);
	WriteDWORD(f, size);
	WriteDWORD(f, version);
}

inline void WriteChunk(FILE* const __restrict f, const DWORD& id, const DWORD& size, const void* const __restrict data)
{
	WriteChunkHeader(f, id, size);
	fwrite(data, size, 1, f);
}

struct PlaneInfo
{
	int id;
	int size;
	int version;
	int Type;
	int Value;
	int Left;//bool
	int Right;//bool
	int LeftValue;
	int RightValue;
	PlaneInfo()
	{
		id = 0x1;
		size = 0x18;
		version = 0x310;
		Type = 0;
		Value = 0;
		Left = 1;
		Right = 0;
		LeftValue = 0;
		RightValue = 0;
	}
}; PlaneInfo planeInfo;




void RwTextureExtension::Export(FILE* const __restrict f)
{
	WriteData(f, this, sizeof(RwTextureExtension));
}


struct RawImage
{
	WORD width;
	WORD height;
	Colour* pixels;
};

struct RwImage
{
	RwChunkHeader TextureNative;
	RwChunkHeader Struct;
	RawImage data;

	RwImage()
	{
		TextureNative.id = 0x15;
		Struct.id = 1;
		Struct.size = 4;
		TextureNative.size = Struct.size + 12 * 2;
	}
};

struct TDX
{
	RwChunkHeader TextureDictionary;
	RwChunkHeader Struct;

	RwImage* images;
	RwChunkHeader extension;
};



void RwTexture::Export(FILE* const __restrict f)
{
	WriteChunkHeader(f, 0x1, structSize);
	/*DWORD flags = GetFlags();
	WriteData(f,&flags,4);*/
	if (uAddress == D3DTADDRESS_CLAMP)
	{
		if (vAddress == D3DTADDRESS_CLAMP)
			WriteDWORD(f, 0x00003306);
		else
			WriteDWORD(f, 0x00001306);
	}
	else if (vAddress == D3DTADDRESS_CLAMP)
		WriteDWORD(f, 0x00003106);
	else
		WriteDWORD(f, 0x00001106);
	WriteChunkHeader(f, 0x2, imageSize);
	WriteData(f, name, imageSize);
	if (!alphaMap)
	{
		WriteChunkHeader(f, 0x2, 4);
		WriteDWORD(f, 0);
	}
	else
	{
		WriteChunkHeader(f, 0x2, imageSize);
		WriteData(f, name, imageSize);
	}
	WriteChunkHeader(f, 0x3, extensionSize);
	extension.Export(f);
}

void RwMaterial::Export(FILE* const __restrict f)
{
	WriteChunkHeader(f, 0x7, size);
	WriteChunkHeader(f, 0x1, structSize);
	WriteDWORD(f, 0);
	if (extraLayer)
	{
		if (reverse)
			WriteDWORD(f, 0x80808080, 2);
		else
			WriteDWORD(f, 0x80FFFFFF, 2);
	}
	else if (reverse)
		WriteDWORD(f, 0xFF808080, 2);
	else
		WriteDWORD(f, 0xFFFFFFFF, 2);
	WriteData(f, &flags, 4);
	WriteDWORD(f, 0x3F800000, 3);
	WriteChunkHeader(f, 0x6, textureSize);
	texture.Export(f);
	WriteChunkHeader(f, 0x3, extensionSize);
	extension.Export(f);
	WriteChunkHeader(f, 0x6, textureSize);
	texture.Export(f);
	WriteBYTE(f, animated);
	WriteData(f, &anim, sizeof(Animation));
	/*WriteDWORD(f,0,2);
	WriteDWORD(f,0x3F800000,2);
	WriteDWORD(f,0,2);
	WriteDWORD(f,0x3F800000,2);*/
	WriteDWORD(f, 0, 6);
	WriteWORD(f, 0);
}




void RwMaterialExtension::Export(FILE* const __restrict f)
{
	WriteChunkHeader(f, 0x294AF01, th3extensionSize);
	WriteData(f, &index, 4);
	WriteDWORD(f, 0, 3);
	WriteDWORD(f, flags);
	WriteBYTE(f, sound);
	WriteDWORD(f, 0xFFFFFFFF, 2);
	WriteDWORD(f, 0xFF000000, 2);
	WriteBYTE(f, alpha);
	WriteBYTE(f, 0, 3);
	WriteDWORD(f, 0);
	WriteBYTE(f, 1);
	WriteWORD(f, 0);
}


char tmp[128];


bool RwMaterialDictionary::AlreadyExported(RwTexture* tex, char** pName)
{
	const DWORD numExported = alreadyExported.size();
	if (numExported == 0)
		return false;
	else
	{
		char* name = tex->name;
		///*if (!strcmp("grate", name))
		  //MessageBox(0, name, "INSIDE", 0);*/
		DWORD uAddress = tex->uAddress;
		DWORD vAddress = tex->vAddress;
		for (DWORD i = 0; i < numExported; i++)
		{
			char* p = name;
			char* c = alreadyExported[i].name;
			bool equals = true;
			while (*p != 0x0)
			{
				if (*p != *c)
					equals = false;
				p++;
				c++;
			}
			if (equals && *p == *c)
			{
				/*if (!strcmp("grate", name))
				  MessageBox(0, alreadyExported[i].name, "ALRDYXPO??", 0);*/
				if (uAddress == alreadyExported[i].uAddress && vAddress == alreadyExported[i].vAddress)
					return true;
				else
				{
					*pName = tmp;
					numCopied++;
					strcpy(tmp, tex->name);
					sprintf(tex->name, "copied%u", numCopied);
					return false;
				}
			}
		}
	}
	return false;
}



void RwMaterialDictionary::ExportTDX(FILE* const __restrict f, bool sky)
{
	TGAHEADER* tgaHeader;
	char imageName[MAX_PATH] = "";
	GetEnvironmentVariable("TEMP", imageName, MAX_PATH);
	DWORD pos = strlen(imageName);
	imageName[pos] = '\\';
	pos++;
	FILE* imageFile;
	BYTE* imageData;
	BYTE* pFile;
	DWORD size;
	DWORD numMaterials = materials.size();
	//BYTE** pFile = new BYTE*[numMaterials];
	WriteChunkHeader(f, 0x16, 0);
	WriteChunkHeader(f, 1, 4);
	WriteDWORD(f, numMaterials);
	DWORD imageSize = 0;
	DWORD tdxSize = 0;
	DWORD mats = 0;
	numCopied = 0;

	for (DWORD i = 0; i < numMaterials; i++)
	{
		char* name = materials[i].texture.name;
		/*if (!strcmp("grate", name))
		  MessageBox(0, name, name, 0);*/

		if (!AlreadyExported(&materials[i].texture, &name))
		{
			/*if (!strcmp("grate", name))
			  MessageBox(0, name, "ye", 0);*/
			alreadyExported.push_back(expo(&materials[i].texture, name));
			strcpy(&imageName[pos], name);
			strcat(imageName, ".tga");

			imageFile = fopen(imageName, "rb");
			if (imageFile)
			{
				mats++;
				fseek(imageFile, 0, SEEK_END);

				size = ftell(imageFile);

				pFile = new BYTE[size];

				fseek(imageFile, 0, SEEK_SET);
				fread(pFile, size, 1, imageFile);
				fclose(imageFile);

				tgaHeader = (TGAHEADER*)pFile;

				imageData = (BYTE*)((DWORD)pFile + sizeof(TGAHEADER));
				imageSize = tgaHeader->width * tgaHeader->height * 4;
				tdxSize += imageSize + 320;
				WriteChunkHeader(f, 0x15, imageSize + 308);
				WriteChunkHeader(f, 1, imageSize + 284);
				WriteDWORD(f, 8);
				if (materials[i].texture.uAddress == D3DTADDRESS_CLAMP)
				{
					if (materials[i].texture.vAddress == D3DTADDRESS_CLAMP)
						WriteDWORD(f, 0x00003306);
					else
						WriteDWORD(f, 0x00001306);
				}
				else if (materials[i].texture.vAddress == D3DTADDRESS_CLAMP)
					WriteDWORD(f, 0x00003106);
				else
					WriteDWORD(f, 0x00001106);
				WriteData(f, materials[i].texture.name, 128);
				WriteDWORD(f, 0, 32);
				WriteDWORD(f, 0x00000500);
				WriteDWORD(f, 1);
				WriteWORD(f, (WORD)tgaHeader->width);
				WriteWORD(f, (WORD)tgaHeader->height);
				WriteDWORD(f, 0x00040120);
				if (sky)
				{
					for (DWORD j = 0; j < imageSize / 4; j++)
					{
						if (((Colour*)imageData)[j].a == 0xFF)
							((Colour*)imageData)[j].a = 0xFE;
					}
				}
				WriteDWORD(f, imageSize);
				WriteData(f, imageData, imageSize);
				WriteChunkHeader(f, 3, 0);

				delete[] pFile;
			}
			else
			{
				strcpy(&imageName[pos], name);
				/*if(name!=materials[i].texture.name)
				delete [] name;*/
				strcat(imageName, ".dds");
				imageFile = fopen(imageName, "rb");
				if (imageFile)
				{
					mats++;
					fseek(imageFile, 0, SEEK_END);
					DWORD size = ftell(imageFile);
					BYTE* data = new BYTE[size];
					fseek(imageFile, 0, SEEK_SET);
					fread(data, size, 1, imageFile);
					fclose(imageFile);
					imageData = data;
					DDS_HEADER* header = (DDS_HEADER*)imageData;
					imageData += sizeof(DDS_HEADER);

					WORD width = (WORD)header->width;
					WORD height = (WORD)header->height;

					if (header->pixelFormat[1] & 0x4)
					{
						imageSize = header->width * header->height * 4;
						BYTE* levelData = new BYTE[imageSize];
						DecompressDXT(imageData, header->imageSize, header->width, header->height, levelData, (((header->pixelFormat[2] & 0xFF000000) >> 24) - 0x30));
						/*if((((header->pixelFormat[2] & 0xFF000000)>>24)-0x30)==1)
						{
						bool trans=false;
						for(DWORD i=0; i<imageSize/4; i++)
						{
						if(((Colour*)levelData)[i].a<255)
						{
						trans=true;
						break;
						}
						}
						if(!trans && materials[i].transparent)
						{
						for(DWORD i=0; i<imageSize/4; i++)
						{
						((Colour*)levelData)[i].a = 255-((Colour*)levelData)[i].a;
						}
						}
						}*/
						tdxSize += imageSize + 320;
						WriteChunkHeader(f, 0x15, imageSize + 308);
						WriteChunkHeader(f, 1, imageSize + 284);
						WriteDWORD(f, 8);
						if (materials[i].texture.uAddress == D3DTADDRESS_CLAMP)
						{
							if (materials[i].texture.vAddress == D3DTADDRESS_CLAMP)
								WriteDWORD(f, 0x00003306);
							else
								WriteDWORD(f, 0x00001306);
						}
						else if (materials[i].texture.vAddress == D3DTADDRESS_CLAMP)
							WriteDWORD(f, 0x00003106);
						else
							WriteDWORD(f, 0x00001106);
						WriteData(f, materials[i].texture.name, 128);
						WriteDWORD(f, 0, 32);
						WriteDWORD(f, 0x00000500);
						WriteDWORD(f, 1);
						WriteWORD(f, width);
						WriteWORD(f, height);
						WriteDWORD(f, 0x00040120);
						WriteDWORD(f, imageSize);
						if (sky)
						{
							for (DWORD j = 0; j < imageSize / 4; j++)
							{
								if (((Colour*)levelData)[j].a == 0xFF)
									((Colour*)levelData)[j].a = 0xFE;
							}
						}
						WriteData(f, levelData, imageSize);
						WriteChunkHeader(f, 3, 0);
						delete[] levelData;
					}
					else
					{
						DWORD bytesPerPixel = header->imageSize / header->width / header->height;
						bool usingAlpha = header->pixelFormat[1] & 0x2;

						if (bytesPerPixel == 4)
						{
							//MessageBox(0,"sure this is 32 bit?",imageName,0);
							tdxSize += imageSize + 320;
							WriteChunkHeader(f, 0x15, imageSize + 308);
							WriteChunkHeader(f, 1, imageSize + 284);
							WriteDWORD(f, 8);
							if (materials[i].texture.uAddress == D3DTADDRESS_CLAMP)
							{
								if (materials[i].texture.vAddress == D3DTADDRESS_CLAMP)
									WriteDWORD(f, 0x00003306);
								else
									WriteDWORD(f, 0x00001306);
							}
							else if (materials[i].texture.vAddress == D3DTADDRESS_CLAMP)
								WriteDWORD(f, 0x00003106);
							else
								WriteDWORD(f, 0x00001106);
							WriteData(f, materials[i].texture.name, 128);
							WriteDWORD(f, 0, 32);
							WriteDWORD(f, 0x00000500);
							WriteDWORD(f, 1);
							WriteWORD(f, width);
							WriteWORD(f, height);
							WriteDWORD(f, 0x00040120);
							WriteDWORD(f, imageSize);
							if (sky)
							{
								for (DWORD j = 0; j < imageSize / 4; j++)
								{
									if (((Colour*)imageData)[j].a == 0xFF)
										((Colour*)imageData)[j].a = 0xFE;
								}
							}
							WriteData(f, imageData, imageSize);
							WriteChunkHeader(f, 3, 0);
						}
						else if (bytesPerPixel == 2)
						{
							DWORD numPixels = imageSize / 4;
							Colour* levelData = new Colour[numPixels];
							//MessageBox(0,"sure this is 16 bit?",imageName,0);
							if (usingAlpha)
							{
								for (DWORD j = 0; j < numPixels; j++)
								{
									BYTE r = ((BYTE)((((WORD*)imageData)[j] & 0x003E) >> 1));
									BYTE g = ((BYTE)((((WORD*)imageData)[j] & 0x7C0) >> 6));
									BYTE b = ((BYTE)((((WORD*)imageData)[j] & 0xF800) >> 12));
									BYTE a = ((BYTE)((((WORD*)imageData)[j] & 0x1)));
									levelData[j].r = (r << 3) | (r >> 2);
									levelData[j].g = (g << 3) | (g >> 2);
									levelData[j].b = (b << 3) | (b >> 2);
									if (a && sky)
										levelData[j].a = 254;
									else if (a)
										levelData[j].a = 255;
									else
										levelData[j].a = 0;
								}
							}
							else
							{
								for (DWORD j = 0; j < numPixels; j++)
								{
									BYTE r = (BYTE)(((WORD*)imageData)[j] & 0x001F);
									BYTE g = ((BYTE)((((WORD*)imageData)[j] & 0x7E0) >> 5));
									BYTE b = ((BYTE)((((WORD*)imageData)[j] & 0xF800) >> 11));
									levelData[j].r = (r << 3) | (r >> 2);
									levelData[j].g = (g << 2) | (g >> 4);
									levelData[j].b = (b << 3) | (b >> 2);
									if (sky)
										levelData[j].a = 254;
									else
										levelData[j].a = 255;
								}
							}
							tdxSize += imageSize + 320;
							WriteChunkHeader(f, 0x15, imageSize + 308);
							WriteChunkHeader(f, 1, imageSize + 284);
							WriteDWORD(f, 8);
							if (materials[i].texture.uAddress == D3DTADDRESS_CLAMP)
							{
								if (materials[i].texture.vAddress == D3DTADDRESS_CLAMP)
									WriteDWORD(f, 0x00003306);
								else
									WriteDWORD(f, 0x00001306);
							}
							else if (materials[i].texture.vAddress == D3DTADDRESS_CLAMP)
								WriteDWORD(f, 0x00003106);
							else
								WriteDWORD(f, 0x00001106);
							WriteData(f, materials[i].texture.name, 128);
							WriteDWORD(f, 0, 32);
							WriteDWORD(f, 0x00000500);
							WriteDWORD(f, 1);
							WriteWORD(f, width);
							WriteWORD(f, height);
							WriteDWORD(f, 0x00040120);
							WriteDWORD(f, imageSize);
							/*if (sky)
							{
							  for (DWORD j = 0; j < imageSize/4; j++)
							  {
								//if (levelData[j] == 0xFF)
								((Colour*)levelData)[j].a = 0xFE;
							  }
							}*/
							WriteData(f, levelData, imageSize);
							WriteChunkHeader(f, 3, 0);
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

					delete[] data;
				}
			}
		}
	}
	tdxSize += 28;
	WriteChunkHeader(f, 3, 0);
	fseek(f, 4, SEEK_SET);
	WriteDWORD(f, tdxSize);
	fseek(f, 16, SEEK_CUR);
	WriteDWORD(f, mats);
}

void RwMaterialDictionary::Export(FILE* const __restrict f)
{
	DWORD numMaterials = materials.size();
	WriteChunkHeader(f, 0x8, size);
	WriteChunkHeader(f, 0x1, structSize);
	WriteDWORD(f, numMaterials);
	WriteDWORD(f, 0xFFFFFFFF, numMaterials);

	for (DWORD i = 0; i < numMaterials; i++)
	{
		materials[i].Export(f);
	}
}


#define MAX_FACES_PER_LEAF 4
#define OPTIMAL_FACES_PER_LEAF 2
#define MAX_TREE_LEVELS 7

WORD Th3Mesh::CreateTree(const BBox& bbox, const Face* __restrict const faces, const DWORD* __restrict const faceIndices, const DWORD numFaces, DWORD level)
{
	if ((numFaces <= OPTIMAL_FACES_PER_LEAF) || (level == MAX_TREE_LEVELS))
	{
		WORD index = (WORD)CollNodes.size();

		CollNodes.push_back(TreeNodez(numFaces, faceIndices));
		CollNodes[index].type = 1;
		CollNodes[index].index = numLeafs;
		CollNodes[index].bbox = bbox;
		numLeafs++;
		collPolyCount += numFaces;
		return index;
	}
	else
	{
		D3DXVECTOR3 mid_width((*(D3DXVECTOR3*)&bbox.Max - *(D3DXVECTOR3*)&bbox.Min) * 0.5f);
		D3DXVECTOR3 mid_split(mid_width + *(D3DXVECTOR3*)&bbox.Min);

		DWORD lessX = 0, lessY = 0, lessZ = 0, greaterX = 0, greaterY = 0, greaterZ = 0;

		for (DWORD i = 0; i < numFaces; i++)
		{
			if (vertices[faces[faceIndices[i]].a].x < mid_split.x && vertices[faces[faceIndices[i]].b].x < mid_split.x && vertices[faces[faceIndices[i]].c].x < mid_split.x)
				lessX++;
			else if (vertices[faces[faceIndices[i]].a].x >= mid_split.x && vertices[faces[faceIndices[i]].b].x >= mid_split.x && vertices[faces[faceIndices[i]].c].x >= mid_split.x)
				greaterX++;
			else
			{
				lessX++;
				greaterX++;
			}

			if (vertices[faces[faceIndices[i]].a].y < mid_split.y && vertices[faces[faceIndices[i]].b].y < mid_split.y && vertices[faces[faceIndices[i]].c].y < mid_split.y)
				lessY++;
			else if (vertices[faces[faceIndices[i]].a].y >= mid_split.y && vertices[faces[faceIndices[i]].b].y >= mid_split.y && vertices[faces[faceIndices[i]].c].y >= mid_split.y)
				greaterY++;
			else
			{
				lessY++;
				greaterY++;
			}

			if (vertices[faces[faceIndices[i]].a].z < mid_split.z && vertices[faces[faceIndices[i]].b].z < mid_split.z && vertices[faces[faceIndices[i]].c].z < mid_split.z)
				lessZ++;
			else if (vertices[faces[faceIndices[i]].a].z >= mid_split.z && vertices[faces[faceIndices[i]].b].z >= mid_split.z && vertices[faces[faceIndices[i]].c].z >= mid_split.z)
				greaterZ++;
			else
			{
				lessZ++;
				greaterZ++;
			}
		}

		int best_axis = 0;
		float best_diff;
		const int duplicate_treshold = (numFaces * 7) / 10;
		int duplicates = lessX + greaterX - numFaces;
		if (duplicates >= duplicate_treshold)
			best_axis = -1;
		else
			best_diff = (float)fabsf((float)(lessX - greaterX));


		float new_diff = (float)fabsf((float)(lessY - greaterY));
		duplicates = lessY + greaterY - numFaces;
		if ((new_diff < best_diff || best_axis == -1) && duplicates < duplicate_treshold)
		{
			best_diff = new_diff;
			best_axis = 1;
		}

		new_diff = (float)fabsf((float)(lessZ - greaterZ));
		duplicates = lessZ + greaterZ - numFaces;
		if ((new_diff < best_diff || best_axis == -1) && duplicates < duplicate_treshold)
		{
			best_diff = new_diff;
			best_axis = 2;
		}

		if (best_axis == -1)
		{
			WORD index = (WORD)CollNodes.size();

			CollNodes.push_back(TreeNodez(numFaces, faceIndices));
			CollNodes[index].type = 1;
			CollNodes[index].index = numLeafs;
			CollNodes[index].bbox = bbox;
			numLeafs++;
			collPolyCount += numFaces;
			return index;
		}
		else if (best_axis == 0)
		{
			DWORD* const __restrict leftFaces = new DWORD[lessX];
			DWORD* const __restrict rightFaces = new DWORD[greaterX];

			lessX = 0;
			greaterX = 0;

			for (DWORD i = 0; i < numFaces; i++)
			{
				if (vertices[faces[faceIndices[i]].a].x < mid_split.x && vertices[faces[faceIndices[i]].b].x < mid_split.x && vertices[faces[faceIndices[i]].c].x < mid_split.x)
				{
					leftFaces[lessX] = faceIndices[i];
					lessX++;
				}
				else if (vertices[faces[faceIndices[i]].a].x >= mid_split.x && vertices[faces[faceIndices[i]].b].x >= mid_split.x && vertices[faces[faceIndices[i]].c].x >= mid_split.x)
				{
					rightFaces[greaterX] = faceIndices[i];
					greaterX++;
				}
				else
				{
					leftFaces[lessX] = faceIndices[i];
					lessX++;
					rightFaces[greaterX] = faceIndices[i];
					greaterX++;
				}
			}

			BBox less_bbox = bbox;
			BBox greater_bbox = bbox;
			less_bbox.Max.x = mid_split.x;
			greater_bbox.Min.x = mid_split.x;

			WORD index = (WORD)CollNodes.size();
			//CollNodes.push_back(TreeNode(0, mid_split.x));
			CollNodes.push_back(TreeNodez());
			CollNodes[index].index = numBranches;
			numBranches++;
			//TreeNode* node = &CollNodes.back();
			WORD left = CreateTree(less_bbox, faces, leftFaces, lessX, level + 1);
			CollNodes[index].left = left;
			WORD right = CreateTree(greater_bbox, faces, rightFaces, greaterX, level + 1);
			CollNodes[index].right = right;
			CollNodes[index].type = 2;
			CollNodes[index].axis = 0;
			//CollNodes[index].split = mid_split.x;
			CollNodes[index].bbox = bbox;

			delete[] leftFaces;
			delete[] rightFaces;

			return index;
		}
		else if (best_axis == 1)
		{
			DWORD* const __restrict leftFaces = new DWORD[lessY];
			DWORD* const __restrict rightFaces = new DWORD[greaterY];

			lessY = 0;
			greaterY = 0;

			for (DWORD i = 0; i < numFaces; i++)
			{
				if (vertices[faces[faceIndices[i]].a].y < mid_split.y && vertices[faces[faceIndices[i]].b].y < mid_split.y && vertices[faces[faceIndices[i]].c].y < mid_split.y)
				{
					leftFaces[lessY] = faceIndices[i];
					lessY++;
				}
				else if (vertices[faces[faceIndices[i]].a].y >= mid_split.y && vertices[faces[faceIndices[i]].b].y >= mid_split.y && vertices[faces[faceIndices[i]].c].y >= mid_split.y)
				{
					rightFaces[greaterY] = faceIndices[i];
					greaterY++;
				}
				else
				{
					leftFaces[lessY] = faceIndices[i];
					lessY++;
					rightFaces[greaterY] = faceIndices[i];
					greaterY++;
				}
			}

			BBox less_bbox = bbox;
			BBox greater_bbox = bbox;
			less_bbox.Max.y = mid_split.y;
			greater_bbox.Min.y = mid_split.y;

			WORD index = (WORD)CollNodes.size();
			//CollNodes.push_back(TreeNode(4, mid_split.y));
			CollNodes.push_back(TreeNodez());
			CollNodes[index].index = numBranches;
			numBranches++;
			//TreeNode* node = &CollNodes.back();
			WORD left = CreateTree(less_bbox, faces, leftFaces, lessY, level + 1);
			CollNodes[index].left = left;
			WORD right = CreateTree(greater_bbox, faces, rightFaces, greaterY, level + 1);
			CollNodes[index].right = right;
			CollNodes[index].type = 2;
			CollNodes[index].axis = 4;
			//CollNodes[index].split = mid_split.y;
			CollNodes[index].bbox = bbox;

			delete[] leftFaces;
			delete[] rightFaces;

			return index;
		}
		else
		{
			DWORD* const __restrict leftFaces = new DWORD[lessZ];
			DWORD* const __restrict rightFaces = new DWORD[greaterZ];

			lessZ = 0;
			greaterZ = 0;

			for (DWORD i = 0; i < numFaces; i++)
			{
				if (vertices[faces[faceIndices[i]].a].z < mid_split.z && vertices[faces[faceIndices[i]].b].z < mid_split.z && vertices[faces[faceIndices[i]].c].z < mid_split.z)
				{
					leftFaces[lessZ] = faceIndices[i];
					lessZ++;
				}
				else if (vertices[faces[faceIndices[i]].a].z >= mid_split.z && vertices[faces[faceIndices[i]].b].z >= mid_split.z && vertices[faces[faceIndices[i]].c].z >= mid_split.z)
				{
					rightFaces[greaterZ] = faceIndices[i];
					greaterZ++;
				}
				else
				{
					leftFaces[lessZ] = faceIndices[i];
					lessZ++;
					rightFaces[greaterZ] = faceIndices[i];
					greaterZ++;
				}
			}

			BBox less_bbox = bbox;
			BBox greater_bbox = bbox;
			less_bbox.Max.z = mid_split.z;
			greater_bbox.Min.z = mid_split.z;

			WORD index = (WORD)CollNodes.size();
			//CollNodes.push_back(TreeNode(8, mid_split.z));
			CollNodes.push_back(TreeNodez());
			CollNodes[index].index = numBranches;
			numBranches++;
			//TreeNode* node = &CollNodes.back();
			WORD left = CreateTree(less_bbox, faces, leftFaces, lessZ, level + 1);
			CollNodes[index].left = left;
			WORD right = CreateTree(greater_bbox, faces, rightFaces, greaterZ, level + 1);
			CollNodes[index].right = right;
			CollNodes[index].type = 2;
			CollNodes[index].axis = 8;
			//CollNodes[index].split = mid_split.z;
			CollNodes[index].bbox = bbox;

			delete[] leftFaces;
			delete[] rightFaces;

			return index;
		}
	}
}

WORD Th3Mesh::CreateTree(const BBox& bbox, const Face* __restrict const faces, const DWORD numFaces, DWORD level)
{
	if ((numFaces <= MAX_FACES_PER_LEAF) || (level == MAX_TREE_LEVELS))
	{
		numLeafs = 1;
		WORD index = (WORD)CollNodes.size();
		CollNodes.push_back(TreeNodez(numFaces));
		CollNodes[index].type = 1;
		collPolyCount = numFaces;
		return index;
	}
	else
	{
		D3DXVECTOR3 mid_width((*(D3DXVECTOR3*)&bbox.Max - *(D3DXVECTOR3*)&bbox.Min) * 0.5f);
		D3DXVECTOR3 mid_split(mid_width + *(D3DXVECTOR3*)&bbox.Min);

		DWORD lessX = 0, lessY = 0, lessZ = 0, greaterX = 0, greaterY = 0, greaterZ = 0;

		for (DWORD i = 0; i < numFaces; i++)
		{
			if (vertices[faces[i].a].x < mid_split.x && vertices[faces[i].b].x < mid_split.x && vertices[faces[i].c].x < mid_split.x)
				lessX++;
			else if (vertices[faces[i].a].x >= mid_split.x && vertices[faces[i].b].x >= mid_split.x && vertices[faces[i].c].x >= mid_split.x)
				greaterX++;
			else
			{
				lessX++;
				greaterX++;
			}

			if (vertices[faces[i].a].y < mid_split.y && vertices[faces[i].b].y < mid_split.y && vertices[faces[i].c].y < mid_split.y)
				lessY++;
			else if (vertices[faces[i].a].y >= mid_split.y && vertices[faces[i].b].y >= mid_split.y && vertices[faces[i].c].y >= mid_split.y)
				greaterY++;
			else
			{
				lessY++;
				greaterY++;
			}

			if (vertices[faces[i].a].z < mid_split.z && vertices[faces[i].b].z < mid_split.z && vertices[faces[i].c].z < mid_split.z)
				lessZ++;
			else if (vertices[faces[i].a].z >= mid_split.z && vertices[faces[i].b].z >= mid_split.z && vertices[faces[i].c].z >= mid_split.z)
				greaterZ++;
			else
			{
				lessZ++;
				greaterZ++;
			}
		}

		int best_axis = 0;
		float best_diff;
		const int duplicate_treshold = (numFaces * 7) / 10;
		int duplicates = lessX + greaterX - numFaces;
		if (duplicates >= duplicate_treshold)
			best_axis = -1;
		else
			best_diff = (float)fabsf((float)(lessX - greaterX));


		float new_diff = (float)fabsf((float)(lessY - greaterY));
		duplicates = lessY + greaterY - numFaces;
		if ((new_diff < best_diff || best_axis == -1) && duplicates < duplicate_treshold)
		{
			best_diff = new_diff;
			best_axis = 1;
		}

		new_diff = (float)fabsf((float)(lessZ - greaterZ));
		duplicates = lessZ + greaterZ - numFaces;
		if ((new_diff < best_diff || best_axis == -1) && duplicates < duplicate_treshold)
		{
			best_diff = new_diff;
			best_axis = 2;
		}

		if (best_axis == -1)
		{
			numLeafs = 1;
			WORD index = (WORD)CollNodes.size();
			CollNodes.push_back(TreeNodez(numFaces));
			CollNodes[index].type = 1;
			collPolyCount = numFaces;
			return index;
		}
		else if (best_axis == 0)
		{
			DWORD* const __restrict leftFaces = new DWORD[lessX];
			DWORD* const __restrict rightFaces = new DWORD[greaterX];

			lessX = 0;
			greaterX = 0;

			for (DWORD i = 0; i < numFaces; i++)
			{
				if (vertices[faces[i].a].x < mid_split.x && vertices[faces[i].b].x < mid_split.x && vertices[faces[i].c].x < mid_split.x)
				{
					leftFaces[lessX] = i;
					lessX++;
				}
				else if (vertices[faces[i].a].x >= mid_split.x && vertices[faces[i].b].x >= mid_split.x && vertices[faces[i].c].x >= mid_split.x)
				{
					rightFaces[greaterX] = i;
					greaterX++;
				}
				else
				{
					leftFaces[lessX] = i;
					lessX++;
					rightFaces[greaterX] = i;
					greaterX++;
				}
			}

			BBox less_bbox = bbox;
			BBox greater_bbox = bbox;
			less_bbox.Max.x = mid_split.x;
			greater_bbox.Min.x = mid_split.x;

			WORD index = (WORD)CollNodes.size();
			//CollNodes.push_back(TreeNode(0, mid_split.x));
			CollNodes.push_back(TreeNodez());
			CollNodes[index].index = numBranches;
			numBranches++;
			//TreeNode* node = &CollNodes.back();
			WORD left = CreateTree(less_bbox, faces, leftFaces, lessX, level + 1);
			CollNodes[index].left = left;
			WORD right = CreateTree(greater_bbox, faces, rightFaces, greaterX, level + 1);
			CollNodes[index].right = right;
			CollNodes[index].type = 2;
			CollNodes[index].axis = 0;
			//CollNodes[index].split = mid_split.x;
			CollNodes[index].bbox = bbox;

			delete[] leftFaces;
			delete[] rightFaces;

			return index;
		}
		else if (best_axis == 1)
		{
			DWORD* const __restrict leftFaces = new DWORD[lessY];
			DWORD* const __restrict rightFaces = new DWORD[greaterY];

			lessY = 0;
			greaterY = 0;

			for (DWORD i = 0; i < numFaces; i++)
			{
				if (vertices[faces[i].a].y < mid_split.y && vertices[faces[i].b].y < mid_split.y && vertices[faces[i].c].y < mid_split.y)
				{
					leftFaces[lessY] = i;
					lessY++;
				}
				else if (vertices[faces[i].a].y >= mid_split.y && vertices[faces[i].b].y >= mid_split.y && vertices[faces[i].c].y >= mid_split.y)
				{
					rightFaces[greaterY] = i;
					greaterY++;
				}
				else
				{
					leftFaces[lessY] = i;
					lessY++;
					rightFaces[greaterY] = i;
					greaterY++;
				}
			}

			BBox less_bbox = bbox;
			BBox greater_bbox = bbox;
			less_bbox.Max.y = mid_split.y;
			greater_bbox.Min.y = mid_split.y;

			WORD index = (WORD)CollNodes.size();
			//CollNodes.push_back(TreeNode(4, mid_split.y));
			CollNodes.push_back(TreeNodez());
			CollNodes[index].index = numBranches;
			numBranches++;
			//TreeNode* node = &CollNodes.back();
			WORD left = CreateTree(less_bbox, faces, leftFaces, lessY, level + 1);
			CollNodes[index].left = left;
			WORD right = CreateTree(greater_bbox, faces, rightFaces, greaterY, level + 1);
			CollNodes[index].right = right;
			CollNodes[index].type = 2;
			CollNodes[index].axis = 4;
			//CollNodes[index].split = mid_split.y;
			CollNodes[index].bbox = bbox;

			delete[] leftFaces;
			delete[] rightFaces;

			return index;
		}
		else
		{
			DWORD* leftFaces = new DWORD[lessZ];
			DWORD* rightFaces = new DWORD[greaterZ];

			lessZ = 0;
			greaterZ = 0;

			for (DWORD i = 0; i < numFaces; i++)
			{
				if (vertices[faces[i].a].z < mid_split.z && vertices[faces[i].b].z < mid_split.z && vertices[faces[i].c].z < mid_split.z)
				{
					leftFaces[lessZ] = i;
					lessZ++;
				}
				else if (vertices[faces[i].a].z >= mid_split.z && vertices[faces[i].b].z >= mid_split.z && vertices[faces[i].c].z >= mid_split.z)
				{
					rightFaces[greaterZ] = i;
					greaterZ++;
				}
				else
				{
					leftFaces[lessZ] = i;
					lessZ++;
					rightFaces[greaterZ] = i;
					greaterZ++;
				}
			}

			BBox less_bbox = bbox;
			BBox greater_bbox = bbox;
			less_bbox.Max.z = mid_split.z;
			greater_bbox.Min.z = mid_split.z;

			WORD index = (WORD)CollNodes.size();
			//CollNodes.push_back(TreeNode(8, mid_split.z));
			CollNodes.push_back(TreeNodez());
			CollNodes[index].index = numBranches;
			numBranches++;
			//TreeNode* node = &CollNodes.back();
			WORD left = CreateTree(less_bbox, faces, leftFaces, lessZ, level + 1);
			CollNodes[index].left = left;
			WORD right = CreateTree(greater_bbox, faces, rightFaces, greaterZ, level + 1);
			CollNodes[index].right = right;
			CollNodes[index].type = 2;
			CollNodes[index].axis = 8;
			//CollNodes[index].split = mid_split.z;
			CollNodes[index].bbox = bbox;

			delete[] leftFaces;
			delete[] rightFaces;

			return index;
		}
	}
}


void Th3Mesh::Export(FILE* const __restrict f)
{

	if (secondAtomic)
		planeInfo.Right = 1;
	WriteChunkHeader(f, 0x0A, planeSize);
	WriteData(f, &planeInfo, sizeof(planeInfo));
	WriteChunkHeader(f, 0x9, size);
	WriteChunkHeader(f, 0x1, structSize);
	WriteDWORD(f, 0);
	WriteDWORD(f, Polygons.size());
	WriteDWORD(f, GetNumVertices());
	WriteData(f, &bbox, sizeof(BBox));
	WriteDWORD(f, 0, 2);
	DWORD numVertices = GetNumVertices();
	if (numVertices > 0xFFFE)
		MessageBox(0, 0, 0, 0);
	for (DWORD i = 0; i < numVertices; i++)
	{
		WriteData(f, &vertices[i].x, 12);
	}
	for (DWORD i = 0; i < numVertices; i++)
	{
		WriteDWORD(f, vertices[i].colour.GetBGR());
	}
	for (DWORD i = 0; i < numVertices; i++)
	{
		WriteData(f, &vertices[i].tUV[0].u, 8);
	}
	for (DWORD i = 0, numPolies = Polygons.size(); i < numPolies; i++)
	{
		WriteWORD(f, Polygons[i].sound);
		WriteData(f, &Polygons[i].a, 6);
	}
	WriteChunkHeader(f, 0x3, extensionSize);
	WriteChunkHeader(f, 0x50E, splitSize);
	WriteDWORD(f, 1);
	WriteDWORD(f, numSplits);
	WriteDWORD(f, numIndices);
	for (DWORD i = 0; i < numSplits; i++)
	{
		WriteDWORD(f, matSplits[i].numIndices);
		WriteDWORD(f, matSplits[i].matId);
		for (DWORD j = 0; j < matSplits[i].numIndices; j++)
		{
			WriteDWORD(f, (DWORD)matSplits[i].Indices[j]);
		}
	}
	if (collPolyCount != 0)
	{
		WriteChunkHeader(f, 0x11D, collisionSize);
		WriteDWORD(f, (DWORD)numLeafs);
		fwrite(&collPolyCount, 4, 1, f);
		DWORD numNodes = CollNodes.size();
		if (numNodes > 0xFFFE)
			MessageBox(0, 0, 0, 0);

		if (numNodes)
		{
			WORD totalFaces = 0;
			for (DWORD i = 0; i < numNodes; i++)
			{
				if (CollNodes[i].type == 2)//branch
				{
					TreeNodez* left = &CollNodes[CollNodes[i].right];
					TreeNodez* right = &CollNodes[CollNodes[i].left];

					if (left <= &CollNodes[i] || right <= &CollNodes[i])
						MessageBox(0, 0, 0, 0);

					fwrite(&left->type, 1, 1, f);
					fwrite(&right->type, 1, 1, f);

					fwrite(&CollNodes[i].axis, 2, 1, f);

					fwrite(&left->index, 2, 1, f);
					fwrite(&right->index, 2, 1, f);

					fwrite((BYTE*)(&left->bbox.Max.x) + CollNodes[i].axis, 4, 1, f);//hack to write float x if axis = 0 y if axis = 4 z if axis = 8
					fwrite((BYTE*)(&right->bbox.Min.x) + CollNodes[i].axis, 4, 1, f);
				}
			}
			for (DWORD i = 0; i < numNodes; i++)
			{
				if (CollNodes[i].type == 1)//leaf
				{
					WORD numFacess = (WORD)CollNodes[i].faces.size();
					fwrite(&totalFaces, 2, 1, f);
					fwrite(&numFacess, 2, 1, f);
					totalFaces += numFacess;

				}
			}

			for (DWORD i = 0; i < numNodes; i++)
			{
				if (CollNodes[i].type == 1)//leaf
				{
					WORD numFacess = (WORD)CollNodes[i].faces.size();
					for (WORD j = 0; j < numFacess; j++)
					{
						fwrite(&CollNodes[i].faces[j], 4, 1, f);
					}

				}
			}
		}
	}
	WriteChunkHeader(f, 0x120, effectSize);
	WriteDWORD(f, 0);
	WriteChunkHeader(f, 0x0294AF01, th3ExtensionSize);
	WriteDWORD(f, 6);
	for (WORD i = 0, numPolies = Polygons.size(); i < numPolies; i++)
	{
		WriteWORD(f, Polygons[i].flags);
	}
	WriteDWORD(f, 0);
	WriteDWORD(f, name.checksum);
	WriteBYTE(f, 0, 3);
}

void Th3Mesh::GeneratePolygons(const MaterialSplit* const __restrict splits, Collision* const __restrict collision, DWORD* vertexIndices, bool renderShadow)
{
	if (generateFromMesh)
	{
		WORD numFaces = (WORD)collision->GetNumFaces();
		if (numFaces > 1024)
			renderShadow = false;
		for (WORD i = 0; i < numFaces; i++)
		{
			FaceInfo faceInfo = FaceInfo();

			memcpy(&faceInfo, collision->GetFaceInfo(i), sizeof(faceInfo));

			faceInfo.flags &= ~0xF800;

			if (!(faceInfo.flags & 0x0200))
			{
				faceInfo.flags |= 0x0500;
			}
			else if (faceInfo.flags & 0x0004)
			{
				faceInfo.flags &= ~0x0200;
				faceInfo.flags |= 0x0500;
			}
			if (faceInfo.flags & 0x0001)//skateable th4/thug
			{
				faceInfo.flags &= ~0x0001;//unset skateable th4/thug
				faceInfo.flags |= 0x0080;//set skateable th3
			}
			else if (faceInfo.flags & 0x0006 || !(faceInfo.flags & 0x0090))
				faceInfo.flags |= 0x0080;
			if (faceInfo.flags & 0x0008 && !(faceInfo.flags & 0x0010))//ramp non collideable not set
			{
				faceInfo.flags |= 0x00080;//set skateable th3
			}
			if (!renderShadow)
			{
				faceInfo.flags |= 0x0500;
				faceInfo.flags &= ~0x0200;
			}
			Polygons.push_back(Face(collision->GetFace(i), &faceInfo));
		}
	}
	else
	{

		WORD numFaces = (WORD)collision->GetNumFaces();
		if (numFaces > 1024)
			renderShadow = false;


		for (WORD i = 0; i < numFaces; i++)
		{
			FaceInfo faceInfo = FaceInfo();

			memcpy(&faceInfo, collision->GetFaceInfo(i), sizeof(faceInfo));

			faceInfo.flags &= ~0xF800;

			if (!(faceInfo.flags & 0x0200))
			{
				faceInfo.flags |= 0x0500;
			}
			else if (faceInfo.flags & 0x0004)
			{
				faceInfo.flags &= ~0x0200;
				faceInfo.flags |= 0x0500;
			}
			if (faceInfo.flags & 0x0001)//skateable th4/thug
			{
				faceInfo.flags &= ~0x0001;//unset skateable th4/thug
				if (!(faceInfo.flags & 0x0010))
					faceInfo.flags |= 0x0080;//set skateable th3
			}
			else if (faceInfo.flags & 0x0006 || !(faceInfo.flags & 0x0090))
				faceInfo.flags |= 0x0080;
			if (faceInfo.flags & 0x0008 && !(faceInfo.flags & 0x0010))//ramp non collideable not set
			{
				faceInfo.flags |= 0x00080;//set skateable th3
			}
			if (!renderShadow)
			{
				faceInfo.flags |= 0x0500;
				faceInfo.flags &= ~0x0200;
			}
			Polygons.push_back(Face(collision->GetFace(i), &faceInfo, vertexIndices));
		}
	}

	for (DWORD i = 0; i < numSplits; i++)
	{
		numIndices += splits[i].numIndices;
	}

}

Th3Mesh::Th3Mesh(Mesh* const __restrict mesh, const BYTE blendValue, const bool renderShadow)
{
	//ZeroMemory(this,sizeof(Mesh));
	secondAtomic = false;
	size = 0;
	structSize = 0;
	extensionSize = 0;
	effectSize = 0;
	splitSize = 0;
	collisionSize = 0;
	meshVertexCount = 0;
	//meshPolyCount=0;
	collPolyCount = 0;
	th3ExtensionSize = 0;
	numIndices = 0;
	numSplits = 0;
	generateFromMesh = false;
	numLeafs = 0;
	numBranches = 0;

	name = mesh->GetName();
	//bbox = mesh->GetBBox();
	/*bbox.Min.z*=-1;
	bbox.Max.z*=-1;*/
	extern MaterialSplit* lastSplit;
	extern DWORD numLastSplit;
	if (matSplits.size())
	{
		lastSplit = &mesh->matSplits.front();
		numLastSplit = mesh->matSplits.size();
	}
	else
		lastSplit = NULL;
	AddVertices(mesh->GetNumVertices() ? mesh->GetVertices() : NULL, mesh->GetNumVertices(), false, true, blendValue);
	meshVertexCount = (WORD)vertices.size();
	Collision* tmpCollision = mesh->GetCollision();
	//collision = tmpCollision;
	vector<DWORD> vertexIndices;
	if (tmpCollision)
	{

		if (tmpCollision->GetNumVertices())
		{
			//MessageBox(0,"wtf","wtf",0);
			/*AddVertices(tmpCollision->GetVertices(), tmpCollision->GetNumVertices(), true, true);
			bbox.refitBBox(tmpCollision->GetBBox());*/


			WORD numCollVerts = (WORD)tmpCollision->GetNumVertices();
			const Vertex* verts = tmpCollision->GetVertices();
			vertexIndices.resize(numCollVerts);

			if (numCollVerts == meshVertexCount && tmpCollision->SameAs(&vertices.front(), vertices.size()))
			{
				for (DWORD i = 0; i < numCollVerts; i++)
				{
					vertexIndices[i] = i;
				}
			}
			else
			{
				for (WORD i = 0; i < numCollVerts; i++)
				{
					vertexIndices[i] = i + meshVertexCount;
				}

				for (WORD i = 0; i < numCollVerts; i++)
				{
					bool found = false;
					for (WORD j = 0; j < meshVertexCount; j++)
					{
						if (verts[i].x == vertices[j].x && verts[i].y == vertices[j].y && verts[i].z == vertices[j].z)
						{
							vertexIndices[i] = j;
							found = true;
							break;
						}
					}
					if (!found)
					{
						vertexIndices[i] = vertices.size();
						Vertex v = verts[i];
						v.z *= -1;
						if (v.x > bbox.Max.x)
							bbox.Max.x = v.x;
						else if (v.x < bbox.Min.x)
							bbox.Min.x = v.x;
						if (v.y > bbox.Max.y)
							bbox.Max.y = v.y;
						else if (v.y < bbox.Min.y)
							bbox.Min.y = v.y;
						if (v.z > bbox.Max.z)
							bbox.Max.z = v.z;
						else if (v.z < bbox.Min.z)
							bbox.Min.z = v.z;
						vertices.push_back(v);
					}
				}
			}
		}
		else/* if(tmpCollision->GetNumFaces() != 0)*/
		{
			generateFromMesh = true;
		}
	}

	if (renderShadow)
	{
		for (DWORD i = 0; i < mesh->GetNumSplits(); i++)
		{
			AddMatSplit(mesh->GetSplit(i), globalMaterialList, true, true);// , true);
			//matSplits[matSplits.size()-1].type = 0;
			matSplits.back().type = 0;
			numSplits++;
		}
	}
	else
	{
		/*MaterialSplit* matSplit = new MaterialSplit();
		for(DWORD i=0; i<tmpCollision->GetNumFaces(); i++)
		{
		SimpleFace* tmpFace = tmpCollision->GetFace(i);
		if(i==0)
		{
		matSplit->matId = globalMaterialList->GetNumMaterials();
		}
		else
		{
		matSplit->Indices.push_back(tmpFace->a);

		}
		matSplit->Indices.push_back(tmpFace->a);
		matSplit->Indices.push_back(tmpFace->b);
		matSplit->Indices.push_back(tmpFace->c);
		if(i!=tmpCollision->GetNumFaces()-1)
		{
		matSplit->Indices.push_back(tmpFace->c);
		matSplit->Indices.push_back(0);
		matSplit->Indices.push_back(0);
		matSplit->Indices.push_back(0);
		}
		matSplit->numIndices = matSplit->Indices.size();
		}
		AddMatSplit(matSplit, globalMaterialList, true, true);
		numSplits++;
		delete  matSplit;*/
	}
	if (tmpCollision)
	{
		GeneratePolygons(GetNumSplits() ? GetMatSplits() : NULL, tmpCollision, &vertexIndices.front(), renderShadow);
		vertexIndices.clear();
		const DWORD numFaces = Polygons.size();
		if (numFaces)
		{
			for (DWORD i = (DWORD)meshVertexCount; i < vertices.size(); i++)
			{
				bool found = false;
				for (DWORD j = 0; j < numFaces; j++)
				{
					if (Polygons[j].a == i || Polygons[j].b == i || Polygons[j].c == i)
					{
						found = true;
						break;
					}
				}

				if (!found)
				{
					for (DWORD j = 0; j < numFaces; j++)
					{
						if (Polygons[j].a > i)
							Polygons[j].a--;
						if (Polygons[j].b > i)
							Polygons[j].b--;
						if (Polygons[j].c > i)
							Polygons[j].c--;
					}
					vertices.erase(vertices.begin() + i);
					i--;
				}
			}
			CollNodes.reserve(numFaces / 2);
			if (generateFromMesh)
				CreateTree(this->GetBBox(), &Polygons.front(), numFaces);
			else
				CreateTree(tmpCollision->GetBBox(), &Polygons.front(), numFaces);
		}
	}
	else
	{
		for (DWORD i = 0; i < numSplits; i++)
		{
			numIndices += matSplits[i].numIndices;
		}
	}
	/*else
	{
	Polygons.push_back(0,1,2);
	}*/
}



void Th3Scene::EraseMaterial(const DWORD index)
{
	for (DWORD i = 0, numMeshes = meshes.size(); i < numMeshes; i++)
	{
		for (DWORD j = 0, numSplits = meshes[i].matSplits.size(); j < numSplits; j++)
		{
			if (meshes[i].matSplits[j].matId > index)
				meshes[i].matSplits[j].matId--;
		}
	}
	RWmatList.EraseMaterial(index);
}

void Th3Scene::EraseMaterial(const DWORD index, DWORD newIndex)
{
	if (newIndex > index)
		newIndex--;
	for (DWORD i = 0, numMeshes = meshes.size(); i < numMeshes; i++)
	{
		for (DWORD j = 0, numSplits = meshes[i].matSplits.size(); j < numSplits; j++)
		{
			if (meshes[i].matSplits[j].matId > index)
				meshes[i].matSplits[j].matId--;
			else if (meshes[i].matSplits[j].matId == index)
				meshes[i].matSplits[j].matId = newIndex;
		}
	}
	RWmatList.EraseMaterial(index);
}

bool Th3Scene::UnusedMaterial(const DWORD index)
{
	for (DWORD i = 0, numMeshes = meshes.size(); i < numMeshes; i++)
	{
		for (DWORD j = 0, numSplits = meshes[i].matSplits.size(); j < numSplits; j++)
		{
			if (meshes[i].matSplits[j].matId == index)
				return false;
		}
	}
	return true;
}

void Th3Scene::OptimizeMaterials()
{
	for (DWORD i = 0; i < RWmatList.materials.size(); i++)
	{
		for (DWORD j = i + 1; j < RWmatList.materials.size(); j++)
		{
			RwMaterial* tmpMat = &RWmatList.materials[i];
			RwMaterial* other = &RWmatList.materials[j];

			if (tmpMat->transparent == other->transparent && tmpMat->doubled == other->doubled && tmpMat->drawOrder == other->drawOrder && tmpMat->ignoreAlpha == other->ignoreAlpha && tmpMat->alphaMap == other->alphaMap && tmpMat->reverse == other->reverse)
			{
				if (tmpMat->texture.GetTexId() == other->texture.GetTexId() && tmpMat->texture.uAddress == other->texture.uAddress && tmpMat->texture.vAddress == other->texture.vAddress)
				{
					EraseMaterial((DWORD)i, j);
					i--;
					j--;
					break;
				}
			}
			else if (UnusedMaterial(i))
			{
				EraseMaterial((DWORD)i);
				i--;
				j--;
				break;
			}
		}
	}
}

struct Th3Face
{
	WORD sound;
	Face face;
};



DWORD Th3Scene::ProcessMesh(DWORD ptr, DWORD end)
{
	ptr += 16;//skip

	mesh.numPolygons = *(DWORD*)ptr;
	ptr += 4;
	mesh.numVerts = *(DWORD*)ptr;
	ptr += 4;

	mesh.bbox = *(BBox*)ptr;
	ptr += sizeof(BBox)+8;

	SimpleVertex* vertices = (SimpleVertex*)ptr;
	ptr += mesh.numVerts * 12;

	Colour* RGB = (Colour*)ptr;
	ptr += 4 * mesh.numVerts;

	TexCoord* cord = (TexCoord*)ptr;
	ptr += 12 * mesh.numVerts;

	Th3Face* face = (Th3Face*)ptr;
	ptr += mesh.numPolygons * sizeof(Th3Face);

	for (int i = 0; i < mesh.numVerts; i++)
	{
		mesh.verts[i].x = vertices[i].x;
		mesh.verts[i].y = vertices[i].y;
		mesh.verts[i].z = vertices[i].z;

		mesh.verts[i].colour = RGB[i];
		mesh.verts[i].tUV[0].u = cord[i].u;
		mesh.verts[i].tUV[0].v = cord[i].v;
	}

	for (int i = 0; i < mesh.numPolygons; i++)
	{

	}


	
}

DWORD Th3Scene::ProcessPlaneChunk(DWORD ptr, DWORD end)
{
	RwChunkHeader* parent = (RwChunkHeader*)ptr;
	RwChunkHeader* header = NULL;;
	while (ptr < end)
	{
		ptr += sizeof(RwChunkHeader);

		switch (parent->id)
		{
		case 0x9:
			ptr = ProcessMesh(ptr, ptr + parent->size);
			break;
		default:
			ptr += header->size;
			break;

		}
		RwChunkHeader* parent = (RwChunkHeader*)ptr;
		RwChunkHeader* header = NULL;;
	}
	mesh.clear();
	return ptr;
}

DWORD Th3Scene::ProcessMaterialChunk(DWORD ptr, DWORD end)
{
	Material material;
	DWORD flags = 0;
	RwChunkHeader* header = (RwChunkHeader*)ptr;
	ptr += sizeof(RwChunkHeader);

	while (ptr < end)
	{
		switch (header->id)
		{
		case 0x6:
			ptr += sizeof(RwChunkHeader);
			flags = *(DWORD*)ptr;
			ptr += 4;
			break;
		case 0x2:
			if (header->size && *(DWORD*)ptr != 0)
			{
				if (!material.GetNumTextures())
				{
					Texture texture;
					texture.SetId(Checksum((char*)ptr));
					material.AddTexture(&texture);
				}
				else
					material.alphaMap = true;
			}
			ptr += header->size;
			break;
		default:
			ptr += header->size;
			break;
		case 0x3:
			header = (RwChunkHeader*)ptr;
			ptr += sizeof(RwChunkHeader);
			ptr += header->size;
			header = (RwChunkHeader*)ptr;
			ptr += sizeof(RwChunkHeader);
			DWORD old = ptr;
			if (header->id == 0x3)
			{
				ptr += sizeof(RwChunkHeader);
				ptr += 16;
				flags = *(DWORD*)ptr;
				ptr += 5;
				ptr += 16;
				DWORD blendMode = *(DWORD*)ptr;
			}
			ptr = old + header->size;
			break;
		}
	}
	matList.AddMaterial(&material);
	return ptr;
}

DWORD Th3Scene::ProcessChunkHeader(DWORD ptr)
{
	RwChunkHeader* parent = (RwChunkHeader*)ptr;
	RwChunkHeader* header = NULL;;
	ptr += sizeof(RwChunkHeader);

	switch (parent->id)
	{
	case 0xB:
		RWmatList.numCopied = 0;
		header = (RwChunkHeader*)ptr;
		ptr += sizeof(RwChunkHeader);
		ptr += header->size;
		break;
	case 0x7:
		ptr = ProcessMaterialChunk(ptr, ptr + parent->size);
		RWmatList.numCopied++;
		break;
	default:
		ptr += parent->size;
		break;
	case 0x8:
		header = (RwChunkHeader*)ptr;
		ptr += sizeof(RwChunkHeader);
		DWORD numMaterials = *(DWORD*)ptr;
		ptr += 4;
		matList.Reserve(numMaterials);
		ptr += numMaterials * 4;
		break;
	}
	return ptr;
}

Th3Scene::Th3Scene(char* Path, bool sky)
{
	SetName(Path); blendValue = 2;
	cullMode = D3DCULL_CW;

	BYTE* pFile = NULL;
	DWORD size = 0, ptr = 0;
	char path[256] = "";

	memcpy(path, Path, strlen(Path) + 1);
	DWORD len = strlen(Path);

	path[len - 3] = 'T';
	path[len - 2] = 'D';
	path[len - 1] = 'X';

	FILE* f = fopen(path, "rb");
	if (f)
	{
		fseek(f, 0, SEEK_END);

		size = ftell(f);

		pFile = new BYTE[size];

		fseek(f, 0, SEEK_SET);
		fread(pFile, size, 1, f);
		fclose(f);

		ProcessTDXFile((DWORD)pFile, (DWORD)pFile + size);

		delete[] pFile;
	}

	path[len - 3] = 'B';
	path[len - 2] = 'S';
	path[len - 1] = 'P';

	fopen(path, "rb");
	if (f)
	{
		fseek(f, 0, SEEK_END);

		DWORD size = ftell(f);

		BYTE* pFile = new BYTE[size];

		fseek(f, 0, SEEK_SET);
		fread(pFile, size, 1, f);
		fclose(f);

		DWORD ptr = (DWORD)(pFile);

		while (ptr < (DWORD)pFile + size)
		{
			ptr = ProcessChunkHeader(ptr);
		}
	}
}
/*
Th3Scene::Th3Scene(char* path)
{


	char Path[256] = "";

	FILE* f = fopen(path, "rb");
	if (f)
	{
		fseek(f, 0, SEEK_END);

		DWORD size = ftell(f);

		BYTE* pFile = new BYTE[size];

		fseek(f, 0, SEEK_SET);
		fread(pFile, size, 1, f);
		fclose(f);

		DWORD ptr = (DWORD)(pFile);

		while (ptr < (DWORD)pFile + size)
		{
			ptr = ProcessChunkHeader(ptr);
		}
	}
}
*/
void Th3Scene::Export(FILE* const __restrict f)
{
	//OptimizeMaterials();
	DWORD numMeshes = 0;

	WriteChunkHeader(f, 0x0BUL, size);
	WriteChunkHeader(f, 0x1UL, structSize);
	WriteDWORD(f, 0);
	static const BYTE unknown[24] = { 0x00 ,0x00 ,0x00 ,0x80 ,0x00 ,0x00 ,0x00 ,0x80 ,0x00 ,0x00 ,0x00 ,0x80 ,0x00 ,0x00 ,0x80 ,0x3F ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x80 ,0x3F };
	WriteData(f, (void*)unknown, 24);
	WriteDWORD(f, numPolygons);
	WriteDWORD(f, numVertices);
	numMeshes = meshes.size();
	WriteDWORD(f, numMeshes);
	WriteDWORD(f, numMeshes + 1);
	WriteDWORD(f, 0);
	fwrite(&flags, 4, 1, f);
	RWmatList.Export(f); printf("done exporting matList\n");

	DWORD divider = 1;
	if (numMeshes > 100)
		divider = numMeshes / 100;

	for (DWORD i = 0; i < numMeshes; i++)
	{
		SetProgressbarValue(i / divider);
		printf("Exporting mesh %s ", meshes[i].GetName().GetString());
		/*if (meshes[i].GetName().checksum == 0xE18484C7 || meshes[i].GetName().checksum == 0xC78484E1)
		{
		  if (meshes[i].GetVertex(2).colour.a != 0xFF)
			MessageBox(0, "yaw", "yaw", 0);
		}*/
		for (DWORD j = 0; j < meshes[i].numSplits; j++)
		{
			if (meshes[i].matSplits[j].matId < RWmatList.materials.size())
			{
				RwMaterial* mat = &RWmatList.materials[meshes[i].matSplits[j].matId];
				if (mat->ignoreAlpha)
					for (DWORD k = 0; k < meshes[i].matSplits[j].Indices.size(); k++)
					{
						Vertex& v = meshes[i].GetVertex(meshes[i].matSplits[j].Indices[k]);
						v.colour.a = 0xFF;
					}
			}
		}
		meshes[i].Export(f);
		printf("DONE\n");
	}

	WriteChunkHeader(f, 0x09, 0x87);
	WriteChunkHeader(f, 0x01, 0x2C);
	WriteBYTE(f, 0, 44);
	WriteChunkHeader(f, 0x03, 0x43);
	WriteChunkHeader(f, 0x50E, 0x0C);
	WriteBYTE(f, 0, 12);
	WriteChunkHeader(f, 0x120, 0x04);

	WriteBYTE(f, 0, 4);
	WriteChunkHeader(f, 0x0294AF01, 0x0F);
	WriteBYTE(f, 6);
	WriteBYTE(f, 0, 14);
	WriteChunkHeader(f, 0x3, 0);
}

Th3Scene::Th3Scene()
{
	ZeroMemory(name, MAX_PATH);
	numPolygons = 0;
	numVertices = 0;
	size = 0;
	flags = 0x4000002D;
	structSize = 0x34;
}

void Th3Scene::ProcessTDXParent(DWORD ptr, RwChunkHeader* parent)
{
	const DWORD endPos = parent->size + ptr;

	while (ptr < endPos)
	{
		RwChunkHeader* header = (RwChunkHeader*)ptr;
		ptr += 12;

		switch (parent->id)
		{
		case 0x16:
			switch (header->id)
			{
			case 0x15:
				ProcessTDXParent(ptr, header);
				break;
			default:
				ptr += header->size;
				break;
			case 0x1:
				DWORD numMaterials = *(DWORD*)ptr;
				Scene* scene = this;
				scene->matList.Reserve(numMaterials);
				ptr += header->size;
				break;
			}
			break;
		case 0x15:
			if (header->id == 1)
			{
				ptr += 8;
				char imageName[MAX_PATH];
				GetEnvironmentVariable("TEMP", imageName, MAX_PATH);
				DWORD len = strlen(imageName);
				imageName[len] = '\\';
				len++;
				char* textureName = (char*)ptr;
				const DWORD numChars = strlen(textureName);
				for (DWORD i = len; i < numChars; i++)
				{
					imageName[i] = textureName[i - len];
				}
				imageName[numChars] = '\0';
				FILE* f = fopen(imageName, "w+b");
				ptr += 256;
				fclose(f);
			}
			else
				ptr += header->size;
			break;
		}

		if (parent->id == 0x16)
		{
			if (header->id == 1)
			{
				DWORD numMaterials = *(DWORD*)ptr;
				Scene* scene = this;
				scene->matList.Reserve(numMaterials);
				ptr += header->size;
			}
			else if (header->id == 0x15)
			{
				ProcessTDXParent(ptr, header);
			}
			else
				ptr += header->size;
		}
		else if (parent->id == 0x15)
		{
			if (header->id == 1)
			{
				ptr += 8;
				char imageName[MAX_PATH];
				GetEnvironmentVariable("TEMP", imageName, MAX_PATH);
				DWORD len = strlen(imageName);
				imageName[len] = '\\';
				len++;
				char* textureName = (char*)ptr;
				const DWORD numChars = strlen(textureName);
				for (DWORD i = len; i < numChars; i++)
				{
					imageName[i] = textureName[i - len];
				}
				imageName[numChars] = '\0';
				FILE* f = fopen(imageName, "w+b");
				ptr += 256;
				fclose(f);

			}
			else
				ptr += header->size;
		}
		else
			ptr += header->size;
	}
}

void Th3Scene::ProcessTDXFile(DWORD ptr, DWORD eof)
{
	while (ptr < eof)
	{
		RwChunkHeader* header = (RwChunkHeader*)ptr;
		ptr += 12;
		if (header->id == 0x16)
		{
			ProcessTDXParent(ptr, header);
		}
		else
			ptr += header->size;
	}
	_fcloseall();
}

/*Th3Scene::Th3Scene(char* Path, bool sky)
{
	SetName(Path); blendValue = 2;
	cullMode = D3DCULL_CW;

	BYTE* pFile = NULL;
	DWORD size = 0, ptr = 0;
	char path[256] = "";

	memcpy(path, Path, strlen(Path) + 1);
	DWORD len = strlen(Path);

	path[len - 3] = 'T';
	path[len - 2] = 'D';
	path[len - 1] = 'X';

	FILE* f = fopen(path, "rb");
	if (f)
	{
		fseek(f, 0, SEEK_END);

		size = ftell(f);

		pFile = new BYTE[size];

		fseek(f, 0, SEEK_SET);
		fread(pFile, size, 1, f);
		fclose(f);

		ProcessTDXFile((DWORD)pFile, (DWORD)pFile + size);

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
					tmpMesh->AddCollision(collision[i], header.Version);
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
			sprintf(tmp2, "_Sky\\%s_Sky.scn.xbx", tmp);
			//path[pos] = '\\';
			memcpy(&path[pos], tmp2, strlen(tmp2) + 1);
			f = fopen(path, "rb");
			if (f)
			{
				skyDome = new Th3Scene(path, true);
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
	}
}
*/
Th3Scene::Th3Scene(Scene* const __restrict scene)
{
	ZeroMemory(name, MAX_PATH);
	numPolygons = 0;
	numVertices = 0;
	size = 0;
	flags = 0x4000002D;
	structSize = 0x34;
	RWmatList = RwMaterialDictionary(&scene->matList);
	printf("matList initialized\n");

	/* DWORD opaqueCount = 0;

	for(DWORD i=0; i<scene->GetNumMeshes(); i++)
	{
	Mesh* tmpMesh = scene->GetMesh(i);
	if(!tmpMesh->IsTransparent())
	opaqueCount++;
	}

	DWORD alphaIndex = opaqueCount;
	opaqueCount = 0;

	Mesh** sortedMeshes = (Mesh**)malloc(4*scene->GetNumMeshes());

	for(DWORD i=0; i<scene->GetNumMeshes(); i++)
	{
	Mesh* tmpMesh = scene->GetMesh(i);
	if(tmpMesh->IsTransparent())
	{
	sortedMeshes[alphaIndex] = tmpMesh;
	alphaIndex++;
	}
	else
	{
	sortedMeshes[opaqueCount] = tmpMesh;
	opaqueCount++;
	}
	}*/
	DWORD numMeshes = scene->GetNumMeshes();
	meshes.reserve(numMeshes);

	for (DWORD i = 0; i < numMeshes; i++)
	{
		Mesh* tmpMesh = scene->GetMesh(i);
		//Mesh* tmpMesh = sortedMeshes[i];
		DWORD index = meshes.size();

		if (tmpMesh->GetNumSplits() != 0 && !tmpMesh->IsDeleted())
		{
			meshes.push_back(Th3Mesh(tmpMesh, scene->GetBlendMode()));
			printf("mesh %d ", index);
			numPolygons += meshes[index].Polygons.size();
			numVertices += meshes[index].GetNumVertices();

			meshes[index].SetSize();
			printf("DONE\n");
		}
		else if (!tmpMesh->IsDeleted())
		{
			meshes.push_back(Th3Mesh(tmpMesh, scene->GetBlendMode(), false));
			numPolygons += meshes[index].Polygons.size();
			numVertices += meshes[index].GetNumVertices();

			meshes[index].SetSize();
		}
	}

	//meshes[meshes.size()-1].secondAtomic = true;
	meshes.back().secondAtomic = true;
	OptimizeMaterials();
	SetSize();
	// free(sortedMeshes);
}

DWORD Th3Scene::GetPlaneSize(const DWORD index) const
{
	DWORD size = 0;
	for (DWORD i = index; i < meshes.size(); i++)
	{
		size += meshes[i].size + sizeof(PlaneInfo) + 12;
	}

	size += 147;

	return size;
}

void Th3Scene::SetSize()
{
	RWmatList.SetSize();

	size = RWmatList.size + (12 * 2) + 52;
	for (DWORD i = 0; i < meshes.size(); i++)
	{
		//meshes[i].planeSize = GetPlaneSize(i);
		meshes[i].planeSize = meshes[i].size + sizeof(PlaneInfo) + 12;
		if (i == meshes.size() - 1)
			meshes[i].planeSize += 147;
		size += (meshes[i].size + (2 * 12) + sizeof(PlaneInfo));
	}
	size += 159;
}

static const DWORD RailNode = Checksum("RailNode").checksum;

inline void WriteQBKey(const DWORD& key, FILE* const __restrict f, bool ending = false)
{
	fputc(0x16, f);
	fwrite(&key, 4, 1, f);
	if (ending)
	{
		fputc(0x2, f);
		WriteDWORD(f, 1);
	}
}

inline void WriteQBKey(const DWORD& key, SimpleVertex* const __restrict vertex, FILE* const __restrict f)
{
	WriteQBKey(key, f);
	WriteWORD(f, 0x1E07);
	WriteData(f, vertex, 12);
}

inline void WriteQBKey(const DWORD& key, const Checksum& data, FILE* const __restrict f)
{
	WriteQBKey(key, f);
	WriteBYTE(f, 0x7);
	WriteQBKey(data.checksum, f);
}

inline void WriteQBKey(const char* const __restrict key, FILE* const __restrict f, const bool ending = false)
{
	fputc(0x16, f);
	fwrite(&Checksum(key), 4, 1, f);
	if (ending)
	{
		fputc(0x2, f);
		WriteDWORD(f, 1);
	}
}

inline void WriteQBKey(Checksum & key, FILE* const __restrict f, const bool ending = false)
{
	fputc(0x16, f);
	fwrite(&key, 4, 1, f);
	if (ending)
	{
		fputc(0x2, f);
		WriteDWORD(f, 1);
	}
}

inline void WriteQBKey(const char* const __restrict key, const char* const __restrict string, FILE* const __restrict f)
{
	WriteQBKey(key, f);
	fputc(7, f);
	fputc(0x1B, f);
	WriteDWORD(f, strlen(string) + 1);
	WriteData(f, (void*)string, strlen(string) + 1);

}

inline void WriteQBKey(const char* const __restrict key, SimpleVertex* const __restrict vertex, FILE* const __restrict f)
{
	WriteQBKey(key, f);
	WriteWORD(f, 0x1E07);
	WriteData(f, vertex, 12);
}

inline void WriteQBKey(const char* const __restrict key, DWORD val, FILE* f)
{
	WriteQBKey(key, f);
	WriteBYTE(f, 0x7);
	WriteBYTE(f, 0x17);
	WriteDWORD(f, val);
}

inline void WriteQBKey(const char* const __restrict key, Checksum data, FILE* f)
{
	WriteQBKey(key, f);
	WriteBYTE(f, 0x7);
	WriteQBKey(data.checksum, f);
}

inline void WriteArray(const char* const __restrict key, const DWORD& count, const WORD* const __restrict data, FILE* const __restrict f)
{
	WriteQBKey(key, f);
	fputc(7, f);
	fputc(5, f);

	for (DWORD i = 0; i < count; i++)
	{
		fputc(0x17, f);
		fwrite(&data[i], 2, 1, f);
		WriteBYTE(f, 0, 2);
	}
	fputc(6, f);
}

inline void WriteArray(const char* const __restrict key, const DWORD& count, const Checksum* const __restrict data, FILE* const __restrict f)
{
	WriteQBKey(key, f);
	fputc(7, f);
	fputc(2, f);
	WriteDWORD(f, 1);
	fputc(5, f);

	for (DWORD i = 0; i < count; i++)
	{
		fputc(0x16, f);
		fwrite(&data[i], 4, 1, f);
	}
	fputc(6, f);
}

void Scene::SaveScene(const char* const __restrict path)
{
	const static DWORD version = 1;

	FILE* f = fopen(path, "wb");
	if (f)
	{
		fwrite(&version, 4, 1, f);
		fwrite(&MAX_TEXCOORDS, 4, 1, f);
		extern DWORD MAX_PASSES;
		fwrite(&MAX_PASSES, 4, 1, f);
		fwrite(&blendValue, 4, 1, f);
		DWORD numMeshes = meshes.size();
		fwrite(&numMeshes, 4, 1, f);
		DWORD pNodes = 0;
		if (nodes.size())
			pNodes = (DWORD)&nodes.front();
		for (DWORD i = 0; i < numMeshes; i++)
		{
			Mesh* mesh = &meshes[i];
			if (!mesh->IsDeleted())
			{
				DWORD pMesh = (DWORD)mesh + 16;
				fwrite((void*)pMesh, 32, 1, f);
				fwrite(&mesh->center, 12, 1, f);
				/*int nodeIndex = ((DWORD)mesh->node/pNodes)-1;
				fwrite(&nodeIndex, 4, 1, f);*/

				DWORD numVertices = mesh->GetNumVertices();
				fwrite(&numVertices, 4, 1, f);
				if (numVertices)
					fwrite(mesh->GetVertices(), sizeof(Vertex), numVertices, f);

				DWORD numSplits = mesh->GetNumSplits();
				fwrite(&numSplits, 4, 1, f);
				for (DWORD j = 0; j < numSplits; j++)
				{
					fwrite(&mesh->matSplits[j].matId, 4, 1, f);
					DWORD numIndices = mesh->matSplits[j].Indices.size();
					fwrite(&numIndices, 4, 1, f);
					if (numIndices)
						fwrite(&mesh->matSplits[j].Indices.front(), 2, numIndices, f);
				}

				Collision* collision = mesh->GetCollision();
				numVertices = collision->GetNumVertices();
				fwrite(&numVertices, 4, 1, f);
				if (numVertices)
				{
					fwrite(collision->GetVertices(), sizeof(Vertex), numVertices, f);
				}
				DWORD numPolygons = collision->GetNumFaces();
				fwrite(&numPolygons, 4, 1, f);
				if (numPolygons)
				{
					fwrite(collision->GetFaces(), sizeof(SimpleFace), numPolygons, f);
					fwrite(collision->GetFacesInfo(), sizeof(FaceInfo), numPolygons, f);
				}
			}
			else
			{
				DWORD null = 0;
				fwrite(&null, 4, 1, f);
			}
		}

		bool hasSky = skyDome != 0;
		fwrite(&hasSky, 1, 1, f);
		if (hasSky)
		{
			DWORD numMeshes = skyDome->GetNumMeshes();
			fwrite(&numMeshes, 4, 1, f);
			for (DWORD i = 0; i < numMeshes; i++)
			{
				Mesh* mesh = skyDome->GetMesh(i);
				DWORD pMesh = (DWORD)mesh + 16;
				fwrite((void*)pMesh, 32, 1, f);
				fwrite(&mesh->center, 12, 1, f);

				DWORD numVertices = mesh->GetNumVertices();
				fwrite(&numVertices, 4, 1, f);
				if (numVertices)
					fwrite(mesh->GetVertices(), sizeof(Vertex), numVertices, f);

				DWORD numSplits = mesh->GetNumSplits();
				fwrite(&numSplits, 4, 1, f);
				for (DWORD j = 0; j < numSplits; j++)
				{
					fwrite(&mesh->matSplits[j].matId, 4, 1, f);
					DWORD numIndices = mesh->matSplits[j].Indices.size();
					fwrite(&numIndices, 4, 1, f);
					if (numIndices)
						fwrite(&mesh->matSplits[j].Indices.front(), 2, numIndices, f);
				}

				Collision* collision = mesh->GetCollision();
				if (numVertices)
					numVertices = collision->GetNumVertices();
				fwrite(&numVertices, 4, 1, f);
				if (numVertices)
				{
					fwrite(collision->GetVertices(), sizeof(Vertex), numVertices, f);
				}
				DWORD numPolygons = collision->GetNumFaces();
				fwrite(&numPolygons, 4, 1, f);
				if (numPolygons)
				{
					fwrite(collision->GetFaces(), sizeof(SimpleFace), numPolygons, f);
					fwrite(collision->GetFacesInfo(), sizeof(FaceInfo), numPolygons, f);
				}
			}
		}

		DWORD numNodes = nodes.size();
		fwrite(&numNodes, 4, 1, f);
		for (DWORD i = 0; i < numNodes; i++)
		{
			fwrite(&nodes[i], 54, 1, f);
			DWORD numLinks = nodes[i].Links.size();
			fwrite(&numLinks, 4, 1, f);
			if (numLinks)
				fwrite(&nodes[i].Links.front(), 2, numLinks, f);
		}

		DWORD numGlobals = globals.size();
		fwrite(&numGlobals, 4, 1, f);
		for (DWORD i = 0; i < numGlobals; i++)
		{
			fwrite(&globals[i], 4, 1, f);
		}

		extern vector <Script> Scripts;
		DWORD numScripts = Scripts.size();
		fwrite(&numScripts, 4, 1, f);
		for (DWORD i = 0; i < numScripts; i++)
		{
			fwrite(&Scripts[i].checksum, 4, 1, f);
			DWORD len = 0;
			while (Scripts[i].name[len])
			{
				len++;
			}
			len++;
			fwrite(&len, 4, 1, f);
			fwrite(&Scripts[i].name, len, 1, f);
		}

		DWORD numTriggers = triggers.size();
		fwrite(&numTriggers, 4, 1, f);
		for (DWORD i = 0; i < numTriggers; i++)
		{
			fwrite(&triggers[i], 4, 1, f);
		}

		DWORD numKnown = KnownScripts.size();
		fwrite(&numKnown, 4, 1, f);
		for (DWORD i = 0; i < numKnown; i++)
		{
			fwrite(&KnownScripts[i], 8, 1, f);
			fwrite(&KnownScripts[i].func, KnownScripts[i].size, 1, f);
		}
	}
}

static void __mkdir(const char* dir) {
	char* p = NULL;

	for (p = (char*)(dir + 1); *p; p++)
		if (*p == '/') {
			*p = 0;
			CreateDirectory(dir, NULL);
			*p = '/';
		}

	size_t len = strlen(dir);
	if (dir[len - 1] == '/')
	{
		((char*)dir)[len - 1] = 0;
		CreateDirectory(dir, NULL);
		((char*)dir)[len - 1] = '/';
	}
	else
		CreateDirectory(dir, NULL);
}

static const Checksum const GetTerrainType(const Checksum const TerrainType)
{
	static const DWORD Default = Checksum("TERRAIN_DEFAULT").checksum;
	static const DWORD GrindConc = Checksum("TERRAIN_GRINDCONC").checksum;
	static const DWORD GrindMetal = Checksum("TERRAIN_GRINDMETAL").checksum;
	static const DWORD GrindWire = Checksum("TERRAIN_GRINDWIRE").checksum;
	static const DWORD GrindWood = Checksum("TERRAIN_GRINDWOOD").checksum;
	static const DWORD GrindTree = Checksum("TERRAIN_GRINDBANYONTREE").checksum;
	static const DWORD GrindBrass = Checksum("TERRAIN_GRINDBRASSRAIL").checksum;
	static const DWORD GrindCat = Checksum("TERRAIN_GRINDCATWALK").checksum;
	static const DWORD GrindCable = Checksum("TERRAIN_GRINDCABLE").checksum;
	static const DWORD GrindChain = Checksum("TERRAIN_GRINDCHAIN").checksum;
	static const DWORD GrindChainLink = Checksum("TERRAIN_GRINDCHAINLINK").checksum;
	static const DWORD GrindElectricWire = Checksum("TERRAIN_GRINDELECTRICWIRE").checksum;
	static const DWORD GrindGlassMonster = Checksum("TERRAIN_GRINDGLASSMONSTER").checksum;
	static const DWORD GrindRoundMetalPole = Checksum("TERRAIN_GRINDROUNDMETALPOLE").checksum;
	static const DWORD GrindNeonLight = Checksum("TERRAIN_GRINDNEONLIGHT").checksum;
	static const DWORD GrindTankTurret = Checksum("TERRAIN_GRINDTANKTURRET").checksum;
	static const DWORD WoodRailing = Checksum("TERRAIN_WOODRAILING").checksum;
	static const DWORD GrindWoodLog = Checksum("TERRAIN_GRINDWOODLOG").checksum;
	static const DWORD GrindPlastic = Checksum("TERRAIN_GRINDPLASTIC").checksum;
	static const DWORD GrindPlasticBarrier = Checksum("TERRAIN_GRINDPLASTICBARRIER").checksum;

	static const DWORD ConcSmooth = Checksum("TERRAIN_CONCSMOOTH").checksum;
	static const DWORD MetalSmooth = Checksum("TERRAIN_METALSMOOTH").checksum;
	static const DWORD MetalRough = Checksum("TERRAIN_METALROUGH").checksum;
	static const DWORD Wood = Checksum("TERRAIN_WOOD").checksum;
	static const DWORD ChainLink = Checksum("TERRAIN_CHAINLINK").checksum;
	static const DWORD MetalFuture = Checksum("TERRAIN_METALFUTURE").checksum;
	static const DWORD PlexiGlass = Checksum("TERRAIN_PLEXIGLASS").checksum;
	static const DWORD FiberGlass = Checksum("TERRAIN_FIBERGLASS").checksum;
	static const DWORD ConcRough = Checksum("TERRAIN_CONCROUGH").checksum;
	static const DWORD MetalCorrugated = Checksum("TERRAIN_METALCORRUGATED").checksum;
	static const DWORD MetalGrating = Checksum("TERRAIN_METALGRATING").checksum;
	static const DWORD MetalTin = Checksum("TERRAIN_METALTin").checksum;
	static const DWORD WoodMasonite = Checksum("TERRAIN_WOODMASONITE").checksum;
	static const DWORD WoodPlywood = Checksum("TERRAIN_WOODPLYWOOD").checksum;
	static const DWORD WoodFlimsy = Checksum("TERRAIN_WOODFlimsy").checksum;
	static const DWORD WoodShingle = Checksum("TERRAIN_WOODSHINGLE").checksum;
	static const DWORD WoodPier = Checksum("TERRAIN_WOODPIER").checksum;
	static const DWORD Brick = Checksum("TERRAIN_BRICK").checksum;
	static const DWORD Tile = Checksum("TERRAIN_TILE").checksum;
	static const DWORD Asphalt = Checksum("TERRAIN_ASPHALT").checksum;
	static const DWORD Rock = Checksum("TERRAIN_ROCK").checksum;
	static const DWORD Gravel = Checksum("TERRAIN_GRAVEL").checksum;
	static const DWORD Sidewalk = Checksum("TERRAIN_SIDEWALK").checksum;
	static const DWORD Grass = Checksum("TERRAIN_GRASS").checksum;
	static const DWORD GrassDried = Checksum("TERRAIN_GRASSDRIED").checksum;
	static const DWORD Dirt = Checksum("TERRAIN_DIRT").checksum;
	static const DWORD DirtPacked = Checksum("TERRAIN_DIRTPACKED").checksum;
	static const DWORD Water = Checksum("TERRAIN_WATER").checksum;
	static const DWORD Ice = Checksum("TERRAIN_ICE").checksum;
	static const DWORD Snow = Checksum("TERRAIN_SNOW").checksum;
	static const DWORD Sand = Checksum("TERRAIN_SAND").checksum;
	static const DWORD Carpet = Checksum("TERRAIN_CARPET").checksum;
	static const DWORD Conveyor = Checksum("TERRAIN_CONVEYOR").checksum;
	static const DWORD Generic1 = Checksum("TERRAIN_GENERIC1").checksum;
	static const DWORD Generic2 = Checksum("TERRAIN_GENERIC2").checksum;
	static const DWORD Wheels = Checksum("TERRAIN_WHEELS").checksum;
	static const DWORD WetConc = Checksum("TERRAIN_WETCONC").checksum;
	static const DWORD MetalFence = Checksum("TERRAIN_METALFENCE").checksum;
	static const DWORD GrindTrain = Checksum("TERRAIN_GRINDTRAIN").checksum;
	static const DWORD GrindRope = Checksum("TERRAIN_GRINDROPE").checksum;


	/*


	switch (TerrainType.checksum)
	{
	case Default:
	  return *(Checksum*)&Default;
	case GrindTankTurret:
	case GrindConc:
	  return *(Checksum*)&ConcSmooth;
	case GrindWire:
	case GrindMetal:
	case GrindBrass:
	case GrindCat:
	case GrindRoundMetalPole:
	  return *(Checksum*)&MetalSmooth;

	case GrindWood:
	case GrindTree:
	case WoodRailing:
	case GrindWoodLog:
	  return *(Checksum*)&Wood;

	case GrindCable:
	  return *(Checksum*)&MetalRough;

	case GrindChainLink:
	case GrindChain:
	  return *(Checksum*)&ChainLink;

	case GrindElectricWire:
	  return *(Checksum*)&MetalFuture;

	case GrindPlastic:
	case GrindPlasticBarrier:
	case GrindGlassMonster:
	  return *(Checksum*)&PlexiGlass;

	case GrindNeonLight:
	  return *(Checksum*)&FiberGlass;
	default:
	  break;
	}
	return TerrainType;*/

	static const DWORD KnownTerrains[41] = { Default, ConcSmooth, ConcRough, MetalSmooth, MetalRough, MetalCorrugated, MetalGrating, MetalTin, WoodMasonite, WoodPlywood, WoodFlimsy, WoodShingle, WoodPier, Brick, Tile, Asphalt, Rock, Gravel, Sidewalk, Grass, GrassDried, Dirt, DirtPacked, Water, Ice, Snow, Sand, Carpet, Conveyor, Generic1, Generic2, Wheels, WetConc, MetalFence, GrindTrain, GrindRope, Wood, ChainLink, MetalFuture, PlexiGlass, FiberGlass };
	for (DWORD i = 0; i < 41; i++)
	{
		if (TerrainType.checksum == KnownTerrains[i])
			return TerrainType;
	}


	if (TerrainType.checksum == GrindTankTurret || TerrainType.checksum == GrindConc)
		return *(Checksum*)&ConcSmooth;
	else if (TerrainType.checksum == GrindWire || TerrainType.checksum == GrindMetal || TerrainType.checksum == GrindBrass || TerrainType.checksum == GrindCat || TerrainType.checksum == GrindRoundMetalPole)
		return *(Checksum*)&MetalSmooth;
	else if (TerrainType.checksum == GrindWood || TerrainType.checksum == GrindTree || TerrainType.checksum == WoodRailing || TerrainType.checksum == GrindWoodLog)
		return *(Checksum*)&Wood;
	else if (TerrainType.checksum == GrindCable)
		return *(Checksum*)&MetalRough;
	else if (TerrainType.checksum == GrindChainLink || TerrainType.checksum == GrindChain)
		return *(Checksum*)&ChainLink;
	else if (TerrainType.checksum == GrindElectricWire)
		return *(Checksum*)&MetalFuture;
	else if (TerrainType.checksum == GrindPlastic || TerrainType.checksum == GrindPlasticBarrier || TerrainType.checksum == GrindGlassMonster)
		return *(Checksum*)&PlexiGlass;
	else if (TerrainType.checksum == GrindNeonLight)
		return *(Checksum*)&FiberGlass;
	else
		return *(Checksum*)&ConcSmooth;
}

void Scene::ExportBSP(const char* const __restrict path)
{
	char Path[MAX_PATH] = "";
	DWORD len = strlen(path);
	//FILE* f = fopen(path,"wb");
	Th3Scene* scene = new Th3Scene(this);
	memcpy(Path, path, len);

	Path[len - 1] = 'x';
	Path[len - 2] = 'd';
	Path[len - 3] = 't';
	FILE* f = fopen(Path, "wb");
	if (f)
	{
		scene->RWmatList.ExportTDX(f);
		fclose(f);
	}
	_fcloseall();
	f = fopen(path, "wb");
	if (f)
	{
		printf("going to export main level\n");
		SetProgressbarMaximum(300);
		scene->Export(f);
		SetProgressbarValue(100);
		SetProgressbarMaximum(200);
		fclose(f);
		/*fclose(f);
		_fcloseall();
		Th3Scene* scene = new Th3Scene(this);
		memcpy(Path,path,len);

		Path[len-1] = 'x';
		Path[len-2] = 'd';
		Path[len-3] = 't';
		f = fopen(Path,"wb");
		if(f)
		{
		scene->matList.ExportTDX(f);
		fclose(f);
		}
		f = fopen(path,"wb");
		if(f)
		{
		printf("going to export main level\n");
		SetProgressbarMaximum(300);
		scene->Export(f);
		SetProgressbarValue(100);
		SetProgressbarMaximum(200);
		fclose(f);
		}*/

		/*memcpy(Path,path,strlen(path));

		Path[len-1] = 'x';
		Path[len-2] = 'd';
		Path[len-3] = 't';
		f = fopen(Path,"wb");
		if(f)
		{
		scene->matList.ExportTDX(f);
		fclose(f);
		}*/

		delete scene;
		planeInfo = PlaneInfo();

		if (skyDome)
		{
			DWORD i = 0;
			while (path[i])
			{
				char c = path[i];
				if (c >= 'A' && c <= 'Z') c += 32;
				Path[i] = c;
				i++;
			}

			char tmp[25] = "";
			DWORD pos = ((string)Path).find_last_of("\\");
			DWORD endPos = ((string)Path).find(".bsp");
			for (DWORD i = pos; i < endPos; i++)
			{
				tmp[i - pos] = path[i];
			}
			/*char tmp2[100] = "";
			sprintf(tmp2, "_Sky");
			memcpy(&Path[pos], tmp2, strlen(tmp2)+1);*/
			//#ifndef _DEBUG
			//CreateDirectory(Path,NULL);
			//#endif
			sprintf(&Path[pos], "\\%s_Sky.bsp", tmp);
			//sprintf(tmp2, "\\%s_Sky.bsp", tmp);
			//memcpy(&Path[pos+4], tmp2, strlen(tmp2)+1);


			f = fopen(Path, "wb");
			if (f)
			{
				globalMaterialList = &this->skyDome->matList;
				scene = new Th3Scene(this->skyDome);
				printf("going to export sky\n");
				scene->Export(f);
				fclose(f);

				DWORD len2 = strlen(Path);
				Path[len2 - 1] = 'x';
				Path[len2 - 2] = 'd';
				Path[len2 - 3] = 't';
				f = fopen(Path, "wb");
				if (f)
				{
					scene->RWmatList.ExportTDX(f, true);
					fclose(f);
				}

				delete scene;
				planeInfo = PlaneInfo();
				globalMaterialList = &this->matList;
			}
			else
				printf("sky folder doesn't exists can't export:(\n");
		}
	}
	else
		printf("wtf\n");
	SetProgressbarValue(100);
	memcpy(Path, path, len + 1);

	Path[len - 1] = '\0';
	Path[len - 2] = 'b';
	Path[len - 3] = 'q';

	f = fopen(Path, "wb");
	if (f)
	{
		WriteQBKey("NodeArray", f);
		WriteWORD(f, 0x207);
		WriteDWORD(f, 0x1);
		WriteBYTE(f, 0x5);

		DWORD numNodes = nodes.size(), numFucked = 0;

		DWORD divider = 1;
		if (numNodes > 100)
			DWORD divider = numNodes / 100;

		for (DWORD i = 0; i < numNodes; i++)
		{
			SetProgressbarValue(i / divider);
			WriteBYTE(f, 0x3);
			WORD furthestLink = 0xFFFF;
			if (nodes[i].Position.x || nodes[i].Position.y || nodes[i].Position.z)
				WriteQBKey("Position", &nodes[i].Position, f);
			if (nodes[i].Angles.x || nodes[i].Angles.y || nodes[i].Angles.z)
				WriteQBKey("Angles", &nodes[i].Angles, f);
			WriteQBKey("Name", nodes[i].Name, f);
			if (nodes[i].Class.checksum != Restart::GetClass() && nodes[i].Compressed.checksum)
			{
				for (int j = 0; j < compressedNodes.size(); j++)
				{
					if (compressedNodes[j].Name.checksum == nodes[i].Compressed.checksum)
					{
						if (!compressedNodes[j].Class.checksum && nodes[i].Class.checksum)
							WriteQBKey("Class", nodes[i].Class, f);
						if (!compressedNodes[j].TrickObject && nodes[i].TrickObject)
							WriteQBKey("TrickObject", f);
						if (!compressedNodes[j].Cluster.checksum && nodes[i].Cluster.checksum)
							WriteQBKey("Cluster", nodes[i].Cluster, f);
						if (!compressedNodes[j].TerrainType.checksum && nodes[i].TerrainType.checksum)
							WriteQBKey("TerrainType", GetTerrainType(nodes[i].TerrainType), f);
						if (!compressedNodes[j].Type.checksum && nodes[i].Type.checksum)
							WriteQBKey("Type", nodes[i].Type, f);


						
							if (!compressedNodes[j].Type.checksum && nodes[i].Type.checksum)
								WriteQBKey("Type", nodes[i].Type, f);
							if (nodes[i].Class.checksum == RailNode::GetClass())
							{
								float furthestDist = 0.0f;
								for (DWORD k = 0, numLinks = nodes[i].Links.size(); k < numLinks; k++)
								{
									float distance = nodes[i].Position.DistanceTo(nodes[nodes[i].Links[k]].Position);

									if (distance > furthestDist || furthestLink == 0xFFFF)
									{
										furthestLink = nodes[i].Links[k];
										furthestDist = distance;
									}
								}
							}

						if (nodes[i].Trigger.checksum)
						{
							WriteQBKey("TriggerScript", nodes[i].Trigger, f);
							if (nodes[i].NetEnabled)
								WriteQBKey("NetEnabled", f);
							if (nodes[i].Permanent)
								WriteQBKey("Permanent", f);
						}

						if (!compressedNodes[j].CreatedAtStart && nodes[i].CreatedAtStart)
							WriteQBKey("CreatedAtStart", f);
						if (!compressedNodes[j].AbsentInNetGames && nodes[i].AbsentInNetGames)
							WriteQBKey("AbsentInNetGames", f);
						if (nodes[i].Links.size())
						{
							if (furthestLink == 0xFFFF)
								WriteArray("Links", nodes[i].Links.size(), &nodes[i].Links.front(), f);
							else
								WriteArray("Links", 1, &furthestLink, f);

						}


						WriteQBKey(nodes[i].Compressed, f);
						break;
					}

				}
			}
			else
			{
				WriteQBKey("Class", nodes[i].Class, f);
				if (nodes[i].TrickObject)
					WriteQBKey("TrickObject", f);
				if (nodes[i].Cluster.checksum)
					WriteQBKey("Cluster", nodes[i].Cluster, f);
				//WriteQBKey("NodeNum", i, f);
				if (nodes[i].TerrainType.checksum)
					WriteQBKey("TerrainType", GetTerrainType(nodes[i].TerrainType), f);
				if (nodes[i].Class.checksum == 0x1806DDF8)//Restart
				{
					WriteQBKey("RestartName", nodes[i].RestartName.GetString(), f);
					WriteQBKey("Type", Checksum("UserDefined"), f);
					if (nodes[i].Name.checksum == 0xF1EFFD1B)//team1
						WriteArray("restart_types", 1, &Checksum("CTF_1"), f);
					else if (nodes[i].Name.checksum == 0x68E6ACA1)//team2
						WriteArray("restart_types", 1, &Checksum("CTF_2"), f);
					else
						WriteArray("restart_types", 1, &Checksum("MultiPlayer"), f);
					if (nodes[i].Trigger.checksum)
						WriteQBKey("TriggerScript", nodes[i].Trigger, f);
					else
						WriteQBKey("TriggerScript", Checksum("TRG_SpawnSkater"), f);

				}
				/*else if(nodes[i].Class.checksum==EnvironmentObject::GetClass() && nodes[i].CreatedAtStart)
				{
				if(nodes[i].LinksTo(0x1806DDF8, &nodes[i].Links[0], &nodes.front()))
				WriteQBKey("TriggerScript", Checksum("sk3_killskater"),f);
				else if(nodes[i].Trigger.checksum)
				{
				WriteQBKey("NetEnabled",f);
				WriteQBKey("Permanent",f);
				WriteQBKey("TriggerScript", nodes[i].Trigger,f);
				}
				nodes[i].FixCheck(0x1806DDF8, &nodes.front());
				}*/
				
				/*else if(nodes[i].Class.checksum==EnvironmentObject::GetClass() && nodes[i].CreatedAtStart)
				{
				bool found = false;
				for(DWORD j=0; j<nodes[i].Links.size(); j++)
				{
				if(!found)
				{
				if(nodes[nodes[i].Links[j]].Class.checksum == 0x1806DDF8)
				{
				WriteQBKey("TriggerScript", Checksum("sk3_killskater"),f);
				break;
				}
				else if(nodes[nodes[i].Links[j]].Class.checksum == EnvironmentObject::GetClass())
				{
				for(DWORD k=0; k<nodes[nodes[i].Links[j]].Links.size(); k++)
				{
				if(nodes[nodes[nodes[i].Links[j]].Links[k]].Class.checksum == 0x1806DDF8)
				{
				WriteQBKey("TriggerScript", Checksum("sk3_killskater"),f);
				found=true;
				break;
				}
				}
				}
				}
				else
				break;
				}
				}*/
				else
				{
					if (nodes[i].Type.checksum)
						WriteQBKey("Type", nodes[i].Type, f);
					//float furthestDist = 0.0f;
					if (nodes[i].Class.checksum == RailNode::GetClass())
					{
						float furthestDist = 0.0f;
						for (DWORD j = 0, numLinks = nodes[i].Links.size(); j < numLinks; j++)
						{
							float distance = nodes[i].Position.DistanceTo(nodes[nodes[i].Links[j]].Position);

							if (distance > furthestDist || furthestLink == 0xFFFF)
							{
								furthestLink = nodes[i].Links[j];
								furthestDist = distance;
							}
						}
						/*for(DWORD j=0, numLinks = nodes[i].Links.size(); j<numLinks; j++)
						{
						furthestLink = nodes[i].Links[j];


						if(nodes[furthestLink].exportedLink)
						{
						furthestLink=0xFFFE;
						}
						else
						{
						nodes[furthestLink].exportedLink=true;
						break;
						}
						}*/
						/*furthestLink = nodes[i].Links.back();
						if(nodes[furthestLink].Class.checksum != RailNode::GetClass())
						furthestLink=0xFF;*/
					}
					/*for(DWORD j=0; j<nodes[i].Links.size(); j++)
					{
					if(nodes[nodes[i].Links[j]].Class.checksum == RailNode::GetClass())
					{*/
					/*float distance = nodes[i].Position.DistanceTo(nodes[nodes[i].Links[j]].Position);

					if(distance>furthestDist || furthestLink == 0xFFFF)
					{*/
					//furthestLink = nodes[i].Links[j];
					/*furthestDist = distance;
					}*/
					/* }
					}*/
				}
				if (nodes[i].Trigger.checksum)
				{
					WriteQBKey("TriggerScript", nodes[i].Trigger, f);
					if (nodes[i].NetEnabled)
						WriteQBKey("NetEnabled", f);
					if (nodes[i].Permanent)
						WriteQBKey("Permanent", f);
				}
				if (nodes[i].CreatedAtStart)
					WriteQBKey("CreatedAtStart", f);
				if (nodes[i].AbsentInNetGames)
					WriteQBKey("AbsentInNetGames", f);
				if (nodes[i].Links.size())
				{
					if (furthestLink == 0xFFFF)
						WriteArray("Links", nodes[i].Links.size(), &nodes[i].Links.front(), f);
					/*else if(furthestLink==0xFFFE)
					{
					furthestLink = nodes.size();
					WriteArray("Links", 1, &furthestLink, f);
					WriteQBKey("FuckedLinks",f);
					float furthestDist = 0.0f;
					furthestLink = 0xFFFF;
					for(DWORD j=0, numLinks = nodes[i].Links.size(); j<numLinks; j++)
					{
					float distance = nodes[i].Position.DistanceTo(nodes[nodes[i].Links[j]].Position);

					if(distance>furthestDist || furthestLink == 0xFFFF)
					{
					furthestLink = nodes[i].Links[j];
					furthestDist = distance;
					}
					}
					nodes.push_back(Node(nodes[furthestLink]));
					nodes.back().Links.erase(nodes.back().Links.begin(),nodes.back().Links.end());
					char chc[128];
					sprintf(chc, "FuckedRailNodes%d", numFucked);
					numFucked++;
					nodes.back().Name = Checksum(chc);
					numNodes++;
					}*/
					else
						WriteArray("Links", 1, &furthestLink, f);
				}
			}

			WriteBYTE(f, 0x4);
		}
		/*extern vector <Node> addedNodes;
		for (DWORD i = 0; i < addedNodes.size(); i++)
		{
		  WriteBYTE(f, 0x3);
		  WORD furthestLink = 0xFFFF;
		  if (addedNodes[i].Position.x || addedNodes[i].Position.y || addedNodes[i].Position.z)
			WriteQBKey("Position", &addedNodes[i].Position, f);
		  if (addedNodes[i].Angles.x || addedNodes[i].Angles.y || addedNodes[i].Angles.z)
			WriteQBKey("Angles", &addedNodes[i].Angles, f);
		  WriteQBKey("Name", addedNodes[i].Name, f);
		  WriteQBKey("Class", addedNodes[i].Class, f);
		  if (addedNodes[i].TrickObject)
			WriteQBKey("TrickObject", f);
		  if (addedNodes[i].Cluster.checksum)
			WriteQBKey("Cluster", addedNodes[i].Cluster, f);
		  //WriteQBKey("NodeNum", i, f);
		  if (addedNodes[i].TerrainType.checksum)
			WriteQBKey("TerrainType", GetTerrainType(addedNodes[i].TerrainType), f);
		  if (addedNodes[i].Class.checksum == 0x1806DDF8)//Restart
		  {
			WriteQBKey("RestartName", addedNodes[i].RestartName.GetString(), f);
			WriteQBKey("Type", Checksum("UserDefined"), f);
			if (addedNodes[i].Name.checksum == 0xF1EFFD1B)//team1
			  WriteArray("restart_types", 1, &Checksum("CTF_1"), f);
			else if (addedNodes[i].Name.checksum == 0x68E6ACA1)//team2
			  WriteArray("restart_types", 1, &Checksum("CTF_2"), f);
			else
			  WriteArray("restart_types", 1, &Checksum("MultiPlayer"), f);
			if (addedNodes[i].Trigger.checksum)
			  WriteQBKey("TriggerScript", nodes[i].Trigger, f);
			else
			  WriteQBKey("TriggerScript", Checksum("TRG_SpawnSkater"), f);
		  }
		  else if (addedNodes[i].Trigger.checksum)
		  {
			WriteQBKey("TriggerScript", addedNodes[i].Trigger, f);
			if (addedNodes[i].NetEnabled)
			  WriteQBKey("NetEnabled", f);
			if (addedNodes[i].Permanent)
			  WriteQBKey("Permanent", f);
		  }
		  else
		  {
			if (addedNodes[i].Type.checksum)
			  WriteQBKey("Type", nodes[i].Type, f);
			//float furthestDist = 0.0f;
			if (addedNodes[i].Class.checksum == RailNode::GetClass())
			{
			  float furthestDist = 0.0f;
			  for (DWORD j = 0, numLinks = addedNodes[i].Links.size(); j<numLinks; j++)
			  {
				float distance = addedNodes[i].Position.DistanceTo(addedNodes[addedNodes[i].Links[j]].Position);
				addedNodes[i].Links[j] += nodes.size();
				if (distance>furthestDist || furthestLink == 0xFFFF)
				{
				  furthestLink = addedNodes[i].Links[j];
				  furthestDist = distance;
				}
			  }
			}
		  }

		  if (addedNodes[i].CreatedAtStart)
			WriteQBKey("CreatedAtStart", f);
		  if (addedNodes[i].AbsentInNetGames)
			WriteQBKey("AbsentInNetGames", f);
		  if (addedNodes[i].Links.size())
		  {
			if (furthestLink == 0xFFFF)
			  WriteArray("Links", nodes[i].Links.size(), &addedNodes[i].Links.front(), f);
			else
			  WriteArray("Links", 1, &furthestLink, f);
		  }

		  WriteBYTE(f, 0x4);
		}*/
		WriteBYTE(f, 0x6);
		fputc(2, f);
		WriteDWORD(f, 1);

		WriteArray("TriggerScripts", triggers.size(), &triggers.front(), f);
		fputc(2, f);
		WriteDWORD(f, 1);

		fputc(0x23, f);
		WriteQBKey("LoadTerrain", f, true);
		WriteQBKey("LoadSound", f);
		fputc(0x1B, f);
		WriteDWORD(f, 19);
		WriteData(f, "Shared\\glasspane2x", 19);
		fputc(2, f);
		WriteDWORD(f, 1);
		WriteQBKey("SetTerrainDefault", f, true);
		WriteQBKey("SetTerrainAsphalt", f, true);
		WriteQBKey("SetTerrainConcSmooth", f, true);
		WriteQBKey("SetTerrainConcRough", f, true);
		WriteQBKey("SetTerrainMetalSmooth", f, true);
		WriteQBKey("SetTerrainMetalRough", f, true);
		WriteQBKey("SetTerrainMetalCorrugated", f, true);
		WriteQBKey("SetTerrainMetalGrating", f, true);
		WriteQBKey("SetTerrainMetalTin", f, true);
		WriteQBKey("SetTerrainWood", f, true);
		WriteQBKey("SetTerrainWoodMasonite", f, true);
		WriteQBKey("SetTerrainWoodPlywood", f, true);
		WriteQBKey("SetTerrainWoodShingle", f, true);
		WriteQBKey("SetTerrainWoodPier", f, true);
		WriteQBKey("SetTerrainWoodFlimsy", f, true);
		WriteQBKey("SetTerrainBrick", f, true);
		WriteQBKey("SetTerrainTile", f, true);
		WriteQBKey("SetTerrainRock", f, true);
		WriteQBKey("SetTerrainGravel", f, true);
		WriteQBKey("SetTerrainGrass", f, true);
		WriteQBKey("SetTerrainGrassDried", f, true);
		WriteQBKey("SetTerrainDirt", f, true);
		WriteQBKey("SetTerrainDirtPacked", f, true);
		WriteQBKey("SetTerrainSand", f, true);
		WriteQBKey("SetTerrainCarpet", f, true);
		WriteQBKey("SetTerrainChainlink", f, true);
		WriteQBKey("SetTerrainWetConc", f, true);
		WriteQBKey("SetTerrainPlexiglass", f, true);
		WriteQBKey("SetTerrainWater", f, true);
		WriteQBKey("SetTerrainMetalFuture", f, true);
		WriteQBKey("SetTerrainSnow", f, true);
		WriteQBKey("SetTerrainFiberglass", f, true);
		WriteQBKey("SetTerrainIce", f, true);
		WriteQBKey("SetTerrainGeneric1", f, true);
		WriteQBKey("SetTerrainGeneric2", f, true);
		WriteQBKey("SetTerrainGrindTrain", f, true);
		WriteQBKey("SetTerrainGrindRope", f, true);
		WriteQBKey("SetTerrainSidewalk", f, true);
		WriteQBKey("SetTerrainWetConc", f, true);
		WriteQBKey("SetTerrainMetalFence", f, true);
		WriteQBKey("SetTerrainConveyor", f, true);

		if (GetScript("SetTh2Physics"))
			WriteQBKey("SetTh2Physics", f, true);
		else if (GetScript("LoadSounds"))
			WriteQBKey("LoadSounds", f, true);

		fputc(0x24, f);
		fputc(0x2, f);
		WriteDWORD(f, 1);

		fputc(0x23, f);
		WriteQBKey("TRG_SpawnSkater", f, true);

		WriteQBKey("MakeSkaterGoto", f);
		WriteQBKey("StartSkating1", f, true);

		fputc(0x24, f);
		fputc(0x2, f);
		WriteDWORD(f, 1);

		for (DWORD i = 0; i < globals.size(); i++)
		{
			fputc(0x16, f);
			WriteDWORD(f, globals[i].checksum);
			WriteData(f, (void*)init, sizeof(init));
		}

		for (DWORD i = 1; i < triggers.size(); i++)//skip first(LoadTerrain)
		{
			DWORD size;
			const void* scriptData = GetScript(triggers[i].checksum, &size);

			fputc(0x23, f);
			WriteQBKey(triggers[i].checksum, f, true);
			if (scriptData)
				WriteData(f, scriptData, size);
			fputc(0x24, f);
			fputc(0x2, f);
			WriteDWORD(f, 1);
		}
		for (DWORD i = 0; i < KnownScripts.size(); i++)
		{
			for (DWORD j = 0; j < triggers.size(); j++)
			{
				if (triggers[j].checksum == KnownScripts[i].name.checksum)
					goto Next2;
			}
			fputc(0x23, f);
			WriteQBKey(KnownScripts[i].name.checksum, f, true);
			if (KnownScripts[i].size)
				WriteData(f, KnownScripts[i].func, KnownScripts[i].size);
			fputc(0x24, f);
			fputc(0x2, f);
			WriteDWORD(f, 1);
		Next2:;
		}

		for (DWORD i = 0; i < compressedNodes.size(); i++)
		{
			WriteQBKey(*(Checksum*)&compressedNodes[i].Name, f);
			fputc(0x07, f);
			fputc(0x03, f);

			if (compressedNodes[i].CreatedAtStart)
				WriteQBKey("CreatedAtStart", f);
			if (compressedNodes[i].Class.checksum)
				WriteQBKey("Class", compressedNodes[i].Class, f);
			if (compressedNodes[i].Type.checksum)
				WriteQBKey("Type", compressedNodes[i].Type, f);
			if (compressedNodes[i].TrickObject)
				WriteQBKey("TrickObject", f);
			if (compressedNodes[i].Cluster.checksum)
				WriteQBKey("Cluster", compressedNodes[i].Cluster, f);
			if (compressedNodes[i].AbsentInNetGames)
				WriteQBKey("AbsentInNetGames", f);
			if (compressedNodes[i].TerrainType.checksum)
				WriteQBKey("TerrainType", GetTerrainType(compressedNodes[i].TerrainType), f);
			fputc(0x4, f);
			fputc(0x1, f);
		}
		/*if(Scripts[i].checksum==Checksum("Position",false).checksum || Scripts[i].checksum==Checksum("Angles",false).checksum
		|| Scripts[i].checksum==Checksum("Type",false).checksum || Scripts[i].checksum==Checksum("Class",false).checksum
		|| Scripts[i].checksum==Checksum("Name",false).checksum || Scripts[i].checksum==Checksum("TriggerScript",false).checksum
		|| Scripts[i].checksum==Checksum("TriggerScripts",false).checksum || Scripts[i].checksum==Checksum("Links",false).checksum
		|| Scripts[i].checksum==Checksum("NetEnabled",false).checksum || Scripts[i].checksum==Checksum("Permanent",false).checksum
		|| Scripts[i].checksum==Checksum("CreatedAtStart",false).checksum || Scripts[i].checksum==Checksum("AbsentInNetGames",false).checksum
		|| Scripts[i].checksum==Checksum("LoadSound",false).checksum || Scripts[i].checksum==Checksum("SetTerrainDefault",false).checksum
		|| Scripts[i].checksum==Checksum("SetTerrainAsphalt",false).checksum || Scripts[i].checksum==Checksum("SetTerrainConcSmooth",false).checksum
		|| Scripts[i].checksum==Checksum("SetTerrainConcRough",false).checksum || Scripts[i].checksum==Checksum("SetTerrainMetalSmooth",false).checksum
		|| Scripts[i].checksum==Checksum("SetTerrainMetalRough",false).checksum || Scripts[i].checksum==Checksum("SetTerrainMetalCorrugated",false).checksum
		|| Scripts[i].checksum==Checksum("SetTerrainMetalGrating",false).checksum || Scripts[i].checksum==Checksum("SetTerrainMetalTin",false).checksum
		|| Scripts[i].checksum==Checksum("SetTerrainWood",false).checksum || Scripts[i].checksum==Checksum("SetTerrainWoodMasonite",false).checksum
		|| Scripts[i].checksum==Checksum("SetTerrainWoodPlywood",false).checksum || Scripts[i].checksum==Checksum("SetTerrainWoodShingle",false).checksum
		|| Scripts[i].checksum==Checksum("SetTerrainWoodPier",false).checksum || Scripts[i].checksum==Checksum("SetTerrainWoodFlimsy",false).checksum
		|| Scripts[i].checksum==Checksum("SetTerrainBrick",false).checksum || Scripts[i].checksum==Checksum("SetTerrainTile",false).checksum
		|| Scripts[i].checksum==Checksum("SetTerrainRock",false).checksum || Scripts[i].checksum==Checksum("SetTerrainGravel",false).checksum
		|| Scripts[i].checksum==Checksum("SetTerrainGrass",false).checksum || Scripts[i].checksum==Checksum("SetTerrainGrassDried",false).checksum
		|| Scripts[i].checksum==Checksum("SetTerrainDirt",false).checksum || Scripts[i].checksum==Checksum("SetTerrainDirtPacked",false).checksum
		|| Scripts[i].checksum==Checksum("SetTerrainSand",false).checksum || Scripts[i].checksum==Checksum("SetTerrainCarpet",false).checksum
		|| Scripts[i].checksum==Checksum("SetTerrainChainlink",false).checksum || Scripts[i].checksum==Checksum("SetTerrainWetConc",false).checksum
		|| Scripts[i].checksum==Checksum("SetTerrainPlexiglass",false).checksum || Scripts[i].checksum==Checksum("SetTerrainWater",false).checksum
		|| Scripts[i].checksum==Checksum("SetTerrainMetalFuture",false).checksum || Scripts[i].checksum==Checksum("SetTerrainSnow",false).checksum
		|| Scripts[i].checksum==Checksum("SetTerrainConveyor",false).checksum || Scripts[i].checksum==Checksum("SetTerrainIce",false).checksum
		|| Scripts[i].checksum==Checksum("SetTerrainFiberglass",false).checksum || Scripts[i].checksum==Checksum("SetTerrainGeneric2",false).checksum
		|| Scripts[i].checksum==Checksum("SetTerrainGeneric1",false).checksum || Scripts[i].checksum==Checksum("SetTerrainGrindTrain",false).checksum
		|| Scripts[i].checksum==Checksum("SetTerrainSidewalk",false).checksum || Scripts[i].checksum==Checksum("SetTerrainGrindRope",false).checksum
		|| Scripts[i].checksum==Checksum("SetTerrainWetConc",false).checksum || Scripts[i].checksum==Checksum("SetTerrainMetalFence",false).checksum
		|| Scripts[i].checksum==Checksum("LoadSounds",false).checksum || Scripts[i].checksum==Checksum("TRG_SpawnSkater",false).checksum
		|| Scripts[i].checksum==Checksum("MakeSkaterGoto",false).checksum || Scripts[i].checksum==Checksum("StartSkating1",false).checksum
		|| Scripts[i].checksum==Checksum("RestartName",false).checksum || Scripts[i].checksum==Checksum("Restart_Types",false).checksum
		|| Scripts[i].checksum==Checksum("Cluster",false).checksum || Scripts[i].checksum==Checksum("Multiplayer",false).checksum
		|| Scripts[i].checksum==Checksum("UserDefined",false).checksum || Scripts[i].checksum==Checksum("TrickObject",false).checksum
		|| Scripts[i].checksum==Checksum("Crown",false).checksum || Scripts[i].checksum==Checksum("Zone",false).checksum
		|| Scripts[i].checksum==Checksum("ZONE_KEY",false).checksum || Scripts[i].checksum==Checksum("CTF_1",false).checksum
		|| Scripts[i].checksum==Checksum("CTF_2",false).checksum)*/
		const static DWORD checksums[] = {
		  Checksum("Position",false).checksum, Checksum("Angles",false).checksum, Checksum("Name",false).checksum, Checksum("Class",false).checksum, Checksum("CreatedAtStart",false).checksum,
		  Checksum("Class", false).checksum, Checksum("Type", false).checksum, Checksum("TriggerScript", false).checksum, Checksum("Links", false).checksum, Checksum("TrickObject", false).checksum, Checksum("Cluster", false).checksum,
		  Checksum("Permanent",false).checksum, Checksum("NetEnabled",false).checksum, Checksum("RestartName",false).checksum, Checksum("UserDefined",false).checksum,
		  Checksum("Restart_Types",false).checksum, Checksum("Multiplayer",false).checksum, Checksum("Zone",false).checksum, Checksum("Crown",false).checksum, Checksum("CTF_1",false).checksum,
		  Checksum("CTF_2",false).checksum, Checksum("AbsentInNetGames",false).checksum, Checksum("ZONE_KEY",false).checksum, Checksum("TerrainType", false).checksum, Checksum("SetTerrainAsphalt",false).checksum,
		  Checksum("SetTerrainConcSmooth",false).checksum, Checksum("SetTerrainDirtPacked",false).checksum, Checksum("SetTerrainCarpet",false).checksum, Checksum("SetTerrainWetConc",false).checksum,
		  Checksum("SetTerrainMetalRough",false).checksum, Checksum("SetTerrainMetalCorrugated",false).checksum, Checksum("SetTerrainMetalGrating",false).checksum, Checksum("TriggerScripts",false).checksum,
		  Checksum("SetTerrainMetalTin",false).checksum, Checksum("SetTerrainWood",false).checksum, Checksum("SetTerrainWoodMasonite",false).checksum, Checksum("SetTerrainWoodPier",false).checksum,
		  Checksum("SetTerrainWoodPlywood",false).checksum, Checksum("SetTerrainWoodShingle",false).checksum, Checksum("SetTerrainWoodFlimsy",false).checksum, Checksum("SetTerrainBrick",false).checksum,
		  Checksum("SetTerrainTile",false).checksum, Checksum("SetTerrainRock",false).checksum, Checksum("SetTerrainGravel",false).checksum, Checksum("SetTerrainGrass",false).checksum,
		  Checksum("SetTerrainGrassDried",false).checksum, Checksum("SetTerrainDirt",false).checksum, Checksum("SetTerrainSand",false).checksum, Checksum("SetTerrainChainlink",false).checksum,
		  Checksum("SetTerrainPlexiglass",false).checksum, Checksum("SetTerrainMetalFuture",false).checksum, Checksum("SetTerrainConveyor",false).checksum, Checksum("SetTerrainFiberglass",false).checksum,
		  Checksum("SetTerrainGeneric1",false).checksum, Checksum("SetTerrainSidewalk",false).checksum, Checksum("SetTerrainWetConc",false).checksum, Checksum("LoadSounds",false).checksum,
		  Checksum("MakeSkaterGoto",false).checksum, Checksum("SetTerrainDefault",false).checksum, Checksum("SetTerrainMetalSmooth",false).checksum, Checksum("LoadSound",false).checksum,
		  Checksum("SetTerrainConcRough",false).checksum, Checksum("SetTerrainWater",false).checksum, Checksum("SetTerrainSnow",false).checksum, Checksum("SetTerrainIce",false).checksum,
		  Checksum("SetTerrainGeneric2",false).checksum, Checksum("SetTerrainGrindTrain",false).checksum, Checksum("SetTerrainGrindRope",false).checksum, Checksum("StartSkating1",false).checksum,
		  Checksum("SetTerrainMetalFence",false).checksum, Checksum("TRG_SpawnSkater",false).checksum, Checksum("NodeArray",false).checksum
		};



		for (DWORD i = 0, numScripts = Scripts.size(); i < numScripts; i++)
		{
			if (Scripts[i].checksum == 0x31ADBD6E || Scripts[i].checksum == 0x6EBDAD31)
				MessageBox(0, "YEAE", 0, 0);
			for (DWORD j = 0; j < globals.size(); j++)
			{
				if (Scripts[i].checksum == globals[j].checksum)
				{
					fputc(0x2B, f);
					WriteDWORD(f, Scripts[i].checksum);
					fwrite(Scripts[i].name, strlen(Scripts[i].name) + 1, 1, f);
					goto Next;
				}
			}
			for (DWORD j = 0, numChecksums = (sizeof(checksums) / sizeof(Checksum)); j < numChecksums; j++)
			{
				if (Scripts[i].checksum == checksums[j])
				{
					fputc(0x2B, f);
					WriteDWORD(f, Scripts[i].checksum);
					fwrite(Scripts[i].name, strlen(Scripts[i].name) + 1, 1, f);
					goto Next;
				}
			}
			for (DWORD j = 0; j < triggers.size(); j++)
			{
				if (Scripts[i].checksum == triggers[j].checksum)
				{
					fputc(0x2B, f);
					WriteDWORD(f, Scripts[i].checksum);
					fwrite(Scripts[i].name, strlen(Scripts[i].name) + 1, 1, f);
					goto Next;
				}
			}
			for (DWORD j = 0; j < nodes.size(); j++)
			{
				if (nodes[j].Name.checksum == Scripts[i].checksum || nodes[j].Class.checksum == Scripts[i].checksum
					|| nodes[j].Type.checksum == Scripts[i].checksum || nodes[j].Cluster.checksum == Scripts[i].checksum
					|| nodes[j].Trigger.checksum == Scripts[i].checksum)
				{
					fputc(0x2B, f);
					WriteDWORD(f, Scripts[i].checksum);
					fwrite(Scripts[i].name, strlen(Scripts[i].name) + 1, 1, f);
					goto Next;
				}
			}
			/*for (DWORD j = 0; j<addedNodes.size(); j++)
			{
			  if (addedNodes[j].Name.checksum == Scripts[i].checksum || addedNodes[j].Class.checksum == Scripts[i].checksum
				|| addedNodes[j].Type.checksum == Scripts[i].checksum || addedNodes[j].Cluster.checksum == Scripts[i].checksum
				|| addedNodes[j].Trigger.checksum == Scripts[i].checksum)
			  {
				fputc(0x2B, f);
				WriteDWORD(f, Scripts[i].checksum);
				fwrite(Scripts[i].name, strlen(Scripts[i].name) + 1, 1, f);
				goto Next;
			  }
			}*/
			for (DWORD j = 0; j < KnownScripts.size(); j++)
			{
				if (Scripts[i].checksum == KnownScripts[j].name.checksum)
				{
					fputc(0x2B, f);
					WriteDWORD(f, Scripts[i].checksum);
					fwrite(Scripts[i].name, strlen(Scripts[i].name) + 1, 1, f);
					goto Next;
				}
				BYTE* pFile = KnownScripts[j].func;
				BYTE* max = pFile + KnownScripts[j].size;
				while (pFile < max)
				{
					const BYTE opcode = *pFile;
					pFile++;
					switch (opcode)
					{
					case 0x1C:
					case 0x1B:
						pFile += *(DWORD*)pFile + 4;
						break;
					case 0x16:
						if (Scripts[i].checksum == *(DWORD*)pFile)
						{
							fputc(0x2B, f);
							WriteDWORD(f, Scripts[i].checksum);
							fwrite(Scripts[i].name, strlen(Scripts[i].name) + 1, 1, f);
							goto Next;
						}
						pFile += 4;
						break;
					case 0x17:
					case 0x1A:
					case 0x2E:
					case 2:
						pFile += 4;
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
					}
				}
			}
		Next:;
		}
		fputc(0, f);
		fclose(f);
	}

	/*if (compressedNodes.size())
	{
		memcpy(Path, path, len + 1);

		Path[len - 2] = 0x0;
		Path[len - 3] = 'q';

		f = fopen(Path, "wb");
		if (f)
		{
			fputc(0xAA, f);
			for (int i = 0; i < compressedNodes.size(); i++)
			{
				WriteQBKey(compressedNodes[i].Name.checksum, f);
				if (compressedNodes[i].CreatedAtStart)
					WriteQBKey("CreatedAtStart", f);
				if(compressedNodes[i].Class.checksum)
					WriteQBKey("Class", compressedNodes[i].Class, f);
				if (compressedNodes[i].Type.checksum)
					WriteQBKey("Type", compressedNodes[i].Type, f);
				if (compressedNodes[i].TrickObject)
					WriteQBKey("TrickObject", f);
				if (compressedNodes[i].Cluster.checksum)
					WriteQBKey("Cluster", compressedNodes[i].Cluster, f);
				if (compressedNodes[i].AbsentInNetGames)
					WriteQBKey("AbsentInNetGames", f);
				if (compressedNodes[i].TerrainType.checksum)
					WriteQBKey("TerrainType", GetTerrainType(compressedNodes[i].TerrainType), f);
				fputc(0x1, f);
			}
			fputc(0x0, f);
			fclose(f);
	    }
		
	}*/
	SetProgressbarValue(200);
}

vector<expo> alreadyExported;
DWORD numCopied = 0;

bool AlreadyExported(Texture* tex, char** pName)
{
	const DWORD numExported = alreadyExported.size();
	if (numExported == 0)
		return false;
	else
	{
		char* name = tex->GetName();
		DWORD uAddress = tex->uAddress;
		DWORD vAddress = tex->vAddress;
		for (DWORD i = 0; i < numExported; i++)
		{
			char* p = name;
			char* c = alreadyExported[i].name;
			bool equals = true;
			while (*p)
			{
				if (*p != *c)
					equals = false;
				p++;
				c++;
			}
			if (equals)
			{
				if (uAddress == alreadyExported[i].uAddress && vAddress == alreadyExported[i].vAddress)
					return true;
				else
				{
					*pName = tmp;
					numCopied++;
					strcpy(tmp, tex->GetName());
					sprintf(tex->GetName(), "copied%u", numCopied);
				}
			}
		}
	}
	return false;
}

void Scene::ExportScn(const char* const __restrict path)
{
	char Path[MAX_PATH] = "";
	DWORD len = strlen(path);
	//FILE* f = fopen(path,"wb");
	memcpy(Path, path, len);

	/*Path[len + 4] = '0x0';
	Path[len + 3] = 'x';
	Path[len + 2] = 'b';
	Path[len + 1] = 'x';
	Path[len] = '.';
	Path[len - 1] = 'n';
	Path[len - 2] = 'c';
	Path[len - 3] = 's';*/
	FILE* f = fopen(Path, "wb");

	WriteDWORD(f, 1, 3);

	DWORD numMaterials = matList.GetNumMaterials();
	WriteDWORD(f, numMaterials);
	for (DWORD i = 0; i < numMaterials; i++)
	{
		Material* material = matList.GetMaterial(i);
		char id[200] = "";
		sprintf(id, "Material%u", i);
		WriteDWORD(f, Checksum(id).checksum, 2);
		DWORD numTextures = 1;// material->GetNumTextures();
		WriteDWORD(f, numTextures);
		WriteDWORD(f, 0xFFFFFFFF);

		fputc(true, f);
		fwrite(&(float)material->drawOrder, 4, 1, f);

		/*if (material->doubled)
		{*/
		fputc(false, f);
		fputc(true, f);
		/*}
		else
		{
		  fputc(false, f);
		  fputc(false, f);
		}*/

		WriteDWORD(f, 0);

		fputc(false, f);

		WriteDWORD(f, 0);

		for (DWORD j = 0; j < numTextures; j++)
		{
			Texture* texture = material->GetTexture(j);
			WriteDWORD(f, Checksum(texture->GetName()).checksum);

			DWORD flags = 0;
			Animation* anim = NULL;
			if (material->transparent)
				flags |= 0x0040;
			if (material->invisible)
				flags |= 0x0100;
			if (material->doubled)
				flags |= 0x0200;
			else
				flags |= 0x0080;
			if (material->animated)
			{
				anim = GetAnimation(material->GetMatId());
				if (anim)
					flags |= 0x0001;
			}
			WriteDWORD(f, flags);

			fputc(false, f);
			float half = 0.5f;
			WriteDWORD(f, *(DWORD*)&half, 3);

			WriteDWORD(f, 0);

			WriteDWORD(f, texture->fixedAlpha);

			if (texture->uAddress == D3DTADDRESS_WRAP)
				WriteDWORD(f, 0);
			else if (texture->uAddress == D3DTADDRESS_CLAMP)
				WriteDWORD(f, 1);
			else
				WriteDWORD(f, 2);
			if (texture->vAddress == D3DTADDRESS_WRAP)
				WriteDWORD(f, 0);
			else if (texture->vAddress == D3DTADDRESS_CLAMP)
				WriteDWORD(f, 1);
			else
				WriteDWORD(f, 2);

			WriteDWORD(f, 0, 3);

			if (anim)
			{
				WriteDWORD(f, *(DWORD*)&anim->UVel);
				WriteDWORD(f, *(DWORD*)&anim->VVel);
				WriteDWORD(f, *(DWORD*)&anim->UFrequency);
				WriteDWORD(f, *(DWORD*)&anim->VFrequency);
				WriteDWORD(f, *(DWORD*)&anim->UAmplitude);
				WriteDWORD(f, *(DWORD*)&anim->VAmplitude);
				WriteDWORD(f, *(DWORD*)&anim->UPhase);
				WriteDWORD(f, *(DWORD*)&anim->VPhase);
			}

			WriteDWORD(f, 1);
			WriteDWORD(f, 4);

			if (j == 0 && numTextures > 1)
			{
				WriteDWORD(f, 0xC1100000);
				WriteDWORD(f, 0xC1100000);
			}
			else
			{
				WriteDWORD(f, 0xC1000000);
				WriteDWORD(f, 0xC1000000);
			}
		}
	}

	DWORD numMeshes = 0;
	for (DWORD i = 0; i < GetNumMeshes(); i++)
	{
		if (!meshes[i].IsDeleted() && meshes[i].GetNumSplits())
			numMeshes++;
	}

	WriteDWORD(f, numMeshes);
	for (DWORD i = 0; i < GetNumMeshes(); i++)
	{
		Mesh* mesh = &meshes[i];
		if (!mesh->IsDeleted() && mesh->GetNumSplits())
		{
			WriteDWORD(f, mesh->GetName().checksum);
			WriteDWORD(f, 0xFFFFFFFF);

			WriteDWORD(f, 0x7);
			DWORD numSplits = mesh->GetNumSplits();
			WriteDWORD(f, numSplits);

			SimpleVertex center;
			SimpleVertex diag;
			BBox bbox = mesh->GetBBox();
			bbox.Max.z *= -1.0f;
			bbox.Min.z *= -1.0f;
			std::swap(bbox.Max.z, bbox.Min.z);

			center.x = (bbox.Max.x + bbox.Min.x) / 2;
			diag.x = (bbox.Max.x - bbox.Min.x);
			float radius = (diag.x * diag.x);

			center.y = (bbox.Max.y + bbox.Min.y) / 2;
			diag.y = (bbox.Max.y - bbox.Min.y);
			radius += (diag.y * diag.y);

			center.z = (bbox.Max.z + bbox.Min.z) / 2;
			diag.z = (bbox.Max.z - bbox.Min.z);
			radius += (diag.z * diag.z);

			radius = sqrtf(radius) / 2.0f;

			WriteData(f, (BYTE*)&bbox, 24);
			WriteData(f, (BYTE*)&center, 12);
			WriteDWORD(f, *(DWORD*)&radius);

			for (DWORD j = 0; j < numSplits; j++)
			{
				MaterialSplit* split = mesh->GetSplit(j);
				DWORD stride = 12;
				Material* mat = matList.GetMaterial(split->matId);
				if (mat)
					stride += 8 * mat->GetNumTextures();
				else
					stride += 8;
				stride += 4;
				stride += 12;

				struct vList
				{
					WORD oldIndex;
					WORD newIndex;

					vList(WORD o, WORD n)
					{
						oldIndex = o;
						newIndex = n;
					}
				};

				vector<vList> list;

				for (DWORD k = 0; k < split->Indices.size(); k++)
				{
					bool added = false;
					for (DWORD l = 0; l < list.size(); l++)
					{
						if (list[l].oldIndex == split->Indices[k])
						{
							added = true;
							break;
						}
					}
					if (!added)
						list.push_back(vList(split->Indices[k], list.size()));
				}

				SimpleVertex v = *(SimpleVertex*)&mesh->GetVertex(list[0].oldIndex);
				v.z *= -1.0f;
				bbox.Max = v;
				bbox.Min = v;
				for (DWORD k = 1; k < list.size(); k++)
				{
					v = *(SimpleVertex*)&mesh->GetVertex(list[k].oldIndex);
					v.z *= -1.0f;
					if (bbox.Max.x < v.x)
						bbox.Max.x = v.x;
					else if (bbox.Min.x > v.x)
						bbox.Min.x = v.x;
					if (bbox.Max.y < v.y)
						bbox.Max.y = v.y;
					else if (bbox.Min.y > v.y)
						bbox.Min.y = v.y;
					if (bbox.Max.z < v.z)
						bbox.Max.z = v.z;
					else if (bbox.Min.z > v.z)
						bbox.Min.z = v.z;
				}

				center.x = (bbox.Max.x + bbox.Min.x) / 2;
				diag.x = (bbox.Max.x - bbox.Min.x);
				radius = (diag.x * diag.x);

				center.y = (bbox.Max.y + bbox.Min.y) / 2;
				diag.y = (bbox.Max.y - bbox.Min.y);
				radius += (diag.y * diag.y);

				center.z = (bbox.Max.z + bbox.Min.z) / 2;
				diag.z = (bbox.Max.z - bbox.Min.z);
				radius += (diag.z * diag.z);

				radius = sqrtf(radius) / 2.0f;

				WriteData(f, (BYTE*)&center, 12);
				WriteDWORD(f, *(DWORD*)&radius);
				WriteData(f, (BYTE*)&bbox, 24);
				WriteDWORD(f, 0);

				char id[200] = "";
				if (matList.GetMaterial(split->matId))
					sprintf(id, "Material%u", split->matId);
				else
					sprintf(id, "Material1");
				WriteDWORD(f, Checksum(id).checksum);

				WriteDWORD(f, 1);

				WriteDWORD(f, 0);
				/*for (DWORD k = 0; k < split->Indices.size(); k++)
				{
				  for (DWORD l = 0; l < list.size(); l++)
				  {
					if (list[l].oldIndex == split->Indices[k])
					{
					  WriteWORD(f, list[l].newIndex);
					  break;
					}
				  }
				}*/

				WriteWORD(f, (WORD)split->Indices.size());
				for (DWORD k = 0; k < split->Indices.size(); k++)
				{
					for (DWORD l = 0; l < list.size(); l++)
					{
						if (list[l].oldIndex == split->Indices[k])
						{
							WriteWORD(f, list[l].newIndex);
							break;
						}
					}
				}

				WriteWORD(f, 0, 7);

				fputc(stride, f);
				WriteWORD(f, (WORD)list.size());
				WriteWORD(f, 1);

				WriteDWORD(f, stride * list.size());
				for (DWORD k = 0; k < list.size(); k++)
				{
					WriteData(f, (BYTE*)&mesh->GetVertex(list[k].oldIndex), 8);
					float invZ = mesh->GetVertex(list[k].oldIndex).z * -1.0f;
					WriteDWORD(f, *(DWORD*)&invZ);
					float half = 0.5f;
					WriteDWORD(f, *(DWORD*)&half, 3);
					WriteDWORD(f, *(DWORD*)&mesh->GetVertex(list[k].oldIndex).colour);
					WriteData(f, mesh->GetVertex(list[k].oldIndex).tUV, 8);
				}
				BYTE thoseBytes[] = { 0x52, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x18, 0x1C, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x94, 0x02, 0x46, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xD1, 0xD9, 0xD1, 0xD8, 0x00, 0x00, 0x00, 0x00, 0x10, 0x10, 0xD4, 0xDA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x03, 0x0F, 0x80, 0x1C, 0x01, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xCD, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA0, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC2, 0xC9, 0xC1, 0xC8, 0xC4, 0xC9, 0xC4, 0xC8, 0x00, 0x00, 0xC1, 0xCA, 0x00, 0x00, 0xC4, 0xCA, 0x3D, 0xCC, 0xCD, 0x1D, 0x3A, 0xCC, 0xCA, 0x1A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x89, 0x00, 0x00, 0x00, 0xCD, 0x00, 0x02, 0x00, 0xA0, 0x00, 0x00, 0x00, 0xA0, 0x00, 0x02, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x11, 0x01, 0x00, 0x21, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0xF2, 0xFF, 0xFF, 0xF1, 0xFF, 0xFF, 0xFF, 0xF4, 0x01, 0x00, 0x00 };
				WriteData(f, thoseBytes, sizeof(thoseBytes));
			}
		}
	}
	WriteDWORD(f, 0);
	fclose(f);

	Path[len - 5] = 'x';
	Path[len - 6] = 'e';
	Path[len - 7] = 't';

	f = fopen(Path, "wb");
	WriteDWORD(f, 1);

	DWORD numMats = 0;
	char imageName[MAX_PATH] = "";
	GetEnvironmentVariable("TEMP", imageName, MAX_PATH);
	DWORD pos = strlen(imageName);
	imageName[pos] = '\\';
	pos++;
	FILE* imageFile;
	for (DWORD i = 0; i < matList.GetNumMaterials(); i++)
	{
		Material* mat = matList.GetMaterial(i);
		char* name = mat->GetTexture(0)->GetName();

		if (!AlreadyExported(mat->GetTexture(0), &name))
		{
			alreadyExported.push_back(expo(mat->GetTexture(0), name));

			strcpy(&imageName[pos], name);
			strcat(imageName, ".tga");

			imageFile = fopen(imageName, "rb");
			if (imageFile)
				numMats++;
		}
	}
	WriteDWORD(f, numMats);

	alreadyExported.clear();
	TGAHEADER* tgaHeader;

	BYTE* imageData;
	BYTE* pFile;
	DWORD size;

	DWORD imageSize = 0;
	DWORD test = 0;

	BYTE data[] = { 0x17, 0xC6, 0xCF, 0x73, 0x02, 0xA9, 0xBD, 0x2D, 0xBA, 0xD6, 0x34, 0xAD, 0xAA, 0x55, 0xFF, 0xFF, 0x38, 0xCE, 0x54, 0xAD, 0x02, 0x55, 0xFD, 0xFF, 0x18, 0xC6, 0x34, 0xAD, 0x00, 0xDD, 0xAF, 0xAA, 0x38, 0xC6, 0x75, 0xB5, 0x00, 0x55, 0xDF, 0xFF, 0x38, 0xC6, 0x75, 0xB5, 0x00, 0x55, 0xFF, 0xFF, 0x38, 0xC6, 0x75, 0xB5, 0x00, 0x55, 0xFF, 0xFF, 0x38, 0xC6, 0x75, 0xB5, 0x00, 0x55, 0xFF, 0xFF, 0x38, 0xC6, 0x75, 0xB5, 0x00, 0x55, 0xFF, 0xFF, 0x38, 0xC6, 0x75, 0xB5, 0x00, 0x55, 0x5F, 0xFF, 0x38, 0xC6, 0x75, 0xB5, 0x00, 0x55, 0xFD, 0xFF, 0x38, 0xC6, 0x55, 0xAD, 0x08, 0x55, 0xFF, 0xFF, 0x38, 0xC6, 0x54, 0xAD, 0x00, 0x55, 0xFF, 0xFF, 0x9A, 0xD6, 0x54, 0xAD, 0xAA, 0x55, 0xFF, 0xFF, 0x38, 0xCE, 0x54, 0xAD, 0x02, 0x55, 0x77, 0xFF, 0x58, 0xCE, 0xAF, 0x73, 0x08, 0xA7, 0xB6, 0xFA, 0xB6, 0xB5, 0xB2, 0x9C, 0x29, 0x29, 0x09, 0x09, 0xD7, 0xBD, 0x75, 0xB5, 0xEA, 0xAA, 0xEA, 0xAA, 0xD7, 0xBD, 0x75, 0xB5, 0xFF, 0xBF, 0xFF, 0xFA, 0xD7, 0xBD, 0x95, 0xB5, 0xA9, 0xAA, 0xEA, 0xEA, 0xD7, 0xBD, 0x95, 0xB5, 0xEF, 0xFF, 0xFF, 0xFB, 0xD7, 0xBD, 0x96, 0xB5, 0x5F, 0xF5, 0xAF, 0xFE, 0xD7, 0xBD, 0x75, 0xB5, 0xAB, 0xAA, 0xFA, 0xAA, 0x96, 0xB5, 0xD6, 0xBD, 0xAA, 0xAA, 0xA0, 0xAA, 0xD7, 0xBD, 0x95, 0xB5, 0xFF, 0xFF, 0xFF, 0xFB, 0xD6, 0xBD, 0x96, 0xB5, 0xFF, 0xFF, 0xCF, 0xFF, 0xB6, 0xBD, 0x96, 0xB5, 0x03, 0xAA, 0xFE, 0xAA, 0xD7, 0xBD, 0x95, 0xB5, 0xFF, 0xFD, 0xBF, 0x95, 0xD7, 0xBD, 0x95, 0xB5, 0xBF, 0xAF, 0xFB, 0xFF, 0xD7, 0xBD, 0x96, 0xB5, 0xFE, 0xAA, 0xAA, 0xEB, 0xD7, 0xBD, 0x75, 0xB5, 0xFE, 0xFF, 0xF7, 0xBE, 0xF7, 0xC5, 0x34, 0xAD, 0x1E, 0x1A, 0x3A, 0x1E, 0xB6, 0xBD, 0xB3, 0x8C, 0x09, 0x55, 0x55, 0x55, 0xB9, 0xEE, 0xB0, 0x53, 0xAA, 0xFF, 0xFF, 0xFF, 0xB9, 0xEE, 0xB0, 0x53, 0xAA, 0xFF, 0xFF, 0xFF, 0x93, 0x84, 0xB9, 0xEE, 0xAA, 0x00, 0x00, 0x00, 0xD9, 0xEE, 0xB0, 0x53, 0xAA, 0xFF, 0xFF, 0xFF, 0xB9, 0xEE, 0xB0, 0x53, 0xAA, 0xFF, 0xFF, 0xFF, 0xB9, 0xEE, 0xB0, 0x53, 0xAA, 0xFF, 0xFF, 0xFF, 0xB9, 0xEE, 0xD0, 0x53, 0xAA, 0xFF, 0xFF, 0xFF, 0xDA, 0xEE, 0xB0, 0x53, 0xAA, 0xFF, 0xFF, 0xFF, 0xB3, 0x84, 0x99, 0xEE, 0xAA, 0x00, 0x00, 0x00, 0xB9, 0xEE, 0xD0, 0x53, 0xAA, 0xFF, 0xFF, 0xFF, 0x98, 0xE6, 0xD1, 0x53, 0xAA, 0xFF, 0xFF, 0xFF, 0xB9, 0xEE, 0xD0, 0x53, 0xAA, 0xFF, 0xFF, 0xFF, 0xDA, 0xEE, 0x8F, 0x53, 0xAA, 0xFF, 0xFF, 0xFF, 0x98, 0xE6, 0xD1, 0x53, 0xAA, 0xFF, 0xFF, 0xFF, 0xB6, 0xBD, 0xB3, 0x84, 0x28, 0x55, 0x55, 0x55, 0xB3, 0x84, 0x31, 0x7C, 0x00, 0x00, 0x02, 0x00, 0xF4, 0x8C, 0x92, 0x84, 0xEF, 0xBD, 0xAD, 0xAD, 0xD3, 0x8C, 0xB3, 0x84, 0x55, 0x59, 0x59, 0x5A, 0xD3, 0x8C, 0xB3, 0x84, 0x55, 0x55, 0xA5, 0x55, 0xD3, 0x8C, 0xB3, 0x84, 0x59, 0x59, 0x55, 0x55, 0xD3, 0x8C, 0xB3, 0x84, 0x65, 0x59, 0x55, 0x55, 0xD3, 0x8C, 0xB3, 0x84, 0x59, 0x55, 0x95, 0x55, 0xF4, 0x8C, 0x93, 0x84, 0xEB, 0xFE, 0xFF, 0x7F, 0xD3, 0x8C, 0xB3, 0x84, 0x59, 0x69, 0x59, 0x69, 0xD3, 0x8C, 0x52, 0x7C, 0x2B, 0x0F, 0x8B, 0xAF, 0xF4, 0x8C, 0x93, 0x84, 0xFE, 0xAF, 0xB7, 0xF7, 0xD3, 0x8C, 0xB3, 0x84, 0x59, 0x56, 0x56, 0x55, 0xD3, 0x8C, 0xB3, 0x84, 0x99, 0xA9, 0x95, 0x56, 0xD3, 0x8C, 0x52, 0x7C, 0xBC, 0xBE, 0xBC, 0x3E, 0xD3, 0x8C, 0x52, 0x7C, 0x3C, 0xBE, 0xBC, 0xBC, 0xF4, 0x8C, 0x92, 0x84, 0xF7, 0xF7, 0xB7, 0xBF, 0xF4, 0x8C, 0x93, 0x84, 0xFF, 0xFD, 0xFF, 0xEB, 0xF4, 0x8C, 0x92, 0x84, 0xED, 0xBD, 0xAD, 0xED, 0xB3, 0x84, 0x31, 0x7C, 0x00, 0x0A, 0x2A, 0xAA, 0xB3, 0x84, 0x31, 0x7C, 0x00, 0x2A, 0x2A, 0xAA, 0xD3, 0x8C, 0xB3, 0x84, 0x55, 0x56, 0x56, 0x55, 0xB3, 0x84, 0x31, 0x7C, 0x00, 0x00, 0x80, 0x00, 0xB3, 0x84, 0x31, 0x7C, 0x22, 0xAA, 0x0A, 0x8A, 0xB3, 0x84, 0x31, 0x7C, 0x80, 0x80, 0x80, 0xA0, 0xD3, 0x8C, 0x52, 0x7C, 0xA3, 0xEB, 0xC3, 0xAB, 0xD3, 0x8C, 0x52, 0x7C, 0xAF, 0x8F, 0x0B, 0x0F, 0xF4, 0x8C, 0x92, 0x84, 0xF7, 0xF7, 0xB7, 0xB7, 0xD3, 0x8C, 0x72, 0x7C, 0xAA, 0xE2, 0xA8, 0xEA, 0xD3, 0x8C, 0xB3, 0x84, 0x59, 0x65, 0x59, 0x55, 0xF4, 0x8C, 0x93, 0x84, 0xB7, 0xBF, 0xFF, 0xBB, 0xF4, 0x8C, 0x92, 0x84, 0x96, 0xD6, 0xDF, 0xFE, 0xF4, 0x8C, 0x93, 0x84, 0xBF, 0xBF, 0xBF, 0xF7, 0x17, 0xCE, 0xB3, 0x8C, 0x55, 0xA9, 0xA9, 0xA9, 0xB3, 0x84, 0xB9, 0xEE, 0x00, 0xAA, 0xAA, 0xAA, 0xB3, 0x84, 0xB9, 0xEE, 0x00, 0xAA, 0xAA, 0xAA, 0x93, 0x84, 0xB9, 0xEE, 0x00, 0xAA, 0xAA, 0xAA, 0xB6, 0xBD, 0xED, 0x22, 0xAA, 0x00, 0x00, 0x00, 0x78, 0xE6, 0xF1, 0x5B, 0xFF, 0xAA, 0xAA, 0xAA, 0x99, 0xEE, 0xD0, 0x53, 0xFF, 0xAA, 0xAA, 0xAA, 0xB9, 0xEE, 0xB0, 0x53, 0xFF, 0xAA, 0xAA, 0xAA, 0x99, 0xEE, 0xD0, 0x53, 0xFF, 0xAA, 0xAA, 0xAA, 0xB3, 0x84, 0xB9, 0xEE, 0x00, 0xAA, 0xAA, 0xAA, 0xB3, 0x84, 0xB9, 0xEE, 0x00, 0xAA, 0xAA, 0xAA, 0x99, 0xE6, 0xD0, 0x53, 0xFF, 0xAA, 0xAA, 0xAA, 0x17, 0xCE, 0xD3, 0x8C, 0x55, 0xAA, 0xAA, 0xAA, 0x99, 0xE6, 0xF0, 0x5B, 0xFF, 0xAA, 0xAA, 0xAA, 0x78, 0xE6, 0xF1, 0x5B, 0xFF, 0xAA, 0xAA, 0xAA, 0xB6, 0xBD, 0xB3, 0x8C, 0x55, 0x20, 0x20, 0x20, 0xB6, 0xB5, 0xB2, 0x9C, 0x01, 0x01, 0x0A, 0x09, 0xD7, 0xBD, 0x95, 0xB5, 0xAA, 0xAA, 0xAB, 0xEA, 0xB6, 0xBD, 0x96, 0xB5, 0xAA, 0xFA, 0x40, 0xFF, 0xD7, 0xBD, 0x95, 0xB5, 0xAA, 0xAB, 0xE9, 0xAA, 0xD7, 0xBD, 0x75, 0xB5, 0xAA, 0xAA, 0xFA, 0xAA, 0xD7, 0xBD, 0x75, 0xB5, 0xAE, 0xBF, 0xFF, 0xAF, 0xD7, 0xBD, 0x95, 0xB5, 0xBF, 0xAB, 0xAF, 0xFF, 0xD7, 0xBD, 0xB6, 0xB5, 0xEA, 0xFA, 0xFF, 0xAB, 0xB6, 0xBD, 0x96, 0xB5, 0xAA, 0xFE, 0x55, 0xFF, 0x96, 0xB5, 0xD6, 0xBD, 0xAA, 0xAA, 0xA8, 0xAA, 0xB6, 0xBD, 0x96, 0xB5, 0xAA, 0xFF, 0xAA, 0xF0, 0xB6, 0xBD, 0x96, 0xB5, 0xAA, 0x0F, 0xAA, 0xFF, 0xD7, 0xBD, 0x75, 0xB5, 0xAB, 0xBF, 0xAA, 0xAA, 0x17, 0xC6, 0x96, 0xB5, 0xFF, 0xFF, 0xED, 0xFF, 0xD6, 0xBD, 0x34, 0xAD, 0xAA, 0xBA, 0xAA, 0xAA, 0xD7, 0xBD, 0x34, 0xAD, 0x1A, 0x3A, 0x3A, 0x3A, 0xB6, 0xB5, 0xB2, 0x9C, 0x01, 0x09, 0x09, 0x02, 0x96, 0xB5, 0xD6, 0xBD, 0xAA, 0xAA, 0xA2, 0xAA, 0xD7, 0xBD, 0x75, 0xB5, 0xBE, 0xAF, 0xAB, 0xAF, 0xD7, 0xBD, 0x96, 0xB5, 0xAA, 0xFA, 0xFB, 0xFF, 0x96, 0xB5, 0xD6, 0xBD, 0xAA, 0x2A, 0x2A, 0xAA, 0x96, 0xB5, 0xD6, 0xBD, 0xA0, 0x80, 0xA0, 0xAA, 0xD7, 0xBD, 0x75, 0xB5, 0xAA, 0xEA, 0xAA, 0xAA, 0xD7, 0xBD, 0x75, 0xB5, 0xAA, 0xAA, 0xFA, 0xFE, 0xD7, 0xBD, 0x96, 0xB5, 0xFA, 0xFB, 0xBB, 0xDF, 0x17, 0xC6, 0x96, 0xB5, 0xFF, 0xBF, 0xFF, 0xFF, 0xB6, 0xBD, 0x96, 0xB5, 0xAA, 0xFF, 0xFF, 0x56, 0xD7, 0xBD, 0x75, 0xB5, 0xFA, 0xEB, 0xAB, 0xFA, 0x96, 0xB5, 0xD6, 0xBD, 0xAA, 0xAA, 0xAA, 0xA8, 0xD7, 0xBD, 0x95, 0xB5, 0xAA, 0x7A, 0xAA, 0xEA, 0xD6, 0xBD, 0x75, 0xB5, 0xFF, 0xFF, 0xFF, 0xBF, 0xF7, 0xBD, 0x75, 0xB5, 0x1F, 0x9F, 0x9F, 0x3F, 0xB6, 0xBD, 0xB2, 0x9C, 0x09, 0x09, 0x09, 0x01, 0xD7, 0xBD, 0x95, 0xB5, 0x6A, 0xAA, 0xEB, 0xEA, 0xD7, 0xBD, 0x75, 0xB5, 0xAF, 0xBE, 0xAA, 0xAF, 0xD7, 0xBD, 0x95, 0xB5, 0xFF, 0xFF, 0xFB, 0xFA, 0xD7, 0xBD, 0x95, 0xB5, 0xFB, 0x7B, 0x5F, 0xDF, 0xD6, 0xBD, 0x96, 0xB5, 0xFF, 0x95, 0x95, 0xBF, 0xB6, 0xB5, 0xB6, 0xBD, 0xAA, 0x6A, 0x5A, 0xAA, 0xD7, 0xBD, 0x95, 0xB5, 0xA6, 0xAA, 0xE0, 0xEA, 0xD7, 0xBD, 0x95, 0xB5, 0xFF, 0xFF, 0x2B, 0xAF, 0xD7, 0xBD, 0x95, 0xB5, 0xFF, 0xFF, 0xDF, 0xFE, 0xD7, 0xBD, 0x95, 0xB5, 0xAA, 0xAA, 0xAA, 0xAB, 0xD7, 0xBD, 0x95, 0xB5, 0xFF, 0xFE, 0xFF, 0xFF, 0xD7, 0xBD, 0x95, 0xB5, 0xFF, 0x97, 0x55, 0x7F, 0xD7, 0xBD, 0x75, 0xB5, 0xFA, 0xAA, 0xAA, 0xFF, 0xD6, 0xBD, 0x96, 0xB5, 0xBF, 0xAA, 0xF6, 0x55, 0xF7, 0xBD, 0x75, 0xB5, 0x1F, 0x9F, 0x17, 0x1F, 0xD6, 0xBD, 0xD2, 0x9C, 0x0B, 0x09, 0x29, 0x09, 0xD7, 0xBD, 0x95, 0xB5, 0xFE, 0xAA, 0xAF, 0xAA, 0xD7, 0xBD, 0x95, 0xB5, 0xBF, 0xF6, 0xBF, 0xAF, 0xD7, 0xBD, 0xB6, 0xB5, 0xFF, 0xBF, 0xAB, 0xFF, 0xD7, 0xBD, 0x96, 0xB5, 0xAA, 0xBA, 0xEA, 0x6A, 0xD7, 0xBD, 0x75, 0xB5, 0xAA, 0xAA, 0xAA, 0xAB, 0xD7, 0xBD, 0x95, 0xB5, 0xFA, 0x6A, 0x6A, 0xAA, 0xD7, 0xBD, 0x96, 0xB5, 0xAF, 0xA9, 0xFF, 0xFF, 0xD7, 0xBD, 0x95, 0xB5, 0xAA, 0xEA, 0x6A, 0xAA, 0xD7, 0xBD, 0x95, 0xB5, 0xD7, 0xBF, 0xB5, 0xB7, 0xD6, 0xBD, 0x96, 0xB5, 0xAA, 0xAA, 0xAA, 0x28, 0xD7, 0xBD, 0x95, 0xB5, 0xAA, 0xEA, 0xAA, 0xAA, 0xB6, 0xBD, 0x96, 0xB5, 0xAA, 0xAA, 0x03, 0xFF, 0xB6, 0xBD, 0x96, 0xB5, 0xAA, 0xAA, 0xAA, 0xFF, 0xD7, 0xBD, 0x95, 0xB5, 0xD5, 0xFF, 0xFF, 0x7B, 0xD7, 0xBD, 0x34, 0xAD, 0x3A, 0x1A, 0x3A, 0x3A, 0xB6, 0xB5, 0xB2, 0x9C, 0x01, 0x0A, 0x09, 0x09, 0x96, 0xB5, 0xD6, 0xBD, 0xA5, 0xAA, 0xAA, 0xAA, 0xD7, 0xBD, 0x95, 0xB5, 0xEB, 0xFF, 0xFF, 0xFF, 0x96, 0xB5, 0xD6, 0xBD, 0xAA, 0xAA, 0xA0, 0xAA, 0xD7, 0xBD, 0x95, 0xB5, 0xFB, 0xFF, 0xBF, 0xAA, 0xD7, 0xBD, 0x95, 0xB5, 0xFF, 0xFF, 0xA7, 0xBF, 0x96, 0xB5, 0xD6, 0xBD, 0xAA, 0xAA, 0xAA, 0xA9, 0xD7, 0xBD, 0x96, 0xB5, 0xEB, 0xEB, 0xAB, 0xAB, 0xD7, 0xBD, 0x95, 0xB5, 0xEB, 0xEB, 0xFE, 0xFE, 0xB6, 0xBD, 0x96, 0xB5, 0x96, 0x55, 0xBF, 0x56, 0x96, 0xB5, 0xD6, 0xBD, 0xAA, 0xAA, 0x5A, 0x9A, 0xD7, 0xBD, 0x95, 0xB5, 0xA5, 0xFF, 0xFF, 0xD5, 0xD7, 0xBD, 0x75, 0xB5, 0xDE, 0xFF, 0xFF, 0xFA, 0xD7, 0xBD, 0x95, 0xB5, 0x55, 0x95, 0xBF, 0x55, 0xD7, 0xBD, 0x75, 0xB5, 0xFB, 0xAA, 0xAA, 0xAA, 0xD7, 0xBD, 0x75, 0xB5, 0x1F, 0x1F, 0x1F, 0x1E, 0xB6, 0xBD, 0xB2, 0x9C, 0x09, 0x09, 0x01, 0x09, 0xD7, 0xBD, 0x96, 0xB5, 0xFF, 0xFE, 0xFE, 0xFE, 0xB6, 0xBD, 0x96, 0xB5, 0xAA, 0xA5, 0x65, 0xFF, 0xD7, 0xBD, 0x95, 0xB5, 0x9A, 0xAA, 0xAA, 0xA3, 0x96, 0xB5, 0xD6, 0xBD, 0x2A, 0xAA, 0xAA, 0xAA, 0xD7, 0xBD, 0x75, 0xB5, 0xFF, 0xEB, 0xAA, 0xAA, 0xD7, 0xBD, 0x95, 0xB5, 0xAB, 0xA9, 0xAA, 0xAA, 0x17, 0xC6, 0x96, 0xB5, 0xFF, 0xEF, 0xFB, 0xFF, 0xD7, 0xBD, 0x95, 0xB5, 0xAA, 0xEA, 0xAA, 0xAA, 0xF7, 0xBD, 0x95, 0xB5, 0x57, 0xFF, 0xFF, 0xAF, 0xD7, 0xBD, 0x95, 0xB5, 0xFF, 0xBF, 0xAF, 0xFE, 0xD7, 0xBD, 0x75, 0xB5, 0xBE, 0xBC, 0xAA, 0xAA, 0xD7, 0xBD, 0x75, 0xB5, 0xAA, 0xAA, 0xBE, 0xBF, 0xD7, 0xBD, 0x95, 0xB5, 0xFE, 0xFE, 0xFF, 0xBF, 0xB6, 0xBD, 0x96, 0xB5, 0xFF, 0xFF, 0xA6, 0x05, 0xF7, 0xBD, 0x75, 0xB5, 0x1F, 0x1F, 0x1F, 0x1E, 0xB2, 0x9C, 0xB6, 0xBD, 0x54, 0x56, 0x54, 0x56, 0x96, 0xB5, 0xD6, 0xBD, 0xA5, 0xAA, 0x15, 0xA9, 0xD7, 0xBD, 0x95, 0xB5, 0xD7, 0xFF, 0xFD, 0xBF, 0xD7, 0xBD, 0x95, 0xB5, 0xAA, 0xFF, 0xFF, 0xAA, 0xD7, 0xBD, 0x95, 0xB5, 0xEB, 0xAB, 0xAF, 0xFF, 0xD7, 0xBD, 0x95, 0xB5, 0xFF, 0xED, 0xFF, 0xD5, 0xD7, 0xBD, 0x95, 0xB5, 0xFF, 0xEF, 0xEB, 0xBF, 0x96, 0xB5, 0xD6, 0xBD, 0xAA, 0x56, 0x56, 0xAA, 0x96, 0xB5, 0xD6, 0xBD, 0xAA, 0xA9, 0xA9, 0xAA, 0x17, 0xC6, 0x96, 0xB5, 0xAF, 0xF5, 0xFF, 0xFF, 0xD7, 0xBD, 0x95, 0xB5, 0xFA, 0xAA, 0xFF, 0xFE, 0xD7, 0xBD, 0x95, 0xB5, 0xFF, 0xFF, 0x7B, 0xFF, 0xD7, 0xBD, 0x95, 0xB5, 0xAF, 0xFF, 0x55, 0xFF, 0xD7, 0xBD, 0x95, 0xB5, 0xBA, 0xFF, 0xF5, 0xFF, 0xD7, 0xBD, 0x75, 0xB5, 0xEA, 0xAA, 0xAA, 0xAB, 0xF7, 0xBD, 0x75, 0xB5, 0x1F, 0x1F, 0x3F, 0x9E, 0x18, 0xC6, 0xB2, 0x9C, 0xAB, 0xA9, 0xA9, 0xA9, 0xD7, 0xBD, 0x95, 0xB5, 0xAA, 0xAA, 0xED, 0xBF, 0xD7, 0xBD, 0x95, 0xB5, 0xAE, 0xAF, 0xB5, 0xFF, 0x96, 0xB5, 0xD6, 0xBD, 0xAA, 0xAA, 0xAA, 0x95, 0xD7, 0xBD, 0x75, 0xB5, 0xE0, 0xAA, 0xAF, 0xFE, 0xD7, 0xBD, 0x95, 0xB5, 0xF5, 0xFF, 0xFF, 0x2D, 0xD7, 0xBD, 0x95, 0xB5, 0xFF, 0xBF, 0xFF, 0xDE, 0xD7, 0xBD, 0x75, 0xB5, 0xBE, 0xAE, 0xAA, 0xAA, 0xD7, 0xBD, 0x75, 0xB5, 0xF8, 0xFA, 0xEA, 0x8A, 0x96, 0xB5, 0xD6, 0xBD, 0x80, 0x2A, 0xA8, 0xAA, 0xB6, 0xBD, 0x96, 0xB5, 0xF6, 0x55, 0x55, 0x6A, 0xB6, 0xBD, 0x96, 0xB5, 0xFF, 0x55, 0x95, 0x69, 0xB6, 0xBD, 0x96, 0xB5, 0xFF, 0x5A, 0x5A, 0x95, 0xD7, 0xBD, 0x95, 0xB5, 0xAF, 0xEF, 0xBF, 0xBF, 0xD7, 0xBD, 0x75, 0xB5, 0x02, 0xAA, 0xFE, 0xFE, 0xF7, 0xBD, 0x75, 0xB5, 0x9E, 0x1F, 0x1E, 0x1E, 0xB6, 0xB5, 0xB2, 0x9C, 0x01, 0x01, 0x09, 0x09, 0xD7, 0xBD, 0x95, 0xB5, 0xBF, 0xF7, 0xF7, 0xAF, 0xD7, 0xBD, 0x95, 0xB5, 0xFE, 0x57, 0xFF, 0xBF, 0xD7, 0xBD, 0x95, 0xB5, 0xFA, 0xBA, 0xBF, 0xBE, 0xD7, 0xBD, 0x75, 0xB5, 0x6A, 0xEA, 0xFA, 0xFA, 0xD7, 0xBD, 0x95, 0xB5, 0x29, 0xA9, 0xEF, 0xF5, 0xD7, 0xBD, 0x75, 0xB5, 0xFA, 0xFA, 0xAF, 0xAE, 0xD7, 0xBD, 0x95, 0xB5, 0xFA, 0xEB, 0xBF, 0xAA, 0xD7, 0xBD, 0x95, 0xB5, 0xAF, 0xAB, 0x02, 0x80, 0xD7, 0xBD, 0x95, 0xB5, 0xFF, 0xFA, 0xFE, 0xAA, 0xD7, 0xBD, 0x95, 0xB5, 0xFF, 0xFF, 0x7F, 0xFA, 0xB6, 0xBD, 0x96, 0xB5, 0xAA, 0xCF, 0xA9, 0xFF, 0xD7, 0xBD, 0x95, 0xB5, 0xBF, 0xAB, 0xAF, 0x9E, 0x96, 0xB5, 0xD6, 0xBD, 0xAA, 0xAA, 0x2A, 0x2A, 0xD7, 0xBD, 0x75, 0xB5, 0xFE, 0xAF, 0xAF, 0xBF, 0xF7, 0xBD, 0x34, 0xAD, 0x3A, 0x1A, 0x3A, 0x1A, 0x95, 0xB5, 0x71, 0x84, 0x03, 0x0B, 0xAB, 0xFD, 0xB6, 0xBD, 0xB2, 0x9C, 0x00, 0x82, 0xAA, 0x55, 0xB6, 0xB5, 0xB2, 0x9C, 0x00, 0x02, 0x0A, 0x55, 0xB6, 0xB5, 0xB2, 0x9C, 0x80, 0x00, 0x28, 0x55, 0x96, 0xB5, 0xB2, 0x9C, 0x00, 0x00, 0xA0, 0x55, 0x1B, 0xE7, 0xB2, 0x9C, 0xFF, 0xFF, 0xFF, 0x55, 0xB6, 0xB5, 0xB2, 0x9C, 0x00, 0x02, 0x00, 0x55, 0xB2, 0x9C, 0x9A, 0xD6, 0xAA, 0xAA, 0xAA, 0x00, 0xB6, 0xB5, 0xB2, 0x9C, 0x00, 0xA0, 0x80, 0x55, 0xB6, 0xB5, 0xB2, 0x9C, 0x00, 0x02, 0xAA, 0x55, 0xB6, 0xB5, 0xB2, 0x9C, 0x00, 0x00, 0x08, 0x55, 0x99, 0xD6, 0xCF, 0x7B, 0xAA, 0xAA, 0xAA, 0xFF, 0x99, 0xD6, 0xCF, 0x7B, 0xAA, 0xAA, 0xAA, 0xFF, 0xB6, 0xB5, 0xB2, 0x9C, 0x00, 0x80, 0xA8, 0x55, 0xB6, 0xB5, 0xB2, 0x9C, 0x00, 0xA2, 0xAA, 0x55, 0xD6, 0xBD, 0xD2, 0x9C, 0x38, 0x3E, 0x3E, 0xD5 };

	for (DWORD i = 0; i < matList.GetNumMaterials(); i++)
	{
		Material* mat = matList.GetMaterial(i);
		char* name = mat->GetTexture(0)->GetName();
		if (!AlreadyExported(mat->GetTexture(0), &name))
		{
			alreadyExported.push_back(expo(mat->GetTexture(0), name));

			strcpy(&imageName[pos], name);
			strcat(imageName, ".tga");

			imageFile = fopen(imageName, "rb");
			if (imageFile)
			{
				test++;
				WriteDWORD(f, Checksum(name).checksum);
				fseek(imageFile, 0, SEEK_END);

				size = ftell(imageFile);

				pFile = new BYTE[size];

				fseek(imageFile, 0, SEEK_SET);
				fread(pFile, size, 1, imageFile);
				fclose(imageFile);

				tgaHeader = (TGAHEADER*)pFile;

				imageData = (BYTE*)((DWORD)pFile + sizeof(TGAHEADER));
				imageSize = tgaHeader->width * tgaHeader->height * 4;
				WriteDWORD(f, (DWORD)tgaHeader->width);
				WriteDWORD(f, (DWORD)tgaHeader->height);
				WriteDWORD(f, 1);
				WriteDWORD(f, 0x20, 2);
				WriteDWORD(f, 0, 2);
				WriteDWORD(f, imageSize);
				WriteData(f, imageData, imageSize);
			}
		}
	}
	if (test != numMats)
		MessageBox(0, "WTF?", "", 0);
	alreadyExported.clear();
	fclose(f);

	Path[len - 5] = 'l';
	Path[len - 6] = 'o';
	Path[len - 7] = 'c';

	f = fopen(Path, "wb");
	WriteDWORD(f, 10);
	numMeshes = 0;
	DWORD numFaces = 0;
	DWORD numVerts = 0;
	for (DWORD i = 0; i < GetNumMeshes(); i++)
	{
		Mesh* mesh = GetMesh(i);
		Collision* col = mesh->GetCollision();
		if (!mesh->IsDeleted() && col && col->GetNumVertices() && col->GetNumVertices())
		{
			numMeshes++;
			numFaces += col->GetNumFaces();
			numVerts += col->GetNumVertices();
		}
	}
	WriteDWORD(f, numMeshes);
	WriteDWORD(f, numVerts);
	WriteDWORD(f, numFaces);
	WriteDWORD(f, 0);
	WriteDWORD(f, numVerts);
	WriteDWORD(f, 0);
	WriteDWORD(f, 0);

	numFaces = 0;
	numVerts = 0;
	numMeshes = 0;

	for (DWORD i = 0; i < GetNumMeshes(); i++)
	{
		Mesh* mesh = GetMesh(i);
		Collision* col = mesh->GetCollision();
		if (!mesh->IsDeleted() && col && col->GetNumVertices())
		{
			WriteDWORD(f, mesh->GetName().checksum);
			WriteWORD(f, 0); // _flags
			WriteWORD(f, col->GetNumVertices());
			WriteWORD(f, col->GetNumFaces());

			WriteWORD(f, 0);

			WriteDWORD(f, numFaces);
			numFaces += 10 * col->GetNumFaces();

			BBox bbox = mesh->GetBBox();
			bbox.Max.z *= -1.0f;
			bbox.Min.z *= -1.0f;
			std::swap(bbox.Max.z, bbox.Min.z);

			WriteData(f, (BYTE*)&bbox.Min, 12);
			float whole = 1.0f;
			WriteDWORD(f, *(DWORD*)&whole);
			WriteData(f, (BYTE*)&bbox.Max, 12);
			WriteDWORD(f, *(DWORD*)&whole);

			WriteDWORD(f, numVerts * 12);

			WriteDWORD(f, numMeshes);
			numMeshes += 8;

			WriteDWORD(f, numVerts);
			numVerts += col->GetNumVertices();
			WriteDWORD(f, 0);
		}
	}
	for (DWORD i = 0; i < GetNumMeshes(); i++)
	{
		Mesh* mesh = GetMesh(i);
		Collision* col = mesh->GetCollision();
		if (!mesh->IsDeleted() && col && col->GetNumVertices())
		{
			for (DWORD j = 0; j < col->GetNumVertices(); j++)
			{
				WriteData(f, (BYTE*)col->GetVertex(j), 8);
				float invZ = col->GetVertex(j)->z * -1.0f;
				WriteDWORD(f, *(DWORD*)&invZ);
			}
		}
	}
	for (DWORD i = 0; i < GetNumMeshes(); i++)
	{
		Mesh* mesh = GetMesh(i);
		Collision* col = mesh->GetCollision();
		if (!mesh->IsDeleted() && col && col->GetNumVertices())
		{
			for (DWORD j = 0; j < col->GetNumVertices(); j++)
			{
				fputc(0x80, f);
			}
		}
	}
	int rem = ((int)numVerts) % 4;
	if (rem != 0)
	{
		for (int i = 0; i < (4 - rem); i++)
		{
			fputc(0x69, f);
		}
	}
	for (DWORD i = 0; i < GetNumMeshes(); i++)
	{
		Mesh* mesh = GetMesh(i);
		Collision* col = mesh->GetCollision();
		if (!mesh->IsDeleted() && col && col->GetNumVertices())
		{
			for (DWORD j = 0; j < col->GetNumFaces(); j++)
			{
				WORD flags = col->GetFaceInfo(j)->flags;
				//flags &= ~0x40;
				if (flags & 0x4)
					flags &= ~0x1;
				WriteWORD(f, flags);
				WriteWORD(f, col->GetFaceInfo(j)->sound);
				WriteData(f, col->GetFace(j), 6);
			}
		}
	}
	for (DWORD i = 0; i < GetNumMeshes(); i++)
	{
		Mesh* mesh = GetMesh(i);
		Collision* col = mesh->GetCollision();
		if (!mesh->IsDeleted() && col && col->GetNumVertices())
		{
			for (DWORD j = 0; j < col->GetNumFaces(); j++)
				fputc(0, f);
		}
	}

	rem = ((int)ftell(f)) % 4;
	if (rem != 0)
	{
		for (int i = 0; i < (4 - rem); i++)
		{
			fputc(0x69, f);
		}
	}

	WriteDWORD(f, numMeshes);
	numFaces = 0;

	for (DWORD i = 0; i < GetNumMeshes(); i++)
	{
		Mesh* mesh = GetMesh(i);
		Collision* col = mesh->GetCollision();
		if (!mesh->IsDeleted() && col && col->GetNumVertices())
		{
			WriteWORD(f, 0x3);
			WriteWORD(f, col->GetNumFaces() * 3);
			WriteDWORD(f, numFaces);
			numFaces += col->GetNumFaces() * 3;
		}
	}

	for (DWORD i = 0; i < GetNumMeshes(); i++)
	{
		Mesh* mesh = GetMesh(i);
		Collision* col = mesh->GetCollision();
		if (!mesh->IsDeleted() && col && col->GetNumVertices())
		{
			for (DWORD j = 0; j < col->GetNumFaces(); j++)
			{
				WriteWORD(f, col->GetFace(j)->a);
				WriteWORD(f, col->GetFace(j)->b);
				WriteWORD(f, col->GetFace(j)->c);
			}
		}
	}
	fclose(f);

	Path[len - 5] = '\0';
	Path[len - 6] = 'b';
	Path[len - 7] = 'q';

	f = fopen(Path, "wb");
	if (f)
	{
		WriteQBKey("Test_NodeArray", f);
		WriteBYTE(f, 0x7);//WriteWORD(f, 0x207);
		WriteBYTE(f, 0x1);
		WriteBYTE(f, 0x5);
		WriteBYTE(f, 0x1);

		DWORD numNodes = nodes.size(), numFucked = 0;

		DWORD divider = 1;
		if (numNodes > 100)
			DWORD divider = numNodes / 100;
		bool e = false;

		for (DWORD i = 0; i < numNodes; i++)
		{
			//SetProgressbarValue(i / divider);
			WriteBYTE(f, 0x3);
			WriteBYTE(f, 0x1);
			WORD furthestLink = 0xFFFF;
			//if (nodes[i].Position.x || nodes[i].Position.y || nodes[i].Position.z)
			WriteQBKey("Position", &nodes[i].Position, f);
			// WriteBYTE(f, 0x1);
		   //if (nodes[i].Angles.x || nodes[i].Angles.y || nodes[i].Angles.z)
			WriteQBKey("Angles", &nodes[i].Angles, f);
			//WriteBYTE(f, 0x1);
			if (e == false && nodes[i].Class.checksum == Restart::GetClass())
			{
				WriteQBKey("Name", Checksum("TRG_PlayerRestart"), f);
				e = true;
			}
			else
				WriteQBKey("Name", nodes[i].Name, f);
			// WriteBYTE(f, 0x1);
			if (nodes[i].Class.checksum == EnvironmentObject::GetClass())
			{
				nodes[i].Class = Checksum("LevelGeometry");
			}
			//else
			WriteQBKey("Class", nodes[i].Class, f);
			if (nodes[i].TrickObject)
			{
				bool found = false;
				for (DWORD j = 0; j < GetNumMeshes(); j++)
				{
					if (nodes[i].Name.checksum == GetMesh(j)->GetName().checksum)
					{
						found = true;
						if (GetMesh(j)->IsDeleted() || GetMesh(j)->GetNumVertices() == 0)
							MessageBox(0, "??!", "", 0);
						break;
					}
				}
				if (found)
					WriteQBKey("TrickObject", f);
			}
			if (nodes[i].Cluster.checksum)
				WriteQBKey("Cluster", nodes[i].Cluster, f);
			//WriteQBKey("NodeNum", i, f);
			if (nodes[i].TerrainType.checksum)
				WriteQBKey("TerrainType", Checksum("TERRAIN_DEFAULT"), f);
			if (nodes[i].Class.checksum == 0x1806DDF8)//Restart
			{
				Checksum types[] = { Checksum("MultiPlayer"), Checksum("Player1"), Checksum("Horse") };
				WriteQBKey("RestartName", nodes[i].RestartName.GetString(), f);
				WriteQBKey("Type", Checksum("UserDefined"), f);
				if (nodes[i].Name.checksum == 0xF1EFFD1B)//team1
					WriteArray("restart_types", 1, &Checksum("CTF_1"), f);
				else if (nodes[i].Name.checksum == 0x68E6ACA1)//team2
					WriteArray("restart_types", 1, &Checksum("CTF_2"), f);
				else
					WriteArray("restart_types", 3, types, f);

				if (nodes[i].Trigger.checksum)
					WriteQBKey("TriggerScript", nodes[i].Trigger, f);
				else
					WriteQBKey("TriggerScript", Checksum("TRG_SpawnSkater"), f);
			}

			else if (nodes[i].Trigger.checksum)
			{
				WriteQBKey("TriggerScript", nodes[i].Trigger, f);
				/*if (nodes[i].NetEnabled)
				  WriteQBKey("NetEnabled", f);
				if (nodes[i].Permanent)
				  WriteQBKey("Permanent", f);*/
			}
			else
			{
				if (nodes[i].Type.checksum)
					WriteQBKey("Type", nodes[i].Type, f);
				if (nodes[i].Class.checksum == RailNode::GetClass())
				{
					float furthestDist = 0.0f;
					for (DWORD j = 0, numLinks = nodes[i].Links.size(); j < numLinks; j++)
					{
						float distance = nodes[i].Position.DistanceTo(nodes[nodes[i].Links[j]].Position);

						if (distance > furthestDist || furthestLink == 0xFFFF)
						{
							furthestLink = nodes[i].Links[j];
							furthestDist = distance;
						}
					}
				}
			}

			if (nodes[i].CreatedAtStart)
				WriteQBKey("CreatedAtStart", f);
			if (nodes[i].AbsentInNetGames)
				WriteQBKey("AbsentInNetGames", f);
			if (nodes[i].Links.size())
			{
				if (furthestLink == 0xFFFF)
					WriteArray("Links", nodes[i].Links.size(), &nodes[i].Links.front(), f);
				else
					WriteArray("Links", 1, &furthestLink, f);
			}

			WriteBYTE(f, 0x4);
			WriteBYTE(f, 0x1);
		}
		WriteBYTE(f, 0x6);
		fputc(2, f);
		WriteDWORD(f, 1);

		fputc(0x23, f);
		WriteQBKey("LoadTerrain", f, true);
		WriteQBKey("LoadSound", f);
		fputc(0x1B, f);
		WriteDWORD(f, 19);
		WriteData(f, "Shared\\glasspane2x", 19);
		fputc(2, f);
		WriteDWORD(f, 1);
		WriteQBKey("SetTerrainDefault", f, true);
		WriteQBKey("SetTerrainAsphalt", f, true);
		WriteQBKey("SetTerrainConcSmooth", f, true);
		WriteQBKey("SetTerrainConcRough", f, true);
		WriteQBKey("SetTerrainMetalSmooth", f, true);
		WriteQBKey("SetTerrainMetalRough", f, true);
		WriteQBKey("SetTerrainMetalCorrugated", f, true);
		WriteQBKey("SetTerrainMetalGrating", f, true);
		WriteQBKey("SetTerrainMetalTin", f, true);
		WriteQBKey("SetTerrainWood", f, true);
		WriteQBKey("SetTerrainWoodMasonite", f, true);
		WriteQBKey("SetTerrainWoodPlywood", f, true);
		WriteQBKey("SetTerrainWoodShingle", f, true);
		WriteQBKey("SetTerrainWoodPier", f, true);
		WriteQBKey("SetTerrainWoodFlimsy", f, true);
		WriteQBKey("SetTerrainBrick", f, true);
		WriteQBKey("SetTerrainTile", f, true);
		WriteQBKey("SetTerrainRock", f, true);
		WriteQBKey("SetTerrainGravel", f, true);
		WriteQBKey("SetTerrainGrass", f, true);
		WriteQBKey("SetTerrainGrassDried", f, true);
		WriteQBKey("SetTerrainDirt", f, true);
		WriteQBKey("SetTerrainDirtPacked", f, true);
		WriteQBKey("SetTerrainSand", f, true);
		WriteQBKey("SetTerrainCarpet", f, true);
		WriteQBKey("SetTerrainChainlink", f, true);
		WriteQBKey("SetTerrainWetConc", f, true);
		WriteQBKey("SetTerrainPlexiglass", f, true);
		WriteQBKey("SetTerrainWater", f, true);
		WriteQBKey("SetTerrainMetalFuture", f, true);
		WriteQBKey("SetTerrainSnow", f, true);
		WriteQBKey("SetTerrainFiberglass", f, true);
		WriteQBKey("SetTerrainIce", f, true);
		WriteQBKey("SetTerrainGeneric1", f, true);
		WriteQBKey("SetTerrainGeneric2", f, true);
		WriteQBKey("SetTerrainGrindTrain", f, true);
		WriteQBKey("SetTerrainGrindRope", f, true);
		WriteQBKey("SetTerrainSidewalk", f, true);
		WriteQBKey("SetTerrainWetConc", f, true);
		WriteQBKey("SetTerrainMetalFence", f, true);
		WriteQBKey("SetTerrainConveyor", f, true);

		if (GetScript("SetTh2Physics"))
			WriteQBKey("SetTh2Physics", f, true);
		else if (GetScript("LoadSounds"))
			WriteQBKey("LoadSounds", f, true);

		fputc(0x24, f);
		fputc(0x2, f);
		WriteDWORD(f, 1);

		fputc(0x23, f);
		WriteQBKey("TRG_SpawnSkater", f, true);

		WriteQBKey("MakeSkaterGoto", f);
		WriteQBKey("StartSkating1", f, true);

		fputc(0x24, f);
		fputc(0x2, f);
		WriteDWORD(f, 1);

		for (DWORD i = 0; i < globals.size(); i++)
		{
			fputc(0x16, f);
			WriteDWORD(f, globals[i].checksum);
			WriteData(f, (void*)init, sizeof(init));
		}

		/*for (DWORD i = 1; i<triggers.size(); i++)//skip first(LoadTerrain)
		{
		  DWORD size;
		  /*const void* scriptData = GetScript(triggers[i].checksum, &size);

		  fputc(0x23, f);
		  WriteQBKey(triggers[i].checksum, f, true);
		  DWORD flags = Checksum("flags").checksum;
		  if (scriptData)
		  {
			BYTE* data = (BYTE*)scriptData;
			for (DWORD j = 0; j < size - 11; j++)
			{
			  if (*(DWORD*)&data[j] == flags)
			  {

				for (DWORD k = 0; k < KnownScripts.size(); k++)
				{
				  if (triggers[i].checksum == KnownScripts[k].name.checksum)
				  {
					j += 11;
					while (data[j] == 0x16 && j<KnownScripts[k].size - 8)
					{
					  MessageBox(0, KnownScripts[k].name.GetString(), KnownScripts[k].name.GetString(), 0);
					  KnownScripts[k].InsertByte(j, 0x1);
					  size++;
					  MessageBox(0, "", "", 0);
					  j += 6;
					}
					break;
				  }
				}
			  }
			}*/
			/*KnownScript* script = GetScript(triggers[i].checksum);
			if (script && script->size)
			{
			  BYTE* data = script->func;
			  for (DWORD j = 0; j < script->size - 11; j++)
			  {
				if (*(DWORD*)&data[j] == Checksum("flags").checksum)
				{
				  j += 11;
				  while (data[j] == 0x16 && j<script->size - 8)
				  {
					MessageBox(0, script->name.GetString(), script->name.GetString(), 0);
					script->InsertByte(j, 0x1);
					j += 6;
				  }
				}
			  }
			  WriteData(f, script->func, script->size);
			}
			fputc(0x24, f);
			fputc(0x2, f);
			WriteDWORD(f, 1);
		  }*/
		bool gap = false;
		DWORD flags = Checksum("flags").checksum;
		for (DWORD i = 0; i < KnownScripts.size(); i++)
		{
			/*for (DWORD j = 0; j < triggers.size(); j++)
			{
			  if (triggers[j].checksum == KnownScripts[i].name.checksum)
				goto Next2;
			}*/
			fputc(0x23, f);
			WriteQBKey(KnownScripts[i].name.checksum, f, true);
			//MessageBox(0, KnownScripts[i].name.GetString(), KnownScripts[i].name.GetString(), 0);
			if (KnownScripts[i].size > 11)
			{
				//MessageBox(0, KnownScripts[i].name.GetString(), KnownScripts[i].name.GetString(), 0);
				BYTE* data = KnownScripts[i].func;
				//MessageBox(0, KnownScripts[i].name.GetString(), KnownScripts[i].name.GetString(), 0);
				for (DWORD j = 0; j < KnownScripts[i].size - 11; j++)
				{
					//MessageBox(0, KnownScripts[i].name.GetString(), KnownScripts[i].name.GetString(), 0);
					if (*(DWORD*)(data + j) == flags)
					{
						gap = true;
						//MessageBox(0, KnownScripts[i].name.GetString(), KnownScripts[i].name.GetString(), 0);
						j += 11;
						while (data[j] == 0x16 && j < KnownScripts[i].size - 8)
						{
							//MessageBox(0, KnownScripts[i].name.GetString(), KnownScripts[i].name.GetString(), 0);
							KnownScripts[i].InsertByte(j, 0x1);
							j += 6;
						}
					}
				}
				if (!gap)
					WriteData(f, KnownScripts[i].func, KnownScripts[i].size);
			}
			else if (KnownScripts[i].size)
				WriteData(f, KnownScripts[i].func, KnownScripts[i].size);
			fputc(0x24, f);
			fputc(0x2, f);
			WriteDWORD(f, 1);
		Next2:;
		}

		const static DWORD checksums[] = {
		  Checksum("Position", false).checksum, Checksum("Angles", false).checksum, Checksum("Name", false).checksum, Checksum("ClassSetTerrain", false).checksum, Checksum("CreatedAtStart", false).checksum,
		  Checksum("Class", false).checksum, Checksum("Type", false).checksum, Checksum("TriggerScript", false).checksum, Checksum("Links", false).checksum, Checksum("TrickObject", false).checksum, Checksum("Cluster", false).checksum,
		  Checksum("Permanent", false).checksum, Checksum("NetEnabled", false).checksum, Checksum("RestartName", false).checksum, Checksum("UserDefined", false).checksum,
		  Checksum("Restart_Types", false).checksum, Checksum("Multiplayer", false).checksum, Checksum("Zone", false).checksum, Checksum("Crown", false).checksum, Checksum("CTF_1", false).checksum,
		  Checksum("CTF_2", false).checksum, Checksum("AbsentInNetGames", false).checksum, Checksum("ZONE_KEY", false).checksum, Checksum("TerrainType", false).checksum, Checksum("SetTerrainAsphalt", false).checksum,
		  Checksum("SetTerrainConcSmooth", false).checksum, Checksum("SetTerrainDirtPacked", false).checksum, Checksum("SetTerrainCarpet", false).checksum, Checksum("SetTerrainWetConc", false).checksum,
		  Checksum("SetTerrainMetalRough", false).checksum, Checksum("SetTerrainMetalCorrugated", false).checksum, Checksum("SetTerrainMetalGrating", false).checksum, Checksum("TriggerScripts", false).checksum,
		  Checksum("SetTerrainMetalTin", false).checksum, Checksum("SetTerrainWood", false).checksum, Checksum("SetTerrainWoodMasonite", false).checksum, Checksum("SetTerrainWoodPier", false).checksum,
		  Checksum("SetTerrainWoodPlywood", false).checksum, Checksum("SetTerrainWoodShingle", false).checksum, Checksum("SetTerrainWoodFlimsy", false).checksum, Checksum("SetTerrainBrick", false).checksum,
		  Checksum("SetTerrainTile", false).checksum, Checksum("SetTerrainRock", false).checksum, Checksum("SetTerrainGravel", false).checksum, Checksum("SetTerrainGrass", false).checksum,
		  Checksum("SetTerrainGrassDried", false).checksum, Checksum("SetTerrainDirt", false).checksum, Checksum("SetTerrainSand", false).checksum, Checksum("SetTerrainChainlink", false).checksum,
		  Checksum("SetTerrainPlexiglass", false).checksum, Checksum("SetTerrainMetalFuture", false).checksum, Checksum("SetTerrainConveyor", false).checksum, Checksum("SetTerrainFiberglass", false).checksum,
		  Checksum("SetTerrainGeneric1", false).checksum, Checksum("SetTerrainSidewalk", false).checksum, Checksum("SetTerrainWetConc", false).checksum, Checksum("LoadSounds", false).checksum,
		  Checksum("MakeSkaterGoto", false).checksum, Checksum("SetTerrainDefault", false).checksum, Checksum("SetTerrainMetalSmooth", false).checksum, Checksum("LoadSound", false).checksum,
		  Checksum("SetTerrainConcRough", false).checksum, Checksum("SetTerrainWater", false).checksum, Checksum("SetTerrainSnow", false).checksum, Checksum("SetTerrainIce", false).checksum,
		  Checksum("SetTerrainGeneric2", false).checksum, Checksum("SetTerrainGrindTrain", false).checksum, Checksum("SetTerrainGrindRope", false).checksum, Checksum("StartSkating1", false).checksum,
		  Checksum("SetTerrainMetalFence", false).checksum, Checksum("TRG_SpawnSkater", false).checksum, Checksum("NodeArray", false).checksum
		};



		for (DWORD i = 0, numScripts = Scripts.size(); i < numScripts; i++)
		{
			if (Scripts[i].checksum == 0x31ADBD6E || Scripts[i].checksum == 0x6EBDAD31)
				MessageBox(0, "YEAE", 0, 0);
			for (DWORD j = 0; j < globals.size(); j++)
			{
				if (Scripts[i].checksum == globals[j].checksum)
				{
					fputc(0x2B, f);
					WriteDWORD(f, Scripts[i].checksum);
					fwrite(Scripts[i].name, strlen(Scripts[i].name) + 1, 1, f);
					goto Next;
				}
			}
			for (DWORD j = 0, numChecksums = (sizeof(checksums) / sizeof(Checksum)); j < numChecksums; j++)
			{
				if (Scripts[i].checksum == checksums[j])
				{
					fputc(0x2B, f);
					WriteDWORD(f, Scripts[i].checksum);
					fwrite(Scripts[i].name, strlen(Scripts[i].name) + 1, 1, f);
					goto Next;
				}
			}
			for (DWORD j = 0; j < triggers.size(); j++)
			{
				if (Scripts[i].checksum == triggers[j].checksum)
				{
					fputc(0x2B, f);
					WriteDWORD(f, Scripts[i].checksum);
					fwrite(Scripts[i].name, strlen(Scripts[i].name) + 1, 1, f);
					goto Next;
				}
			}
			for (DWORD j = 0; j < nodes.size(); j++)
			{
				if (nodes[j].Name.checksum == Scripts[i].checksum || nodes[j].Class.checksum == Scripts[i].checksum
					|| nodes[j].Type.checksum == Scripts[i].checksum || nodes[j].Cluster.checksum == Scripts[i].checksum
					|| nodes[j].Trigger.checksum == Scripts[i].checksum)
				{
					fputc(0x2B, f);
					WriteDWORD(f, Scripts[i].checksum);
					fwrite(Scripts[i].name, strlen(Scripts[i].name) + 1, 1, f);
					goto Next;
				}
			}
			for (DWORD j = 0; j < KnownScripts.size(); j++)
			{
				if (Scripts[i].checksum == KnownScripts[j].name.checksum)
				{
					fputc(0x2B, f);
					WriteDWORD(f, Scripts[i].checksum);
					fwrite(Scripts[i].name, strlen(Scripts[i].name) + 1, 1, f);
					goto Next;
				}
				BYTE* pFile = KnownScripts[j].func;
				BYTE* max = pFile + KnownScripts[j].size;
				while (pFile < max)
				{
					const BYTE opcode = *pFile;
					pFile++;
					switch (opcode)
					{
					case 0x1C:
					case 0x1B:
						pFile += *(DWORD*)pFile + 4;
						break;
					case 0x16:
						if (Scripts[i].checksum == *(DWORD*)pFile)
						{
							fputc(0x2B, f);
							WriteDWORD(f, Scripts[i].checksum);
							fwrite(Scripts[i].name, strlen(Scripts[i].name) + 1, 1, f);
							goto Next;
						}
						pFile += 4;
						break;
					case 0x17:
					case 0x1A:
					case 0x2E:
					case 2:
						pFile += 4;
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
					}
				}
			}
		Next:;
		}
		fputc(0, f);
		fclose(f);
	}
}

void Scene::ExportObj(const char* const __restrict path)
{
	char* levelName = GetLevelName();
	DWORD pathLen = strlen(path);
	DWORD levelLen = strlen(levelName);
	char* filename = new char[pathLen + 5 + levelLen];
	memcpy(filename, path, pathLen);
	for (DWORD i = pathLen, j = 0; j <= levelLen; j++, i++)
	{
		filename[i] = levelName[j];
	}

	strcat(filename, ".obj");

	MessageBox(0, filename, filename, MB_OK);

	FILE* f = fopen(filename, "wt+");
	fprintf(f, "mtllib %s.mtl\n", levelName);
	DWORD vcount = 1;

	for (DWORD i = 0, numMeshes = meshes.size(); i < numMeshes; i++)
	{
		DWORD numVertices = meshes[i].GetNumVertices();
		if (numVertices == 0 || meshes[i].matSplits.size() == 0)
			continue;
		fprintf(f, "g %s\n", meshes[i].GetName().GetString2());
		fprintf(f, "o %s\n", meshes[i].GetName().GetString2());

		const Vertex* vertices = meshes[i].GetVertices();

		for (DWORD j = 0; j < numVertices; j++)
		{
			fprintf(f, "v %.5f %.5f %.5f\n", vertices[j].x, vertices[j].y, vertices[j].z * -1);
			fprintf(f, "vt %.5f %.5f\n", vertices[j].tUV[0].u, vertices[j].tUV[0].v);
		}


		const MaterialSplit* matSplits = &meshes[i].matSplits.front();



		for (DWORD j = 0, numSplits = meshes[i].GetNumSplits(); j < numSplits; j++)
		{
			fprintf(f, "usemtl Material%d\n", matSplits[j].matId);
			WORD v1 = 0, v2 = 0, v3 = 0;
			bool tmp = false;
			for (DWORD k = 0, numIndices = matSplits[j].numIndices; k < numIndices; k++)
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


	delete[] filename;
	delete[] levelName;
	fclose(f);

}
struct MeshDraw
{
	Mesh mesh;
	float drawOrder;
};

void Scene::sort()
{
	/*DWORD opaqueCount = 0;
	for (DWORD i = 0; i<meshes.size(); i++)
	{
	  if (!meshes[i].IsTransparent())
		opaqueCount++;
	}

	DWORD alphaIndex = opaqueCount;
	opaqueCount = 0;
	Mesh* meshes = (Mesh*)malloc(this->meshes.size()*sizeof(Mesh));
	memcpy(meshes, &this->meshes.front(), this->meshes.size()*sizeof(Mesh));
	for (DWORD i = 0; i<this->meshes.size(); i++)
	{
	  if (this->meshes[i].IsTransparent())
	  {
		meshes[alphaIndex].drawOrder = this->meshes[i].GetDrawOrder();
		//meshes[alphaIndex].mesh = std::move(this->meshes[i]);
		meshes[alphaIndex] = /*std::move(this->meshes[i]);

		alphaIndex++;
	  }
	  else
	  {
		meshes[opaqueCount].drawOrder = this->meshes[i].GetDrawOrder();
		//meshes[opaqueCount].mesh = std::move(this->meshes[i]);
		meshes[opaqueCount] = /*std::move(this->meshes[i]);

		opaqueCount++;
	  }
	}

	//this->meshes.clear();
	for (DWORD i = 0; i < this->meshes.size(); i++)
	{
	  this->meshes[i] = /*std::move(meshes[i]);
	}*/

	std::sort(this->meshes.begin(), this->meshes.end());
	for (DWORD i = 0; i < this->meshes.size(); i++)
	{
		if (this->meshes[i].IsVisible())
		{
			this->meshes[i].sort();
			meshes[i].OptimizeVerts();
		}
	}
	/*for (DWORD i = 0; i < matList.GetNumMaterials(); i++)
	{
	  Material* material = matList.GetMaterial(i);
	  bool found = false;
	  for (DWORD j = 0; j < meshes.size(); j++)
	  {
		Mesh* mesh = GetMesh(j);

		for (DWORD k = 0; k < mesh->GetNumSplits(); k++)
		{
		  MaterialSplit* split = mesh->GetSplit(k);
		  if (split->matId==i)
		  {
			found = true;
			goto found;
		  }
		}
	  }
	  if (!found)
	  {
		matList.EraseMaterial(i);
		i--;
	  }
	found:;
	}*/
}

void Scene::SortMeshes()
{
	DWORD opaqueCount = 0;
	for (DWORD i = 0; i < meshes.size(); i++)
	{
		if (!meshes[i].IsTransparent())
			opaqueCount++;
	}

	DWORD alphaIndex = opaqueCount;
	opaqueCount = 0;
	Mesh* meshes = (Mesh*)malloc(this->meshes.size() * sizeof(Mesh));
	memcpy(meshes, &this->meshes.front(), this->meshes.size() * sizeof(Mesh));
	for (DWORD i = 0; i < this->meshes.size(); i++)
	{
		if (this->meshes[i].IsTransparent())
		{
			meshes[alphaIndex] = this->meshes[i];
			alphaIndex++;
		}
		else
		{
			meshes[opaqueCount] = this->meshes[i];
			opaqueCount++;
		}
	}
	memcpy(&this->meshes.front(), meshes, this->meshes.size() * sizeof(Mesh));
	free(meshes);
}

void Scene::SetName(const char* name)
{
	strcpy(this->name, name);
	/*DWORD StringLength = ((string)name).find(".scn");
	if(StringLength==string::npos)
	{
	StringLength = ((string)name).find("scn.dat");
	if(StringLength == string::npos)
	StringLength = strlen(name)+1;
	}

	//this->name = new char[StringLength];

	for(DWORD i=0; i<StringLength; i++)
	{
	this->name[i] = name[i];
	}

	this->name[StringLength] = '\x0';*/
}


const DWORD Scene::GetNumVertices()
{
	DWORD numVertices = 0;
	for (DWORD i = 0; i < meshes.size(); i++)
	{
		numVertices += meshes[i].GetNumVertices();
	}

	return numVertices;
}

const DWORD Scene::GetNumTriangles()
{
	DWORD numTriangles = 0;
	for (DWORD i = 0; i < meshes.size(); i++)
	{
		numTriangles += meshes[i].GetNumTriangles();
	}
	return numTriangles;
}

void Scene::AddMaterial(Material* const __restrict material)
{
	matList.AddMaterial(material);//std::move(material));
}

void Scene::AddMesh(Mesh* const __restrict mesh)
{
	/*Collision* collision = mesh->GetCollision();
	if(collision)
	{
	if(!(collision->GetNumFaces() || collision->GetNumVertices()))
	{
	mesh->DeinitCollision();
	delete collision;
	}
	}*/
	meshes.push_back(std::move(*mesh));
	/*Collision* collision = mesh->GetCollision();
	if(collision)
	{
	mesh->DeinitCollision();
	if(!(collision->GetNumFaces() || collision->GetNumVertices()))
	{
	delete collision;
	meshes.back().DeinitCollision();
	}
	}*/
}

void Scene::ProcessTexFile(DWORD ptr, bool sky)
{
	DWORD numImages = ((DWORD*)ptr)[0];
	if (!sky && !texFiles.numFiles)
	{
		texFiles.numFiles = numImages;
		texFiles.fileNames = new char* [numImages];
	}
	else
	{
		sky = true;
		char** temp = (char**)realloc(texFiles.fileNames, (texFiles.numFiles + numImages) * 4);
		if (temp)
			texFiles.fileNames = temp;
		else
			return;
	}
	ptr += 4;
	DWORD width, height, levels, dxt, checksum, pixelSize, levelSize, paletteSize, blendFlags;
	Colour* levelData;
	DDS_HEADER header;
	TGAHEADER tgaHeader = { 0 };
	tgaHeader.bitsPerPixel = 32;
	tgaHeader.imageDescriptor = 32;
	tgaHeader.dataTypeCode = 2;
	memset(&header, 0, sizeof(header));
	header.magic = 0x20534444;
	header.structSize = 0x7C;
	header.flags = 0x0A1007;
	header.caps[0] = 0x401008;
	header.pixelFormat[0] = 0x20;
	header.pixelFormat[1] = 0x4;
	unsigned char tgaFooterExt[8] = { 0 };
	char tgaFooterTag[17] = "TRUEVISION-XFILE";

	char imageName[MAX_PATH] = "";
	char tmp[25] = "";


	for (DWORD i = 0; i < numImages; i++)
	{
		checksum = ((DWORD*)ptr)[0];
		width = ((DWORD*)ptr)[1];
		height = ((DWORD*)ptr)[2];
		levels = ((DWORD*)ptr)[3];
		pixelSize = ((DWORD*)ptr)[4];
		blendFlags = ((DWORD*)ptr)[5];
		dxt = ((DWORD*)ptr)[6];
		paletteSize = ((DWORD*)ptr)[7];
		ptr += 32;
		if (dxt)
		{
			if (dxt == 2)//???
				dxt = 1;//D3DFMT_DXT1
			header.width = width;
			header.height = height;
			header.mipMapNum = levels;
			header.pixelFormat[2] = 0x545844 + ((dxt + 0x30) << 24);
			header.imageSize = ((DWORD*)ptr)[0];

			GetEnvironmentVariable("TEMP", imageName, MAX_PATH);
			sprintf(tmp, "\\%08X.dds", checksum);
			strcat(imageName, tmp);
			if (!sky)
				texFiles.fileNames[i] = new char[strlen(imageName) + 1];
			else
				texFiles.fileNames[i + texFiles.numFiles] = new char[strlen(imageName) + 1];

			if (!sky)
				memcpy(texFiles.fileNames[i], imageName, strlen(imageName) + 1);
			else
				memcpy(texFiles.fileNames[i + texFiles.numFiles], imageName, strlen(imageName) + 1);
			FILE* f = fopen(imageName, "wb");
			fwrite(&header, sizeof(DDS_HEADER), 1, f);

			for (DWORD j = 0; j < levels; j++)
			{
				levelSize = ((DWORD*)ptr)[0];
				ptr += 4;

				levelData = (Colour*)ptr;
				fwrite(levelData, levelSize, 1, f);
				ptr += levelSize;
			}
			fclose(f);
		}
		else if (pixelSize == 0x16)//D3DFMT_A1R5G5B5
		{
			tgaHeader.width = width;
			tgaHeader.height = height;
			Colour* colourData = new Colour[width * height];

			GetEnvironmentVariable("TEMP", imageName, MAX_PATH);
			sprintf(tmp, "\\%08X.tga", checksum);
			strcat(imageName, tmp);

			if (!sky)
				texFiles.fileNames[i] = new char[strlen(imageName) + 1];
			else
				texFiles.fileNames[i + texFiles.numFiles] = new char[strlen(imageName) + 1];
			if (!sky)
				memcpy(texFiles.fileNames[i], imageName, strlen(imageName) + 1);
			else
				memcpy(texFiles.fileNames[i + texFiles.numFiles], imageName, strlen(imageName) + 1);
			FILE* f = fopen(imageName, "wb");
			fwrite(&tgaHeader, sizeof(TGAHEADER), 1, f);

			for (DWORD j = 0; j < levels; j++)
			{
				levelSize = ((DWORD*)ptr)[0];
				ptr += 4;

				levelData = (Colour*)ptr;

				if (j == 0)
				{
					for (DWORD k = 0; k < levelSize / 2; k++)
					{
						colourData[k].r = ((BYTE)((WORD*)levelData)[k] & 31);
						colourData[k].g = ((BYTE)((((WORD*)levelData)[k] & 992) << 5));
						colourData[k].b = ((BYTE)((((WORD*)levelData)[k] & 31744) << 10));
						/*if(blendFlags==0x20)
						colourData[k].a = 1-((BYTE)((((WORD*)levelData)[k] & 32768)<<11));
						else*/
						colourData[k].a = ((BYTE)((((WORD*)levelData)[k] & 32768) << 11));
					}
					fwrite(colourData, levelSize * 2, 1, f);
					delete[] colourData;
				}
				ptr += levelSize;
			}
		}
		else if (pixelSize == 0x20)//D3DFMT_A8R8G8B8
		{
			tgaHeader.width = width;
			tgaHeader.height = height;

			GetEnvironmentVariable("TEMP", imageName, MAX_PATH);
			sprintf(tmp, "\\%08X.tga", checksum);
			strcat(imageName, tmp);

			if (!sky)
				texFiles.fileNames[i] = new char[strlen(imageName) + 1];
			else
				texFiles.fileNames[i + texFiles.numFiles] = new char[strlen(imageName) + 1];
			if (!sky)
				memcpy(texFiles.fileNames[i], imageName, strlen(imageName) + 1);
			else
				memcpy(texFiles.fileNames[i + texFiles.numFiles], imageName, strlen(imageName) + 1);
			FILE* f = fopen(imageName, "wb");
			fwrite(&tgaHeader, sizeof(TGAHEADER), 1, f);

			for (DWORD j = 0; j < levels; j++)
			{
				levelSize = ((DWORD*)ptr)[0];
				ptr += 4;

				levelData = (Colour*)ptr; //if(blendFlags==0x20) levelData->a = 1-levelData->a;

				if (j == 0)
					fwrite(levelData, levelSize, 1, f);
				ptr += levelSize;
			}
			fwrite(&tgaFooterExt, sizeof(tgaFooterExt), 1, f);
			fwrite(&tgaFooterTag, sizeof(tgaFooterTag), 1, f);
			fputc(0x2E, f);
			fputc(0x00, f);

			fclose(f);
		}
		else if (pixelSize == 0x8)//D3DFMT_P8
		{
			Colour* colourData = new Colour[width * height];
			Colour* unswizzled = new Colour[width * height];

			tgaHeader.width = width;
			tgaHeader.height = height;

			GetEnvironmentVariable("TEMP", imageName, MAX_PATH);
			sprintf(tmp, "\\%08X.tga", checksum);
			strcat(imageName, tmp);

			if (!sky)
				texFiles.fileNames[i] = new char[strlen(imageName) + 1];
			else
				texFiles.fileNames[i + texFiles.numFiles] = new char[strlen(imageName) + 1];
			if (!sky)
				memcpy(texFiles.fileNames[i], imageName, strlen(imageName) + 1);
			else
				memcpy(texFiles.fileNames[i + texFiles.numFiles], imageName, strlen(imageName) + 1);
			FILE* f = fopen(imageName, "wb");
			fwrite(&tgaHeader, sizeof(TGAHEADER), 1, f);

			BYTE* palette;

			palette = (BYTE*)ptr;
			ptr += paletteSize;//always 1024?

			for (DWORD j = 0; j < levels; j++)
			{
				levelSize = ((DWORD*)ptr)[0];
				ptr += 4;

				levelData = (Colour*)ptr;
				if (j == 0)
				{
					for (DWORD k = 0; k < levelSize; k++)
					{
						colourData[k].r = palette[((BYTE*)levelData)[k] * 4];
						colourData[k].g = palette[((BYTE*)levelData)[k] * 4 + 1];
						colourData[k].b = palette[((BYTE*)levelData)[k] * 4 + 2];
						/*if(blendFlags==0x20)
						colourData[k].a = 1-palette[((BYTE*)levelData)[k]*4+3];
						else*/
						colourData[k].a = palette[((BYTE*)levelData)[k] * 4 + 3];
					}
					Unswizzle(colourData, 4, width, height, unswizzled);
					fwrite(unswizzled, levelSize * 4, 1, f);
				}
				ptr += levelSize;
			}
			fwrite(&tgaFooterExt, sizeof(tgaFooterExt), 1, f);
			fwrite(&tgaFooterTag, sizeof(tgaFooterTag), 1, f);
			fputc(0x2E, f);
			fputc(0x00, f);

			fclose(f);
			delete[] colourData;
			delete[] unswizzled;
		}
		else
		{
			MessageBox(0, "unsupported texture", "unsupported texture", MB_OK);
		}

	}
	if (sky)
		texFiles.numFiles += numImages;

}

void Scene::CreateTextures()
{
	DEBUGSTART()
	{
		matList.CreateTextures();
		if (skyDome)
		{
			skyDome->matList.CreateTextures();
		}
	}
	DEBUGEND();
}

/*void Scene::FixNodes()
{
const DWORD numNodes = this->nodes.size();
for(DWORD i=0, numMeshes = meshes.size(); i<numMeshes; i++)
{
if(meshes[i].node)
{
for(DWORD j=0; j<numNodes; j++)
{
if(this->nodes[j].Name.checksum == this->meshes[i].GetName().checksum)
{
meshes[i].node = &this->nodes[j];
break;
}
}
DEBUGSTART()
{
meshes[i].node->SetPosition(this->meshes[i].GetCenter());
}
DEBUGEND();
}
}
}*/
#define OnGround 0
#define InAir 1
#define OnWall 2
#define OnRail 3
#define OutSide 4

void Scene::ReleaseBuffers()
{
	for (DWORD i = 0; i < meshes.size(); i++)
	{
		if (meshes[i].flags & MeshFlags::isVisible)
			meshes[i].ReleaseBuffers();
	}
	if (skyDome)
		skyDome->ReleaseBuffers();
}

void Scene::CreateBuffers(IDirect3DDevice9Ex* const __restrict Device, const DWORD& fvf)
{
	for (DWORD i = 0; i < meshes.size(); i++)
	{
		if (meshes[i].flags & MeshFlags::isVisible)
			meshes[i].CreateBuffers(Device, fvf);
	}
	if (skyDome)
		skyDome->CreateBuffers(Device, fvf);
}

__declspec(restrict)Mesh* const __restrict Scene::GetMesh(const DWORD& index)
{
	return &meshes[index];
}
__declspec(restrict)Mesh* const __restrict Scene::GetMesh(const Checksum& name)
{
	for (DWORD i = 0; i < meshes.size(); i++)
	{
		if (meshes[i].GetName().checksum == name.checksum)
			return &meshes[i];
	}
	return NULL;
}
__declspec(restrict)Mesh* const __restrict Scene::GetMesh(const Checksum& name, DWORD& index)
{
	for (DWORD i = 0; i < meshes.size(); i++)
	{
		if (meshes[i].GetName().checksum == name.checksum)
		{
			index = i;
			return &meshes[i];
		}
	}
	return NULL;
}

/*__forceinline bool Scene::Pickmesh(DWORD index, const D3DXVECTOR3* rayOrigin, const D3DXVECTOR3* rayDirection, float* outDistance, DWORD* outMatIndex, DWORD* outTriangleIndex)
{
if(renderCollision)
return meshes[index].PickCollision(rayOrigin, rayDirection, outDistance, outTriangleIndex);
else
return meshes[index].PickMesh(rayOrigin, rayDirection, outDistance, outMatIndex, outTriangleIndex);
return false;
}*/

__declspec (noalias) bool Scene::GetHitPoint(const Ray& __restrict ray, CollisionResult& __restrict closestHit) const
{
	const int numMeshes = meshes.size();
	//const UINT numMeshes = meshes.size();

	if (!renderCollision)
	{
		//#pragma omp parallel for default(shared) schedule(dynamic)
		for (DWORD i = 0; i < numMeshes; i++)
			/*using namespace Concurrency;
			parallel_for((UINT)0, numMeshes, [&](UINT i)*/
		{
			if (meshes[i].IsVisible())
				meshes[i].GetHitPoint(ray, closestHit);
		}//);
	}
	else
	{
		//#pragma loop(hint_parallel(4))
	//#pragma omp parallel for default(shared) schedule(dynamic)
		for (DWORD i = 0; i < numMeshes; i++)
			/*using namespace Concurrency;
			parallel_for((UINT)0, numMeshes, [&](UINT i)*/
		{
			if (!meshes[i].IsDeleted())
			{
				const Collision* __restrict const collision = meshes[i].GetCollision();
				if (collision)
					collision->GetHitPoint(ray, closestHit, &meshes[i]);
			}
		}//);
	}
	return closestHit.distance != FLT_MAX;
}

__declspec (noalias)DWORD Scene::GetState(const Ray& __restrict ray, D3DXVECTOR3& __restrict newPos) const
{
	const DWORD numMeshes = meshes.size();
	extern Player* __restrict player;

	D3DXVECTOR3 rayOrigin, rayDirection, translatedOrigin, translatedDirection;
	extern D3DVIEWPORT9 port;
	extern Camera* __restrict camera;
	D3DXMATRIX world, invWorld;
	D3DXVECTOR3 pos, pos2;
	D3DXVECTOR3 max, min;

	//#pragma omp parallel for
	for (DWORD i = 0; i < numMeshes; i++)
	{
		if (!meshes[i].IsDeleted())
		{
			const Collision* __restrict const collision = meshes[i].GetCollision();
			if (collision)
			{
				BBox bbox = meshes[i].GetBBoxX();

				D3DXVec3Project(&pos, (D3DXVECTOR3*)&bbox.Max, &port, &camera->proj(), &camera->view(), &matWorld);
				D3DXVec3Project(&pos2, (D3DXVECTOR3*)&bbox.Min, &port, &camera->proj(), &camera->view(), &matWorld);
				if ((pos.z >= 1.0f && pos.z <= 0.0f) && (pos2.z >= 1.0f && pos2.z <= 0.0f))
					continue;

				collision->FindGround(ray, &meshes[i], newPos);
			}
		}
	}
	if (player->height <= 18.0f)
		return OnGround;
	if (player->height == FLT_MAX)
		return OutSide;
	return InAir;
}

__declspec (noalias) bool Scene::AboveSurface(const Ray& __restrict ray) const
{
	const DWORD numMeshes = meshes.size();

	D3DXVECTOR3 rayOrigin, rayDirection, translatedOrigin, translatedDirection;
	extern D3DVIEWPORT9 port;
	extern Camera* __restrict camera;
	D3DXMATRIX world, invWorld;
	D3DXVECTOR3 pos, pos2;
	D3DXVECTOR3 max, min;

	//#pragma omp parallel for
	for (DWORD i = 0; i < numMeshes; i++)
	{
		if (!meshes[i].IsDeleted())
		{
			const Collision* __restrict const collision = meshes[i].GetCollision();
			if (collision)
			{
				BBox bbox = meshes[i].GetBBoxX();

				D3DXVec3Project(&pos, (D3DXVECTOR3*)&bbox.Max, &port, &camera->proj(), &camera->view(), &matWorld);
				D3DXVec3Project(&pos2, (D3DXVECTOR3*)&bbox.Min, &port, &camera->proj(), &camera->view(), &matWorld);
				if ((pos.z >= 1.0f && pos.z <= 0.0f) && (pos2.z >= 1.0f && pos2.z <= 0.0f))
					continue;

				if (collision->AboveSurface(ray, &meshes[i]))
					return true;
			}
		}
	}
	return false;
}

DWORD Scene::Collide(const Ray& __restrict ray, const Ray& __restrict downRay, D3DXVECTOR3& __restrict newPos) const
{
	const int numMeshes = meshes.size();
	CollisionResult closestHit;
	D3DXVECTOR3 rayOrigin, rayDirection, translatedOrigin, translatedDirection;
	extern D3DVIEWPORT9 port;
	extern Camera* __restrict camera;
	D3DXMATRIX world, invWorld;
	D3DXVECTOR3 pos, pos2;
	D3DXVECTOR3 max, min;
	/*D3DXVECTOR3 invdir = ray.direction * -1.0f;
	int sign[3];
	sign[0] = (invdir.x < 0);
	sign[1] = (invdir.y < 0);
	sign[2] = (invdir.z < 0);*/

	//#pragma omp parallel for default(shared) schedule(dynamic)
	for (int i = 0; i < numMeshes; i++)
	{
		if (!meshes[i].IsDeleted())
		{
			const Collision* __restrict const collision = meshes[i].GetCollision();
			if (collision)
			{
				BBox bbox = meshes[i].GetBBoxX();

				D3DXVec3Project(&pos, (D3DXVECTOR3*)&bbox.Max, &port, &camera->proj(), &camera->view(), &matWorld);
				D3DXVec3Project(&pos2, (D3DXVECTOR3*)&bbox.Min, &port, &camera->proj(), &camera->view(), &matWorld);
				if ((pos.z >= 1.0f && pos.z <= 0.0f) && (pos2.z >= 1.0f && pos2.z <= 0.0f))
					continue;

				if (collision->AboveSurface(downRay, &meshes[i]));
				collision->Intersect(ray, closestHit, &meshes[i], newPos);
			}
		}
	}
	if (closestHit.mesh != NULL)
	{
		/*if (closestHit.GetCollType() & 0x40)
		{
		for (DWORD i = 0; i < nodes.size(); i++)
		{
		if (nodes[i].Name.checksum == closestHit.mesh->GetName().checksum)
		{
		if (nodes[i].Trigger.checksum == Checksum("sk3_killskater").checksum)
		{
		newPos.x = nodes[nodes[i].Links[0]].Position.x;
		newPos.y = nodes[nodes[i].Links[0]].Position.y;
		newPos.z = -nodes[nodes[i].Links[0]].Position.z;
		break;
		}
		else if (nodes[i].Trigger.checksum)
		{
		for (DWORD j = 0, numScripts = KnownScripts.size(); j < numScripts; j++)
		{
		if (KnownScripts[j].name.checksum == nodes[i].Trigger.checksum)
		{
		BYTE* pFile = KnownScripts[j].func;
		BYTE* eof = pFile + KnownScripts[j].size;
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
		while (*pFile != 0x16)
		pFile++;

		pFile++;
		DWORD nodeName = *(DWORD*)pFile;
		for (DWORD k = 0; k < nodes.size(); k++)
		{
		if (nodes[k].Name.checksum == nodeName)
		{
		newPos.x = nodes[k].Position.x;
		newPos.y = nodes[k].Position.y;
		newPos.z = -nodes[k].Position.z;
		break;
		}
		}
		}
		pFile += 4;
		break;
		}
		}
		break;
		}
		}
		}
		break;
		}
		}
		}*/
		extern Player* __restrict player;
		if ((closestHit.GetCollType() & 0x80) && player->faceNormal.y > -0.60f)
			return OnGround;
		else
			return InAir;
	}
	return OutSide;
}

DWORD Scene::Collide(const Ray& __restrict ray) const//, D3DXVECTOR3 & __restrict newPos) const
{
	extern Player* __restrict player;
	const int numMeshes = meshes.size();
	CollisionResult closestHit;
	D3DXVECTOR3 rayOrigin, rayDirection, translatedOrigin, translatedDirection;
	extern D3DVIEWPORT9 port;
	extern Camera* __restrict camera;
	D3DXMATRIX world, invWorld;
	D3DXVECTOR3 pos, pos2;
	D3DXVECTOR3 max, min;
	/*D3DXVECTOR3 invdir = ray.direction * -1.0f;
	int sign[3];
	sign[0] = (invdir.x < 0);
	sign[1] = (invdir.y < 0);
	sign[2] = (invdir.z < 0);*/

	//#pragma omp parallel for default(shared) schedule(dynamic)
	for (int i = 0; i < numMeshes; i++)
	{
		if (!meshes[i].IsDeleted())
		{
			const Collision* __restrict const collision = meshes[i].GetCollision();
			if (collision)
			{
				BBox bbox = meshes[i].GetBBoxX();

				D3DXVec3Project(&pos, (D3DXVECTOR3*)&bbox.Max, &port, &camera->proj(), &camera->view(), &matWorld);
				D3DXVec3Project(&pos2, (D3DXVECTOR3*)&bbox.Min, &port, &camera->proj(), &camera->view(), &matWorld);
				if ((pos.z >= 1.0f && pos.z <= 0.0f) && (pos2.z >= 1.0f && pos2.z <= 0.0f))
					continue;
				/*float tmin, tmax, tymin, tymax, tzmin, tzmax;
				BBox bbox = meshes[i].GetBBoxX();
				if (bbox.Max.y>0.0f)
				  bbox.Max.y += 10.0f;
				elses
				  bbox.Max.y -= 10.0f;
				if (bbox.Min.y > 0.0f)
				  bbox.Min.y += 10.0f;
				else
				  bbox.Min.y -= 10.0f;
				BYTE* bounds = (BYTE*)&bbox;
				tmin = ((*(D3DXVECTOR3*)(bounds + sign[0]*12)).x - ray.origin.x)*invdir.x;
				tmax = ((*(D3DXVECTOR3*)(bounds + (1-sign[0])*12)).x - ray.origin.x)*invdir.x;
				tymin = ((*(D3DXVECTOR3*)(bounds + sign[1] * 12)).y - ray.origin.y)*invdir.y;
				tymax = ((*(D3DXVECTOR3*)(bounds + (1 - sign[1]) * 12)).y - ray.origin.y)*invdir.y;
				if ((tmin > tymax) || (tymin > tmax))
				  continue;
				if (tymin > tmin)
				  tmin = tymin;
				if (tymax < tmax)
				  tmax = tymax;
				tzmin = ((*(D3DXVECTOR3*)(bounds + sign[2] * 12)).z - ray.origin.z)*invdir.z;
				tzmax = ((*(D3DXVECTOR3*)(bounds + (1 - sign[2]) * 12)).z - ray.origin.z)*invdir.z;
				if ((tmin > tzmax) || (tzmin > tmax))
				  continue;*/
				collision->Intersect(ray, closestHit, &meshes[i], player->newPos);
			}
		}
	}
	if (closestHit.mesh != NULL)
	{
		player->hitFace = closestHit.mesh->GetCollision()->GetFace(closestHit.triIndex);
		if (player->faceNormal.y >= -0.50f)
		{

			Ray ray(player->newPos, D3DXVECTOR3(0.0f, -1.0f, 0.0f));
			return GetState(ray, player->newPos);
		}
		/*if (closestHit.GetCollType() & 0x40)
		{
		  for (DWORD i = 0; i < nodes.size(); i++)
		  {
			if (nodes[i].Name.checksum == closestHit.mesh->GetName().checksum)
			{
			  if (nodes[i].Trigger.checksum == Checksum("sk3_killskater").checksum)
			  {
				newPos.x = nodes[nodes[i].Links[0]].Position.x;
				newPos.y = nodes[nodes[i].Links[0]].Position.y;
				newPos.z = -nodes[nodes[i].Links[0]].Position.z;
				break;
			  }
			  else if (nodes[i].Trigger.checksum)
			  {
				for (DWORD j = 0, numScripts = KnownScripts.size(); j < numScripts; j++)
				{
				  if (KnownScripts[j].name.checksum == nodes[i].Trigger.checksum)
				  {
					BYTE* pFile = KnownScripts[j].func;
					BYTE* eof = pFile + KnownScripts[j].size;
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
						  while (*pFile != 0x16)
							pFile++;

						  pFile++;
						  DWORD nodeName = *(DWORD*)pFile;
						  for (DWORD k = 0; k < nodes.size(); k++)
						  {
							if (nodes[k].Name.checksum == nodeName)
							{
							  newPos.x = nodes[k].Position.x;
							  newPos.y = nodes[k].Position.y;
							  newPos.z = -nodes[k].Position.z;
							  break;
							}
						  }
						}
						pFile += 4;
						break;
					  }
					}
					break;
				  }
				}
			  }
			  break;
			}
		  }
		}*/
		/*Ray downRay(newPos, D3DXVECTOR3(0.0f, -1.0f, 0.0f));
		downRay.origin.y += 1.0f;
		if (closestHit.mesh->GetCollision()->AboveSurface(downRay, closestHit.mesh))
		  return OutSide;*/
		if (!(closestHit.GetCollType() & 0x10))
			return OnGround;
		else
			return InAir;
	}
	return InAir;
}

__declspec (noalias) bool Scene::CastRay(const Ray& __restrict ray, CollisionResult& __restrict result) const
{
	const int numMeshes = meshes.size();

	for (DWORD i = 0; i < numMeshes; i++)
	{
		if (!meshes[i].IsDeleted())
		{
			const Collision* __restrict const collision = meshes[i].GetCollision();
			if (collision)
				collision->CastRay(ray, &meshes[i], result);
		}
	}
	return result.distance != FLT_MAX;
}

__declspec (noalias) bool Scene::Intersect(const Ray& __restrict ray, CollisionResult& __restrict closestHit) const
{
	const int numMeshes = meshes.size();
	//const UINT numMeshes = meshes.size();

	if (!renderCollision)
	{
		//#pragma omp parallel for default(shared) schedule(dynamic)
		for (DWORD i = 0; i < numMeshes; i++)
			/*using namespace Concurrency;
			parallel_for((UINT)0, numMeshes, [&](UINT i)*/
		{
			if (meshes[i].IsVisible())
				meshes[i].Intersect(ray, closestHit);
		}//);
	}
	else
	{
		//#pragma loop(hint_parallel(4))
	//#pragma omp parallel for default(shared) schedule(dynamic)
		for (DWORD i = 0; i < numMeshes; i++)
			/*using namespace Concurrency;
			parallel_for((UINT)0, numMeshes, [&](UINT i)*/
		{
			if (!meshes[i].IsDeleted())
			{
				const Collision* __restrict const collision = meshes[i].GetCollision();
				if (collision)
					collision->Intersect(ray, closestHit, &meshes[i]);
			}
		}//);
	}
	return closestHit.mesh != NULL;
}

bool Scene::Intersect(const D3DXVECTOR3* const __restrict rayOrigin, const D3DXVECTOR3* const __restrict rayDirection, float* outDistance, DWORD* outMeshIndex, DWORD* outMatIndex, DWORD* outTriangleIndex)
{
	float closestIntersect = 0.0f;
	bool intersect = false;
	DWORD closestMatIndex = 0, closestTri = 0;

	DWORD meshNum = meshes.size();
	/*if(numMeshes && numMeshes<meshes.size())
	meshNum = numMeshes;
	else
	meshNum = meshes.size();*/
	if (renderCollision)
	{
		for (DWORD i = 0; i < meshNum; i++)
		{
			if (!meshes[i].IsDeleted() && /*meshes[i].Intersect(rayOrigin,rayDirection) &&*/ meshes[i].PickCollision(rayOrigin, rayDirection, outDistance, outTriangleIndex))
			{
				if (closestIntersect >= *outDistance || !intersect)
				{
					intersect = true;
					*outMeshIndex = i;
					closestIntersect = *outDistance;
					if (outTriangleIndex)
						closestTri = *outTriangleIndex;
				}
			}
		}
	}
	else
	{
		for (DWORD i = 0; i < meshNum; i++)
		{
			if (meshes[i].IsVisible() && /*meshes[i].Intersect(rayOrigin,rayDirection) &&*/ meshes[i].PickMesh(rayOrigin, rayDirection, outDistance, outMatIndex, outTriangleIndex))
			{
				if (closestIntersect >= *outDistance || !intersect)
				{
					intersect = true;
					*outMeshIndex = i;
					closestIntersect = *outDistance;
					if (outTriangleIndex)
						closestTri = *outTriangleIndex;
					if (outMatIndex)
						closestMatIndex = *outMatIndex;
				}
			}
		}
	}
	//printf("meshnum %d name 0x%X\n",*outMeshIndex,meshes[*outMeshIndex].GetName().checksum);
	*outDistance = closestIntersect;
	if (outMatIndex)
		*outMatIndex = closestMatIndex;
	if (outTriangleIndex)
		*outTriangleIndex = closestTri;
	return intersect;
}

void Scene::RenderShaded(IDirect3DDevice9Ex* const __restrict Device)
{
	IDirect3DDevice9Ex* const __restrict Dev = Device;
	if (renderCollision)
	{
		//Dev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		//Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		//Device->SetTexture(0,NULL);
		Dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
		Dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVBLENDFACTOR);
		Dev->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);


		DWORD meshNum = meshes.size();
		for (DWORD i = 0; i < meshNum; i++)
		{
			if (!meshes[i].IsDeleted())
				meshes[i].RenderCollision();
		}

		Dev->SetFVF(D3DFVF_XYZ);
		Dev->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, *(DWORD*)&bias);
		Dev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

		for (DWORD i = 0; i < meshNum; i++)
		{
			if (!meshes[i].IsDeleted())
				meshes[i].RenderCollision(meshes[i].IsSelected());
		}

		Dev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		//Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		Dev->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, 0);
		//Device->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|MAX_TEXCOORDS);
		//Dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		/*Device->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
		Device->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);*/
	}
	else
	{
		DWORD meshNum = meshes.size();

		for (DWORD i = 0; i < meshNum; i++)
		{
			if (meshes[i].IsVisible())
				meshes[i].RenderShaded();
		}
	}
}

void Scene::RenderNoTrans(IDirect3DDevice9Ex* const __restrict Device)
{
	DWORD meshNum = meshes.size();

	for (DWORD i = 0; i < meshNum; i++)
	{
		if (meshes[i].IsVisible())
			meshes[i].Render(&matList);
	}
}

void Scene::Render(IDirect3DDevice9Ex* const __restrict Device)
{
	IDirect3DDevice9Ex* const __restrict Dev = Device;
	/*if(skyDome)
	{
	Device->SetTransform(D3DTS_WORLD, &matSky);
	for(DWORD i=0; i<MAX_PASSES; i++)
	Device->SetSamplerState( i, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP );//Smoothly stretch the sky width
	skyDome->Render(Device);
	for(DWORD i=0; i<MAX_PASSES; i++)
	Device->SetSamplerState( i, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP );
	Device->SetTransform(D3DTS_WORLD, &matWorld);
	}*/

	const DWORD meshNum = meshes.size();
	/*if(numMeshes && numMeshes<meshes.size())
	meshNum = numMeshes;
	else
	meshNum = meshes.size();*/
	Dev->SetRenderState(D3DRS_ALPHAREF, 128);
	for (DWORD i = 0; i < meshNum; i++)
	{
		if (meshes[i].IsVisible() && !meshes[i].IsTransparent())
			meshes[i].Render0(&matList);
	}

	Dev->SetRenderState(D3DRS_ALPHAREF, alphaRef);
	Dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);


	for (DWORD i = 0; i < meshNum; i++)
	{
		if (meshes[i].IsVisible() && meshes[i].IsTransparent())
			meshes[i].Render2(&matList);
	}


	/* Device->SetFVF(D3DFVF_XYZ);
	Device->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, *(DWORD *)&bias);
	Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	Device->SetTexture(0,0);
	for(DWORD i=0; i<meshNum; i++)
	{
	if(meshes[i].IsVisible())
	meshes[i].Render0(Device, &matList);
	}
	Device->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, 0);
	Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	Device->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|MAX_TEXCOORDS);*/
	/*DWORD meshNum = meshes.size();
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	Device->SetTexture(0,NULL);
	Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	Device->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
	for(DWORD i=0; i<meshNum; i++)
	{
	meshes[i].Render3(Device, &matList);
	}*/

	//Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}
