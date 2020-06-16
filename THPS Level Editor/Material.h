#ifndef MATERIAL_H
#define MATERIAL_H

#include "Includes.h"
#include "Structs.h"
#include <d3dx9.h>
#pragma unmanaged
using std::vector;

static const float uVel_water = 0.03f;
static const float vVel_water = -0.04f; // 0.03
static const float uFreq_water = 0.3f;  //0.03
static const float vFreq_water = 0.05f;
static const float uAmp_water = 0.0f;
static const float vAmp_water = 0.0f;  
static const float uPhase_water = 1.f;  //0.03
static const float vPhase_water = 2.f;  //0.0

/*static const float uVel_water = 0.0f;
static const float vVel_water = 1.99999995529f;
static const float uFreq_water = 5.0f;
static const float vFreq_water = 0.0f;
static const float uAmp_water = 0.0f;
static const float vAmp_water = 0.0f;
static const float uPhase_water = 0.2f;
static const float vPhase_water = 0.0f;*/


static const float uVel_linear = 1.0f;
static const float vVel_linear = 1.0f;
static const float uFreq_linear = 0.0f;
static const float vFreq_linear = 0.05f;
static const float uAmp_linear = 0.0f;
static const float vAmp_linear = 0.0f;
static const float uPhase_linear = 0.0f;
static const float vPhase_linear = 0.0f;

static const float uVel_Th2 = 0.0f;
static const float vVel_Th2 = 0.0f;
static const float uFreq_Th2 = 0.0f;
static const float vFreq_Th2 = 0.0f;
static const float uAmp_Th2 = 0.0f;
static const float vAmp_Th2 = 0.0f;
static const float uPhase_Th2 = 0.0f;
static const float vPhase_Th2 = 0.0f;


struct Animation
{
  float UVel;//0
  float VVel;//4
  float UFrequency;//8//  fsin(UFreq*t+UPhase)*UAmp+t*UVel
  float VFrequency;//12
  float UAmplitude;//16  fsin(amp*360*Uphase*0.02)*UFreq
  float VAmplitude;//20
  float UPhase;//24
  float VPhase;//28

  Animation()
  {
  }

  BYTE GetType()
  {
    if(UFrequency==uFreq_water && VFrequency==vFreq_water && UAmplitude==uAmp_water && VAmplitude==vAmp_water && UPhase==uPhase_water && VPhase==vPhase_water)
      return 0;//water
    else if(UFrequency==uFreq_linear  && VFrequency==vFreq_linear  && UAmplitude==uAmp_linear  && VAmplitude==vAmp_linear && UPhase==uPhase_linear  && VPhase==vPhase_linear)
      return 1;//linear
    else
      return 2;//custom
  }

  Animation(float uVel, float vVel)
  {
    UVel = uVel;
    VVel = vVel;
    UFrequency = 0.0f;
    VFrequency = 0.0f;
    UAmplitude = 0.0f;
    VAmplitude = 0.0f;
    UPhase = 0.0f;
    VPhase = 0.0f;
  }

  Animation(BYTE type)
  {
    if(type==0)
    {
      UVel=uFreq_water;
      VVel=vFreq_water;
      UFrequency=uFreq_water;
      VFrequency=vFreq_water;
      UAmplitude=uAmp_water;
      VAmplitude=vAmp_water;
      UPhase=uPhase_water;
      VPhase=vPhase_water;
    }
    else
    {
      UVel=uFreq_linear;
      VVel=vFreq_linear;
      UFrequency=uFreq_linear;
      VFrequency=vFreq_linear;
      UAmplitude=uAmp_linear;
      VAmplitude=vAmp_linear;
      UPhase=uPhase_linear;
      VPhase=vPhase_linear;
    }
  }
};

struct UVAnim
{
  DWORD matId;
  Animation anim;

  BYTE GetAnimType()
  {
    if(anim.UFrequency==uFreq_water && anim.VFrequency==vFreq_water && anim.UAmplitude==uAmp_water && anim.VAmplitude==vAmp_water && anim.UPhase==uPhase_water && anim.VPhase==vPhase_water)
      return 0;//water
    else if(anim.UFrequency==uFreq_linear  && anim.VFrequency==vFreq_linear  && anim.UAmplitude==uAmp_linear  && anim.VAmplitude==vAmp_linear && anim.UPhase==uPhase_linear  && anim.VPhase==vPhase_linear)
      return 1;//linear
    else
      return 2;//custom
  }

