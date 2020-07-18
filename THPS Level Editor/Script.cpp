#include "Th4Scene.h"
#include "Scene.h"
#include "Mesh.h"
#include "Structs.h"
#include "Player.h"

using std::vector;
#pragma unmanaged
DWORD numFucked=0;
//#define remove

void Node::ApplyCluster(Node* const __restrict nodes)
{
  DWORD numLinks = Links.size();
  for (DWORD i = 0; i < numLinks; i++)
  {
    if (!nodes[Links[i]].Cluster.checksum && ((nodes[Links[i]].TrickObject && nodes[Links[i]].Class.checksum == EnvironmentObject::GetClass()) || nodes[Links[i]].Class.checksum == RailNode::GetClass()))
    {
      nodes[Links[i]].Cluster.checksum = this->Cluster.checksum;
      nodes[Links[i]].TrickObject = true;
      nodes[Links[i]].ApplyCluster(nodes);
    }
  }
}

__declspec (noalias) bool Scene::FixLinksTHUG()
{
  bool added=false;
  const DWORD oldCount = nodes.size();
#define noextras
#define reverser
#ifndef reverser
  for(DWORD i=0; i<nodes.size(); i++)
  {
#else
  DWORD i = oldCount;
  while(i != 0)
  {
    i--;
#endif
    if(nodes[i].Class.checksum == RailNode::GetClass())
    {
      if(nodes[i].Links.size())
      {
        for(DWORD j=0; j<nodes[i].Links.size(); j++)
        {
          if(nodes[nodes[i].Links[j]].Class.checksum == RailNode::GetClass() && nodes[nodes[i].Links[j]].CreatedAtStart && !nodes[nodes[i].Links[j]].AbsentInNetGames)
          {
            if(j==0)
            {
              __assume(j==0);
              if(nodes[nodes[i].Links[j]].checked)
              {
#ifdef noextras
                nodes[i].Links.erase(nodes[i].Links.begin()+j);
                j--;
#else
                const DWORD index = nodes.size();
                nodes.push_back(nodes[nodes[i].Links[j]]);
                char name[128];
                sprintf(name, "FedRn%u", numFucked);
                Node &node = nodes.back();
                node.Name = Checksum(name);
                node.Links.clear();
                nodes[i].Links[j] = index;
                numFucked++;
                added=true;
#endif
              }
              else
                nodes[nodes[i].Links[j]].checked = true;
            }
            else
            {
              nodes[i].Links.erase(nodes[i].Links.begin()+j);
              j--;
            }
          }
          else
          {
            nodes[i].Links.erase(nodes[i].Links.begin()+j);
            j--;
          }
        }
        if(nodes[i].Links.size() == 0)
        {
          DeleteNode(i);
#ifdef reverser
          i++;
#else
          i--;
#endif
        }
      }
    }
  }
  for(DWORD i=0, numNodes = nodes.size(); i<numNodes; i++)
  {
    nodes[i].checked=false;
  }
  return added;
}

__declspec (noalias) bool Scene::FixLinksFinal()
{
  bool added=false;
  const DWORD oldCount = nodes.size();
#undef noextras
#define reverser
#ifndef reverser
  for(DWORD i=0; i<nodes.size(); i++)
  {
#else
  DWORD i = oldCount;
  while(i != 0)
  {
    i--;
#endif
    if(nodes[i].Class.checksum == RailNode::GetClass())
    {
      if(nodes[i].Links.size())
      {
        for(DWORD j=0; j<nodes[i].Links.size(); j++)
        {
          if(nodes[nodes[i].Links[j]].Class.checksum == RailNode::GetClass() && nodes[nodes[i].Links[j]].CreatedAtStart && !nodes[nodes[i].Links[j]].AbsentInNetGames)
          {
            if(j==0)
            {
              __assume(j==0);
              if(nodes[nodes[i].Links[j]].checked)
              {
#ifdef noextras
                nodes[i].Links.erase(nodes[i].Links.begin()+j);
                j--;
#else
                const DWORD index = nodes.size();
                nodes.push_back(nodes[nodes[i].Links[j]]);
                char name[128];
                sprintf(name, "FedRn%u", numFucked);
                Node &node = nodes.back();
                node.Name = Checksum(name);
                node.Links.clear();
                nodes[i].Links[j] = index;
                numFucked++;
                added=true;
#endif
              }
              else
                nodes[nodes[i].Links[j]].checked = true;
            }
            else
            {
              nodes[i].Links.erase(nodes[i].Links.begin()+j);
              j--;
            }
          }
          else
          {
            nodes[i].Links.erase(nodes[i].Links.begin()+j);
            j--;
          }
        }
        if(nodes[i].Links.size() == 0)
        {
          DeleteNode(i);
#ifdef reverser
          i++;
#else
          i--;
#endif
        }
      }
    }
  }
  for(DWORD i=0, numNodes = nodes.size(); i<numNodes; i++)
  {
    nodes[i].checked=false;
  }
  return added;
}

__declspec (noalias) bool Scene::FixLinks()
{
  bool added=false;
  const DWORD oldCount = nodes.size();
#define noextras
#define reverser
#ifndef reverser
  for(DWORD i=0; i<nodes.size(); i++)
  {
#else
  DWORD i = oldCount;
  while(i != 0)
  {
    i--;
#endif
    if(nodes[i].Class.checksum == RailNode::GetClass())
    {
      if(nodes[i].Links.size())
      {
        for(DWORD j=0; j<nodes[i].Links.size(); j++)
        {
          if(nodes[nodes[i].Links[j]].Class.checksum == RailNode::GetClass() && nodes[nodes[i].Links[j]].CreatedAtStart && !nodes[nodes[i].Links[j]].AbsentInNetGames)
          {
            if(j==0)
            {
              __assume(j==0);
              if(nodes[nodes[i].Links[j]].checked)
              {
#ifdef noextras
                nodes[i].Links.erase(nodes[i].Links.begin()+j);
                j--;
#else
                const DWORD index = nodes.size();
                nodes.push_back(nodes[nodes[i].Links[j]]);
                char name[128];
                sprintf(name, "FedRn%u", numFucked);
                nodes.back().Name = Checksum(name);
                //nodes.back().Links.clear();
                nodes[i].Links[j] = index;
                numFucked++;
                added=true;
#endif
              }
              else
                nodes[nodes[i].Links[j]].checked = true;
            }
            else
            {


              WORD link = 0xFFFF;

              for(DWORD k=0; k<nodes.size(); k++)
              {
                if(nodes[k].Class.checksum == RailNode::GetClass())
                {
                  for(DWORD l=0; l<nodes[k].Links.size(); l++)
                  {
                    if(nodes[k].Links[l] == i)
                    {
                      link = k;
                      goto done;
                    }
                  }
                }
              }
done:
              if(link!=0xFFFF)
              {
                const D3DXVECTOR3 A = *(D3DXVECTOR3*)&nodes[i].Position;
                D3DXVECTOR3 B = *(D3DXVECTOR3*)&nodes[nodes[i].Links[j]];
                const D3DXVECTOR3 C = *(D3DXVECTOR3*)&nodes[link];

                float a = D3DXVec3Length(&(B-C));
                float b = D3DXVec3Length(&(A-B));
                const float c = D3DXVec3Length(&(A-C));

                const float aa = a*a;
                const float cc = c*c;

                const float jAngle = acosf((b*b + cc - aa)/2*b*c);

                B = *(D3DXVECTOR3*)&nodes[nodes[i].Links[0]];
                a = D3DXVec3Length(&(B-C));
                b = D3DXVec3Length(&(A-B));

                const float otherAngle = acosf((b*b + cc - aa)/2*b*c);

                if(jAngle<otherAngle)
                {
#ifndef noextras
                  if(nodes[nodes[i].Links[j]].Links.size())
                  {
                    const DWORD index = nodes[i].Links[j];
                    nodes.push_back(nodes[i]);
                    char name[128];
                    sprintf(name, "FedRn%u", numFucked);
                    Node &node = nodes.back();
                    node.Name = Checksum(name);
                    node.Links.clear();
                    node.Links.push_back((WORD)index);
                    numFucked++;
                    added=true;
                  }
                  else
                  {
                    const DWORD index = nodes.size();
                    nodes.push_back(nodes[i]);
                    char name[128];
                    sprintf(name, "FedRn%u", numFucked);
                    Node &node = nodes.back();
                    node.Name = Checksum(name);
                    node.Links.clear();
                    numFucked++;
                    added=true;
                    nodes[nodes[i].Links[j]].Links.push_back((WORD)index);
                  }
#endif
                  nodes[i].Links.erase(nodes[i].Links.begin()+j);
                  j--;
                }
                else
                {
#ifdef noextras
                  const WORD temp = nodes[i].Links[0];
                  nodes[i].Links.erase(nodes[i].Links.begin());
                  j--;
                  if(nodes[temp].Links.size())
                  {
                    const DWORD index = temp;
                    nodes.push_back(nodes[i]);
                    char name[128];
                    sprintf(name, "FedRn%u", numFucked);
                    Node &node = nodes.back();
                    node.Name = Checksum(name);
                    node.Links.clear();
                    node.Links.push_back((WORD)index);
                    numFucked++;
                    added=true;
                  }
                  else
                  {
                    const DWORD index = nodes.size();
                    nodes.push_back(nodes[i]);
                    char name[128];
                    sprintf(name, "FedRn%u", numFucked);
                    Node &node = nodes.back();
                    node.Name = Checksum(name);
                    node.Links.clear();
                    numFucked++;
                    added=true;
                    nodes[temp].Links.push_back((WORD)index);
                  }
                }
#endif

              }
              else
              {
                nodes[i].Links.erase(nodes[i].Links.begin()+j);
                j--;
              }
            }
          }
          else
          {
            nodes[i].Links.erase(nodes[i].Links.begin()+j);
            j--;
          }
        }
        if(nodes[i].Links.size() == 0)
        {
          DeleteNode(i);
#ifdef reverser
          i++;
#else
          i--;
#endif
        }
      }
    }
  }
  for(DWORD i=0, numNodes = nodes.size(); i<numNodes; i++)
  {
    nodes[i].checked=false;
  }
  return added;
}

const BYTE* AddParams(const BYTE* pFile, KnownScript* script, bool THUG = false)
{
  signed char id=0;
  while(true)
  {
    const BYTE op = *pFile;
    pFile++;
    script->Append(op);
    switch(op)
    {
    case 0x49:
    case 0x48:
    case 0x47:
      pFile += 2;
      break;
    case 0x3:
      id++;
      break;
    case 0x4:
      id--;
      break;
    case 0x24:
      if(id>0)
      {
          fprintf(stdout, "Script? %X\n", id);
        //MessageBox(0,"id","",0);
        script->size--;
        script->Append(0x4);
      }
      else
        script->size--;
      pFile--;
      return pFile;
    case 0x1:
      if(id>0)
      {
          fprintf(stdout, "End of Line? %X\n", id);
        script->size--;
        script->Append(0x4);
      }
      else
        script->size--;
      return pFile;
    case 0x1B:
    case 0x1C:
      DWORD len;
      len = *(DWORD*)(pFile)+4;
      script->Append(pFile, len);
      pFile+=len;
      break;
    case 0x2:
      script->size--;
      pFile+=4;
      if(id<=0)
        return pFile;
    case 0x1E:
      script->Append(pFile, 12);
      pFile+=12;
      break;
    case 0x16:
    case 0x17:
    case 0x1A:
    case 0x2E:
      script->Append((DWORD*)pFile);
      pFile+=4;
      break;
    case 0x40:
    case 0x2F:
    case 0x37:
      const DWORD numRands = *(DWORD*)pFile;
      script->Append(*(Checksum*)&numRands);
      pFile+=THUG?numRands*2+4:4;
      const DWORD size = numRands*4;
      fprintf(stdout, "adding RNd1\nsize %u num %u\n", size, numRands);
      script->Append(pFile, size);
      pFile+=size;
      break;
    }
  }
}

__declspec (noalias) void Scene::ParseQBTable(register const BYTE const* __restrict pFile, const BYTE const* __restrict eof)
{
  while(pFile<eof)
  {
    const BYTE opcode = *pFile;
    pFile++;
    switch(opcode)
    {
      /*case 0x1C:
      case 0x1B:
      pFile+=*(DWORD*)pFile+4;
      break;
      //case 0x18:
      case 0x17:
      case 0x1A:
      case 0x16:
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
      break;*/
    case 0x49:
    case 0x48:
    case 0x47:
      pFile += 2;
      break;
    case 0:
      return;
    case 0x2B:
      extern vector<Script> Scripts;
      DWORD check;
      check = *(DWORD*)pFile;
      pFile+=4;
      bool found=false;
      for(DWORD i=0; i<Scripts.size(); i++)
      {
        if(Scripts[i].checksum == check)
        {
          found=true;
          break;
        }
      }
      if(!found)
        Scripts.push_back((Script(check, (char*)pFile)));
      while(*pFile != 0x0)
        pFile++;
      pFile++;
      break;
    }
  }
}

void Scene::LoadSound(Checksum sound, vector<Script> &sounds)
{
  bool found=false;
  switch(sound.checksum)
  {
  case 0x24FFB92D:
    for (DWORD i = 0, numSounds = sounds.size(); i<numSounds; i++)
    {
      if (sound.checksum == sounds[i].checksum)
      {
        found = true;
        break;
      }
    }
    if (!found)
    {
      sounds.push_back(Script(0x24FFB92D, "shared\FallWater"));
      printf("Fallwater\n");
    }
    break;
  case 0xB0A2A4C5:
  case 0x1B3097CC:
    for(DWORD i=0, numSounds=sounds.size(); i<numSounds; i++)
    {
      if(sound.checksum==sounds[i].checksum)
      {
        found=true;
        break;
      }
    }
    if(!found)
      sounds.push_back(Script(0x1B3097CC, "shared\glasspane2x"));
    break;
  default:
    for(DWORD i=0, numSounds=sounds.size(); i<numSounds; i++)
    {
      if(sound.checksum==sounds[i].checksum)
      {
        found=true;
        break;
      }
    }
    if(!found)
      sounds.push_back(Script(sound.checksum, ""));
    break;

  }
}

__declspec (noalias)Node* Scene::FindCompressed(const DWORD qbKey, const BYTE const* __restrict pFile, const BYTE const* __restrict eof)
{
    const BYTE* const beg = pFile;

    DWORD checksum = 0;
    for (DWORD i = 0; i < compressedNodes.size(); i++)
    {
        if (compressedNodes[i].Name.checksum == qbKey)
            return &compressedNodes[i];
    }
    char unk[128] = "";
    /*sprintf(unk, "file %X", pFile);
    MessageBox(0, "FindCompressed", unk, 0);*/
    bool found = false;

    while (pFile < eof)
    {
        const BYTE opcode = *pFile;
        pFile++;
        switch (opcode)
        {
        case 0x49:
        case 0x48:
        case 0x47:
            pFile += 2;
            break;
        case 0x1C:
        case 0x1B:
            pFile += *(DWORD*)pFile + 4;
            break;
            //case 0x18:
        case 0x17:
        case 0x1A:
        case 0x2E:
        case 2:
            pFile += 4;
            break;
        case 1:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 0x2D:
        case 0x25:
        case 0x26:
        case 0x28:
        case 0x18:
        case 0x0E:
        case 0x0F:
        case 0x39:
        case 0x30:
        case 0x24:
        case 0x20:
        case 0x21:
        case 0x42:
            break;
        case 0x23:
            found = true;
            break;
        case 0x1F:
            pFile += 8;
            break;
        case 0x16:
            checksum = *(DWORD*)pFile;
            pFile += 4;
            if (found && checksum == qbKey)
            {
                compressedNodes.push_back(Node());
                Node& node = compressedNodes.back();
                node.Name.checksum = checksum;
                pFile += 2;
                while (*pFile == 0x1 || *pFile == 0x2)
                {
                    if (*pFile == 0x2)
                        pFile += 5;
                    else
                        pFile++;
                }

                while (pFile<eof)
                {
                    const DWORD op = *(BYTE*)pFile;
                    pFile++;
                    switch (op)
                    {
                    case 0x49:
                    case 0x48:
                    case 0x47:
                        pFile += 2;
                        break;
                    case 4:
                        return &node;
                    case 0x1C:
                    case 0x1B:
                        pFile += *(DWORD*)pFile + 4;
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
                        pFile += *(DWORD*)pFile * 6 + 4;
                        break;
                    case 0x2B:
                        return &node;
                        break;
                    case 0:
                        return &node;
                    case 0x16:
                        const DWORD key = *(DWORD*)pFile;
                        pFile += 4;
                        switch (key)
                        {
                        case Checksums::Class:
                            while (*pFile == 0x1 || *pFile == 0x2)
                            {
                                if (*pFile == 0x2)
                                    pFile += 5;
                                else
                                    pFile++;
                            }
                            pFile += 2;
                            node.Class = *(Checksum*)pFile;
                            pFile += sizeof(Checksum);
                            break;
                        case Checksums::Type:
                            while (*pFile == 0x1 || *pFile == 0x2)
                            {
                                if (*pFile == 0x2)
                                    pFile += 5;
                                else
                                    pFile++;
                            }
                            pFile += 2;
                            node.Type = *(Checksum*)pFile;
                            if (node.Type.checksum == Checksums::PedAI || node.Type.checksum == Checksums::DEFAULT)
                            {
                                node.Class.checksum = 0;
                            }
                            pFile += sizeof(Checksum);
                            break;
                        case Checksums::TrickObject:
                            node.TrickObject = true;
                            break;
                        case Checksums::TriggerScripts:
                            while (*pFile == 0x1 || *pFile == 0x2)
                            {
                                if (*pFile == 0x2)
                                    pFile += 5;
                                else
                                    pFile++;
                            }
                            pFile += 2;
                            node.Trigger = *(Checksum*)pFile;
                            pFile += sizeof(Checksum);
                            triggers.push_back(node.Trigger);
                            break;
                        case Checksums::Pos:
                            while (*pFile == 0x1 || *pFile == 0x2)
                            {
                                if (*pFile == 0x2)
                                    pFile += 5;
                                else
                                    pFile++;
                            }
                            pFile += 2;
                            node.Position = *(SimpleVertex*)pFile;
                            node.Position.z *= -1;
                            pFile += sizeof(SimpleVertex);
                            break;
                        case Checksums::Angles:
                            while (*pFile == 0x1 || *pFile == 0x2)
                            {
                                if (*pFile == 0x2)
                                    pFile += 5;
                                else
                                    pFile++;
                            }
                            pFile += 2;
                            node.Angles = *(SimpleVertex*)pFile;
                            node.Angles.y *= -1;
                            //node.Angles.y += D3DX_PI;
                            //node.Angles.x += D3DX_PI;
                            //node->Angles.y*=-1;
                            //node->Angles.x*=-1;
                            //node->Angles.z*=-1;
                            pFile += sizeof(SimpleVertex);
                            break;

                        case Checksums::Cluster:
                              pFile+=2;
                              node.Cluster = *(Checksum*)pFile;
                              pFile+=sizeof(Checksum);
                              break;

                        case Checksums::TerrainType:
                            while (*pFile == 0x1 || *pFile == 0x2)
                            {
                                if (*pFile == 0x2)
                                    pFile += 5;
                                else
                                    pFile++;
                            }
                            pFile += 2;
                            node.TerrainType = *(Checksum*)pFile;
                            pFile += sizeof(Checksum);
                            break;

                        case Checksums::AbsentInNetGames:
                            node.AbsentInNetGames = true;
                            break;

                        case Checksums::NetEnabled:
                            node.NetEnabled = true;
                            break;

                        case Checksums::Permanent:
                            node.Permanent = true;
                            break;

                        case Checksums::Links:
                            while (*pFile != 0x17)
                                pFile++;
                            pFile++;
                            /*if(*(DWORD*)pFile>=0xFFFF)
                            MessageBox(0, "...", "...", 0);*/
                            if (node.Class.checksum == Checksum("Waypoint").checksum)
                            {
                                MessageBox(0, "ok", "ok", MB_OK);
                                /*node.Class.checksum=0;
                                if(node.Trigger.checksum)
                                  triggers.pop_back();*/
                            }
                            node.Links.push_back(*(WORD*)pFile);
                            pFile += 4;
     
                            while (true)
                            {
                                const BYTE opcode = *pFile;
                                pFile++;
                                switch (opcode)
                                {
                                case 0x2:
                                    pFile += 4;
                                    break;
                                case 0x17:
                                    node.Links.push_back(*(WORD*)pFile);
                                    pFile += 4;
                                    break;
                                case 0x6:
                                    goto gotLinks;
                                }
                            }
                        gotLinks:
                            break;

                        case Checksums::CreatedAtStart:
                            node.CreatedAtStart = true;
                            break;

                        case 0xFCD206E1:
                            pFile += 2;
                            const DWORD len = *(DWORD*)pFile;
                            pFile += 4;
                            node.RestartName = Checksum((char*)pFile);
                            pFile += len;
                            break;
                        }
                        break;
                    }
                }
                return &node;
            }
            break;
        case 0x1E:
            pFile += 12;
            break;
        case 0x40:
        case 0x2F:
        case 0x37:
            pFile += *(DWORD*)pFile * 6 + 4;
            break;
        case 0:
            return NULL;
        default:
            fprintf(stdout, "################################\n");
            fprintf(stdout, "UNKNOWN %X file %X Beg %X qb %X\n", opcode, pFile, beg, qbKey);
            break;
        case 0x2B:
            return NULL;
        }
    }
    return NULL;
}
__declspec (noalias) void Scene::AddCompressedNodes(register const BYTE const* __restrict pFile, const BYTE const* __restrict eof, vector<Node> &compressedNodes)//, vector<KnownScript> &scripts)
{
  while(pFile<eof)
  {
    const BYTE opcode = *pFile;
    pFile++;
    switch(opcode)
    {
    case 0x49:
    case 0x48:
    case 0x47:
      pFile += 2;
      break;
    case 0x1C:
    case 0x1B:
      pFile+=*(DWORD*)pFile+4;
      break;
      //case 0x18:
    case 0x17:
    case 0x1A:
    case 0x2E:
    case 2:
      pFile+=4;
      break;
    case 1:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 0x2D:
    case 0x25:
    case 0x26:
    case 0x28:
    case 0x18:
    case 0x0E:
    case 0x0F:
    case 0x39:
    case 0x30:
    case 0x24:
    case 0x23:
    case 0x42:
      break;

    case 0x16:
      if(*(DWORD*)pFile==0x41F86E9B)
      {
        pFile+=4;
        goto found;
      }
      pFile+=4;
      break;
    case 0x1F:
      pFile+=8;
      break;

    case 0x1E:
      pFile+=12;
      break;
    case 0x40:
    case 0x2F:
    case 0x37:
      pFile+=*(DWORD*)pFile*6+4;
      break;
    case 0:
      return;
    default:
        fprintf(stdout, "################################\n");
        fprintf(stdout, "UNKNOWN %X file %X\n", opcode, pFile);
      break;
    case 0x2B:
      extern vector<Script> Scripts;
      const DWORD check = *(DWORD*)pFile;;
      pFile+=4;
      bool found=false;
      for(DWORD i=0; i<Scripts.size(); i++)
      {
        if(Scripts[i].checksum == check)
        {
          found=true;
          break;
        }
      }
      if(!found)
        Scripts.push_back((Script(check, (char*)pFile)));
      while(*pFile != 0x0)
        pFile++;
      pFile++;
      break;
    }
  }

found:
  MessageBox(0,"found","",0);
  while(pFile<eof)
  {
    const BYTE opcode = *pFile;
    pFile++;
    switch(opcode)
    {
    case 0x49:
    case 0x48:
    case 0x47:
      pFile += 2;
      break;
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
    case 0x40:
    case 0x2F:
    case 0x37:
      pFile+=*(DWORD*)pFile*6+4;
      break;
    case 0x2B:
      extern vector<Script> Scripts;
      DWORD check;
      check = *(DWORD*)pFile;
      pFile+=4;
      bool found;
      found=false;
      for(DWORD i=0; i<Scripts.size(); i++)
      {
        if(Scripts[i].checksum == check)
        {
          found=true;
          break;
        }
      }
      if(!found)
        Scripts.push_back((Script(check, (char*)pFile)));
      while(*pFile != 0x0)
        pFile++;
      pFile++;
      break;
    case 0:
      return;

    case 0x16:
      const DWORD qbKey = *(DWORD*)pFile;
      pFile+=4;
      compressedNodes.push_back(Node());
      Node &node = compressedNodes.back();
      node.Name = *(Checksum*)&qbKey;
      while(true)
      {
        const DWORD op = *(BYTE*)pFile;
        pFile++;
        switch(op)
        {
        case 0x49:
        case 0x48:
        case 0x47:
          pFile += 2;
          break;
        case 4:
          goto gotNode;
          break;
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
        case 0x40:
        case 0x2F:
        case 0x37:
          pFile+=*(DWORD*)pFile*6+4;
          break;
        case 0x2B:
          extern vector<Script> Scripts;
          DWORD check;
          check = *(DWORD*)pFile;
          pFile+=4;
          bool found;
          found=false;
          for(DWORD i=0; i<Scripts.size(); i++)
          {
            if(Scripts[i].checksum == check)
            {
              found=true;
              break;
            }
          }
          if(!found)
            Scripts.push_back((Script(check, (char*)pFile)));
          while(*pFile != 0x0)
            pFile++;
          pFile++;
          break;
        case 0:
          return;
        case 0x16:
          const DWORD key = *(DWORD*)pFile;
          pFile+=4;
          switch(key)
          {
          case Checksums::Class:
            pFile+=2;
            node.Class = *(Checksum*)pFile;
            pFile+=sizeof(Checksum);
            break;
          case Checksums::Type:
            pFile+=2;
            node.Type = *(Checksum*)pFile;
            if(node.Type.checksum==Checksums::PedAI || node.Type.checksum==Checksums::DEFAULT)
            {
              node.Class.checksum=0;
            }
            pFile+=sizeof(Checksum);
            break;
          case Checksums::TrickObject:
            node.TrickObject=true;
            break;
          case Checksums::TriggerScripts:
            pFile+=2;
            node.Trigger = *(Checksum*)pFile;
            pFile+=sizeof(Checksum);
            triggers.push_back(node.Trigger);
            break;
          case Checksums::Pos:
              while (*pFile == 0x1 || *pFile == 0x2)
              {
                  if (*pFile == 0x2)
                      pFile += 5;
                  else
                      pFile++;
              }
              pFile += 2;
              node.Position = *(SimpleVertex*)pFile;
              node.Position.z *= -1;
              pFile += sizeof(SimpleVertex);
              break;
          case Checksums::Angles:
              pFile += 2;
              node.Angles = *(SimpleVertex*)pFile;
              node.Angles.y *= -1;
              //node.Angles.y += D3DX_PI;
              //node.Angles.x += D3DX_PI;
              pFile += sizeof(SimpleVertex);
              break;

          case Checksums::Cluster:
            pFile+=2;
            node.Cluster = *(Checksum*)pFile;
            pFile+=sizeof(Checksum);
            break;

          case Checksums::TerrainType:
            pFile += 2;
            node.TerrainType = *(Checksum*)pFile;
            pFile += sizeof(Checksum);
            break;

          case Checksums::AbsentInNetGames:
            node.AbsentInNetGames = true;
            break;

          case Checksums::NetEnabled:
            node.NetEnabled = true;
            break;

          case Checksums::Permanent:
            node.Permanent = true;
            break;

          case Checksums::Links:
              while (*pFile != 0x17)
                  pFile++;
              pFile++;
              /*if(*(DWORD*)pFile>=0xFFFF)
              MessageBox(0, "...", "...", 0);*/
              if (node.Class.checksum == Checksum("Waypoint").checksum)
              {
                  MessageBox(0, "ok", "ok", MB_OK);
                  /*node.Class.checksum=0;
                  if(node.Trigger.checksum)
                    triggers.pop_back();*/
              }
              node.Links.push_back(*(WORD*)pFile);
              pFile += 4;

              while (true)
              {
                  const BYTE opcode = *pFile;
                  pFile++;
                  switch (opcode)
                  {
                  case 0x2:
                      pFile += 4;
                      break;
                  case 0x17:
                      node.Links.push_back(*(WORD*)pFile);
                      pFile += 4;
                      break;
                  case 0x6:
                      goto gotLinks;
                  }
              }
          gotLinks:
              break;

          case Checksums::CreatedAtStart:
            node.CreatedAtStart = true;
            break;

          case 0xFCD206E1:
            pFile+=2;
            const DWORD len = *(DWORD*)pFile;
            pFile+=4;
            node.RestartName = Checksum((char*)pFile);
            pFile+=len;
            break;
          }
          break;
        }
      }
gotNode:
      break;
    }
  }
}

_declspec (noalias) const BYTE const* __restrict Scene::ParseScript(register const BYTE const* __restrict pFile, KnownScript** pScript, vector<KnownScript> &scripts, vector<Script> &sounds, bool THUG, vector<Checksum> *extraTriggers)
{
  KnownScript* script = *pScript;
  const DWORD qbKey = *(DWORD*)pFile;
  pFile+=4;

  switch (qbKey)
  {
  case 0xA1116237:
    script->Script("LaunchPanelMessage2");

    while (true)
    {
      const BYTE op = *pFile;
      pFile++;
      switch (op)
      {
      case 0x49:
      case 0x48:
      case 0x47:
        pFile += 2;
        break;
      case 0x1B:
      case 0x1C:
        pFile += *(DWORD*)(pFile)+4;
        break;
      case 0x1:
        goto doneParsing;
        break;
      case 0x2:
        pFile += 4;
        goto doneParsing;
        break;
      case 0x17:
      case 0x1A:
      case 0x2E:
        pFile += 4;
        break;
      case 0x40:
      case 0x2F:
      case 0x37:
        THUG ? pFile += *(DWORD*)pFile * 6 + 4 : pFile += *(DWORD*)pFile * 4 + 4;
        break;
      case 0x16:
        const DWORD key = *(DWORD*)pFile;
        pFile += 4;
        switch (key)
        {
        case 0x40C698AF:
          script->Script(*(Checksum*)&key);
          while (*pFile != 0x16) pFile++;
          pFile++;
          DWORD k;
          k = *(DWORD*)pFile;
          pFile += 4;
          script->Append(0x7);
          script->Append(0x16);
          if (k == 0x44E9A8EC)
            script->Append(*(Checksum*)&k);
          else
            script->Append(Checksum("thps4_message_props"));
          break;

        case 0xC4745838:
          script->Script(Checksum("text"));
          script->Append(0x7);
          pFile++;
          if (*pFile == 0x2D)
          {
            script->Append(0x2D);
            pFile += 2;
            script->Script(*(Checksum*)pFile);
            pFile += 4;
          }
          else if (*pFile == 0x16)
          {
            script->Append(0x16);
            pFile++;
            script->Append(*(Checksum*)pFile);
            pFile += 4;
          }
          else
          {
            script->Append(0x1B);
            pFile++;
            const DWORD len = *(DWORD*)pFile + 4;
            script->Append(pFile, len);
            pFile += len;
          }
          break;
        }
      }
    }
  case 0x4562E4A6:
    script->Script(qbKey);
    script->EndScript();
    break;
  case 0x9FBBD5F0:
  case 0x3F4B93F8:
    script->Script("Sk3_TeleportToNode");

    while (true)
    {
      const BYTE op = *pFile;
      pFile++;
      switch (op)
      {
      case 0x49:
      case 0x48:
      case 0x47:
        pFile += 2;
        break;
      case 0x1B:
      case 0x1C:
        pFile += *(DWORD*)(pFile)+4;
        break;
      case 0x1:
        goto doneParsing;
        break;
      case 0x2:
        pFile += 4;
        goto doneParsing;
        break;
      case 0x17:
      case 0x1A:
      case 0x2E:
        pFile += 4;
        break;
      case 0x40:
      case 0x2F:
      case 0x37:
        THUG ? pFile += *(DWORD*)pFile * 6 + 4 : pFile += *(DWORD*)pFile * 4 + 4;
        break;
      case 0x16:
        const DWORD key = *(DWORD*)pFile;
        pFile += 4;
        switch (key)
        {
        case 0x9F92BA78://78 BA 92 9F
          script->Script(*(Checksum*)&key);
          script->Append(0x7);
          pFile++;
          BYTE b;
          b = *pFile;
          if (b == 0x2D)
          {
            script->Append(0x2D);
            pFile++;
          }
          else
          {
            while (b != 0x16)
            {
              pFile++;
              switch (b)
              {
              case 0x1B:
              case 0x1C:
                pFile += *(DWORD*)(pFile)+4;
                break;
              case 0x17:
              case 0x1A:
              case 0x2E:
                pFile += 4;
                break;
              case 0x2:
                pFile += 4;
                break;
              case 0x40:
              case 0x2F:
              case 0x37:
                THUG ? pFile += *(DWORD*)pFile * 6 + 4 : pFile += *(DWORD*)pFile * 4 + 4;
                break;
              }
              b = *pFile;
            }
          }
          pFile++;
          script->Script(*(Checksum*)pFile);
          pFile += 4;
          break;
        }
        break;
      }
    }
  case 0x74F14AC3:
  case 0xAFA20E18:
  case 0x5151B2B1:
    script->Script(*(Checksum*)&qbKey);
    BYTE b;
    b = *pFile;
    if (b == 0x2D)
    {
      script->Append(0x2D);
      pFile++;
    }
    else
    {
      while (b != 0x16)
      {
        pFile++;
        switch (b)
        {
        case 0x1B:
        case 0x1C:
          pFile += *(DWORD*)(pFile)+4;
          break;
        case 0x17:
        case 0x1A:
        case 0x2E:
          pFile += 4;
          break;
        case 0x2:
          pFile += 4;
          break;
        case 0x40:
        case 0x2F:
        case 0x37:
          THUG ? pFile += *(DWORD*)pFile * 6 + 4 : pFile += *(DWORD*)pFile * 4 + 4;
          break;
        }
        b = *pFile;
      }
    }
    pFile++;
    DWORD checkc;
    checkc = *(DWORD*)pFile;
    if (checkc == 0xB0A2A4C5)//C5 A4 A2 B0
      *(Checksum*)&checkc = Checksum("glasspane2x");
    script->Script(checkc);
    LoadSound(*(Checksum*)&checkc, sounds);
    pFile += 4;
    script->EndScript();
    break;
  case 0xE5399FB2:
    DWORD start;
    DWORD lastKey;
    lastKey = 0;
    DWORD lastPos;
    start = script->size;
    script->Script(Checksum("EndGap"));

    signed char n;
    n = 0;

    while (true)
    {
      const BYTE op = *pFile;
      pFile++;
      switch (op)
      {
      case 0x0A:
        pFile++;
        if (lastKey)
        {
          if (lastKey == 0x3B442E26)
          {
            BYTE* pSc = &script->func[lastPos] + 4;
            while (*pSc != 0x16) pSc++;
            pSc++;
            lastKey = *(DWORD*)pSc;
            *(Checksum*)pSc = Checksum("added");
          }
          else
            *(Checksum*)&script->func[lastPos] = Checksum("added");
          script->InsertSub(start, lastKey, *(DWORD*)pFile);
        }
        pFile += 4;
        break;
      case 0x0B:
        pFile++;
        if (lastKey)
        {
          if (lastKey == 0x3B442E26)
          {
            BYTE* pSc = &script->func[lastPos] + 4;
            while (*pSc != 0x16) pSc++;
            pSc++;
            lastKey = *(DWORD*)pSc;
            *(Checksum*)pSc = Checksum("added");
          }
          else
            *(Checksum*)&script->func[lastPos] = Checksum("added");
          script->InsertAdd(start, lastKey, *(DWORD*)pFile);
        }
        pFile += 4;
        break;
      case 0x1B:
      case 0x1C:
        pFile += *(DWORD*)(pFile)+4;
        break;
      case 0x3:
        n++;
        break;
      case 0x4:
        n--;
        break;
      case 0x1:
        goto doneParsing;
        break;
      case 0x2:
        pFile += 4;
        if (n <= 0)
          goto doneParsing;
        break;
      case 0x17:
      case 0x1A:
      case 0x2E:
        pFile += 4;
        break;
      case 0x40:
      case 0x2F:
      case 0x37:
        THUG ? pFile += *(DWORD*)pFile * 6 + 4 : pFile += *(DWORD*)pFile * 4 + 4;
        break;
      case 0x16:
        const DWORD key = *(DWORD*)pFile;
        lastKey = key;
        lastPos = script->size + 1;
        pFile += 4;
        switch (key)
        {
          /*case 0xEC1CD520:
            script->Script(Checksum("Continue"));
            script->Append(0x07);
            script->Append(0x3);
            if (*pFile != 0x1)
            pFile += 2;
            else
            pFile++;
            while(*pFile != 0x16) pFile++;
            pFile++;
            script->Script(*(Checksum*)pFile);
            pFile+=4;
            if(*pFile!=0x7)
            MessageBox(0,"WTAWAGWA","",0);
            script->Append(0x7);
            pFile++;
            while(*pFile == 0x0E) pFile++;
            if(*pFile==0x2D)
            {
            while(*pFile == 0x0E) pFile++;
            script->Append(0x2D);
            pFile++;
            script->Script(*(Checksum*)pFile);
            pFile+=4;
            }
            else
            {
            pFile++;
            script->Script(*(Checksum*)pFile);
            pFile+=4;
            }
            script->Append(0x4);
            break;*/
        case 0xCD66C8AE:
          script->Script(Checksum("Score"));
          script->Append(0x7);
          pFile++;
          if (*pFile == 0x2D)
          {
            script->Append(0x2D);
            pFile += 2;
            script->Script(*(Checksum*)pFile);
            pFile += 4;
          }
          else if (*pFile == 0x17)
          {
            script->Append(0x17);
            pFile++;
            script->Append(*(Checksum*)pFile);
            pFile += 4;
          }
          else if (*pFile == 0x1A)
          {
            script->Append(0x1A);
            pFile++;
            script->Append(*(Checksum*)pFile);
            pFile += 4;

          }
          else
            MessageBox(0, "wtf", "score", 0);
          break;

        case 0xC4745838:
          script->Script(Checksum("text"));
          script->Append(0x7);
          pFile++;
          if (*pFile == 0x2D)
          {
            script->Append(0x2D);
            pFile += 2;
            script->Script(*(Checksum*)pFile);
            pFile += 4;
          }
          else if (*pFile == 0x16)
          {
            script->Append(0x16);
            pFile++;
            script->Append(*(Checksum*)pFile);
            pFile += 4;
          }
          else
          {
            script->Append(0x1B);
            pFile++;
            const DWORD len = *(DWORD*)pFile + 4;
            script->Append(pFile, len);
            pFile += len;
          }
          break;
        default:
          lastKey = 0;
          break;
        case 0x7C9E51AB:
          script->Script(Checksum("gapscript"));
          script->Append(0x7);
        retry2:
          pFile++;
          if (*pFile == 0x2D)
          {
            script->Append(0x2D);
            pFile++;
          }
          else if (*pFile == 0x0E)
          {
            goto retry2;
          }
          else if (*pFile != 0x16)
          {
            MessageBox(0, "ao", "", 0);
            while (*pFile != 0x16)
            {
              pFile++;
            }
          }
          pFile++;
          DWORD qb;
          qb = *(DWORD*)pFile;
          if (!GetTrigger(qb))
            triggers.push_back(qb);
          script->Script(*(Checksum*)&qb);
          pFile += 4;
          break;
        case 0x3B442E26://78 BA 92 9F
          script->Script(Checksum("GapId"));
          //foundEndId=true;
          script->Append(0x7);
        retry:
          pFile++;
          if (*pFile == 0x2D)
          {
            script->Append(0x2D);
            pFile++;
          }
          else if (*pFile == 0x0E)
          {
            goto retry;
          }
          else if (*pFile != 0x16)
          {
            MessageBox(0, "ao", "", 0);
            while (*pFile != 0x16)
            {
              pFile++;
            }
          }
          pFile++;
          script->Script(*(Checksum*)pFile);
          pFile += 4;
          break;
        }
        break;
      }
    }
    break;
  case 0x65B9EC39:
    DWORD starte;
    DWORD lastKey2;
    DWORD lastPos2;
    starte = script->size;
    script->Script(Checksum("StartGap"));
    signed char ids;
    ids = 0;
    /*bool added;
    added = false;*/

    while (true)
    {
      const BYTE op = *pFile;
      pFile++;
      if (op != 0x9 && op != 0x0E && op != 0x0F)
      {
        script->Append(op);
        switch (op)
        {
        case 0x49:
        case 0x48:
        case 0x47:
          pFile += 2;
          break;
        case 0x1B:
        case 0x1C:
          DWORD len;
          len = *(DWORD*)(pFile)+4;
          script->Append(pFile, len);
          pFile += len;
          break;
        case 0x0A:
          script->size--;
          pFile++;
          MessageBox(0, "sub", "", 0);
          if (lastKey2 == 0x3B442E26)
          {
            BYTE* pSc = &script->func[lastPos2] + 4;
            while (*pSc != 0x16) pSc++;
            pSc++;
            lastKey2 = *(DWORD*)pSc;
            *(Checksum*)pSc = Checksum("added");
          }
          else
            *(Checksum*)&script->func[lastPos2] = Checksum("added");
          script->InsertSub(starte, lastKey2, *(DWORD*)pFile);
          pFile += 4;
          break;
        case 0x0B:
          script->size--;
          pFile++;
          /*if(!added)
          {*/
          //MessageBox(0, "add", "", 0);
          if (lastKey2 == 0x3B442E26)
          {
            BYTE* pSc = &script->func[lastPos2] + 4;
            while (*pSc != 0x16) pSc++;
            pSc++;
            lastKey2 = *(DWORD*)pSc;
            *(Checksum*)pSc = Checksum("added");
          }
          else
            *(Checksum*)&script->func[lastPos2] = Checksum("added");
          script->InsertAdd(starte, lastKey2, *(DWORD*)pFile);
          // }
          pFile += 4;
          break;
        case 0x3:
          ids++;
          break;
        case 0x4:
          ids--;
          break;
        case 0x1:
          script->size--;
          if (ids > 0)
          {
            printf("ids@%X\n", pFile);
            script->Append(0x4);
          }
          goto doneParsing;
          break;
        case 0x2:
          pFile += 4;
          script->size--;
          if (ids <= 0)
            goto doneParsing;
          break;
        case 0x16:
          lastPos2 = script->size;
          lastKey2 = *(DWORD*)pFile;
          script->Append((DWORD*)pFile);
          pFile += 4;
          break;
        case 0x17:
        case 0x1A:
        case 0x2E:
          script->Append((DWORD*)pFile);
          pFile += 4;
          break;
        case 0x40:
        case 0x2F:
        case 0x37:
          const DWORD numRands = *(DWORD*)pFile;
          script->Append(*(Checksum*)&numRands);
          pFile += THUG ? numRands * 2 + 4 : 4;
          const DWORD size = numRands * 4;
          fprintf(stdout, "adding RNd2\nsize %u num %u\n", size, numRands);
          script->Append(pFile, size);
          pFile += size;
          break;
        }
      }
    }
    break;
  case 0x1310920E:
  case 0xB1058546:
  case 0xDEA3057B:
  case 0x0B38ED6B:
    script->Script(qbKey);
    if (qbKey == 0xB1058546)
    {
      DWORD temp = 0x24FFB92D;
      LoadSound(*(Checksum*)&temp, sounds);
    }

    signed char cnt;
    cnt=0;

    while(true)
    {
      const BYTE op = *pFile;
      pFile++;
      switch(op)
      {
      case 0x49:
      case 0x48:
      case 0x47:
        pFile += 2;
        break;
      case 0x3:
        cnt++;
        break;
      case 0x4:
        cnt--;
        break;
      case 0x1B:
      case 0x1C:
        pFile += *(DWORD*)(pFile)+4;
        break;
      case 0x1:
        goto doneParsing;
        break;
      case 0x2:
        pFile+=4;
        if(cnt<=0)
          goto doneParsing;
        break;
      case 0x17:
      case 0x1A:
      case 0x2E:
        pFile+=4;
        break;
      case 0x40:
      case 0x2F:
      case 0x37:
        THUG?pFile+=*(DWORD*)pFile*6+4:pFile+=*(DWORD*)pFile*4+4;
        break;
      case 0x16:
        const DWORD key = *(DWORD*)pFile;
        pFile+=4;
        switch(key)
        {
        case 0xF6BA43E1:
          script->Script(Checksum("Bail"));
          break;
        case 0x9F92BA78://78 BA 92 9F
          script->Script(Checksum("TeleTo"));
          script->Append(0x7);
          pFile++;
          BYTE b;
          b = *pFile;
          if(b==0x2D)
          {
            script->Append(0x2D);
            pFile++;
          }
          else
          {
            while(b!=0x16)
            {
              pFile++;
              switch(b)
              {
              case 0x1B:
              case 0x1C:
                pFile += *(DWORD*)(pFile)+4;
                break;
              case 0x17:
              case 0x1A:
              case 0x2E:
                pFile+=4;
                break;
              case 0x2:
                pFile+=4;
                break;
              case 0x40:
              case 0x2F:
              case 0x37:
                THUG? pFile+=*(DWORD*)pFile*6+4 : pFile+=*(DWORD*)pFile*4+4;
                break;
              }
              b=*pFile;
            }
          }
          pFile++;
          script->Script(*(Checksum*)pFile);
          pFile+=4;
          break;
        }
        break;
      }
    }
  case 0x7AAE1282:
    script->Script("Sk3_Killskater_Finish");

    while(true)
    {
      const BYTE op = *pFile;
      pFile++;
      switch(op)
      {
      case 0x49:
      case 0x48:
      case 0x47:
        pFile += 2;
        break;
      case 0x1B:
      case 0x1C:
        pFile += *(DWORD*)(pFile)+4;
        break;
      case 0x1:
        goto doneParsing;
        break;
      case 0x2:
        pFile+=4;
        goto doneParsing;
        break;
      case 0x17:
      case 0x1A:
      case 0x2E:
        pFile+=4;
        break;
      case 0x40:
      case 0x2F:
      case 0x37:
        THUG?pFile+=*(DWORD*)pFile*6+4:pFile+=*(DWORD*)pFile*4+4;
        break;
      case 0x16:
        const DWORD key = *(DWORD*)pFile;
        pFile+=4;
        switch(key)
        {
        case 0xF6BA43E1:
          script->Script(Checksum("Bail"));
          break;
        case 0x9F92BA78://78 BA 92 9F
          script->Script(Checksum("TeleTo"));
          script->Append(0x7);
          pFile++;
          BYTE b;
          b = *pFile;
          if(b==0x2D)
          {
            script->Append(0x2D);
            pFile++;
          }
          else
          {
            while(b!=0x16)
            {
              pFile++;
              switch(b)
              {
              case 0x1B:
              case 0x1C:
                pFile += *(DWORD*)(pFile)+4;
                break;
              case 0x17:
              case 0x1A:
              case 0x2E:
                pFile+=4;
                break;
              case 0x2:
                pFile+=4;
                break;
              case 0x40:
              case 0x2F:
              case 0x37:
                THUG? pFile+=*(DWORD*)pFile*6+4 : pFile+=*(DWORD*)pFile*4+4;
                break;
              }
              b=*pFile;
            }
          }
          pFile++;
          script->Script(*(Checksum*)pFile);
          pFile+=4;
          break;
        }
        break;
      }
    }
  case Checksums::Kill:
  case Checksums::Create:
  case Checksums::Shatter:
  case Checksums::ShatterAndDie:
  case Checksums::Visible:
  case Checksums::Invisible:
    script->Script(qbKey);
    //MessageBox(0,"script",(*(Checksum*)&qbKey).GetString(),0);
    signed char ibs;
    ibs=0;

    while(true)
    {
      const BYTE op = *pFile;
      pFile++;
      if(op!=0x9 && op != 0x0E && op != 0x0F)
        script->Append(op);
      switch(op)
      {
      case 0x49:
      case 0x48:
      case 0x47:
        pFile += 2;
        break;
      case 0x1C:
      case 0x1B:
        DWORD len;
        len = *(DWORD*)pFile+4;
        script->Append(pFile, len);
        pFile+=len;
        break;
      case 0x3:
        ibs++;
        break;
      case 0x4:
        ibs--;
        break;
      case 0x1:
        script->size--;
        if(ibs>0)
        {
          printf("ibs@%X\n",pFile);
          script->Append(0x4);
        }
        goto doneParsing;
        break;
        /*case 0x1:
        script->size--;
        goto doneParsing;
        break;*/
      case 0x2:
        pFile+=4;
        script->size--;
        if(ibs<=0)
          goto doneParsing;
        break;
      case 0x16:
      case 0x17:
      case 0x1A:
      case 0x2E:
        if(THUG && *(DWORD*)pFile==0x7DC30BA6)
        {
          // MessageBox(0, "end", "end", 0);
          script->size--;
          pFile+=10;
          if(*pFile==0x9)
          {
            pFile+=6;
          }
        }
        else
        {
          //MessageBox(0, "adding 4 bytes", "",0);
          script->Append((DWORD*)pFile);
          pFile+=4;
        }
        break;
      case 0x40:
      case 0x2F:
      case 0x37:
        const DWORD numRands = *(DWORD*)pFile;
        script->Append(*(Checksum*)&numRands);
        pFile+=THUG?numRands*2+4:4;
        const DWORD size = numRands*4;
        fprintf(stdout, "adding RNd3\nsize %u num %u\n", size, numRands);
        script->Append(pFile, size);
        pFile+=size;
        break;
      }
    }
doneParsing:
    //MessageBox(0, "done parsing", (*(Checksum*)&qbKey).GetString(), 0);
    script->EndScript();
    //found=true;
    break;
  default:
    for(DWORD i=0, numScripts = scripts.size(); i<numScripts; i++)
    {
      //added=true;
      if(qbKey==scripts[i].name.checksum)
      {
        /*if(qbKey==0x1762F8CD)
        MessageBox(0,script->name.GetString(),(*(Checksum*)&qbKey).GetString(),0);*/
        script->Script(qbKey);
        pFile = AddParams(pFile, script, THUG);
        script->EndScript();
        if(!GetTrigger(qbKey))
          triggers.push_back(*(Checksum*)&qbKey);
        const DWORD name = script->name.checksum;
        if(!GetScript(qbKey))
          KnownScripts.push_back(scripts[i]);
        script = GetScript(name);
        if(script)
          *pScript = script;
        goto foundTrigger;
      }
    }
    /*if(extraTriggers && !script->size)
    {
    script->Script(qbKey);
    pFile = AddParams(pFile, script, THUG);
    script->EndScript();
    for(DWORD i=0, numTriggers = extraTriggers->size(); i<numTriggers; i++)
    {
    if(qbKey==(*extraTriggers)[i].checksum)
    goto foundTrigger;
    }
    extraTriggers->push_back(*(Checksum*)&qbKey);
    }*/
foundTrigger:
    break;
  }
  return pFile;
}

const BYTE* Ignore(const BYTE* pFile,const BYTE* eof, bool THUG)
{
  DWORD ifCounter = 0;
  bool addNot = false;
  while (pFile < eof)
  {
    const BYTE opcode = *pFile;
    pFile++;
    switch (opcode)
    {
    case 0x49:
    case 0x48:
    case 0x47:
      pFile += 2;
      break;
    case 0x25:
      ifCounter++;
      while (*pFile != 0x16) { if (*pFile == 0x39) addNot = true; pFile++; }
      pFile++;
      pFile += 4;
      break;
    case 0x26:
      break;
    case 0x28:
      if (ifCounter == 0)
        return pFile;
      ifCounter--;
      break;

    case 0x1C:
    case 0x1B:
      pFile += *(DWORD*)pFile + 4;
      break;
      //case 0x18:
    case 0x17:
    case 0x1A:
    case 0x2E:
    case 2:
      pFile += 4;
      break;
    case 1:
    case 0x3:
    case 0x4:
    case 5:
    case 6:
    case 7:
    case 9:
    case 0x2D:
    case 0x18:
    case 0x0E:
    case 0x0F:
    case 0x39:
    case 0x30:
      break;
    case 0x16:
      pFile += 4;
      break;
    case 0x1F:
      pFile += 8;
      break;

    case 0x1E:
      pFile += 12;
      break;
    case 0x2B:
      extern vector<Script> Scripts;
      DWORD check;
      check = *(DWORD*)pFile;
      pFile += 4;
      Scripts.push_back((Script(check, (char*)pFile)));
      while (*pFile != 0x0)
        pFile++;
      pFile++;
      break;
    case 0x40:
    case 0x2F:
    case 0x37:
      //MessageBox(0,"rand","",0);
      THUG ? pFile += *(DWORD*)pFile * 6 + 4 : pFile += *(DWORD*)pFile * 4 + 4;
      break;
    case 0x23:
      break;
    case 0x24:
      break;
    }

  }
}

__declspec (noalias) const BYTE* __restrict KnownScript::GetParam(Checksum param, const BYTE* __restrict params) const
{
  using namespace Executer;
  bool useParams = true;
  DWORD name = *(DWORD*)pFile;
  while (name != param.checksum)
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
      name = *(DWORD*)pFile;
      pFile += 4;
      break;
    }
  }
  pFile++;
  if (*pFile != 0x2D)
  {
    useParams = false;
  }
  while (*pFile != 0x16)
    pFile++;

  pFile++;
  if (useParams)
  {
    param.checksum = *(DWORD*)pFile;
    while (*(DWORD*)params != param.checksum)
      params++;
    params += 4;
    const BYTE* __restrict eofe = eof - 5;
    
    while (params < eofe)
    {
      const BYTE opcode = *params;
      params++;
      switch (opcode)
      {
      case 0x1C:
      case 0x1B:
        params += *(DWORD*)params + 4;
        break;
      case 0x17:
      case 0x1A:
      case 0x2E:
        params += 4;
        break;
      case 2:
        params += 4;
        break;
      case 1:
        break;
      case 0x1F:
        params += 8;
        break;
      case 0x1E:
        params += 12;
        break;
      case 0x40:
      case 0x2F:
      case 0x37:
        params += *(DWORD*)params * 4 + 4;
        break;
      case 0x16:
        return params;
        break;
      }
    }
  }
  return pFile;
}

__declspec (noalias) void KnownScript::Execute(const Node* __restrict node, const BYTE* __restrict params) const
{
  using namespace Executer;
  extern Scene* __restrict scene;
  extern Player* __restrict player;
  pFile = func;
  eof = pFile + size;
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
        pFile += 4;
        if (node)
        {
          DWORD nodeName = *(DWORD*)GetParam(Checksum("nodename"), params);

          for (DWORD k = 0; k < scene->nodes.size(); k++)
          {
            if (scene->nodes[k].Name.checksum == nodeName)
            {
              player->newPos.x = scene->nodes[k].Position.x;
              player->newPos.y = scene->nodes[k].Position.y + 10.0f;
              player->newPos.z = scene->nodes[k].Position.z;
              player->velocity = D3DXVECTOR3(0, 0, 0);
              extern bool jumped;
              jumped = false;
              break;
            }
          }
        }
      }
      else if (*(DWORD*)pFile == Checksum("ShatterAndDie").checksum)
      {
        pFile += 4;
        DWORD nodeName = *(DWORD*)GetParam(Checksum("Name"), params);

        for (DWORD k = 0; k < scene->GetNumMeshes(); k++)
        {
          Mesh* mesh = scene->GetMesh(k);
          if (mesh->GetName().checksum == nodeName)
          {
            mesh->SetVisible(false);
            mesh->SetFlag(MeshFlags::deleted, true);
            break;
          }
        }
      }
      /*else if (*(DWORD*)pFile == Checksum("Shatter").checksum)
      {

      }*/
      else if (*(DWORD*)pFile == Checksum("Kill").checksum)
      {
        pFile += 4;
        DWORD nodeName = *(DWORD*)GetParam(Checksum("Name"), params);

        for (DWORD k = 0; k < scene->GetNumMeshes(); k++)
        {
          Mesh* mesh = scene->GetMesh(k);
          if (mesh->GetName().checksum == nodeName)
          {
            mesh->SetVisible(false);
            mesh->SetFlag(MeshFlags::deleted, true);
            break;
          }
        }
      }
      else if (*(DWORD*)pFile == Checksum("sk3_killskater").checksum)
      {
        pFile += 4;
        if (node && node->Links.size())
        {
          player->newPos.x = scene->nodes[node->Links[0]].Position.x;
          player->newPos.y = scene->nodes[node->Links[0]].Position.y+10.0f;
          player->newPos.z = scene->nodes[node->Links[0]].Position.z;
          player->velocity = D3DXVECTOR3(0, 0, 0);
          extern bool jumped;
          jumped = false;
        }
      }
      else
      {
        DWORD crc = *(DWORD*)pFile;
        for (DWORD k = 0; k < scene->KnownScripts.size(); k++)
        {
          if (scene->KnownScripts[k].name.checksum == crc)
          {
            pFile += 4;
            scene->KnownScripts[k].Execute(node, params);
            break;
          }
        }
      }
      while (*pFile != 0x1 && *pFile != 0x2)
        pFile++;
      break;
    }
  }
}

