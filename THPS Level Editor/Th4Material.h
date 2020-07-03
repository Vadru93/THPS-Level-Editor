#include "Material.h"
#pragma unmanaged

struct Thug2Texture : Texture
{
#pragma pack(push)
#pragma pack(1)
  struct Data
  {
    DWORD textureId;
    DWORD flags;
    bool unk1;
    float r;
    float g;
    float b;
    unsigned long long blendModeAlpha;
    DWORD uAddress;
    DWORD vAddress;
    float uTiling;
    float vTiling;
    DWORD unk6;
  };
  Data* data;
#pragma pack(pop)
  Thug2Texture(Data* data)
  {
    SetFlags(data->flags);
    SetId(data->textureId);
    /* SetRed(data->r);
    SetGreen(data->g);
    SetBlue(data->b);*/
    SetUVMode(data->uAddress, data->vAddress);
    SetBlendMode((DWORD)(data->blendModeAlpha & 0xFFFFFFUL));
    if (data->flags & (1 << 3))
        SetEnvFlag(true);
    else
        SetEnvFlag(false);
    if(GetBlendMode()==5)
    {
      fixedAlpha = (( data->blendModeAlpha >> 32 ) / 2)<<24;
    }
    else
      fixedAlpha = ((DWORD)( data->blendModeAlpha >> 32 ))<<24;
    //SetTiling(uTiling,vTiling);
    //CreateTexture();
  }
};

struct ThugTexture : Texture
{
#pragma pack(push)
#pragma pack(1)
  struct Data
  {
    DWORD textureId;
    DWORD flags;
    bool unk1;
    float r;
    float g;
    float b;
    unsigned long long blendModeAlpha;
    DWORD uAddress;
    DWORD vAddress;
    float uTiling;
    float vTiling;
    DWORD unk6;
  };
  Data* data;
#pragma pack(pop)
  ThugTexture(Data* data)
  {
    SetFlags(data->flags);
    SetId(data->textureId);
    /*SetRed(data->r);
    SetGreen(data->g);
    SetBlue(data->b);*/
    /*SetUVMode(data->uAddress, data->vAddress);
    blendMode = (DWORD)( data->blendModeAlpha & 0xFFFFFFUL );
    /*if(blendMode==5)
    {
    fixedAlpha = (( data->blendModeAlpha >> 32 ) / 2)<<24;
    }
    //else
    fixedAlpha = ((DWORD)( data->blendModeAlpha >> 32 ))<<24;
    fixedAlpha = fixedAlpha >= 0x80000000UL ? 0xFF000000UL : ( fixedAlpha << 1 );*/
    //fixedAlpha = blendMode | fixedAlpha;
    //SetUVMode(uAddress, vAddress);
    //SetTiling(uTiling,vTiling);
    //CreateTexture();

    SetBlendMode((DWORD)(data->blendModeAlpha & 0xFFFFFFUL));
    if (data->flags & (1 << 3))
        SetEnvFlag(true);
    else
        SetEnvFlag(false);
    if (GetBlendMode() == 5)
    {
      fixedAlpha = ((data->blendModeAlpha >> 32)) << 24;
      fixedAlpha = fixedAlpha >= 0x80000000UL ? 0xFF000000UL : (fixedAlpha << 1);
      /*if(data->unk1)
      (*(Colour*)&fixedAlpha).r=1;*/
    }
    else
      fixedAlpha = ((DWORD)(data->blendModeAlpha >> 32)) << 24;

    (*(Colour*)&fixedAlpha).r = (BYTE)(data->r*254.0f);
    (*(Colour*)&fixedAlpha).r = (*(Colour*)&fixedAlpha).r >= 127 ? 254 : (*(Colour*)&fixedAlpha).r * 2;
    (*(Colour*)&fixedAlpha).g = (BYTE)(data->g*254.0f);
    (*(Colour*)&fixedAlpha).g = (*(Colour*)&fixedAlpha).g >= 127 ? 254 : (*(Colour*)&fixedAlpha).g * 2;
    (*(Colour*)&fixedAlpha).b = (BYTE)(data->b*254.0f);
    (*(Colour*)&fixedAlpha).b = (*(Colour*)&fixedAlpha).b >= 127 ? 254 : (*(Colour*)&fixedAlpha).b * 2;
    /*if((data->r==0.5f && (*(Colour*)&fixedAlpha).r!=256) || (data->g==0.5f && (*(Colour*)&fixedAlpha).g!=256) || (data->b==0.5f && (*(Colour*)&fixedAlpha).b!=256))
    MessageBox(0,"WTF","WTF",0);*/
    if (GetBlendMode() == 5 && data->unk1)
    {
      (*(Colour*)&fixedAlpha).r = 254;
      (*(Colour*)&fixedAlpha).g = 254;
      (*(Colour*)&fixedAlpha).b = 254;
    }
    //fixedAlpha = fixedAlpha >= 0x80000000UL ? 0xFF000000UL : ( fixedAlpha << 1 );
    SetUVMode(data->uAddress, data->vAddress);
  }
};