  UVAnim(DWORD id, const BYTE* __restrict pFile)
  {
    matId = id;
    anim.UVel = *(float*)pFile;
    pFile += 4;
    anim.VVel = *(float*)pFile;
    pFile += 4;
    anim.UFrequency = *(float*)pFile;
    pFile += 4;
    anim.VFrequency = *(float*)pFile;
    pFile += 4;
    anim.UAmplitude = *(float*)pFile;
    pFile += 4;
    anim.VAmplitude = *(float*)pFile;
    pFile += 4;
    anim.UPhase = *(float*)pFile;
    pFile += 4;
    anim.VPhase = *(float*)pFile;
    pFile += 4;
  }

  UVAnim(DWORD id, float speedX, float speedY) : anim(speedX, speedY)
  {
    matId = id;
  }

  UVAnim(DWORD id, BYTE type) : anim(type)
  {
    matId=id;
    /*if(type==0)
    {
      anim.UVel=uFreq_water;
      anim.VVel=vFreq_water;
      anim.UFrequency=uFreq_water;
      anim.VFrequency=vFreq_water;
      anim.UAmplitude=uAmp_water;
      anim.VAmplitude=vAmp_water;
      anim.UPhase=uPhase_water;
      anim.VPhase=vPhase_water;
    }
    else
    {
      anim.UVel=uFreq_linear;
      anim.VVel=vFreq_linear;
      anim.UFrequency=uFreq_linear;
      anim.VFrequency=vFreq_linear;
      anim.UAmplitude=uAmp_linear;
      anim.VAmplitude=vAmp_linear;
      anim.UPhase=uPhase_linear;
      anim.VPhase=vPhase_linear;
    }*/
  }

  UVAnim(DWORD id, Animation* anim)
  {
    matId=id;
    this->anim=*anim;
    /*this->anim.UPhase/=10.0f;
    this->anim.VPhase/=10.0f;*/
  }
};

struct Texture
{

  Texture()
  {
    ZeroMemory(this,sizeof(Texture));
    this->uAddress = D3DTADDRESS_WRAP;
    this->vAddress = D3DTADDRESS_WRAP;
    this->blendMode = 20;
  }
  void SetId(const DWORD &id)
  {
    textureId.checksum = id;
  }
  void SetId(const Checksum &id)
  {
    textureId = id;
  }

  DWORD GetTexId()
  {
    return textureId.checksum;
  }

  char* GetName()
  {
    return textureId.GetString();
  }
  
  void SetFlags(const DWORD &flags)
  {
    this->flags = flags;
  }

  DWORD GetFlags()
  {
    return flags;
  }
  /*
  void SetRed(const float &r)
  {
    this->r = r;
  }

  void SetGreen(const float &g)
  {
    this->g = g;
  }

  void SetBlue(const float &b)
  {
    this->b = b;
  }

  float GetRed()
  {
    return this->r;
  }prefetcht0

  float GetGreen()
  {
    return this->g;
  }

  float GetBlue()
  {
    return this->b;
  }
  */
  __declspec(restrict) inline const LPDIRECT3DTEXTURE9 GetImage() const
  {
    return imageData;
  }

  void Release()
  {
#define SAFE_RELEASE(p) { if ( (p) ) { (p)->Release(); p=NULL;} }
    SAFE_RELEASE(imageData);
  }

  void CreateTexture();

