#include "stdafx.h"
#include "Material.h"
#pragma unmanaged
extern MaterialList* __restrict globalMaterialList; extern IDirect3DDevice9Ex* __restrict Device; extern DWORD MAX_PASSES;


/*bool MaterialSplit::operator <(const MaterialSplit & other) const
{
  if(globalMaterialList->GetMaterial(matId)==NULL)
    return true;
  else if(globalMaterialList->GetMaterial(other.matId)==NULL)
    return false;
  else if(globalMaterialList->GetMaterial(matId)->drawOrder==globalMaterialList->GetMaterial(other.matId)->drawOrder)
  {
    if(globalMaterialList->GetMaterial(matId)->transparent==globalMaterialList->GetMaterial(other.matId)->transparent)
      return false;
    else if(!globalMaterialList->GetMaterial(matId)->transparent)
      return false;
    else if(!globalMaterialList->GetMaterial(other.matId)->transparent)
      return true;
    else
      return false;
  }
    return globalMaterialList->GetMaterial(matId)->drawOrder < globalMaterialList->GetMaterial(other.matId)->drawOrder;
}*/

__declspec (noalias) static const inline void SetBlendMode(const DWORD blendMode)
{
  register IDirect3DDevice9Ex* const __restrict Dev = Device;
  //__assume(blendMode == 0 || blendMode == 1 || blendMode == 2 || blendMode == 3 || blendMode == 4 || blendMode == 5 || blendMode == 6 || blendMode == 7 || blendMode == 8 || blendMode == 9 || blendMode == 10 || blendMode == 11 || blendMode == 12 || blendMode == 13 || blendMode == 14 || blendMode == 15 || blendMode == 16 || blendMode == 17 || blendMode == 20);
  switch( blendMode )
  {
  case 0:
    {
      Dev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
      Dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
      Dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
      break;
    }
  case 2:
  case 1:
    {
      Dev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
      Dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
      Dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
      break;
    }
  case 4:
  case 3:
    {
      Dev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT);
      Dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
      Dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
      break;
    }
    /*case 5:
    case 6:
    {
    Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
    Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    break;
    }*/
  case 8:
  case 7:
    {
      Dev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
      Dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
      Dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCALPHA);
      break;
    }
  case 10:
  case 9:
    {
      Dev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
      Dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
      Dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
      break;
    }
  case 11:
    {
      Dev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
      Dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
      Dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
      break;
    }
  case 12:
    {
      Dev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
      Dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTALPHA);
      Dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVDESTALPHA);
      break;
    }
  case 13:
    {
      Dev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
      Dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_INVDESTALPHA);
      Dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_DESTALPHA);
      break;
    }
  case 15:
    {
      Dev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
      Dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
      Dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);
      break;
    }

  case 17:
    {
      Dev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
      Dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
      Dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
      break;
    }
  default:
    {
      __assume(0);
      Dev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
      Dev->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
      Dev->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
      break;
    }
  }
}