struct Th4Texture : Texture
{
#pragma pack(push)
#pragma pack(1)
  struct Data 
  {
    DWORD textureId;
    DWORD flags;
    bool unk1;
    float r;
    float g;
    float b;
    unsigned long long blendModeAlpha;
    DWORD uAddress;
    DWORD vAddress;
    float unk4;
  };
  Data* data;
#pragma pack(pop)
  Th4Texture(Data* data)
  {
    SetFlags(data->flags & ~2);
    SetId(data->textureId);
    /*SetRed(data->r);
    SetGreen(data->g);
    SetBlue(data->b);*/
    SetBlendMode((DWORD)( data->blendModeAlpha & 0xFFFFFFUL ));
    if(GetBlendMode()==5)
    {
      fixedAlpha = (( data->blendModeAlpha >> 32 ))<<24;
      fixedAlpha = fixedAlpha >= 0x80000000UL ? 0xFF000000UL : ( fixedAlpha << 1 );
      /*if(data->unk1)
          (*(Colour*)&fixedAlpha).r=1;*/
    }
    else
      fixedAlpha = ((DWORD)( data->blendModeAlpha >> 32 ))<<24;

      (*(Colour*)&fixedAlpha).r = (BYTE)(data->r*254.0f);
      (*(Colour*)&fixedAlpha).r = (*(Colour*)&fixedAlpha).r >= 127 ? 254 : (*(Colour*)&fixedAlpha).r*2;
      (*(Colour*)&fixedAlpha).g = (BYTE)(data->g*254.0f);
      (*(Colour*)&fixedAlpha).g = (*(Colour*)&fixedAlpha).g >= 127 ? 254 : (*(Colour*)&fixedAlpha).g*2;
      (*(Colour*)&fixedAlpha).b = (BYTE)(data->b*254.0f);
      (*(Colour*)&fixedAlpha).b = (*(Colour*)&fixedAlpha).b >= 127 ? 254 : (*(Colour*)&fixedAlpha).b*2;
      /*if((data->r==0.5f && (*(Colour*)&fixedAlpha).r!=256) || (data->g==0.5f && (*(Colour*)&fixedAlpha).g!=256) || (data->b==0.5f && (*(Colour*)&fixedAlpha).b!=256))
        MessageBox(0,"WTF","WTF",0);*/
      if(GetBlendMode() == 5 && data->unk1)
      {
        (*(Colour*)&fixedAlpha).r=254;
        (*(Colour*)&fixedAlpha).g=254;
        (*(Colour*)&fixedAlpha).b=254;
      }
    //fixedAlpha = fixedAlpha >= 0x80000000UL ? 0xFF000000UL : ( fixedAlpha << 1 );
    SetUVMode(data->uAddress, data->vAddress);
    //CreateTexture();
  }
};

struct ThugMaterial : Material
{
#pragma pack(push)
#pragma pack(1)
  struct Data
  {
    DWORD matId;
    DWORD unkId;
    DWORD numTextures;
    DWORD unk1;
    bool unk2;
    float drawOrder;
    bool singledSided;
    bool doubled;
    int zbias;
    bool unkFlag;//grass?
    float grassHeight;
    DWORD grassLayers;
  };
  Data* data;
#pragma pack(pop)
  ThugMaterial(Data* data);
};

struct Th4Material : Material
{
  //vector<Th4Texture> textures;
#pragma pack(push)
#pragma pack(1)
  struct Data
  {
    DWORD matId;
    DWORD numTextures;
    DWORD unk1;
    bool unk2;
    float drawOrder;
    bool doubled;
    //BYTE unk[10];
    bool unkFlag;//grass?
    float grassHeight;
    DWORD grassLayers;
  };
  Data* data;
#pragma pack(pop)

  Th4Material(Data* data);
};