  inline bool IsTransparent()
  {
    TGAHEADER tgaHeader;

    static char imageName[MAX_PATH] = "";
    static char tmp[25] = "";
    /*GetEnvironmentVariable("TEMP",imageName, MAX_PATH);
    char* name=textureId.GetString();
    if(name == NULL)
    {
    sprintf(tmp,"\\%08X.dds",textureId);
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
    FILE* f=fopen(imageName,"rb");
    if(!f)
    {
    ZeroMemory(imageName,MAX_PATH);*/
    GetEnvironmentVariable("TEMP",imageName, MAX_PATH);
    char* name=textureId.GetString();
    if(name == NULL)
    {
      sprintf(tmp,"\\%08X.tga",textureId.checksum);
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
    FILE*f=fopen(imageName,"rb");
    if(f)
    {
      bool transparent = false;
      fread(&tgaHeader,sizeof(TGAHEADER),1,f);
      DWORD tgaSize = tgaHeader.width*tgaHeader.height;
      Colour* imageColor = new Colour[tgaSize];

      fread(imageColor,tgaSize,4,f);

      for(DWORD i=0; i<tgaSize; i++)
      {
        if(imageColor[i].a != 0 && imageColor[i].a<255)
          transparent = true;
      }
      fclose(f);
      delete [] imageColor;
      // printf("height %d\n", tgaHeader.height);
      return transparent;
    }
    else
      return false;
    /*}
    else
    return false;*/
  }

  inline const DWORD GetHeight()
  {
    static TGAHEADER tgaHeader = {0};

    static char imageName[MAX_PATH] = "";
    static char tmp[25] = "";
    GetEnvironmentVariable("TEMP",imageName, MAX_PATH);
    char* name=textureId.GetString();
    if(name == NULL)
    {
      sprintf(tmp,"\\%08X.dds",textureId.checksum);
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
    FILE* f=fopen(imageName,"rb");
    if(!f)
    {
      //ZeroMemory(imageName,MAX_PATH);
      GetEnvironmentVariable("TEMP",imageName, MAX_PATH);
      sprintf(tmp,"\\%08X.tga",textureId.checksum);
      strcat(imageName,tmp);
      f=fopen(imageName,"rb");
      if(f)
      {
        fread(&tgaHeader,sizeof(TGAHEADER),1,f);
        fclose(f);
        // printf("height %d\n", tgaHeader.height);
        return (DWORD)tgaHeader.height;
      }
      else
        return 1;
    }
    else
    {
      DDS_HEADER ddsHeader = {0};
      fread(&ddsHeader,sizeof(DDS_HEADER),1,f);
      fclose(f);
      return (DWORD)ddsHeader.height;
    }
  }

  inline const DWORD GetWidth()
  {
    static TGAHEADER tgaHeader = {0};

    static char imageName[MAX_PATH] = "";
    static char tmp[25] = "";
    GetEnvironmentVariable("TEMP",imageName, MAX_PATH);
    char* name=textureId.GetString();
    if(name == NULL)
    {
      sprintf(tmp,"\\%08X.dds",textureId.checksum);
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
    FILE* f=fopen(imageName,"rb");
    if(!f)
    {
      //ZeroMemory(imageName,MAX_PATH);
      GetEnvironmentVariable("TEMP",imageName, MAX_PATH);
      sprintf(tmp,"\\%08X.tga",textureId.checksum);
      strcat(imageName,tmp);
      f=fopen(imageName,"rb");
      if(f)
      {
        fread(&tgaHeader,sizeof(TGAHEADER),1,f);
        fclose(f);
        // printf("width %d\n", tgaHeader.width);
        return (DWORD)tgaHeader.width;
      }
      else
        return 1;
    }
    else
    {
      DDS_HEADER ddsHeader = {0};
      fread(&ddsHeader,sizeof(DDS_HEADER),1,f);
      fclose(f);
      return (DWORD)ddsHeader.width;
    }
  }

  void SetUVMode(const DWORD &uAddress, const DWORD &vAddress)
  {
    switch(uAddress)
    {
    case 0:
      {
        this->uAddress = D3DTADDRESS_WRAP;
        break;
      }
    case 1:
      {
        this->uAddress = D3DTADDRESS_CLAMP;
        break;
      }
    case 2:
      {
        this->uAddress = D3DTADDRESS_BORDER;
        break;
      }
    default:
      MessageBox(0,"error wrong uAddress",0,0);
      this->uAddress = D3DTADDRESS_WRAP;
      break;
    }

    switch(vAddress)
    {
    case 0:
      {
        this->vAddress = D3DTADDRESS_WRAP;
        break;
      }
    case 1:
      {
        this->vAddress = D3DTADDRESS_CLAMP;
        break;
      }
    case 2:
      {
        this->vAddress = D3DTADDRESS_BORDER;
        break;
      }
    default:
      MessageBox(0,"error wrong vAddress",0,0);
      this->vAddress = D3DTADDRESS_WRAP;
      break;
    }
  }
  public:
    DWORD uAddress;
    DWORD vAddress;



 private:
   union
   {
   LPDIRECT3DTEXTURE9 imageData;
   DWORD flags;
   };
    public:
      DWORD fixedAlpha;
      DWORD blendMode;
  private:
    Checksum textureId;
    //DWORD flags;
    /*float r;
    float g;
    float b;*/
};

struct Material
{

  Material()
  {
    numTextures=0;
    textures=NULL;
    transparent=false;
    doubled=false;
    ignoreAlpha=false;
    animated=false;
    invisible=false;
    fucked=false;
    reverse=false;
    alphaMap=false;
    extraLayer=false;
  }

  void Reserve(DWORD numTextures)
  {
    textures = new Texture[numTextures];//textures.reserve(numTextures);
  }

  _declspec (noalias) void Submit();
  _declspec (noalias) void SubmitSelected();
  _declspec (noalias) void SubmitPass();

  inline const DWORD GetNumTextures() const
  {
    return numTextures;
  }

  void ReleaseTextures()
  {
    for(DWORD j=0; j<numTextures; j++)
    {
      textures[j].Release();
    }
  }

  void CreateTextures()
  {
    for(DWORD j=0; j<numTextures; j++)
    {
      textures[j].CreateTexture();
    }
  }

  __declspec(restrict) Texture*const __restrict GetTexture(const DWORD index)
  {
    return &textures[index];
  }

  void SetMatId(const DWORD &matId)
  {
    materialId = matId;
  }


  inline const DWORD GetMatId() const
  {
    return materialId;
  }
  void AddTexture2(Texture* texture)
  {
    /* if(textures.size()==0)
    {
    memset(&d3dMaterial,0,sizeof(D3DMATERIAL9));
    d3dMaterial.Diffuse.r = texture->GetRed();
    d3dMaterial.Diffuse.g = texture->GetGreen();
    d3dMaterial.Diffuse.b = texture->GetBlue();
    d3dMaterial.Diffuse.a = 1.0f;
    }*/
    //textures.push_back(texture[0]);
    textures[numTextures]=*texture;
    numTextures++;
  }

#define TEXFLAG_TRANS (1<<6)
#define MATFLAG_PASS_IGNORE_VERTEX_ALPHA	(1<<12)
  void AddTexture(Texture* texture)
  {
    //static bool push_front = false;
    if(numTextures==0)
    {
      if(texture->GetFlags() & TEXFLAG_TRANS)
        transparent=true;
     /* if(texture->fixedAlpha || texture->GetFlags() & (1<<3))
        push_front=true;
      else */if(texture->GetFlags() & MATFLAG_PASS_IGNORE_VERTEX_ALPHA)
        ignoreAlpha=true;
      /*if(texture->blendMode == 1 || texture->blendMode == 2)
        reverse=true;*/
      /*else if(texture->blendMode == 2)
        alphaMap=true;*/
    }
    
    texture->CreateTexture();
    /*if(textures.size()==0)
    {
    memset(&d3dMaterial,0,sizeof(D3DMATERIAL9));
    d3dMaterial.Diffuse.r = texture->GetRed();
    d3dMaterial.Diffuse.g = texture->GetGreen();
    d3dMaterial.Diffuse.b = texture->GetBlue();
    d3dMaterial.Diffuse.a = 1.0f;
    }*/
    /*if(textures.size() && push_front && !texture->fixedAlpha)
    {
      const DWORD flags = texture->GetFlags();
      //if(!transparent)
        //transparent = flags & TEXFLAG_TRANS != 0;
      if(flags & MATFLAG_PASS_IGNORE_VERTEX_ALPHA)
        ignoreAlpha=true;
      textures.insert(textures.begin(),texture[0]);
      push_front=false;
    }
    else*/
      //textures.push_back(texture[0]);
    textures[numTextures] = *texture;
    numTextures++;
  }

  void clearTextures()
  {
    numTextures=0;
    delete [] textures;
    textures=0;
  }

  __declspec(restrict) inline const LPDIRECT3DTEXTURE9 GetTexturePass(const BYTE pass) const
  {
    if(pass<numTextures)
      return textures[pass].GetImage();
    else
      return NULL;
  }

  Texture* GetTextures()
  {
    return textures;
  }

  ~Material()
  {
    extern bool loading;
    if(!loading)
    {
      delete [] textures;
    }
    textures=NULL;
    //textures.clear();
  }

  /*inline const D3DMATERIAL9* GetD3DMaterial() const
  {
  return &d3dMaterial;
  }*/
  /*struct Anim
  {
    float uVel;
    float vVel;
  };*/

private:
  DWORD numTextures;
  Texture* textures;
  //vector <Texture> textures;
  DWORD materialId;
public:
  float drawOrder;
  bool transparent;
  bool doubled;
  bool ignoreAlpha;
  bool animated;
  bool invisible;
  bool reverse;
  bool alphaMap;
  bool extraLayer;
  BYTE fucked;
  BYTE padding[3];
  //D3DMATERIAL9 d3dMaterial;

};

struct MaterialList
{
  ~MaterialList()
  {
    ReleaseTextures();
    materials.clear();
  }

  void EraseMaterial(const DWORD index)
  {
    materials.erase(materials.begin()+index);
  }

  void Resize(DWORD numMaterials)
  {
    materials.resize(numMaterials);
  }

  void CreateTextures()
  {
    for(DWORD i=0, numMaterials = materials.size(); i<numMaterials; i++)
    {
      materials[i].CreateTextures();
    }
  }

  DWORD GetMaterialIndex(const DWORD &texId)
  {
    for(DWORD i=0, numMaterials = materials.size(); i<numMaterials; i++)
    {
      if(materials[i].GetTexture(0)->GetTexId() == texId)
        return i;
    }
    return 0xFFFFFFFF;
  }

  void Merge(MaterialList* const __restrict matList)
  {
    for(DWORD i=0; i<matList->GetNumMaterials(); i++)
    {
      materials.push_back(matList->GetMaterial(i)[0]);
    }
  }

  inline const DWORD GetNumMaterials() const
  {
    return materials.size();
  }

  void ReleaseTextures()
  {

    for(DWORD i=0, numMaterials = materials.size(); i<numMaterials; i++)
    {
      materials[i].ReleaseTextures();
    }
  }

  void AddMaterial(const Material* const __restrict material)
  {
    materials.push_back(*material);//std::move(material[0]));
  }

  _declspec(restrict)Material*const __restrict GetMaterial(const DWORD id, DWORD* index)
  {
    for(DWORD i=0; i<materials.size(); i++)
    {
      if(id == materials[i].GetMatId())
      {
        *index = i;
        return &materials[i];
      }
    }
    return NULL;
  }

  _declspec(restrict)Material*const __restrict GetMaterial(const Checksum checksum)
  {
    for(DWORD i=0; i<materials.size(); i++)
    {
      if(checksum.checksum == materials[i].GetMatId())
      {
        return &materials[i];
      }
    }
    return NULL;
  }

  inline _declspec(restrict)Material* const __restrict GetMaterial(const DWORD index)
  {
    if(index<materials.size())
    {
      return &materials[index];
    }
    return NULL;
  }

  vector <Material> GetMaterials()
  {
    return materials;
  }

  void Reserve(const DWORD numMaterials)
  {
    materials.reserve(numMaterials);
  }

private:
  vector <Material> materials;
};

struct SimplePolygon
{
  WORD a;
  WORD b;
  WORD c;

  SimplePolygon()
  {
    a=0;
    b=0;
    c=0;
  }

  SimplePolygon(WORD &a, WORD &b, WORD &c)
  {
    this->a = a;
    this->b = b;
    this->c = c;
  }
};

struct MaterialSplit;
struct Mesh;
struct SelectedMaterial
{
  Mesh* mesh;
  MaterialSplit* split;

  SelectedMaterial(Mesh* mesh, MaterialSplit* s)
  {
    this->mesh=mesh;
    split=s;
  }
};

struct MaterialSplit
{
  union
  {
    DWORD type;//thps3: 1=tristrip 0 = trilist thps4: 0=unknown 1024=unknown
    IDirect3DIndexBuffer9 *indices;
  };
  union
  {
    DWORD matId;
    Material* material;
  };
  DWORD numIndices;//numTriangles for rendering
  vector <WORD> Indices;

  __declspec (noalias) void RenderSelected(MaterialList* const __restrict materialList, const DWORD numVertices) const//render opaque
  {
    extern IDirect3DDevice9Ex* __restrict Device;
    register IDirect3DDevice9Ex* const __restrict Dev = Device;
    register MaterialList* const __restrict matList = materialList;

    Dev->SetIndices(indices);

    register Material* const __restrict tmpMat = matList->GetMaterial(matId);
    if(tmpMat)
    {
      const Texture*  __restrict const tex = tmpMat->GetTexture(0);
      Dev->SetSamplerState(0, D3DSAMP_ADDRESSU, tex->uAddress);
      Dev->SetSamplerState(0, D3DSAMP_ADDRESSV, tex->vAddress);
      Dev->SetTexture(0, tex->GetImage());
      if(tmpMat->animated)
      {
        extern DWORD numAnimations;
        extern UVAnim* __restrict animations;
        const UVAnim const* __restrict const anims = animations;
        const Animation* __restrict anim=NULL;
        for(DWORD j=0, numAnims = numAnimations; j<numAnims; j++)
        {
          if(anims[j].matId==tmpMat->GetMatId())
          {
            anim=&anims[j].anim;
            break;
          }
        }
        if(anim)
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

          uoff	= ( t * anim->UVel ) + ( anim->UAmplitude * sinf( anim->UFrequency * t + anim->UPhase ));
          voff	= ( t * anim->VVel ) + ( anim->VAmplitude * sinf( anim->VFrequency * t + anim->VPhase ));

          // Reduce offset mod 16 and put it in the range -8 to +8.
          /*uoff	+= 8.0f;
          uoff	-= (float)(( (int)uoff >> 4 ) << 4 );
          voff	+= 8.0f;
          voff	-= (float)(( (int)voff >> 4 ) << 4 );*/
          uoff -= (float)(int)uoff;
          voff -= (float)(int)voff;


          uv_mat._31 = uoff;//( uoff < 0.0f ) ? ( uoff + 8.0f ) : ( uoff - 8.0f );
          uv_mat._32 = voff;//( voff < 0.0f ) ? ( voff + 8.0f ) : ( voff - 8.0f );

          Dev->SetTransform((D3DTRANSFORMSTATETYPE)( D3DTS_TEXTURE0), &uv_mat );
        }
      }
    }
    else
    {
      Dev->SetTexture(0, NULL);
    }

    Dev->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, numVertices, 0, numIndices);
  }

  bool IsSelected()
  {
    extern vector<SelectedMaterial> selectedMaterials;
    for(DWORD i=0; i<selectedMaterials.size(); i++)
    {
      if(selectedMaterials[i].split==this)
        return true;
    }
    return false;
  }

  MaterialSplit()
  {
    type=0;
    matId=0;
    numIndices=0;
  }

  bool operator <(const MaterialSplit & other) const
  {
    extern MaterialList* __restrict globalMaterialList;
    Material* mat = globalMaterialList->GetMaterial(this->matId);
    Material* mat2 = globalMaterialList->GetMaterial(other.matId);
    if (mat == NULL)
      return true;
    if (mat2 == NULL)
      return false;
    return mat->drawOrder < mat2->drawOrder;
  }

  void AddIndices(const WORD* const __restrict indices, const DWORD numIndices, DWORD vOffset)
  {
    if(vOffset)
    {
      this->Indices.reserve(numIndices);

      for(DWORD i=0; i<numIndices; i++)
      {
        this->Indices.push_back((indices[i]+(WORD)vOffset));
      }
    }
    else
    {
      this->Indices.resize(numIndices);
      std::copy(indices, indices+numIndices, this->Indices.begin());
    }
  }

  void AddIndices(const WORD* const __restrict indices, const DWORD numIndices)
  {
    this->Indices.resize(numIndices);
    std::copy(indices, indices+numIndices, this->Indices.begin());
    //this->numIndices = numIndices-2;
  }

  vector <SimplePolygon> GetPolygons()
  {
    vector <SimplePolygon> polygons;
    WORD v1 = 0, v2 = 0, v3 = 0;
    bool tmp = false;

    for(DWORD i=0; i<numIndices; i++)
    {
      if(!tmp)
      {
        tmp=true;
        v1 = Indices[i];
        v2 = Indices[i+1];
        v3 = Indices[i+2];
        
      }
      else
      {
        tmp=false;
        v1 = v3;
        v3 = Indices[i+2];
      }
      if(v1 != v2 && v1 != v3 && v2 != v3)
      {
        polygons.push_back(SimplePolygon(v1,v2,v3));
      }
    }
    return polygons;
  }

  ~MaterialSplit()
  {
    if(indices)
      indices->Release();
    if(Indices.size())
      Indices.clear();
  }
};

struct Thug2MaterialSplit
{
  SimpleVertex bboxMin;
  SimpleVertex bboxMax;
  D3DXVECTOR3 sphereCenter;
  float sphereRadius;

  DWORD type;
  DWORD matId;
  DWORD numIndices;
  WORD* indices;
};

struct InterleavedVertexHeader
{
  DWORD flags;
  DWORD stride;
  DWORD numVertices;
  DWORD numSplits;
};

struct VertexData
{
  float x;
  float y;
  float z;

  float xNormal;
  float yNormal;
  float zNormal;

  Colour* colour;

  float u;
  float v;
};

struct InterleavedVertex
{
  DWORD size;
  DWORD numVertices;
  SimpleVertex* vertices;
  SimpleVertex* normals;
  Colour* colours;
  TexCoord* texCoords;
};
#endif