__declspec (noalias) void Texture::CreateTexture()
{
  imageData=NULL;
  static char imageName[MAX_PATH] = "";
  static char tmp[15] = "";
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
  //HRESULT hres = D3DXCreateTextureFromFile(Device, imageName, &imageData);
  HRESULT hres = D3DXCreateTextureFromFileEx(Device, imageName, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_FROM_FILE, D3DUSAGE_DYNAMIC, D3DFMT_FROM_FILE, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &imageData);
  if(FAILED(hres))
  {
    //MessageBox(0,imageName,"failed to create texture",0);
    //ZeroMemory(imageName,MAX_PATH);
    //ZeroMemory(tmp,15);
    GetEnvironmentVariable("TEMP",imageName, MAX_PATH);
    if(name == NULL)
    {
      sprintf(tmp,"\\%08X.tga\0",textureId.checksum);
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
    /*if(FAILED(*/D3DXCreateTextureFromFileEx(Device, imageName, D3DX_DEFAULT, D3DX_DEFAULT, 1, D3DUSAGE_DYNAMIC, D3DFMT_FROM_FILE, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &imageData);//))
    //MessageBox(0,"Failed to load texture",imageName,0);
  }
}

_declspec (noalias) void Material::SubmitPass()
{
  extern BYTE pass;
  register IDirect3DDevice9Ex* const __restrict Dev = Device;
  Dev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
  Dev->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
  Dev->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
  Dev->SetTexture(pass, GetTexturePass(pass));
}

__declspec (noalias) void Material::SubmitSelected()
{
  register IDirect3DDevice9Ex* const __restrict Dev = Device;

  register DWORD i = 0;//MAX_PASSES;

  __assume(i==0);
  Dev->SetSamplerState(i, D3DSAMP_ADDRESSU, textures[i].uAddress);
  Dev->SetSamplerState(i, D3DSAMP_ADDRESSV, textures[i].vAddress);
  Dev->SetTexture(i, textures[i].GetImage());
  //SetBlendMode(18/*textures[i].blendMode*/);
  Dev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
  Dev->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
  Dev->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
}

__declspec (noalias) void Material::Submit()
{
  static bool animatedLast=false;
  static D3DMATRIX uv_mat = { 1.0f, 0.0f, 0.0f, 0.0f,
								0.0f, 1.0f, 0.0f, 0.0f,
								0.0f, 0.0f, 0.0f, 0.0f,
								0.0f, 0.0f, 0.0f, 0.0f };
  register IDirect3DDevice9Ex* const __restrict Dev = Device;
  /*static bool restore = false;
  if(restore)
  {
    restore=false;
    Dev->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHA );
    Dev->SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHA );
    Dev->SetTextureStageState( 3, D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHA );
  }*/

  //register const DWORD numTextures = textures.size();
  register DWORD i = 0;//MAX_PASSES;
  /*__assume(i>0);

  while(i)
  {
    if(i<numTextures)
    {
      Dev->SetSamplerState(i, D3DSAMP_ADDRESSU, textures[i].uAddress);
      Dev->SetSamplerState(i, D3DSAMP_ADDRESSV, textures[i].vAddress);
      Dev->SetTexture(i, textures[i].GetImage());

      if(textures[i].fixedAlpha != 0)
      {
        Dev->SetRenderState(D3DRS_TEXTUREFACTOR, textures[i].fixedAlpha);
        Dev->SetTextureStageState(i, D3DTSS_COLOROP, D3DTOP_BLENDFACTORALPHA);
        restore=true;
      }
    }
    else
      Dev->SetTexture(i, NULL);
    i--;
  }*/

  /*while(i>=numTextures)
  {
    Dev->SetTexture(i, NULL);
    i--;
  }
  while(i)
  {
    Dev->SetSamplerState(i, D3DSAMP_ADDRESSU, textures[i].uAddress);
    Dev->SetSamplerState(i, D3DSAMP_ADDRESSV, textures[i].vAddress);
    Dev->SetTexture(i, textures[i].GetImage());

    if(textures[i].fixedAlpha != 0)
    {
      Dev->SetRenderState(D3DRS_TEXTUREFACTOR, textures[i].fixedAlpha);
      Dev->SetTextureStageState(i, D3DTSS_COLOROP, D3DTOP_BLENDFACTORALPHA);
      restore=true;
    }
    i--;
  }*/

  __assume(i==0);
  Dev->SetSamplerState(i, D3DSAMP_ADDRESSU, textures[i].uAddress);
  Dev->SetSamplerState(i, D3DSAMP_ADDRESSV, textures[i].vAddress);
  Dev->SetTexture(i, textures[i].GetImage());
  if(animated)
  {
    extern DWORD numAnimations;
    extern UVAnim* __restrict animations;
    const UVAnim const* __restrict const anims = animations;
    const Animation* __restrict anim=NULL;
    for(DWORD j=0, numAnims = numAnimations; j<numAnims; j++)
    {
      if(anims[j].matId==materialId)
      {
        anim=&anims[j].anim;
        break;
      }
    }
    if(anim)
    {
      animatedLast=true;
      Dev->SetTextureStageState(i, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

      extern float frameDelta;
      const float t = frameDelta;//const float t = (float)GetTickCount()* 0.001f;

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
  else if(animatedLast)
  {
    animatedLast=false;
    Dev->SetTextureStageState(i, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
  }
  //SetBlendMode(18/*textures[i].blendMode*/);
}


/*__declspec (noalias) void Material::Submit()
{*/
  //SetBlendMode(textures[0].blendMode);currently

  /*Dev->SetSamplerState(0, D3DSAMP_ADDRESSU, textures[0].uAddress);
  Dev->SetSamplerState(0, D3DSAMP_ADDRESSV, textures[0].vAddress);
  Dev->SetTexture(0, textures[0].GetImage());*/
  /*const DWORD numTextures = textures.size();
  for(DWORD i=0, numPasses = MAX_PASSES; i<numPasses; i++)
  {
    if(i)
    {
      if(i<numTextures)
      {
        if(textures[i].fixedAlpha != 0)
        {
          Dev->SetRenderState(D3DRS_TEXTUREFACTOR, textures[i].fixedAlpha);
          Dev->SetTextureStageState(i, D3DTSS_COLOROP, D3DTOP_BLENDFACTORALPHA);
          restore=true;
        }

        Dev->SetSamplerState(i, D3DSAMP_ADDRESSU, textures[i].uAddress);
        Dev->SetSamplerState(i, D3DSAMP_ADDRESSV, textures[i].vAddress);
        Dev->SetTexture(i, textures[i].GetImage());
      }
      else
        Dev->SetTexture(i, NULL);
    }
    else
    {
      Dev->SetSamplerState(i, D3DSAMP_ADDRESSU, textures[i].uAddress);
      Dev->SetSamplerState(i, D3DSAMP_ADDRESSV, textures[i].vAddress);
      Dev->SetTexture(i, textures[i].GetImage());
    }currently*/

    /*if(i<numTextures)
    {
    if(i)
    {
    //SetBlendMode(textures[i].blendMode);
    if(textures[i].fixedAlpha)
    {
    Dev->SetRenderState(D3DRS_TEXTUREFACTOR, textures[i].fixedAlpha);
    Dev->SetTextureStageState( i, D3DTSS_COLOROP, D3DTOP_BLENDFACTORALPHA );
    restore=true;
    }
    /*else if(textures[i].blendMode==3)
    Device->SetTextureStageState( i, D3DTSS_COLOROP, D3DTOP_SUBTRACT);*/
    //}

    /* Dev->SetSamplerState(i, D3DSAMP_ADDRESSU, textures[i].uAddress);
    Dev->SetSamplerState(i, D3DSAMP_ADDRESSV, textures[i].vAddress);
    Dev->SetTexture(i, textures[i].GetImage());
    }
    else
    Dev->SetTexture(i, NULL);*/
  //}currently
//}