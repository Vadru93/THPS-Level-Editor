#include "Structs.h"
#include "Includes.h"
#ifndef _NODE_H
#define _NODE_H
#pragma unmanaged
using std::vector;
struct Node;
struct NodeInfo
{
  Node* node;
  Checksum name;
};

struct ColouredVertex
{
  float x;
  float y;
  float z;

  Colour colour;
  ColouredVertex(const BYTE r, const BYTE g, const BYTE b)
  {
    x=0;
    y=0;
    z=0;

    colour.r=r;
    colour.g=g;
    colour.b=b;
    colour.a=255;
  }
};

struct AddingRail
{
  D3DXVECTOR3 anchor;
  Colour colour1;
  D3DXVECTOR3 hit;
  Colour colour2;
  bool addedAnchor;
  AddingRail()
  {
    colour1 = Colour(255, 0, 0);
    colour2 = Colour(255, 0, 0);
    addedAnchor=false;
  }
};

struct RenderableRail
{
  IDirect3DVertexBuffer9* verts;
  IDirect3DIndexBuffer9* indices;
  DWORD numVerts;
  DWORD numIndices;
  vector <Checksum> nodes;
  vector <ColouredVertex> vertices;
  vector <WORD> Indices;

  RenderableRail()
  {
    numIndices=0;
    numVerts=0;
    verts=NULL;
    indices=NULL;
  }

  void AddRail(AddingRail rail)
  {
    if (verts)
      verts->Release();
    if (indices)
      indices->Release();
    if (numIndices)
      numIndices /= 2;

    static ColouredVertex vertex = ColouredVertex(0, 255, 0);

    vertex.x = rail.anchor.x;
    vertex.y = rail.anchor.y + 2.0f;
    vertex.z = rail.anchor.z;
    float MAX_DIST = rail.anchor.x - rail.hit.x;
    MAX_DIST *= MAX_DIST;
    static bool axis = false;
    if (((rail.anchor.z - rail.hit.z)*(rail.anchor.z - rail.hit.z))>MAX_DIST)
      axis = true;
    else
      axis = false;

    vertex.x = rail.hit.x;
    vertex.y = rail.hit.y + 2.0f;
    vertex.z = rail.hit.z;
    if (axis)
      vertex.x -= 2.0f;
    else
      vertex.z -= 2.0f;

    vertices.push_back(vertex);

    vertex.y -= 4.0f;
    vertices.push_back(vertex);

    if (axis)
      vertex.x += 4.0f;
    else
      vertex.z += 4.0f;
    vertices.push_back(vertex);

    vertex.y += 4.0f;
    vertices.push_back(vertex);

    Indices.push_back(numIndices + 1);
    Indices.push_back(numIndices + 4);
    Indices.push_back(numIndices + 5);
    Indices.push_back(numIndices + 1);
    Indices.push_back(numIndices);
    Indices.push_back(numIndices + 4);
    Indices.push_back(numIndices);
    Indices.push_back(numIndices + 4);
    Indices.push_back(numIndices + 3);
    Indices.push_back(numIndices + 3);
    Indices.push_back(numIndices + 4);
    Indices.push_back(numIndices + 7);
    Indices.push_back(numIndices + 3);
    Indices.push_back(numIndices + 6);
    Indices.push_back(numIndices + 7);
    Indices.push_back(numIndices + 6);
    Indices.push_back(numIndices + 3);
    Indices.push_back(numIndices + 2);
    Indices.push_back(numIndices + 1);
    Indices.push_back(numIndices + 5);
    Indices.push_back(numIndices + 6);
    Indices.push_back(numIndices + 6);
    Indices.push_back(numIndices + 1);
    Indices.push_back(numIndices + 2);

    numVerts = vertices.size();
    numIndices = Indices.size();
  }

