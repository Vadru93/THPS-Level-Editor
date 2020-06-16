//=============================================================================
// Camera.h by Frank Luna (C) 2004 All Rights Reserved.
//=============================================================================

#ifndef CAMERA_H
#define CAMERA_H

#include <d3dx9.h>
#include <d3d9.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#pragma unmanaged
class Camera
{
public:
  Camera();

  const D3DXMATRIX& view() const;
  const D3DXMATRIX& proj() const;
  const D3DXMATRIX& viewProj() const;

  const D3DXVECTOR3& right() const;
  const D3DXVECTOR3& up() const;
  const D3DXVECTOR3& look() const;

  D3DXVECTOR3& pos();

  void lookAt(D3DXVECTOR3& pos, D3DXVECTOR3& target, D3DXVECTOR3& up);
  void setLens(float fov, float aspect, float nearZ, float farZ);
  void setSpeed(float s);

  void update(const float dt, const BYTE command);
  void updateFPS();
  void updateEditor(float dt, BYTE command);

protected:
  void buildView();

protected:
  D3DXMATRIX mView;
  D3DXMATRIX mProj;
  //D3DXMATRIX mViewProj;

  // Relative to world space.
  D3DXVECTOR3 mPosW;
  D3DXVECTOR3 mRightW;
  D3DXVECTOR3 mUpW;
  D3DXVECTOR3 mLookW;

  float mSpeed;
};

#endif // CAMERA_H