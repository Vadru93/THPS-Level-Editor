#pragma once
#pragma unmanaged
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d9.h>
#include "Th4Scene.h"
#include "Th2Scene.h"
#include "Th2xScene.h"
#include <windowsx.h>
#include <Commdlg.h>
#include <d3dx9.h>
#include "shlobj.h"
#include "Camera.h"
#include <dinput.h>
#include "eventhandler.h"
#include <io.h>
#include <fcntl.h>
#include <stdlib.h>
//#include "Debug.h"
#pragma managed


using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

public ref class NodeProperties
{
public:
  Node* node;

  NodeProperties(Node* node)
  {
    node=node;
  }

  [ReadOnly(false)]
  //[DisplayName("Checksum")]
  property String^ Name
  {
    String^ get()
    {
      return gcnew String(node->Class.GetString());
    }

    /*void set(String^ val)
    {
      DWORD checksum = System::UInt32::Parse(val, System::Globalization::NumberStyles::HexNumber);
      mesh->SetName(*(Checksum*)&checksum);
    }*/
  }

  [ReadOnly(false)]
  property int^ Type
  {
    int^ get()
    {
      return gcnew int(node->Type.checksum);
      MessageBox::Show("adding");
    }

    void set(int^ val)
    {
      node->Type.checksum=(int)val;
    }
  }


  [ReadOnly(false)]
  [Category("Position")]

  [Description("Position of the node")]
  property float^ X
  {
    float^ get()
    {
      return gcnew float(node->Position.x);
    }

    void set(float^ val)
    {
      node->Position.x = (float)val;
    }
  }

  [ReadOnly(false)]
  [Category("Position")]

  property float^ Y
  {
    float^ get()
    {
      return gcnew float(node->Position.y);
    }

    void set(float^ val)
    {
      node->Position.y = (float)val;
    }
  }

  [ReadOnly(false)]
  [Category("Position")]

  property float^ Z
  {
    float^ get()
    {
      return gcnew float(node->Position.z);
    }

    void set(float^ val)
    {
      node->Position.z = (float)val;
    }
  }

};

public ref class MeshProperties
{
public:
  Mesh* mesh;
  /*unsigned int checksum;

  float x;
  float y;
  float z;*/

  MeshProperties(Mesh* mesh)
  {
    //position = gcnew vertx();
    this->mesh=mesh;
    /*
    checksum = mesh->GetName().checksum;
    if(mesh && mesh->node)
    {
      /*position->x = mesh->node->GetPosition().x;
      position->y = mesh->node->GetPosition().y;
      position->z = mesh->node->GetPosition().z;
      x=mesh->node->GetPosition().x;
      y=mesh->node->GetPosition().y;
      z=mesh->node->GetPosition().z;
      //position = gcnew System::String(mesh->node->GetPosition().x.ToString() + mesh->node->GetPosition().y.ToString() + mesh->node->GetPosition().z.ToString());
    }
    else
    {
      x=0.0f;
      y=0.0f;
      z=0.0f;
    }*/
    /*else
    position = gcnew System::String("");*/
  }
  [ReadOnly(false)]
  [DisplayName("Checksum")]
  property String^ Name
  {
    String^ get()
    {
      return mesh->GetName().checksum.ToString("X");
    }

    void set(String^ val)
    {
      DWORD checksum = System::UInt32::Parse(val, System::Globalization::NumberStyles::HexNumber);
      mesh->SetName(*(Checksum*)&checksum);
    }
  }


  [ReadOnly(false)]
  [Category("Position")]

  [Description("Position of the object")]
  property float^ X
  {
    float^ get()
    {
      extern Scene* __restrict scene;
      return gcnew float(scene->GetNode(mesh->GetName())->GetPosition().x);
    }

    void set(float^ val)
    {
      extern Scene* __restrict scene;
      Node* node = scene->GetNode(mesh->GetName());
      mesh->SetPosition(D3DXVECTOR3(float(val), node->GetPosition().y, node->GetPosition().z));
    }
  }

  [ReadOnly(false)]
  [Category("Position")]

  property float^ Y
  {
    float^ get()
    {
      extern Scene* __restrict scene;
      return gcnew float(scene->GetNode(mesh->GetName())->GetPosition().y);
    }

    void set(float^ val)
    {
      extern Scene* __restrict scene;
      Node* node = scene->GetNode(mesh->GetName());
      mesh->SetPosition(D3DXVECTOR3(node->GetPosition().x, float(val), node->GetPosition().z));
    }
  }

  [ReadOnly(false)]
  [Category("Position")]

  property float^ Z
  {
    float^ get()
    {
      extern Scene* __restrict scene;
      return gcnew float(scene->GetNode(mesh->GetName())->GetPosition().z);
    }

    void set(float^ val)
    {
      extern Scene* __restrict scene;
      Node* node = scene->GetNode(mesh->GetName());
      mesh->SetPosition(D3DXVECTOR3(node->GetPosition().x, node->GetPosition().y, float(val)));
    }
  }
  /*property vertx^ Position
  {
  vertx^ get()
  {
  return position;
  }

  void set(vertx^ val)
  {
  position = val;
  }
  }*/
};

#pragma endregion
#pragma unmanaged