  RenderableRail(AddingRail rail, bool green = false)
  {
    numIndices=0;
    numVerts=0;
    verts=NULL;
    indices=NULL;

    ColouredVertex vertex = ColouredVertex(0, 0, 255);
    if (green)
      vertex = ColouredVertex(0, 255, 0);

    vertex.x=rail.anchor.x;
    vertex.y=rail.anchor.y+2.0f;
    vertex.z=rail.anchor.z;
    float MAX_DIST = rail.anchor.x-rail.hit.x;
    MAX_DIST *= MAX_DIST;
    static bool axis = false;
    if(((rail.anchor.z-rail.hit.z)*(rail.anchor.z-rail.hit.z))>MAX_DIST)
      axis = true;
    else
      axis = false;
    if(axis)
      vertex.x-=2.0f;
    else
      vertex.z-=2.0f;

    vertices.push_back(vertex);

    vertex.y -= 4.0f;
    vertices.push_back(vertex);

    if(axis)
      vertex.x+=4.0f;
    else
      vertex.z+=4.0f;
    vertices.push_back(vertex);

    vertex.y += 4.0f;
    vertices.push_back(vertex);


    vertex.x=rail.hit.x;
    vertex.y=rail.hit.y+2.0f;
    vertex.z=rail.hit.z;
    if(axis)
      vertex.x-=2.0f;
    else
      vertex.z-=2.0f;

    vertices.push_back(vertex);

    vertex.y -= 4.0f;
    vertices.push_back(vertex);

    if(axis)
      vertex.x+=4.0f;
    else
      vertex.z+=4.0f;
    vertices.push_back(vertex);

    vertex.y += 4.0f;
    vertices.push_back(vertex);

    Indices.push_back(numIndices+1);
    Indices.push_back(numIndices+4);
    Indices.push_back(numIndices+5);
    Indices.push_back(numIndices+1);
    Indices.push_back(numIndices);
    Indices.push_back(numIndices+4);
    Indices.push_back(numIndices);
    Indices.push_back(numIndices+4);
    Indices.push_back(numIndices+3);
    Indices.push_back(numIndices+3);
    Indices.push_back(numIndices+4);
    Indices.push_back(numIndices+7);
    Indices.push_back(numIndices+3);
    Indices.push_back(numIndices+6);
    Indices.push_back(numIndices+7);
    Indices.push_back(numIndices+6);
    Indices.push_back(numIndices+3);
    Indices.push_back(numIndices+2);
    Indices.push_back(numIndices+1);
    Indices.push_back(numIndices+5);
    Indices.push_back(numIndices+6);
    Indices.push_back(numIndices+6);
    Indices.push_back(numIndices+1);
    Indices.push_back(numIndices+2);

    numVerts = vertices.size();
    numIndices = Indices.size();
  }

  ~RenderableRail()
  {
    if(verts)
      verts->Release();
    if(indices)
      indices->Release();
    nodes.clear();
  }
};

struct Node
{
public:
  SimpleVertex Position;
  SimpleVertex Angles;
  Checksum Name;
  Checksum Class;
  Checksum Type;
  Checksum RestartName;
  Checksum Cluster;
  Checksum Trigger;
  vector <WORD> Links;
  Checksum TerrainType;
  //WORD type;
  Checksum Compressed;
  bool CreatedAtStart;
  bool AbsentInNetGames;
  bool NetEnabled;
  bool Permanent;
  bool TrickObject;

  mutable bool checked;
  //vector <BYTE> IgnoredLights;

  Node()
  {
    ZeroMemory(&this->Position, sizeof(SimpleVertex) * 2 + sizeof(Checksum) * 6);
    ZeroMemory(&this->Compressed, 10);
    //ZeroMemory(this,sizeof(Node));
  }

  inline const bool IsSelected() const
  {
    extern vector <Node*> selectedNodes;
    const DWORD numSelected = selectedNodes.size();

    for(DWORD i=0; i<numSelected; i++)
    {
      if(selectedNodes[i]==this)
        return true;
    }
    return false;
  }

  inline bool IsAdded()
  {
    extern vector <RenderableRail> rails;

    for(DWORD i=0, numRails; i<rails.size(); i++)
    {
      for(DWORD j=0, numNodes = rails[i].nodes.size(); j<numNodes; j++)
      {
        if(rails[i].nodes[j].checksum==Name.checksum)
          return true;
      }
    }
    return false;
  }

  const D3DXVECTOR3 &GetAngle() const
  {
    return *(D3DXVECTOR3*)&Angles;
  }

  const D3DXVECTOR3 &GetPosition() const
  {
    return *(D3DXVECTOR3*)&Position;
  }

  void SetPosition(const D3DXVECTOR3 &pos)
  {
    Position.x = pos.x;
    Position.y = pos.y;
    Position.z = pos.z;
  }

  void SetTrigger(const DWORD trigger)
  {
    Trigger.checksum = trigger;
  }

  bool FixCheck(DWORD checksum, Node* const __restrict nodes)// const
  {
    if(checked)
    {
      checked = false;
      for(DWORD i=0; i<Links.size(); i++)
      {
        if(nodes[Links[i]].Class.checksum == checksum)
          return false;
      }
      for(DWORD i=0; i<Links.size(); i++)
      {
        if (nodes[Links[i]].Links.size() && nodes[Links[i]].Class.checksum == 0xDABD3086 && !nodes[Links[i]].CreatedAtStart && nodes[Links[i]].FixCheck(checksum, nodes))
          return false;
      }
    }
    return false;
  }