__declspec (noalias) void Scene::LoadScriptsFile(char* path, vector<KnownScript> &scripts, vector<Script> &sounds, bool THUG)
{
  FILE* f = fopen(path, "rb");
  if(f)
  {
    vector<Checksum> extraTriggers;
    KnownScript* script = NULL;
    fseek(f, 0, SEEK_END);
    DWORD size = ftell(f);
    fseek(f, 0, SEEK_SET);
    const BYTE* pFile = new BYTE[size];
    fread((void*)pFile, size, 1, f);
    fclose(f);
    const BYTE* pData = pFile;
    //bool added=false;
    bool ifs[32];
    signed char ifCounter=-1;
    DWORD lastIf=0;
    /*DWORD FuckedEndGapSize=0;
    BYTE FuckedEndGap[900];*/

    const BYTE* eof = pFile+size;
    while(pFile<eof)
    {
      const BYTE opcode = *pFile;
      pFile++;
      switch(opcode)
      {
      case 0x49:
      case 0x48:
      case 0x47:
        pFile += 2;
        break;
      case 0x25:
        if(script)
        {
          ifCounter++;
          bool addNot=false;
          while(*pFile != 0x16) {if(*pFile==0x39) addNot=true; pFile++;}
          pFile++;
          const DWORD qbKey = *(DWORD*)pFile;
          pFile+=4;
          switch(qbKey)
          {
          case 0x9CB87E60:
            lastIf=0xFFFFFFFF;
            script->Append(0x25);
            if(addNot)
            {
              script->Script("Not");
            }
            script->Script(qbKey);
            script->EndScript();
            ifs[ifCounter]=true;
            break;
          case 0x2612FB96:
            lastIf=0xFFFFFFFF;
            script->Append(0x25);
            if(addNot)
            {
              script->Script("Not");
            }
            script->Script(qbKey);
            while(true)
            {
              BYTE c;
              c = *pFile;
              pFile++;
              if(c!=0x9 && c!=0x0E && c!=0x0F)
              {
                script->Append(c);
                switch(c)
                {
                case 0x1C:
                case 0x1B:
                  DWORD len;
                  len = *(DWORD*)pFile+4;
                  script->Append(pFile, len);
                  pFile+=len;
                  break;
                case 0x1E:
                  script->Append(pFile, 12);
                  pFile+=12;
                  break;
                case 0x16:
                case 0x17:
                case 0x1A:
                case 0x2E:
                  script->Append(*(Checksum*)pFile);
                  pFile+=4;
                  break;
                case 0x2:
                case 0x1:
                  script->size--;
                  goto dne;
                  break;
                case 0x40:
                case 0x2F:
                case 0x37:
                  const DWORD numRands = *(DWORD*)pFile;
                  script->Append(*(Checksum*)&numRands);
                  pFile+=THUG?numRands*2+4:4;
                  const DWORD size = numRands*4;
                  
                  fprintf(stdout, "adding RNdIf1\nsize %u num %u\n", size, numRands);
                  script->Append(pFile, size);
                  pFile+=size;
                  break;
                }
              }
            }
            break;
          case 0x3A717382:
            lastIf=0xFFFFFFFF;
            script->Append(0x25);
            if(addNot)
            {
              script->Script("Not");
            }
            script->Script(qbKey);
            if(*pFile==0x16)
            {
              pFile++;
              script->Script(*(Checksum*)pFile);
              pFile+=4;
            }
            else if(*pFile==0x5)
            {
              BYTE c;
              __assume(c!=0x6);
              while(true)
              {
                c = *pFile;
                pFile++;
                if(c!=0x9 && c!=0x0E && c!=0x0F)
                {
                  script->Append(c);
                  switch(c)
                  {
                  case 0x1C:
                  case 0x1B:
                    DWORD len;
                    len = *(DWORD*)pFile+4;
                    script->Append(pFile, len);
                    pFile+=len;
                    break;
                  case 0x16:
                    script->Append(*(Checksum*)pFile);
                    pFile+=4;
                    break;
                  case 0x2:
                  case 0x1:
                    script->size--;
                    script->Append(0x6);
                    goto dne;
                    break;
                  case 0x6:
                    goto dne;
                    break;
                  }
                }
              }
            }
dne:
            script->EndScript();
            ifs[ifCounter]=true;
            break;
          case Checksums::InNetGame:
          case 0x850B3CCA:
            lastIf = 0xFFFFFFFF;
            script->Append(0x25);
            if (addNot)
            {
              script->Script("Not");
            }
            script->Script(qbKey);
            script->EndScript();

            ifs[ifCounter] = true;
            break;
          case 0x7D873131:
            lastIf = 0xFFFFFFFF;
            if (addNot)
            {
              script->Append(0x25);
              script->Script("Not");

              script->Script(qbKey);
              script->EndScript();

              ifs[ifCounter] = true;
            }
            else
            {
              pFile = Ignore(pFile, eof, THUG);
              ifCounter--;
            }
            break;
          case 0xCF66717B:
          case 0xE565465B:
          case 0xF6C80F5A:
            lastIf=0xFFFFFFFF;
            script->Append(0x25);
            if(addNot)
            {
              script->Script("Not");
            }
            script->Script(qbKey);
            if(*pFile==0x16)
            {
              pFile++;
              script->Script(*(Checksum*)pFile);
              pFile+=4;
            }
            script->EndScript();

            ifs[ifCounter]=true;
            break;
          default:
            if(addNot)
              lastIf=0xFFFFFFFF;
            else
              lastIf=script->size;
            ifs[ifCounter]=false;
            break;

          }
        }
        break;
      case 0x26:
        if(script)
        {
          if(ifs[ifCounter])
          {
            script->Append(0x26);
            script->EndScript();
          }
          else if(lastIf!=0xFFFFFFFF)
          {
            script->size=lastIf;
          }
        }
        break;
      case 0x28:
        if(script)
        {
          if(ifs[ifCounter])
          {
            ifs[ifCounter]=false;
            script->Append(0x28);
            script->EndScript();
          }
          ifCounter--;
        }
        break;
      case 0x1C:
      case 0x1B:
        pFile+=*(DWORD*)pFile+4;
        break;
        //case 0x18:
      case 0x17:
      case 0x1A:
      case 0x2E:
      case 2:
        pFile+=4;
        break;
      case 1:
      case 0x3:
      case 0x4:
      case 5:
      case 6:
      case 7:
      case 9:
      case 0x2D:
      case 0x18:
      case 0x0E:
      case 0x0F:
      case 0x39:
      case 0x30:
        break;

      case 0x16:
        if(script)
        {
          pFile = ParseScript(pFile, &script, scripts, sounds, THUG, &extraTriggers);
        }
        else
          pFile+=4;
        break;
      case 0x1F:
        pFile+=8;
        break;

      case 0x1E:
        pFile+=12;
        break;
      case 0x2B:
        extern vector<Script> Scripts;
        DWORD check;
        check = *(DWORD*)pFile;
        pFile+=4;
        Scripts.push_back((Script(check, (char*)pFile)));
        while(*pFile != 0x0)
          pFile++;
        pFile++;
        break;
      case 0x40:
      case 0x2F:
      case 0x37:
        //MessageBox(0,"rand","",0);
        THUG?pFile+=*(DWORD*)pFile*6+4:pFile+=*(DWORD*)pFile*4+4;
        break;
      case 0:
        goto done;
      case 0x23:
        for(signed char i=ifCounter; i>=0; i--)
          ifs[ifCounter]=false;
        ifCounter=-1;
        //added=false;
        pFile++;
        scripts.push_back(KnownScript(*(Checksum*)pFile, NULL, 0));
        script = &scripts.back();
        pFile+=4;
        /*pFile = AddParams(pFile, script, THUG);
        script->EndScript();*/
        break;
      case 0x24:
        if(!script->size)
          scripts.pop_back();
        script = NULL;
        break;

      }
    }
done:
    char atoff[124];
    sprintf(atoff, "%p", pFile);
    MessageBox(0, atoff, atoff,0);
    if(scripts.size())
    {
      MessageBox(0,"done","",0);
      for(DWORD i=0, numTriggers = triggers.size(); i<numTriggers; i++)
      {
        if(!GetScript(triggers[i].checksum))
        {
          for(DWORD j=0, numScripts = scripts.size(); j<numScripts; j++)
          {
            if(triggers[i].checksum==scripts[j].name.checksum)
            {
              KnownScripts.push_back(scripts[j]);
              break;
            }
          }
        }
      }
      for(DWORD i=0, numTriggers = extraTriggers.size(); i<numTriggers; i++)
      {
        bool found = false;
        if(!GetScript(extraTriggers[i].checksum))
        {
          for(DWORD j=0, numScripts = scripts.size(); j<numScripts; j++)
          {
            if(extraTriggers[i].checksum==scripts[j].name.checksum)
            {
              found=true;
              KnownScripts.push_back(scripts[j]);
              if(!GetTrigger(extraTriggers[i].checksum))
                triggers.push_back(extraTriggers[i].checksum);
              break;
            }
          }

          if(!found)
          {
            for(DWORD j=0, numScripts = scripts.size(); j<numScripts; j++)
            {
              DWORD gotScript=0xFFFFFFFF;
              pFile=scripts[j].func;
              eof=scripts[j].func+scripts[j].size;
              while(pFile<eof)
              {
                const BYTE op = *pFile;
                pFile++;
                switch(op)
                {
                case 0x49:
                case 0x48:
                case 0x47:
                  pFile += 2;
                  break;
                case 0x1B:
                case 0x1C:
                  pFile+=*(DWORD*)(pFile)+4;
                  break;
                case 0x1:
                case 0x2:
                  if(gotScript!=0xFFFFFFFF)
                  {
                    scripts[j].size=gotScript;
                    goto done1;
                  }
                  pFile+=4;
                  break;
                case 0x1E:
                  pFile+=12;
                  break;
                case 0x16:
                  if(*(DWORD*)pFile==extraTriggers[i].checksum)
                    gotScript=pFile-scripts[j].func-1;
                  pFile+=4;
                  break;

                case 0x17:
                case 0x1A:
                case 0x2E:
                  pFile+=4;
                  break;
                case 0x40:
                case 0x2F:
                case 0x37:
                  pFile+=THUG?*(DWORD*)pFile*6+4:*(DWORD*)pFile*4+4;
                  break;
                }
              }
done1:;
            }
            for(DWORD j=0, numScripts = KnownScripts.size(); j<numScripts; j++)
            {
              DWORD gotScript=0xFFFFFFFF;
              pFile=KnownScripts[j].func;
              eof=KnownScripts[j].func+KnownScripts[j].size;
              while(pFile<eof)
              {
                const BYTE op = *pFile;
                pFile++;
                switch(op)
                {
                case 0x49:
                case 0x48:
                case 0x47:
                  pFile += 2;
                  break;
                case 0x1B:
                case 0x1C:
                  pFile+=*(DWORD*)(pFile)+4;
                  break;
                case 0x1:
                case 0x2:
                  if(gotScript!=0xFFFFFFFF)
                  {
                    KnownScripts[j].size=gotScript;
                    goto done2;
                  }
                  pFile+=4;
                  break;
                case 0x1E:
                  pFile+=12;
                  break;
                case 0x16:
                  if(*(DWORD*)pFile==extraTriggers[i].checksum)
                    gotScript=pFile-KnownScripts[j].func-1;
                  pFile+=4;
                  break;

                case 0x17:
                case 0x1A:
                case 0x2E:
                  pFile+=4;
                  break;
                case 0x40:
                case 0x2F:
                case 0x37:
                  pFile+=THUG?*(DWORD*)pFile*6+4:*(DWORD*)pFile*4+4;
                  break;
                }
              }
done2:;
            }
          }
        }
        else if(!GetTrigger(extraTriggers[i].checksum))
        {
          for(DWORD j=0, numScripts = scripts.size(); j<numScripts; j++)
          {
            if(extraTriggers[i].checksum==scripts[j].name.checksum)
            {
              found=true;
              triggers.push_back(extraTriggers[i].checksum);
              break;
            }
          }
          if(!found)
          {
            for(DWORD j=0, numScripts = scripts.size(); j<numScripts; j++)
            {
              DWORD gotScript=0xFFFFFFFF;
              pFile=scripts[j].func;
              eof=scripts[j].func+scripts[j].size;
              while(pFile<eof)
              {
                const BYTE op = *pFile;
                pFile++;
                switch(op)
                {
                case 0x49:
                case 0x48:
                case 0x47:
                  pFile += 2;
                  break;
                case 0x1B:
                case 0x1C:
                  pFile+=*(DWORD*)(pFile)+4;
                  break;
                case 0x1:
                case 0x2:
                  if(gotScript!=0xFFFFFFFF)
                  {
                    scripts[j].size=gotScript;
                    goto done3;
                  }
                  pFile+=4;
                  break;
                case 0x1E:
                  pFile+=12;
                  break;
                case 0x16:
                  if(*(DWORD*)pFile==extraTriggers[i].checksum)
                    gotScript=pFile-scripts[j].func-1;
                  pFile+=4;
                  break;

                case 0x17:
                case 0x1A:
                case 0x2E:
                  pFile+=4;
                  break;
                case 0x40:
                case 0x2F:
                case 0x37:
                  pFile+=THUG?*(DWORD*)pFile*6+4:*(DWORD*)pFile*4+4;
                  break;
                }
              }
done3:;
            }
            for(DWORD j=0, numScripts = KnownScripts.size(); j<numScripts; j++)
            {
              DWORD gotScript=0xFFFFFFFF;
              pFile=KnownScripts[j].func;
              eof=KnownScripts[j].func+KnownScripts[j].size;
              while(pFile<eof)
              {
                const BYTE op = *pFile;
                pFile++;
                switch(op)
                {
                case 0x49:
                case 0x48:
                case 0x47:
                  pFile += 2;
                  break;
                case 0x1B:
                case 0x1C:
                  pFile+=*(DWORD*)(pFile)+4;
                  break;
                case 0x1:
                case 0x2:
                  if(gotScript!=0xFFFFFFFF)
                  {
                    KnownScripts[j].size=gotScript;
                    goto done4;
                  }
                  pFile+=4;
                  break;
                case 0x1E:
                  pFile+=12;
                  break;
                case 0x16:
                  if(*(DWORD*)pFile==extraTriggers[i].checksum)
                    gotScript=pFile-KnownScripts[j].func-1;
                  pFile+=4;
                  break;

                case 0x17:
                case 0x1A:
                case 0x2E:
                  pFile+=4;
                  break;
                case 0x40:
                case 0x2F:
                case 0x37:
                  pFile+=THUG?*(DWORD*)pFile*6+4:*(DWORD*)pFile*4+4;
                  break;
                }
              }
done4:;
            }
          }
        }
      }
    }
    else
      MessageBox(0,"empty??","",0);
    extraTriggers.clear();
    delete [] pData;
  }
}

