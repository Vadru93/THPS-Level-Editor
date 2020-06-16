#include "scene.h"
#define OnGround 0
#define InAir 1
#define OnWall 2
#define OnRail 3
#define OutSide 4

struct Player
{
  D3DXVECTOR3 position;
  D3DXVECTOR3 newPos;
  D3DXVECTOR3 velocity;
  DWORD state;
  const SimpleFace* hitFace;
  D3DXVECTOR3 faceNormal;
  float height;

  void Update(Scene* scene, float delta)
  {
    if (state == InAir)
      ApplyGravity(delta);
    else if (state == OnGround)
      velocity.y = 0;

    D3DXVECTOR3 distance = velocity*delta;
    newPos = position + distance;

    Ray ray(newPos, D3DXVECTOR3(0.0f, -1.0f, 0.0f));
    ray.origin.y += 1.0f;
    hitFace = NULL;
    if (state != OnRail)
    {
      height = FLT_MAX;
      state = scene->Collide(Ray(position, distance));// , newPos);
    }

    if (state == OnGround) 
    {
      extern bool jumped;
      jumped = false;
      if (faceNormal.y > -0.99 && faceNormal.y <= -0.65f && newPos.y > position.y)
      {
        D3DXVECTOR3 direction = newPos - position;

        float dist = D3DXVec3Length(&distance);//distance = old velocity

        D3DXVec3Normalize(&direction, &direction);
        direction *= dist;

        newPos = position + direction;
      }
    }

    if (state != OutSide && scene->AboveSurface(ray))
      position = newPos;
  }

  void ApplyGravity(float delta)
  {
    velocity.y -= 0.04f*delta;
  }
};