  void ApplyCluster(Node* const __restrict nodes);

  bool FixObCheck(Node* const __restrict nodes)// const
  {
    if (checked)
    {
      checked = false;
      DWORD numLinks = Links.size();
      for (DWORD i = 0; i < numLinks; i++)
      {
        if (nodes[Links[i]].TrickObject && nodes[Links[i]].Cluster.checksum)
          return false;
      }
      for (DWORD i = 0; i < numLinks; i++)
      {
        if (nodes[Links[i]].Links.size() && (nodes[Links[i]].Class.checksum == 0xDABD3086 || nodes[Links[i]].Class.checksum == 0x8E6B02AD) && !nodes[Links[i]].checked && nodes[Links[i]].FixObCheck(nodes))
          return false;
      }
    }
    return false;
  }

  bool LinksToTrickOb(Node* const __restrict nodes, DWORD* cluster)// const
  {
    if (!checked)
    {
      checked = true;
      DWORD numLinks = Links.size();
      for (DWORD i = 0; i < numLinks; i++)
      {
        if (nodes[Links[i]].TrickObject && nodes[Links[i]].Class.checksum == 0xDABD3086 && nodes[Links[i]].Cluster.checksum)
        {
          *cluster = nodes[Links[i]].Cluster.checksum;
          checked = false;
          return true;
        }
      }
      for (DWORD i = 0; i < numLinks; i++)
      {
        if (nodes[Links[i]].Links.size() && (nodes[Links[i]].Class.checksum == 0xDABD3086 || nodes[Links[i]].Class.checksum == 0x8E6B02AD) && !nodes[Links[i]].checked && nodes[Links[i]].LinksToTrickOb(nodes, cluster))
        {
          checked = false;
          return true;
        }
      }
    }
    else
      checked = false;
    return false;
  }

  bool LinksTo(DWORD checksum, WORD* const __restrict link, Node* const __restrict nodes)// const
  {
    if(!checked)
    {
      checked=true;
      DWORD numLinks = Links.size();
      for(DWORD i=0; i<numLinks; i++)
      {
        if(nodes[Links[i]].Class.checksum == checksum)
        {
          *link = Links[i];
          checked = false;
          return true;
        }
      }
      for(DWORD i=0; i<numLinks; i++)
      {
        if (nodes[Links[i]].Links.size() && nodes[Links[i]].Class.checksum == 0xDABD3086 && !nodes[Links[i]].CreatedAtStart && !nodes[Links[i]].checked && nodes[Links[i]].LinksTo(checksum, link, nodes))
        {
          checked = false;
          return true;
        }
      }
    }
    else
      checked=false;
    return false;
  }

  void LoadNodeArray()
  {
  }

  ~Node()
  {
    Links.clear();
    //IgnoredLights.clear();
  }
};
static const Checksum rail = Checksum("RailNode");
struct RailNode : public Node
{
  enum Types
  {
    Conceret,
    Metal

  };

  RailNode(const D3DXVECTOR3 & position, Checksum name)
  {
    this->Position = *(SimpleVertex*)&position;
    this->TerrainType = Checksum("TERRAIN_DEFAULT");
    this->Name = name;
    this->Trigger.checksum = 0;
    
    this->Class = rail;
    CreatedAtStart = true;
    this->Compressed.checksum = 0;
  }

  RailNode(const D3DXVECTOR3 & position, Checksum name, DWORD link)
  {
    this->Position = *(SimpleVertex*)&position;
    this->TerrainType = Checksum("TERRAIN_DEFAULT");
    this->Name = name;
    Links.push_back(link);
    this->Class = rail;
    this->Trigger.checksum = 0;
    CreatedAtStart = true;
    this->Compressed.checksum = 0;
  }

  RailNode(const SimpleVertex &position, const SimpleVertex &angle, const Checksum name, const DWORD numLinks, const WORD* const __restrict links)
  {
    this->Position = position;
    this->Angles = angle;
    this->Name = name;
    this->Trigger.checksum = 0;
    for(DWORD i=0; i<numLinks; i++)//for(DWORD int i=((int)numLinks-1); i>=0; i--)
      Links.push_back(links[i]);
    this->Class = rail;
    CreatedAtStart=true;
    this->Compressed.checksum = 0;
  }
  static DWORD GetClass()
  {
    return rail.checksum;
  }
 // Checksum TerrainType;
 // Checksum TriggerScript;
};
static const Checksum env = Checksum("EnvironmentObject");
struct EnvironmentObject : public Node
{
  //Checksum TriggerScript;