namespace Scripts
{
#define def(x) x = "#x"
  static const char* def(EnvironmentObject);
  const char Angles[] = "Angles";
};
extern const char Angles[] = "Angles";
__declspec (noalias) void Scene::LoadThugNodeArray(register const BYTE const* __restrict pFile, const BYTE const* __restrict eof, vector<KnownScript> &scripts, vector<Script> &sounds, vector<ExtraLayerMesh> &extraLayerMeshes)
{
  //vector<Node> compressedNodes;
  AddCompressedNodes(pFile,eof, compressedNodes);//, scripts);
  if(compressedNodes.size() && scripts.size())
    MessageBox(0,"yea","yea",0);
  else
    MessageBox(0,"no","",0);

  DWORD numRails = 0;
  BYTE ident=0;
  pFile += 5;
  Node* node = NULL;
  KnownScript* script = NULL;
  Node* compNode = NULL;
  bool nodesDone = false;
  //bool added=false;
  bool ifs[32];
  signed char ifCounter=-1;
  DWORD lastIf=0;
  /*DWORD FuckedEndGapSize=0;
  BYTE FuckedEndGap[900];*/

  while(true)
  {
    const BYTE opcode = *pFile;
    pFile++;
    switch(opcode)
    {
    case 0x49:
    case 0x48:
    case 0x47:
      pFile += 2;
      break;
    case 0x25:
      if(script)
      {
        ifCounter++;
        bool addNot=false;
        while(*pFile != 0x16) {if(*pFile==0x39) addNot=true; pFile++;}
        pFile++;
        const DWORD qbKey = *(DWORD*)pFile;
        pFile+=4;
        switch(qbKey)
        {
        case 0x9CB87E60:
          lastIf=0xFFFFFFFF;
          script->Append(0x25);
          if(addNot)
          {
            script->Script("Not");
          }
          script->Script(qbKey);
          script->EndScript();
          ifs[ifCounter]=true;
          break;
        case 0x2612FB96:
          lastIf=0xFFFFFFFF;
          script->Append(0x25);
          if(addNot)
          {
            script->Script("Not");
          }
          script->Script(qbKey);
          while(true)
          {
            BYTE c;
            c = *pFile;
            pFile++;
            if(c!=0x9 && c!=0x0E && c!=0x0F)
            {
              script->Append(c);
              switch(c)
              {
              case 0x1C:
              case 0x1B:
                DWORD len;
                len = *(DWORD*)pFile+4;
                script->Append(pFile, len);
                pFile+=len;
                break;
              case 0x1E:
                script->Append(pFile, 12);
                pFile+=12;
                break;
              case 0x16:
              case 0x17:
              case 0x1A:
              case 0x2E:
                script->Append(*(Checksum*)pFile);
                pFile+=4;
                break;
              case 0x2:
              case 0x1:
                script->size--;
                goto dne;
                break;
              case 0x40:
              case 0x2F:
              case 0x37:
                const DWORD numRands = *(DWORD*)pFile;
                script->Append(*(Checksum*)&numRands);
                pFile+=numRands*2+4;
                const DWORD size = numRands*4;
                fprintf(stdout, "adding RNdIf2\nsize %u num %u\n", size, numRands);
                script->Append(pFile, size);
                pFile+=size;
                break;
              }
            }
          }
          break;
        case 0x3A717382:
          script->Append(0x25);
          if(addNot)
          {
            script->Script("Not");
          }
          script->Script(qbKey);
          if(*pFile==0x16)
          {
            pFile++;
            script->Script(*(DWORD*)pFile);
            pFile+=4;
          }
          else if(*pFile==0x5)
          {
            BYTE c;
            __assume(c!=0x6);
            while(true)
            {
              c = *pFile;
              pFile++;
              if(c!=0x9 && c!=0x0E && c!=0x0F)
              {
                script->Append(c);
                switch(c)
                {
                case 0x1C:
                case 0x1B:
                  DWORD len;
                  len = *(DWORD*)pFile+4;
                  script->Append(pFile, len);
                  pFile+=len;
                  break;
                case 0x16:
                  script->Append(*(Checksum*)pFile);
                  pFile+=4;
                  break;
                case 0x2:
                case 0x1:
                  script->size--;
                  script->Append(0x6);
                  goto dne;
                  break;
                case 0x6:
                  goto dne;
                  break;
                }
              }
            }
          }
dne:
          script->EndScript();
          ifs[ifCounter]=true;
          break;
        case Checksums::InNetGame:
        case 0x850B3CCA:
          script->Append(0x25);
          if (addNot)
          {
            script->Script("Not");
          }
          script->Script(qbKey);
          script->EndScript();

          ifs[ifCounter] = true;
          break;
        case 0x7D873131:
          if (addNot)
          {
            script->Append(0x25);
            script->Script("Not");

            script->Script(qbKey);
            script->EndScript();

            ifs[ifCounter] = true;
          }
          else
          {
            pFile = Ignore(pFile, eof, true);
            ifCounter--;
          }
          break;
        case 0xCF66717B:
        case 0xE565465B:
        case 0xF6C80F5A:
          script->Append(0x25);
          if(addNot)
          {
            script->Script("Not");
          }
          script->Script(qbKey);
          if(*pFile==0x16)
          {
            pFile++;
            script->Script(*(Checksum*)pFile);
            pFile+=4;
          }
          script->EndScript();

          ifs[ifCounter]=true;
          break;
        default:
          ifs[ifCounter]=false;
          break;

        }
      }
      break;
    case 0x26:
      if(script)
      {
        if(ifs[ifCounter])
        {
          script->Append(0x26);
          script->EndScript();
        }
      }
      break;
    case 0x28:
      if(script)
      {
        if(ifs[ifCounter])
        {
          ifs[ifCounter]=false;
          script->Append(0x28);
          script->EndScript();
        }
        ifCounter--;
      }
      break;
    case 3:
      ident++;
      if(ident==1 && !script && !nodesDone)
      {
        nodes.push_back((Node()));
        node = &nodes.back();
      }
      break;
    case 0x1C:
    case 0x1B:
      pFile+=*(DWORD*)pFile+4;
      break;
      //case 0x18:
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
    case 0x23:
      for(signed char i=ifCounter; i>=0; i--)
        ifs[ifCounter]=false;
      ifCounter=-1;
      //added=false;
      pFile++;
      KnownScripts.push_back((KnownScript(*(Checksum*)pFile, NULL, 0)));
      pFile+=4;
      script = &KnownScripts.back();
      //MessageBox(0,"script",script->name.GetString(),0);
      break;
    case 0x24:
      if(!script->size)
        KnownScripts.pop_back();
      script=NULL;
      break;
    case 4:
      ident--;
      if(node==NULL)
        nodesDone=true;
      if(ident==0)
      {
        //MessageBox(0,"added node", node->Name.GetString(), 0);
        node = NULL;
      }
      break;
    case 6:
      if(node==NULL)
        nodesDone=true;
      break;
    case 0:
    case 0x2B:
      goto qbTable;
      break;
    case 0x40:
    case 0x2F:
    case 0x37:
      pFile+=*(DWORD*)pFile*6+4;
      break;
    case 0x16:
      if(node)
      {
        const DWORD qbKey = *(DWORD*)pFile;
        pFile+=4;

        switch(qbKey)
        {
        case Checksums::Pos:
          //MessageBox(0,"found pos", "", 0);
            while (*pFile == 0x2 || *pFile == 0x1)
            {
                
                if (*pFile == 0x2)
                    pFile += 5;
                else
                    pFile++;
            }
          pFile+=2;
          node->Position = *(SimpleVertex*)pFile;
          node->Position.z *= -1;
          pFile+=sizeof(SimpleVertex);
          break;
        case Checksums::Angles:
            while (*pFile == 0x2 || *pFile == 0x1)
            {

                if (*pFile == 0x2)
                    pFile += 5;
                else
                    pFile++;
            }
          pFile+=2;
          node->Angles = *(SimpleVertex*)pFile;
          node->Angles.y *= -1;
          //node->Angles.y += D3DX_PI;
          //node->Angles.x += D3DX_PI;
          pFile+=sizeof(SimpleVertex);
          break;
        case Checksums::Name:
            while (*pFile == 0x2 || *pFile == 0x1)
            {

                if (*pFile == 0x2)
                    pFile += 5;
                else
                    pFile++;
            }
          pFile+=2;
          node->Name = *(Checksum*)pFile;
          pFile+=sizeof(Checksum);
          break;
        case Checksums::TrickObject:
          node->TrickObject=true;
          break;
        case Checksums::Class:
            while (*pFile == 0x2 || *pFile == 0x1)
            {

                if (*pFile == 0x2)
                    pFile += 5;
                else
                    pFile++;
            }
          pFile+=2;
          node->Class = *(Checksum*)pFile;
          if(node->Class.checksum == RailNode::GetClass())
          {
            char nodename[128];
            sprintf(nodename, "rn%u", numRails);
            node->Name = Checksum(nodename);
            numRails++;
          }
          else if(node->Class.checksum == Checksums::Restart)
          {
            /*if(node->Angles.y<0.0f)
              node->Angles.y*=-1;*/
          }
          pFile+=sizeof(Checksum);
          break;
        case Checksums::Type:
            while (*pFile == 0x2 || *pFile == 0x1)
            {

                if (*pFile == 0x2)
                    pFile += 5;
                else
                    pFile++;
            }
          pFile+=2;
          node->Type = *(Checksum*)pFile;
          if(node->Type.checksum==Checksums::PedAI || node->Type.checksum==Checksums::DEFAULT)
          {
            node->Class.checksum=0;
          }
          pFile+=sizeof(Checksum);
          break;

        case Checksums::TriggerScripts:
            while (*pFile == 0x2 || *pFile == 0x1)
            {

                if (*pFile == 0x2)
                    pFile += 5;
                else
                    pFile++;
            }
          pFile+=2;
          node->Trigger = *(Checksum*)pFile;
          pFile+=sizeof(Checksum);
          triggers.push_back(node->Trigger);
          break;

        case Checksums::Cluster:
          pFile+=2;
          node->Cluster = *(Checksum*)pFile;
          pFile+=sizeof(Checksum);
          break;

        case Checksums::TerrainType:
            while (*pFile == 0x2 || *pFile == 0x1)
            {

                if (*pFile == 0x2)
                    pFile += 5;
                else
                    pFile++;
            }
          pFile += 2;
          node->TerrainType = *(Checksum*)pFile;
          pFile += sizeof(Checksum);
          break;

        case Checksums::AbsentInNetGames:
          node->AbsentInNetGames = true;
          break;

        case Checksums::NetEnabled:
          node->NetEnabled = true;
          break;

        case Checksums::Permanent:
          node->Permanent = true;
          break;

        case Checksums::Links:
            while (*pFile != 0x17)
                pFile++;
            pFile++;
          /*if(*(DWORD*)pFile>=0xFFFF)
          MessageBox(0, "...", "...", 0);*/
          if(node->Class.checksum==Checksums::Waypoint)
          {
              fprintf(stdout, "Waypoint %s\n", node->Name.GetString2());
            /*node->Class.checksum=0;
            if(node->Trigger.checksum)
              triggers.pop_back();*/
          }
          node->Links.push_back(*(WORD*)pFile);
          pFile+=4;
          while(true)
          {
            const BYTE opcode = *pFile;
            pFile++;
            switch(opcode)
            {
            case 0x2:
                pFile += 4;
                break;
            case 0x17:
              node->Links.push_back(*(WORD*)pFile);
              pFile+=4;
              break;
            case 0x6:
              goto done;
            }
          }
done:
          break;

          case Checksums::CreatedAtStart:
          node->CreatedAtStart = true;
          break;

        default:
            /*bool comp = false;
          for(DWORD i=0, numComp = compressedNodes.size(); i<numComp; i++)
          {
            if(compressedNodes[i].Name.checksum == qbKey)
            {
                comp = true;
              node->Compressed.checksum = qbKey;
              if(compressedNodes[i].Class.checksum && !node->Class.checksum)
              {
                node->Class = compressedNodes[i].Class;
                if(node->Class.checksum == RailNode::GetClass())
                {
                  char nodename[128];
                  sprintf(nodename, "rn%u", numRails);
                  node->Name = Checksum(nodename);
                  numRails++;
                }
              }
              if (compressedNodes[i].Angles.x || compressedNodes[i].Angles.y || compressedNodes[i].Angles.z)
              {
                  node->Angles = compressedNodes[i].Angles;
              }
              if(compressedNodes[i].Type.checksum && !node->Type.checksum)
                node->Type = compressedNodes[i].Type;
              if(compressedNodes[i].RestartName.checksum && !node->RestartName.checksum)
                node->RestartName = compressedNodes[i].RestartName;
              if(compressedNodes[i].Cluster.checksum && !node->Cluster.checksum)
                node->Cluster = compressedNodes[i].Cluster;
              if(compressedNodes[i].Trigger.checksum && !node->Trigger.checksum)
                node->Trigger = compressedNodes[i].Trigger;
              if(compressedNodes[i].Links.size())
              {
                for(DWORD j=0, numLinks = compressedNodes[i].Links.size(); j<numLinks; j++)
                {
                  node->Links.push_back(compressedNodes[i].Links[j]);
                }
              }
              if (compressedNodes[i].TerrainType.checksum && !node->TerrainType.checksum)
                node->TerrainType = compressedNodes[i].TerrainType;
              if(compressedNodes[i].CreatedAtStart)
                node->CreatedAtStart = true;
              if(compressedNodes[i].AbsentInNetGames)
                node->AbsentInNetGames=true;
              if(compressedNodes[i].NetEnabled)
                node->NetEnabled=true;
              if(compressedNodes[i].Permanent)
                node->Permanent=true;
              if(compressedNodes[i].TrickObject)
                node->TrickObject=true;
              break;
            }
          }
          if (!comp)
          {*/
              compNode = FindCompressed(qbKey, pFile, eof);
              if (compNode)
              {
                  node->Compressed.checksum = qbKey;
                  if (compNode->Class.checksum)
                  {
                      node->Class = compNode->Class;
                      if (node->Class.checksum == RailNode::GetClass())
                      {
                          char nodename[128];
                          sprintf(nodename, "rn%u", numRails);
                          node->Name = Checksum(nodename);
                          numRails++;
                      }
                  }
                  if (compNode->Angles.x || compNode->Angles.y || compNode->Angles.z)
                  {
                      node->Angles = compNode->Angles;
                  }
                  if (compNode->Type.checksum && !node->Type.checksum)
                      node->Type = compNode->Type;
                  if (compNode->RestartName.checksum && !node->RestartName.checksum)
                      node->RestartName = compNode->RestartName;
                  if (compNode->Cluster.checksum && !node->Cluster.checksum)
                      node->Cluster = compNode->Cluster;
                  if (compNode->Trigger.checksum && !node->Trigger.checksum)
                      node->Trigger = compNode->Trigger;
                  if (compNode->Links.size())
                  {
                      for (DWORD j = 0, numLinks = compNode->Links.size(); j < numLinks; j++)
                      {
                          node->Links.push_back(compNode->Links[j]);
                      }
                  }
                  if (compNode->TerrainType.checksum && !node->TerrainType.checksum)
                      node->TerrainType = compNode->TerrainType;
                  if (compNode->CreatedAtStart)
                      node->CreatedAtStart = true;
                  if (compNode->AbsentInNetGames)
                      node->AbsentInNetGames = true;
                  if (compNode->NetEnabled)
                      node->NetEnabled = true;
                  if (compNode->Permanent)
                      node->Permanent = true;
                  if (compNode->TrickObject)
                      node->TrickObject = true;
              }
          //}
          break;

        case 0xFCD206E1:
            while (*pFile == 0x2 || *pFile == 0x1)
            {

                if (*pFile == 0x2)
                    pFile += 5;
                else
                    pFile++;
            }
          pFile+=2;
          const DWORD len = *(DWORD*)pFile;
          pFile+=4;
          node->RestartName = Checksum((char*)pFile);
          pFile+=len;
          break;
        }
      }
      else if(script)
      {
        pFile = ParseScript(pFile, &script, scripts, sounds, true);
      }
      else
        pFile+=4;
      break;
    }
  }
qbTable:
  /*Checksum("GapId");
  Checksum("Score");
  Checksum("Flags");
  Checksum("PURE_AIR");
  Checksum("text");*/
  //compressedNodes.clear(); 
  DWORD numCrowns=0;
  for(DWORD i=0, numNodes = nodes.size(); i<numNodes; i++)
  {
#ifdef remove
    if(nodes[i].CreatedAtStart)
    {
#endif
      if(nodes[i].Type.checksum == 0xAF86421B)//crown
      {
        if(numCrowns<6)
        {
          nodes[i].Class = Checksum("GenericNode");
          numCrowns++;
          nodes.push_back(nodes[i]);
          char name[128];
          sprintf(name, "TRG_Zone%u", numCrowns);
          nodes.back().Name = Checksum(name);
          nodes.back().Type = Checksum("ZONE");
        }
        else
        {
          nodes[i].Type = Checksum("ZONE");
        }
      }
      else if(nodes[i].Class.checksum == 0xEF59C100)
      {
        switch(nodes[i].Type.checksum)
        {
          case 0xBEBB41F0://red
            break;
          case 0x8CCA938A://blue
            if(nodes[i].Name.checksum == 0x06270B12)
            {
              nodes[i].Name = Checksum("TRG_CTF_Team1");
              nodes[i].Class = Checksum("GenericNode");
              nodes[i].Type = Checksum("CTF_1");

              nodes.push_back(Restart());
              nodes.back().Position = nodes[i].Position;
              nodes.back().Angles = nodes[i].Angles;
              nodes.back().Name = Checksum("team1");
              nodes.back().CreatedAtStart=true;
              nodes.back().RestartName = Checksum("Team: CTF");
            }
            else
            {
              fprintf(stdout, "Deleting GameObject Node %s\n", nodes[i].Name.GetString2());
              DeleteNode(i);
              i--;
              numNodes--;
            }
            break;
          case 0xC3EBE05E://green
            break;
          case 0xC2AF1EB1://yellow
            if(nodes[i].Name.checksum == 0x85E38F38)
            {
              nodes[i].Name = Checksum("TRG_CTF_Team2");
              nodes[i].Class = Checksum("GenericNode");
              nodes[i].Type = Checksum("CTF_2");

              nodes.push_back(Restart());
              nodes.back().Position = nodes[i].Position;
              nodes.back().Angles = nodes[i].Angles;
              nodes.back().Name = Checksum("tea21");
              nodes.back().CreatedAtStart=true;
              nodes.back().RestartName = Checksum("Team: CTF");
            }
            else
            {
                fprintf(stdout, "Deleting GameObject Node %s\n", nodes[i].Name.GetString2());
              DeleteNode(i);
              i--;
              numNodes--;
            }
            break;
          default:
            nodes[i].Class = Checksum("EnvironmentObject");
            bool movedMesh = false;
            for(DWORD j=0, numMeshes = meshes.size(); j<numMeshes; j++)
            {
              if(meshes[j].GetName().checksum == nodes[i].Name.checksum)
              {
                D3DXVECTOR3 center = meshes[j].GetCenter();
                if(center.x == 0 && center.y == 0 && center.z == 0)
                {
                    movedMesh = true;
                    fprintf(stdout, "moving GameObj %s\n", meshes[j].GetName().GetString2());
                    meshes[j].UnSetFlag(MeshFlags::visible);
                    meshes[j].SetFlag(MeshFlags::deleted);
                    CloneMesh(&meshes[j]);
                    Mesh &mesh = meshes.back();
                    mesh.SetFlag(MeshFlags::visible);
                    mesh.UnSetFlag(MeshFlags::deleted);
                    D3DXVECTOR3 angle = *(D3DXVECTOR3*)&nodes[i].Angles;
                    /*angle.y*=-1;
                    angle.y+=D3DX_PI;
                    angle.x+=D3DX_PI;*/
                    mesh.SetPosition((D3DXVECTOR3*)&nodes[i].Position, &angle);
                }
                else
                   fprintf(stdout, "NOT moving GameObj %s\n",meshes[j].GetName().GetString2());
                
                break;
              }
            }
            if (!movedMesh)
            {
                fprintf(stdout, "Deleting GameObject Node %s\n", nodes[i].Name.GetString2());
                DeleteNode(i);
                i--;
                numNodes--;
            }
            break;
        }
      }
      else if(nodes[i].Class.checksum == Checksums::LevelObject)// || nodes[i].Class.checksum == Checksums::LevelGeometry)
      {
        nodes[i].Class.checksum = Checksums::GameObject;
        for(DWORD j=0, numMeshes = meshes.size(); j<numMeshes; j++)
        {
          if(meshes[j].GetName().checksum == nodes[i].Name.checksum)
          {
            D3DXVECTOR3 center = meshes[j].GetCenter();
            /*if(center.x == 0 && center.y == 0 && center.z == 0)
            {
            MessageBox(0,nodes[i].Name.GetString(),"moving GameObj",0);*/
            meshes[j].UnSetFlag(MeshFlags::visible);
            meshes[j].SetFlag(MeshFlags::deleted);
            CloneMesh(&meshes[j]);
            Mesh &mesh = meshes.back();
            mesh.SetFlag(MeshFlags::visible);
            mesh.UnSetFlag(MeshFlags::deleted);
            D3DXVECTOR3 angle = *(D3DXVECTOR3*)&nodes[i].Angles;
            // angle.y*=-1;
            //angle.y+=D3DX_PI;
            //angle.x+=D3DX_PI;
            mesh.SetPosition((D3DXVECTOR3*)&nodes[i].Position, &angle);
            for (DWORD k = 0; k<extraLayerMeshes.size(); k++)
            {
              if (extraLayerMeshes[k].name.checksum == nodes[i].Name.checksum)
              {
                extraLayerMeshes[k].SetPosition((D3DXVECTOR3*)&nodes[i].Position, &angle);
              }
            }
            /* }
            else
            MessageBox(0,nodes[i].Name.GetString(),"NOT moving GameObj",0);*/
            break;
          }
        }
        /*DeleteNode(i);
        i--;
        numNodes--;*/
      }
      else if(nodes[i].Class.checksum == Checksums::LevelGeometry)
        nodes[i].Class = Checksum("EnvironmentObject");
      else if(nodes[i].Class.checksum == RailNode::GetClass())
      {
        if(nodes[i].AbsentInNetGames)
        {
          fprintf(stdout, "Deleting absent Node %s\n", nodes[i].Name.GetString2());
          DeleteNode(i);
          i--;
          numNodes--;
        }
      }
      else if(nodes[i].Class.checksum != EnvironmentObject::GetClass() && nodes[i].Class.checksum != Restart::GetClass() && nodes[i].Class.checksum != Checksum("Waypoint").checksum)
      {
        fprintf(stdout, "Deleting Node %s\n", nodes[i].Name.GetString2());
        DeleteNode(i);
        i--;
        numNodes--;
      }
#ifdef remove
    }
    else
    {
      if(nodes[i].Class.checksum == Checksums::LevelGeometry)
      {
        nodes[i].Class = EnvironmentObject::GetClass();
      }
      else if (nodes[i].Class.checksum == Checksums::LevelObject)
      {
          nodes[i].Class.checksum = Checksums::GameObject;
      }
      else if(nodes[i].Class.checksum != EnvironmentObject::GetClass() && nodes[i].Class.checksum != RailNode::GetClass() && nodes[i].Class.checksum != Restart::GetClass())// && nodes[i].Class.checksum != Checksum("Waypoint").checksum)
      {
        fprintf(stdout, "Deleting Node %s\n", nodes[i].Name.GetString2());
        DeleteNode(i);
        i--;
      }
      Mesh* mesh = GetMesh(nodes[i].Name);
      if(mesh)
      {
        fprintf(stdout, "Deleting Mesh %s\n", mesh->Name.GetString2());
        mesh->SetFlag(MeshFlags::deleted);
        mesh->UnSetFlag(MeshFlags::visible);
      }
      /*DeleteNode(i);
      i--;*/
    }
#endif
  }

  numFucked=0;
  for(DWORD i=0; i<nodes.size(); i++)
  {
    for(DWORD j=0; j<nodes[i].Links.size(); j++)
    {
      if(nodes[i].Links[j] >= nodes.size())
        nodes[i].Links[j] = nodes.size()-1;
    }
  }
  FixLinks();
  FixLinksFinal();
  if(nodes.size()>=0xFFFF)
    MessageBox(0, "NodeArray too big...", "", 0);


  for (DWORD i = 0; i < compressedNodes.size(); i++)
  {
      bool found = false;

      for (DWORD j = 0; j < nodes.size(); j++)
      {
          if (nodes[j].Compressed.checksum == compressedNodes[i].Name.checksum)
          {
              found = true;
              break;
          }
      }
      if (!found)
      {
          fprintf(stdout, "Deleting ncomp %s\n", compressedNodes[i].Name.GetString2());
          compressedNodes.erase(compressedNodes.begin() + i);
          i--;
      }
      else
      {
          if (!compressedNodes[i].CreatedAtStart && !compressedNodes[i].Class.checksum && !compressedNodes[i].Type.checksum && !compressedNodes[i].TrickObject && !compressedNodes[i].Cluster.checksum && !compressedNodes[i].AbsentInNetGames && !compressedNodes[i].TerrainType.checksum)
          {
              for (DWORD j = 0; j < nodes.size(); j++)
              {
                  if (nodes[j].Compressed.checksum == compressedNodes[i].Name.checksum)
                  {
                      fprintf(stdout, "Deleting Node %s\n", nodes[i].Name.GetString2());
                      DeleteNode(j);
                      j--;
                  }
              }
              fprintf(stdout, "Deleting ncomp %s\n", compressedNodes[i].Name.GetString2());
              compressedNodes.erase(compressedNodes.begin() + i);
              i--;
          }
          else
          {
              if (compressedNodes[i].Class.checksum == Checksums::LevelGeometry)
                  compressedNodes[i].Class = Checksum("EnvironmentObject");
              else if (compressedNodes[i].Class.checksum == Checksums::LevelObject)
                  compressedNodes[i].Class.checksum = Checksums::GameObject;
          }
      }
  }

  if (compressedNodes.size() == 0)
  {
      for (DWORD i = 0; i < nodes.size(); i++)
      {
          nodes[i].Compressed.checksum = 0;
      }
      GenerateCompressedNodes();
  }

}
_declspec (noalias) void Scene::GenerateCompressedNodes()
{
    DWORD numNodes = nodes.size();
    char msg[128] = "";
    
    for (DWORD i = 0; i < numNodes-1; i++)
    {
        if (!nodes[i].Compressed.checksum)
        {
            if (!nodes[i].Class.checksum)
                nodes[i].Class.checksum = EnvironmentObject::GetClass();
            Node maybeCompress = Node();
            unsigned int compressRate = 0;
            if (nodes[i].CreatedAtStart)
            {
                maybeCompress.CreatedAtStart = true;
                compressRate++;
            }
            if (nodes[i].TrickObject)
            {
                maybeCompress.TrickObject = true;
                compressRate++;
            }
            if (nodes[i].Cluster.checksum)
            {
                maybeCompress.Cluster = nodes[i].Cluster;
                compressRate++;
            }
            maybeCompress.Class = nodes[i].Class;
            if (compressRate>1)
            {
                sprintf(msg, "Node%d_ncomp", i);
                printf("Maybe compress: %s\n", msg);
                maybeCompress.Name = Checksum(msg);
                bool compress = false;

                for (DWORD j = i + 1; j < numNodes; j++)
                {
                    /*if (maybeCompress.CreatedAtStart && !nodes[j].CreatedAtStart)
                        goto Next;
                    if (maybeCompress.TrickObject && !nodes[j].TrickObject)
                        goto Next;
                    if (maybeCompress.Cluster.checksum && maybeCompress.Cluster.checksum != nodes[j].Cluster.checksum)
                        goto Next;
                    if (maybeCompress.Class.checksum && maybeCompress.Class.checksum != nodes[j].Class.checksum)
                        goto Next;
                    */
                    if (!nodes[j].Compressed.checksum)
                    {
                        if (((maybeCompress.CreatedAtStart && nodes[j].CreatedAtStart) || !maybeCompress.CreatedAtStart) && ((maybeCompress.TrickObject && nodes[j].TrickObject) || !maybeCompress.TrickObject) &&
                            ((maybeCompress.Cluster.checksum && maybeCompress.Cluster.checksum == nodes[j].Cluster.checksum) || !maybeCompress.Cluster.checksum) &&
                            ((maybeCompress.Class.checksum && maybeCompress.Class.checksum == nodes[j].Class.checksum) || !maybeCompress.Class.checksum))
                        {
                            compress = true;
                            printf("Setting compression on node %s[%d]\n", nodes[j].Name.GetString2(), j);
                            nodes[j].Compressed = maybeCompress.Name;
                        }
                    }
                    /*Next:
                        if(compress==false)
                          printf("Next? %s[%d]\n", nodes[j].Name.GetString2(), j);*/
                }
                if (compress)
                {
                    nodes[i].Compressed = maybeCompress.Name;

                    compressedNodes.push_back(maybeCompress);
                    printf("Added Compress: %s\n", maybeCompress.Name.GetString2());
                }
            }
        }
    }
}


