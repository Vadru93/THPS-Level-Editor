//=============================================================================
// Camera.cpp by Frank Luna (C) 2004 All Rights Reserved.
//=============================================================================
#include "stdafx.h"
#include "Camera.h"
#define DIRECTINPUT_VERSION 0x0800
#include "Scene.h"
#include "eventhandler.h"
#include "Player.h"
#pragma unmanaged
extern D3DVIEWPORT9 port; extern DIMOUSESTATE mousestate; extern bool resetMouse; extern HWND handleWindow; extern IDirect3DDevice9Ex* __restrict Device;

static DWORD middleX;
static DWORD middleY;
static float middleXf;
static float middleYf;

//__declspec(noalias) void DrawFrame();

Camera::Camera()
{
  D3DXMatrixIdentity(&mView);
  D3DXMatrixIdentity(&mProj);
  //D3DXMatrixIdentity(&mViewProj);

  mPosW   = D3DXVECTOR3(0.0f, 250.0f,-100.0f);
  mRightW = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
  mUpW    = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
  mLookW  = D3DXVECTOR3(0.0f, 10.0f, 0.0f);

  // Client should adjust to a value that makes sense for application's
  // unit scale, and the object the camera is attached to--e.g., car, jet,
  // human walking, etc.
  mSpeed  = 2.0f;
}

const D3DXMATRIX& Camera::view() const
{
  return mView;
}

const D3DXMATRIX& Camera::proj() const
{
  return mProj;
}

/*const D3DXMATRIX& Camera::viewProj() const
{
  return mViewProj;
}*/

const D3DXVECTOR3& Camera::right() const
{
  return mRightW;
}

const D3DXVECTOR3& Camera::up() const
{
  return mUpW;
}

const D3DXVECTOR3& Camera::look() const
{
  return mLookW;
}

D3DXVECTOR3& Camera::pos()
{
  return mPosW;
}

void Camera::lookAt(D3DXVECTOR3& pos, D3DXVECTOR3& target, D3DXVECTOR3& up)
{
  D3DXVECTOR3 L = target - pos;
  D3DXVec3Normalize(&L, &L);

  D3DXVECTOR3 R;
  D3DXVec3Cross(&R, &up, &L);
  D3DXVec3Normalize(&R, &R);

  D3DXVECTOR3 U;
  D3DXVec3Cross(&U, &L, &R);
  D3DXVec3Normalize(&U, &U);

  mPosW   = pos;
  mRightW = R;
  mUpW    = U;
  mLookW  = L;

  buildView();

  //mViewProj = mView * mProj;
}

void Camera::setLens(float fov, float aspect, float nearZ, float farZ)
{
  D3DXMatrixPerspectiveFovLH(&mProj, fov, aspect, nearZ, farZ);
  //mViewProj = mView * mProj;

  middleX = port.Width/2;
  middleY = port.Height/2;
  middleXf = (float)port.Width/2.0f;
  middleYf = (float)port.Height/2.0f;
}

void Camera::setSpeed(float s)
{
  mSpeed = s;
}

void Camera::updateFPS()
{
  extern Player* __restrict player;
  mPosW = player->position;
  mPosW.y += 50.0f;

  float pitch = 0.0f, yAngle = 0.0f;

  pitch = mousestate.lY / 150.0f;
  yAngle = mousestate.lX / 150.0f;

  mousestate.lX = 0;
  mousestate.lY = 0;
  POINT mouse;
  mouse.x = middleX;
  mouse.y = middleY;
  ClientToScreen(handleWindow, &mouse);
  SetCursorPos(mouse.x, mouse.y);

  D3DXMATRIX R;
  D3DXMatrixRotationAxis(&R, &mRightW, pitch);
  D3DXVec3TransformCoord(&mLookW, &mLookW, &R);
  D3DXVec3TransformCoord(&mUpW, &mUpW, &R);

  D3DXMatrixRotationY(&R, yAngle);
  D3DXVec3TransformCoord(&mRightW, &mRightW, &R);
  D3DXVec3TransformCoord(&mUpW, &mUpW, &R);
  D3DXVec3TransformCoord(&mLookW, &mLookW, &R);

  buildView();

  //mViewProj = mView * mProj;
  Device->SetTransform(D3DTS_VIEW, &mView);
  //DrawFrame();
}

void Camera::update(const float dt, const BYTE command)
{
  D3DXVECTOR3 dir(0.0f, 0.0f, 0.0f);

  if(command & FORWARD)
    dir += mLookW;
  else if(command & BACKWARD)
    dir-= mLookW;
  if(command & LEFT)
    dir -= mRightW;
  else if(command & RIGHT)
    dir += mRightW;

  D3DXVec3Normalize(&dir, &dir);
  mPosW += dir/**mSpeed*/*dt;

  float pitch=0.0f, yAngle=0.0f;

  if(resetMouse)
  {
    pitch = (float)mousestate.lY / 20.0f;
    yAngle = (float)mousestate.lX / 20.0f;

    mousestate.lX = 0;
    mousestate.lY = 0;
    POINT mouse;
    mouse.x = middleX;
    mouse.y = middleY;
    ClientToScreen(handleWindow, &mouse);
    SetCursorPos(mouse.x,mouse.y);
  }
  else if(mousestate.rgbButtons[1]){
    pitch = (float)mousestate.lY / 20.0f;
    yAngle = (float)mousestate.lX / 20.0f;
  }

  D3DXMATRIX R;
  D3DXMatrixRotationAxis(&R, &mRightW, D3DXToRadian(pitch));
  D3DXVec3TransformCoord(&mLookW, &mLookW, &R);
  D3DXVec3TransformCoord(&mUpW, &mUpW, &R);

  D3DXMatrixRotationY(&R, D3DXToRadian(yAngle));
  D3DXVec3TransformCoord(&mRightW, &mRightW, &R);
  D3DXVec3TransformCoord(&mUpW, &mUpW, &R);
  D3DXVec3TransformCoord(&mLookW, &mLookW, &R);

  buildView();

  //mViewProj = mView * mProj;
  Device->SetTransform(D3DTS_VIEW,&mView);
  //DrawFrame();
}

void Camera::buildView()
{
  D3DXVec3Normalize(&mLookW, &mLookW);

  D3DXVec3Cross(&mUpW, &mLookW, &mRightW);
  D3DXVec3Normalize(&mUpW, &mUpW);

  D3DXVec3Cross(&mRightW, &mUpW, &mLookW);
  D3DXVec3Normalize(&mRightW, &mRightW);

  const float x = -D3DXVec3Dot(&mPosW, &mRightW);
  const float y = -D3DXVec3Dot(&mPosW, &mUpW);
  const float z = -D3DXVec3Dot(&mPosW, &mLookW);

  mView(0,0) = mRightW.x; 
  mView(1,0) = mRightW.y; 
  mView(2,0) = mRightW.z; 
  mView(3,0) = x;

  mView(0,1) = mUpW.x;
  mView(1,1) = mUpW.y;
  mView(2,1) = mUpW.z;
  mView(3,1) = y;  

  mView(0,2) = mLookW.x; 
  mView(1,2) = mLookW.y; 
  mView(2,2) = mLookW.z; 
  mView(3,2) = z;   

  mView(0,3) = 0.0f;
  mView(1,3) = 0.0f;
  mView(2,3) = 0.0f;
  mView(3,3) = 1.0f;
}