  EnvironmentObject(const Checksum name, const WORD numLinks, const WORD* const __restrict links, bool CreatedAtStart = false, bool TRICKOB = false)
  {
    this->Name = name;
    this->Class = env;
    TrickObject = TRICKOB;
    this->Trigger.checksum = 0;
    //this->shatter.checksum = shatter;
    this->CreatedAtStart = CreatedAtStart;
    this->Compressed = 0;
    for(WORD i=0; i<numLinks; i++)
    {
      Links.push_back(links[i]);
    }
  }

  EnvironmentObject()
  {
    this->Class = env;
    this->Trigger.checksum = 0;
  }

  static DWORD GetClass()
  {
    return env.checksum;
  }
};
static const Checksum res = Checksum("Restart");
struct Restart : public Node
{
  enum Types
  {
    Generic,
    Player1,
    Player2,
    Multiplayer
  };
  enum RestartTypes
  {
    Multi,
    CTF_1,
    CTF_2,
    Horse


  };

  //vector <Checksum> restart_types;
  //Checksum TriggerScript;

  Restart()
  {
    //static const Checksum Class = Checksum("Restart");
    Position.y = 200;
    CreatedAtStart=true;
    Name = Checksum("TRG_Restart01");
    this->Class = res;
    RestartName = Checksum("P1: Restart");
    this->Compressed.checksum = 0;
    //restart_types.push_back(Checksum("Multiplayer"));
    //TriggerScript = Checksum("TRG_SpawnSkater");
  }

  Restart(const SimpleVertex &position, const SimpleVertex &angle, Checksum name,char* restartName, WORD numLinks, WORD* links)
  {
    static const Checksum Class = Checksum("Restart");
    CreatedAtStart=true;
    this->Class = Class;
    //restart_types.push_back(Checksum("Multiplayer"));
    //TriggerScript = Checksum("TRG_SpawnSkater");
    this->Position = position;
    this->Angles = angle;
    this->Name = name;
    this->RestartName = Checksum(restartName);
    this->Trigger.checksum = 0;
    for(WORD i=0; i<numLinks; i++)
    {
      Links.push_back(links[i]);
    }
    this->Compressed.checksum = 0;
  }

  static DWORD GetClass()
  {
    return res.checksum;
  }
};

struct GenericNode : Node
{
  enum Types
  {
    Crown,
    Zone,
    ZONE_KEY,
    CTF_1,
    CTF_2
  };
  //BYTE Zone_Multiplier;

  /*GenericNode(const SimpleVertex &position, const WORD type)
  {
    static const Checksum Class = Checksum("Powerup");
    this->Class = Class;
    this->Position = position;
    this->type=type;
  }*/

  GenericNode(const Checksum name,  const SimpleVertex &position, const Types type)
  {
    static const Checksum crown = Checksum("Crown");
    static const Checksum zone = Checksum("Zone");
    static const Checksum key = Checksum("ZONE_KEY");
    static const Checksum CTF1 = Checksum("CTF_1");
    static const Checksum CTF2 = Checksum("CTF_2");
    static const Checksum Class = Checksum("GenericNode");

    if(type==Crown)
      Type=crown;
    else if(type==Zone)
      Type=zone;
    else if(type==ZONE_KEY)
    {
      Type=key;
      //Zone_Multiplier=1;
    }
    else if(type==CTF_1)
      Type=CTF1;
    else if(type==CTF_2)
      Type=CTF2;
    Name = name;
    Position = position;
    CreatedAtStart=true;
    this->Class=Class;
    this->Compressed.checksum = 0;
  }
};

struct Pedstrain : Node
{
  enum Types
  {
    Pedstrain_A,
    Pedstrain_B,
    Pedstrain_C
  };
  Checksum model;
  Checksum model_lod1;
  BYTE lod_dist1;
  Checksum model_lod2;
  BYTE lod_dist2;
  Checksum model_lod3;
  BYTE lod_dist3;
  BYTE lod_dist4;
};

struct vehicle : Node
{
  enum Types
  {
    Gold,
    PoliceCar,
    Beig,
    Blue,
    Purpl,
    Low_Rider
  };
  Checksum model;
  Checksum model_lod1;
  BYTE lod_dist1;
  Checksum model_lod2;
  BYTE lod_dist2;
  BYTE lod_dist3;
  BYTE heightoffset;
};

struct GameObject : Node
{
  Checksum model;
  Checksum TriggerScript;
};

struct WayPoint : Node
{
};
#endif