__declspec (noalias) void Th4Scene::LoadProSkaterNodeArray(register const BYTE const* __restrict pFile, const BYTE const* __restrict eof, vector<KnownScript> &scripts, vector<Script> &sounds)
{
  DWORD numRails = 0;
  BYTE ident=0;
  pFile += 0xC;
  Node* node = NULL;
  KnownScript* script = NULL;
  bool nodesDone=false;
  bool ifs[32];
  signed char ifCounter=-1;
  DWORD lastIf=0;
  const BYTE* __restrict startOfFile = pFile;

  while(true)
  {
    const BYTE opcode = *pFile;
    pFile++;
    switch(opcode)
    {
    case 0x49:
    case 0x48:
    case 0x47:
      pFile += 2;
      break;
    case 0x25:
      if(script)
      {
        ifCounter++;
        bool addNot=false;
        while(*pFile != 0x16) {if(*pFile==0x39) addNot=true; pFile++;}
        pFile++;
        const DWORD qbKey = *(DWORD*)pFile;
        pFile+=4;
        switch(qbKey)
        {
        case 0x9CB87E60:
          lastIf=0xFFFFFFFF;
          script->Append(0x25);
          if(addNot)
          {
            script->Script("Not");
          }
          script->Script(qbKey);
          script->EndScript();
          ifs[ifCounter]=true;
          break;
        case 0x2612FB96:
          lastIf=0xFFFFFFFF;
          script->Append(0x25);
          if(addNot)
          {
            script->Script("Not");
          }
          script->Script(qbKey);
          while(true)
          {
            BYTE c;
            c = *pFile;
            pFile++;
            if(c!=0x9 && c!=0x0E && c!=0x0F)
            {
              script->Append(c);
              switch(c)
              {
              case 0x1C:
              case 0x1B:
                DWORD len;
                len = *(DWORD*)pFile+4;
                script->Append(pFile, len);
                pFile+=len;
                break;
              case 0x1E:
                script->Append(pFile, 12);
                pFile+=12;
                break;
              case 0x16:
              case 0x17:
              case 0x1A:
              case 0x2E:
                script->Append(*(Checksum*)pFile);
                pFile+=4;
                break;
              case 0x2:
              case 0x1:
                script->size--;
                goto dne;
                break;
              case 0x40:
              case 0x2F:
              case 0x37:
                const DWORD numRands = *(DWORD*)pFile;
                script->Append(*(Checksum*)&numRands);
                pFile+=4;
                const DWORD size = numRands*4;
                fprintf(stdout, "adding RNdIf0\nsize %u num %u\n", size, numRands);
                script->Append(pFile, size);
                pFile+=size;
                break;
              }
            }
          }
          break;
        case 0x3A717382:
          lastIf=0xFFFFFFFF;
          script->Append(0x25);
          if(addNot)
          {
            script->Script("Not");
          }
          script->Script(qbKey);
          if(*pFile==0x16)
          {
            pFile++;
            script->Script(*(DWORD*)pFile);
            pFile+=4;
          }
          else if(*pFile==0x5)
          {
            BYTE c;
            __assume(c!=0x6);
            while(true)
            {
              c = *pFile;
              pFile++;
              if(c!=0x9 && c!=0x0E && c!=0x0F)
              {
                script->Append(c);
                switch(c)
                {
                case 0x1B:
                case 0x1C:
                  DWORD len;
                  len = *(DWORD*)pFile+4;
                  pFile+=len;
                  break;
                case 0x16:
                  script->Append(*(Checksum*)pFile);
                  pFile+=4;
                  break;
                case 0x2:
                case 0x1:
                  script->size--;
                  script->Append(0x6);
                  goto dne;
                  break;
                case 0x6:
                  goto dne;
                  break;
                }
              }
            }
          }
dne:
          script->EndScript();
          ifs[ifCounter]=true;
          break;
        case Checksums::InNetGame:
        case 0x850B3CCA:
          lastIf = 0xFFFFFFFF;
          script->Append(0x25);
          if (addNot)
          {
            script->Script("Not");
          }
          script->Script(qbKey);
          script->EndScript();

          ifs[ifCounter] = true;
          break;
        case 0x7D873131:
          lastIf = 0xFFFFFFFF;
          if (addNot)
          {
            script->Append(0x25);
            script->Script("Not");

            script->Script(qbKey);
            script->EndScript();

            ifs[ifCounter] = true;
          }
          else
          {
            pFile = Ignore(pFile, eof, false);
            ifCounter--;
          }
          break;
        case 0xCF66717B:
        case 0xE565465B:
        case 0xF6C80F5A:
          lastIf=0xFFFFFFFF;
          script->Append(0x25);
          if(addNot)
          {
            script->Script("Not");
          }
          script->Script(qbKey);
          if(*pFile==0x16)
          {
            pFile++;
            script->Script(*(Checksum*)pFile);
            pFile+=4;
          }
          script->EndScript();

          ifs[ifCounter]=true;
          break;
        default:
          ifs[ifCounter]=false;
          if(addNot)
            lastIf=0xFFFFFFFF;
          else
            lastIf=script->size;
          break;

        }
      }
      break;
    case 0x26:
      if(script)
      {
        if(ifs[ifCounter])
        {
          script->Append(0x26);
          script->EndScript();
        }
        else if(lastIf!=0xFFFFFFFF)
        {
          script->size=lastIf;
        }
      }
      break;
    case 0x28:
      if(script)
      {
        if(ifs[ifCounter])
        {
          ifs[ifCounter]=false;
          script->Append(0x28);
          script->EndScript();
        }
        ifCounter--;
      }
      break;
    case 3:
      ident++;
      if(ident==1 && !script && !nodesDone)
      {
        nodes.push_back((Node()));
        node = &nodes.back();
      }
      break;
    case 0x1C:
    case 0x1B:
      pFile+=*(DWORD*)pFile+4;
      break;
      //case 0x18:
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
    case 0x23:
      for(signed char i=ifCounter; i>=0; i--)
        ifs[ifCounter]=false;
      ifCounter=-1;
      //added=false;
      pFile++;
      KnownScripts.push_back((KnownScript(*(Checksum*)pFile, NULL, 0)));
      pFile+=4;
      script = &KnownScripts.back();
      //MessageBox(0,"script",script->name.GetString(),0);
      break;
    case 0x24:
        if (script && !script->size)
            KnownScripts.pop_back();
        else if (!script)
            printf("End Function found without begin function.. @ location: %p\n", pFile- startOfFile);
      script=NULL;
      break;
    case 4:
      ident--;
      if(node==NULL)
        nodesDone=true;
      else if(ident==0)
      {
        printf("added node %s", node->Name.GetString());
        node = NULL;
      }
      break;
    case 0:
    case 0x2B:
      pFile--;
      goto qbTable;
      break;
    case 6:
      if(node==NULL)
        nodesDone=true;
      break;
    case 0x16:
      if(node)
      {
        const DWORD qbKey = *(DWORD*)pFile;
        pFile+=4;

        switch(qbKey)
        {
        case Checksums::Position:
          pFile+=2;
          node->Position = *(SimpleVertex*)pFile;
          pFile+=sizeof(SimpleVertex);
          break;
        case Checksums::Angles:
          pFile+=2;
          node->Angles = *(SimpleVertex*)pFile;
          pFile+=sizeof(SimpleVertex);
          break;
        case Checksums::Name:
          pFile+=2;
          node->Name = *(Checksum*)pFile;
          pFile+=sizeof(Checksum);
          break;
        case Checksums::TrickObject:
          node->TrickObject=true;
          break;
        case Checksums::Class:
          pFile+=2;
          node->Class = *(Checksum*)pFile;
          if(node->Class.checksum == RailNode::GetClass())
          {
            char nodename[128];
            sprintf(nodename, "rn%u", numRails);
            node->Name = Checksum(nodename);
            numRails++;
          }
          pFile+=sizeof(Checksum);
          break;
        case Checksums::Type:
          pFile+=2;
          //if(node->Class.checksum!=Checksum("GameObject").checksum)
          node->Type = *(Checksum*)pFile;
          pFile+=sizeof(Checksum);
          break;

        case Checksums::TriggerScripts:
          pFile+=2;
          if(node->Class.checksum!=Checksums::GameObject)
          {
            node->Trigger = *(Checksum*)pFile;
            pFile+=sizeof(Checksum);
            triggers.push_back(node->Trigger);
          }
          else
            pFile+=sizeof(Checksum);
          break;

        case Checksums::Cluster:
          pFile+=2;
          node->Cluster = *(Checksum*)pFile;
          pFile+=sizeof(Checksum);
          break;

        case Checksums::TerrainType:
          pFile += 2;
          node->TerrainType = *(Checksum*)pFile;
          pFile += sizeof(Checksum);
          break;

        case Checksums::AbsentInNetGames:
          node->AbsentInNetGames = true;
          break;

        case Checksums::NetEnabled:
          node->NetEnabled = true;
          break;

        case Checksums::Permanent:
          node->Permanent = true;
          break;

        case Checksums::Links:
          pFile+=3;
          /*if(*(DWORD*)pFile>=0xFFFF)
          MessageBox(0, "...", "...", 0);*/
          node->Links.push_back(*(WORD*)pFile);
          pFile+=4;
          while(true)
          {
            const BYTE opcode = *pFile;
            pFile++;
            switch(opcode)
            {
            case 0x17:
              node->Links.push_back(*(WORD*)pFile);
              pFile+=4;
              break;
            case 0x6:
              goto done;
            }
          }
done:
          break;

        case Checksums::CreatedAtStart:
          node->CreatedAtStart = true;
          break;

        case 0xFCD206E1:
          pFile+=2;
          const DWORD len = *(DWORD*)pFile;
          pFile+=4;
          node->RestartName = Checksum((char*)pFile);
          pFile+=len;
          break;
        }
      }
      else if(script)
      {
        pFile = ParseScript(pFile, &script, scripts, sounds, false);
      }
      else
        pFile+=4;
      break;
    case 0x40:
    case 0x2F:
    case 0x37:
      pFile+=*(DWORD*)pFile*4+4;
      break;
    }
  }
qbTable:
  ParseQBTable(pFile, eof);

  for(DWORD i=0, numNodes = nodes.size(); i<numNodes; i++)
  {
#ifdef remove
    if(nodes[i].CreatedAtStart)
    {
#endif
      //if(nodes[i].Class.checksum == 0xB7B3BD86/*Checksum("LevelObject").checksum*/ || nodes[i].Class.checksum == 0xBF4D3536/*Checksum("LevelGeometry").checksum*/)
      /*{
      nodes[i].Class = Checksum("EnvironmentObject");
      for(DWORD j=0, numMeshes = meshes.size(); j<numMeshes; j++)
      {
      if(meshes[j].GetName().checksum == nodes[i].Name.checksum)
      {
      D3DXVECTOR3 center = meshes[j].GetCenter();
      if(center.x == 0 && center.y == 0 && center.z == 0)
      {
      meshes[j].UnSetFlag(MeshFlags::visible);
      meshes[j].SetFlag(MeshFlags::deleted);
      CloneMesh(&meshes[j]);
      Mesh &mesh = meshes.back();
      mesh.SetFlag(MeshFlags::visible);
      mesh.UnSetFlag(MeshFlags::deleted);
      mesh.SetPosition((D3DXVECTOR3*)&nodes[i].Position, (D3DXVECTOR3*)&nodes[i].Angles);
      }
      break;
      }
      }
      }*/
      if(nodes[i].Type.checksum == 0xAF86421B)//crown
        continue;
      else if(nodes[i].Class.checksum == 0xEF59C100)
      {
        switch(nodes[i].Type.checksum)
        {
          case 0xBEBB41F0://red
            break;
          case 0x8CCA938A://blue
            if(nodes[i].Name.checksum == 0x06270B12)
            {
              nodes[i].Name = Checksum("TRG_CTF_Team1");
              nodes[i].Class = Checksum("GenericNode");
              nodes[i].Type = Checksum("CTF_1");

              nodes.push_back(Restart());
              nodes.back().Position = nodes[i].Position;
              nodes.back().Angles = nodes[i].Angles;
              nodes.back().Name = Checksum("team1");
              nodes.back().CreatedAtStart=true;
              nodes.back().RestartName = Checksum("Team: CTF");
            }
            else
            {
              DeleteNode(i);
              i--;
              numNodes--;
            }
            break;
          case 0xC3EBE05E://green
            break;
          case 0xC2AF1EB1://yellow
            if(nodes[i].Name.checksum == 0x85E38F38)
            {
              nodes[i].Name = Checksum("TRG_CTF_Team2");
              nodes[i].Class = Checksum("GenericNode");
              nodes[i].Type = Checksum("CTF_2");

              nodes.push_back(Restart());
              nodes.back().Position = nodes[i].Position;
              nodes.back().Angles = nodes[i].Angles;
              nodes.back().Name = Checksum("tea21");
              nodes.back().CreatedAtStart=true;
              nodes.back().RestartName = Checksum("Team: CTF");
            }
            else
            {
              DeleteNode(i);
              i--;
              numNodes--;
            }
            break;
          default:
            nodes[i].Class = Checksum("GameObject");
            for(DWORD j=0, numMeshes = meshes.size(); j<numMeshes; j++)
            {
              if(meshes[j].GetName().checksum == nodes[i].Name.checksum)
              {
                D3DXVECTOR3 center = meshes[j].GetCenter();
                /*if(center.x == 0 && center.y == 0 && center.z == 0)
                {
                MessageBox(0,nodes[i].Name.GetString(),"moving GameObj",0);*/
                meshes[j].UnSetFlag(MeshFlags::visible);
                meshes[j].SetFlag(MeshFlags::deleted);
                CloneMesh(&meshes[j]);
                Mesh &mesh = meshes.back();
                mesh.SetFlag(MeshFlags::visible);
                mesh.UnSetFlag(MeshFlags::deleted);
                D3DXVECTOR3 angle = *(D3DXVECTOR3*)&nodes[i].Angles;
                // angle.y*=-1;
                //  angle.y+=D3DX_PI;
                //angle.x+=D3DX_PI;
                mesh.SetPosition((D3DXVECTOR3*)&nodes[i].Position, &angle);
                for(DWORD k=0; k<extraLayerMeshes.size(); k++)
                {
                  if(extraLayerMeshes[k].name.checksum == nodes[i].Name.checksum)
                  {
                    extraLayerMeshes[k].SetPosition((D3DXVECTOR3*)&nodes[i].Position, &angle);
                  }
                }
                /* }
                else
                MessageBox(0,nodes[i].Name.GetString(),"NOT moving GameObj",0);*/
                break;
              }
            }
            break;
        }
      }
      else if(nodes[i].Class.checksum == Checksums::LevelObject)
      {
        nodes[i].Class = Checksum("GameObject");
        for(DWORD j=0, numMeshes = meshes.size(); j<numMeshes; j++)
        {
          if(meshes[j].GetName().checksum == nodes[i].Name.checksum)
          {
            D3DXVECTOR3 center = meshes[j].GetCenter();

            meshes[j].UnSetFlag(MeshFlags::visible);
            meshes[j].SetFlag(MeshFlags::deleted);
            CloneMesh(&meshes[j]);
            Mesh &mesh = meshes.back();
            mesh.SetFlag(MeshFlags::visible);
            mesh.UnSetFlag(MeshFlags::deleted);
            D3DXVECTOR3 angle = *(D3DXVECTOR3*)&nodes[i].Angles;

            mesh.SetPosition((D3DXVECTOR3*)&nodes[i].Position, &angle);

            for(DWORD k=0; k<extraLayerMeshes.size(); k++)
            {
              if(extraLayerMeshes[k].name.checksum == nodes[i].Name.checksum)
              {
                extraLayerMeshes[k].SetPosition((D3DXVECTOR3*)&nodes[i].Position, &angle);
              }
            }
            break;
          }
        }
      }
      else if (nodes[i].Class.checksum == Checksums::LevelGeometry)
      {
          nodes[i].Class = Checksum("EnvironmentObject");
          for (DWORD j = 0, numMeshes = meshes.size(); j < numMeshes; j++)
          {
              if (meshes[j].GetName().checksum == nodes[i].Name.checksum)
              {
                  D3DXVECTOR3 center = meshes[j].GetCenter();
                  if (center.x == 0 && center.y == 0 && center.z == 0)
                  {
                      nodes[i].Class = Checksum("GameObject");
                      //MessageBox(0,nodes[i].Name.GetString(),"moving GameObj",0);
                      meshes[j].UnSetFlag(MeshFlags::visible);
                      meshes[j].SetFlag(MeshFlags::deleted);
                      CloneMesh(&meshes[j]);
                      Mesh& mesh = meshes.back();
                      mesh.SetFlag(MeshFlags::visible);
                      mesh.UnSetFlag(MeshFlags::deleted);
                      D3DXVECTOR3 angle = *(D3DXVECTOR3*)&nodes[i].Angles;
                      // angle.y*=-1;
                      //  angle.y+=D3DX_PI;
                      //angle.x+=D3DX_PI;
                      mesh.SetPosition((D3DXVECTOR3*)&nodes[i].Position, &angle);
                      /*char Name[256] = "";
                      sprintf(Name, "%s_Coll", mesh.GetName().GetString());
                      DWORD tst = Checksum(Name).checksum;

                      for (DWORD k = 0; k < numMeshes; k++)
                      {

                      }*/
                      for (DWORD k = 0; k < extraLayerMeshes.size(); k++)
                      {
                          if (extraLayerMeshes[k].name.checksum == nodes[i].Name.checksum)
                          {
                              extraLayerMeshes[k].SetPosition((D3DXVECTOR3*)&nodes[i].Position, &angle);
                          }
                      }
                  }
                  /*else
                  MessageBox(0,nodes[i].Name.GetString(),"NOT moving GameObj",0);*/
                  break;
              }
          }
      }
      else if(nodes[i].Class.checksum == RailNode::GetClass())
      {
        if(!nodes[i].CreatedAtStart || nodes[i].AbsentInNetGames)
        {
          DeleteNode(i);
          i--;
          numNodes--;
        }
      }
      /*else if(nodes[i].Class.checksum == Checksum("GameObject").checksum)
      {
      nodes[i].Type=0;
      if(nodes[i].Trigger.checksum)
      EraseScript(nodes[i].Trigger.checksum);
      }*/
      else if(nodes[i].Class.checksum != Checksum("EnvironmentObject").checksum && nodes[i].Class.checksum != Checksum("GameObject").checksum/* && nodes[i].Class.checksum != Checksum("RailNode").checksum*/ && nodes[i].Class.checksum != Checksum("Restart").checksum && nodes[i].Class.checksum != Checksum("Waypoint").checksum)
      {
        DeleteNode(i);
        i--;
        numNodes--;
      }
      nodes[i].Trigger.checksum = 0;
      //KnownScripts.clear();
#ifdef remove
    }
    else
    {
      if(nodes[i].Class.checksum == Checksum("LevelObject").checksum || nodes[i].Class.checksum == Checksum("LevelGeometry").checksum)
      {
        nodes[i].Class = Checksum("EnvironmentObject");
      }
      else if(nodes[i].Class.checksum != Checksum("EnvironmentObject").checksum && nodes[i].Class.checksum != Checksum("RailNode").checksum && nodes[i].Class.checksum != Checksum("Restart").checksum && nodes[i].Class.checksum != Checksum("Waypoint").checksum)
      {
        DeleteNode(i);
        i--;
      }
      Mesh* mesh = GetMesh(nodes[i].Name);
      if(mesh)
      {
        mesh->SetFlag(MeshFlags::deleted);
        mesh->UnSetFlag(MeshFlags::visible);
      }
      /*DeleteNode(i);
      i--;*/
    }
#endif
  }
  numFucked=0;
#define MAX_LINK_DEPTH 32
  for(DWORD i=0; i<MAX_LINK_DEPTH; i++)
    FixLinks();
  FixLinksFinal();
  if(nodes.size()>=0xFFFF)
    MessageBox(0, "NodeArray too big...", "", 0);
}