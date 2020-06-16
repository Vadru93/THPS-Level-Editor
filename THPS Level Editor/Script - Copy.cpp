#include "Scene.h"

using std::vector;
#pragma unmanaged

__declspec (noalias) bool Scene::FixLinksTHUG()
{
  bool added=false;
  DWORD numFucked=0;
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
    if(nodes[i].Class.checksum == 0x8E6B02AD)
    {
      if(nodes[i].Links.size())
      {
        for(DWORD j=0; j<nodes[i].Links.size(); j++)
        {
          if(nodes[nodes[i].Links[j]].Class.checksum == 0x8E6B02AD && nodes[nodes[i].Links[j]].CreatedAtStart && !nodes[nodes[i].Links[j]].AbsentInNetGames)
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
  DWORD numFucked=0;
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
    if(nodes[i].Class.checksum == 0x8E6B02AD)
    {
      if(nodes[i].Links.size())
      {
        for(DWORD j=0; j<nodes[i].Links.size(); j++)
        {
          if(nodes[nodes[i].Links[j]].Class.checksum == 0x8E6B02AD && nodes[nodes[i].Links[j]].CreatedAtStart && !nodes[nodes[i].Links[j]].AbsentInNetGames)
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
  DWORD numFucked=0;
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
    if(nodes[i].Class.checksum == 0x8E6B02AD)
    {
      if(nodes[i].Links.size())
      {
        for(DWORD j=0; j<nodes[i].Links.size(); j++)
        {
          if(nodes[nodes[i].Links[j]].Class.checksum == 0x8E6B02AD && nodes[nodes[i].Links[j]].CreatedAtStart && !nodes[nodes[i].Links[j]].AbsentInNetGames)
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
                if(nodes[k].Class.checksum == 0x8E6B02AD)
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
                  nodes[i].Links.erase(nodes[i].Links.begin()+j);
                  j--;
                }
                else
                {
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


              }
              else
              {
                nodes[i].Links.erase(nodes[i].Links.begin()+j);
                j--;
              }
              //MessageBox(0,"wtgf","",0);
              /*if(nodes[nodes[i].Links[j]].Links.size())
              {
              const DWORD index = nodes[i].Links[j];
              nodes.push_back(nodes[i]);
              char name[128];
              sprintf(name, "FuckedRail%u", numFucked);
              nodes.back().Name = Checksum(name);
              nodes.back().Links.clear();
              nodes.back().Links.push_back((WORD)index);
              numFucked++;
              added=true;
              }
              else
              {
              const DWORD index = nodes.size();
              nodes.push_back(nodes[i]);
              char name[128];
              sprintf(name, "FuckedRail%u", numFucked);
              nodes.back().Name = Checksum(name);
              nodes.back().Links.clear();
              numFucked++;
              added=true;
              nodes[nodes[i].Links[j]].Links.push_back((WORD)index);
              }
              nodes[i].Links.erase(nodes[i].Links.begin()+j);
              j--;*/

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

const BYTE* AddParams(const BYTE* pFile, KnownScript* script)
{
  signed char id=0;
  while(true)
  {
    const BYTE op = *pFile;
    pFile++;
    script->Append(op);
    switch(op)
    {
    case 0x3:
      id++;
      break;
    case 0x4:
      id--;
      break;
    case 0x24:
      if(id>0)
      {
        MessageBox(0,"id","",0);
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
        MessageBox(0,"id","",0);
        script->size--;
        script->Append(0x4);
      }
      else
        script->size--;
      return pFile;
    case 0x2:
      if(id>0)
      {
        MessageBox(0,"id","",0);
        script->size--;
        script->Append(0x4);
      }
      else
        script->size--;
      pFile+=4;
      return pFile;
    case 0x16:
    case 0x17:
    case 0x1A:
    case 0x2E:
      script->Append((DWORD*)pFile);
      pFile+=4;
      break;
    case 0x40:
    case 0x2F:
      const DWORD numRands = *(DWORD*)pFile;
      script->Append(*(Checksum*)&numRands);
      pFile+=numRands*2+4;
      const DWORD size = numRands*4;
      MessageBox(0, "adding RNd", "",0);
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
    case 0x1C:
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
    case 0:
      goto done;
    }
  }
done:;
}

void Scene::LoadSound(Checksum sound, vector<Script> &sounds)
{
  bool found=false;
  switch(sound.checksum)
  {
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

__declspec (noalias) void Scene::AddCompressedNodes(register const BYTE const* __restrict pFile, const BYTE const* __restrict eof, vector<Node> &compressedNodes)//, vector<KnownScript> &scripts)
{
  while(pFile<eof)
  {
    const BYTE opcode = *pFile;
    pFile++;
    switch(opcode)
    {
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
      pFile+=*(DWORD*)pFile*6+4;
      break;
    case 0:
      return;
    default:
      char unk[5];
      sprintf(unk, "%X", opcode);
      MessageBox(0,"unk",unk,0);
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
          case 0x12B4E660:
            pFile+=2;
            node.Class = *(Checksum*)pFile;
            pFile+=sizeof(Checksum);
            break;
          case 0x7321A8D6:
            pFile+=2;
            node.Type = *(Checksum*)pFile;
            if(node.Type.checksum==Checksum("PedAI").checksum || node.Type.checksum==Checksum("default").checksum)
            {
              node.Class.checksum=0;
            }
            pFile+=sizeof(Checksum);
            break;
          case 0x1645B830:
            node.TrickObject=true;
            break;
          case 0x2CA8A299:
            pFile+=2;
            node.Trigger = *(Checksum*)pFile;
            pFile+=sizeof(Checksum);
            triggers.push_back(node.Trigger);
            break;

          case 0x1A3A966B:
            pFile+=2;
            node.Cluster = *(Checksum*)pFile;
            pFile+=sizeof(Checksum);
            break;

          case 0x68910AC6:
            node.AbsentInNetGames = true;
            break;

          case 0x20209C31:
            node.NetEnabled = true;
            break;

          case 0x23627FD7:
            node.Permanent = true;
            break;

          case 0x2E7D5EE7:
            pFile+=3;
            /*if(*(DWORD*)pFile>=0xFFFF)
            MessageBox(0, "...", "...", 0);*/
            if(node.Class.checksum==Checksum("Waypoint").checksum)
            {
              node.Class.checksum=0;
              if(node.Trigger.checksum)
                triggers.pop_back();
            }
            node.Links.push_back(*(WORD*)pFile);
            pFile+=4;
            while(true)
            {
              const BYTE opcode = *pFile;
              pFile++;
              switch(opcode)
              {
              case 0x17:
                node.Links.push_back(*(WORD*)pFile);
                pFile+=4;
                break;
              case 0x6:
                goto gotLinks;
              }
            }
gotLinks:
            break;

          case 0x7C2552B9:
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

__declspec (noalias) void Scene::LoadScriptsFile(char* path, vector<KnownScript> &scripts, vector<Script> &sounds)
{
  FILE* f = fopen(path, "rb");
  if(f)
  {
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
    /*DWORD FuckedEndGapSize=0;
    BYTE FuckedEndGap[900];*/

    const BYTE* eof = pFile+size;
    while(pFile<eof)
    {
      const BYTE opcode = *pFile;
      pFile++;
      switch(opcode)
      {
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
          case 0x3A717382:
            script->Append(0x25);
            if(addNot)
            {
              script->Append(0x16);
              script->Append(Checksum("Not"));
            }
            script->Append(0x16);
            script->Append(*(Checksum*)&qbKey);
            if(*pFile==0x16)
            {
              pFile++;
              script->Append(0x16);
              script->Append(*(Checksum*)pFile);
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
          case 0xCF66717B:
          case 0xE565465B:
          case 0xF6C80F5A:
            script->Append(0x25);
            if(addNot)
            {
              script->Append(0x16);
              script->Append(Checksum("Not"));
            }
            script->Append(0x16);
            script->Append(*(Checksum*)&qbKey);
            if(*pFile==0x16)
            {
              pFile++;
              script->Append(0x16);
              script->Append(*(Checksum*)pFile);
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
        /*case 0x25:

        if(script)
        {
        //bool neg = false;
        script->Append(0x25);
        script->Append(0x16);
        while(*pFile != 0x16) pFile++;
        pFile++;
        script->Append(*(Checksum*)pFile);
        pFile+=4;
        script->EndScript();
        }
        break;
        case 0x27:
        if(script)
        {
        script->Append(0x27);
        script->Append(0x16);
        while(*pFile != 0x16) pFile++;
        pFile++;
        script->Append(*(Checksum*)pFile);
        pFile+=4;
        script->EndScript();
        }
        break;
        case 0x28:
        case 0x26:
        if(script)
        {
        script->Append(opcode);
        script->EndScript();
        }
        break;*/
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
          const DWORD qbKey = *(DWORD*)pFile;
          pFile+=4;

          //bool found = false;
          switch(qbKey)
          {
            /* case 0x70291F04:
            script->Append(0x16);
            script->Append(Checksum("Create"));

            while(true)
            {
            const BYTE op = *pFile;
            pFile++;
            switch(op)
            {
            case 0x1:
            case 0x2:
            goto doneParsing;
            break;
            case 0x17:
            case 0x1A:
            case 0x2E:
            pFile+=4;
            break;
            case 0x40:
            case 0x2F:
            pFile+=*(DWORD*)pFile*6+4;
            break;
            case 0x16:
            const DWORD key = *(DWORD*)pFile;
            pFile+=4;
            switch(key)
            {
            case 0xA1DC81F9://78 BA 92 9F
            script->Append(0x16);
            script->Append(Checksum("Name"));
            script->Append(0x7);
            pFile++;
            if(*pFile==0x2D)
            {
            script->Append(0x2D);
            script->Append(0x16);
            pFile++;
            }
            else if(*pFile==0x16)
            {
            script->Append(0x16);
            }
            else
            {
            while(*pFile!=0x16)
            {
            pFile++;
            }
            }
            pFile++;
            script->Append(*(Checksum*)pFile);
            pFile+=4;
            break;
            case 0x6C4E7971:
            script->Append(0x16);
            script->Append(Checksum("Prefix"));
            const DWORD size = *(DWORD*)(pFile+2)+6;
            script->Append(pFile, size);
            pFile+=size;
            break;
            }
            break;
            }
            }
            case 0x854F17A6:
            script->Append(0x16);
            script->Append(Checksum("Visible"));

            while(true)
            {
            const BYTE op = *pFile;
            pFile++;
            switch(op)
            {
            case 0x1:
            case 0x2:
            goto doneParsing;
            break;
            case 0x17:
            case 0x1A:
            case 0x2E:
            pFile+=4;
            break;
            case 0x40:
            case 0x2F:
            pFile+=*(DWORD*)pFile*6+4;
            break;
            case 0x16:
            const DWORD key = *(DWORD*)pFile;
            pFile+=4;
            switch(key)
            {
            case 0xA1DC81F9://78 BA 92 9F
            script->Append(0x16);
            script->Append(Checksum("Name"));
            script->Append(0x7);
            pFile++;
            if(*pFile==0x2D)
            {
            script->Append(0x2D);
            script->Append(0x16);
            pFile++;
            }
            else if(*pFile==0x16)
            {
            script->Append(0x16);
            }
            else
            {
            while(*pFile!=0x16)
            {
            pFile++;
            }
            }
            pFile++;
            script->Append(*(Checksum*)pFile);
            pFile+=4;
            break;
            case 0x6C4E7971:
            script->Append(0x16);
            script->Append(Checksum("Prefix"));
            const DWORD size = *(DWORD*)(pFile+2)+6;
            script->Append(pFile, size);
            pFile+=size;
            break;
            }
            break;
            }
            }
            case 0xCB8B5900:
            script->Append(0x16);
            script->Append(Checksum("Invisible"));

            while(true)
            {
            const BYTE op = *pFile;
            pFile++;
            switch(op)
            {
            case 0x1:
            case 0x2:
            goto doneParsing;
            break;
            case 0x17:
            case 0x1A:
            case 0x2E:
            pFile+=4;
            break;
            case 0x40:
            case 0x2F:
            pFile+=*(DWORD*)pFile*6+4;
            break;
            case 0x16:
            const DWORD key = *(DWORD*)pFile;
            pFile+=4;
            switch(key)
            {
            case 0xA1DC81F9://78 BA 92 9F
            script->Append(0x16);
            script->Append(Checksum("Name"));
            script->Append(0x7);
            pFile++;
            if(*pFile==0x2D)
            {
            script->Append(0x2D);
            script->Append(0x16);
            pFile++;
            }
            else if(*pFile==0x16)
            {
            script->Append(0x16);
            }
            else
            {
            while(*pFile!=0x16)
            {
            pFile++;
            }
            }
            pFile++;
            script->Append(*(Checksum*)pFile);
            pFile+=4;
            break;
            case 0x6C4E7971:
            script->Append(0x16);
            script->Append(Checksum("Prefix"));
            const DWORD size = *(DWORD*)(pFile+2)+6;
            script->Append(pFile, size);
            pFile+=size;
            break;
            }
            break;
            }
            }
            case 0xF8D6A996:
            script->Append(0x16);
            script->Append(Checksum("Kill"));

            while(true)
            {
            const BYTE op = *pFile;
            pFile++;
            switch(op)
            {
            case 0x1:
            case 0x2:
            goto doneParsing;
            break;
            case 0x17:
            case 0x1A:
            case 0x2E:
            pFile+=4;
            break;
            case 0x40:
            case 0x2F:
            pFile+=*(DWORD*)pFile*6+4;
            break;
            case 0x16:
            const DWORD key = *(DWORD*)pFile;
            pFile+=4;
            switch(key)
            {
            case 0xA1DC81F9://78 BA 92 9F
            script->Append(0x16);
            script->Append(Checksum("Name"));
            script->Append(0x7);
            pFile++;
            if(*pFile==0x2D)
            {
            script->Append(0x2D);
            script->Append(0x16);
            pFile++;
            }
            else if(*pFile==0x16)
            {
            script->Append(0x16);
            }
            else
            {
            while(*pFile!=0x16)
            {
            MessageBox(0,"wf","wa",0);
            pFile++;
            }
            }
            pFile++;
            script->Append(*(Checksum*)pFile);
            pFile+=4;
            break;
            case 0x6C4E7971:
            script->Append(0x16);
            script->Append(Checksum("Prefix"));
            const DWORD size = *(DWORD*)(pFile+2)+6;
            script->Append(pFile, size);
            //MessageBox(0,"prefix","added",0);
            pFile+=size;
            break;
            }
            break;
            }
            }*/
          case 0x4562E4A6:
            script->Append(0x16);
            script->Append(*(Checksum*)&qbKey);
            script->EndScript();
            /*if(FuckedEndGapSize)
            {
            MessageBox(0,"exporting fucked","",0);
            script->Append(FuckedEndGap, FuckedEndGapSize);
            script->Append(0x16);
            script->Append(Checksum("GapId"));
            script->Append(0x07);
            script->Append(0x16);
            script->Append(Checksum("DefaultGapId"));
            script->EndScript();
            }
            else
            {
            script->Append(0x16);
            script->Append(Checksum("EndGap"));
            script->Append(0x16);
            script->Append(Checksum("GapId"));
            script->Append(0x07);
            script->Append(0x16);
            script->Append(Checksum("DuplicateTrigger"));
            script->EndScript();
            }*/
            break;
          case 0x9FBBD5F0:
          case 0x3F4B93F8:
            script->Append(0x16);
            script->Append(Checksum("Sk3_TeleportToNode"));

            while(true)
            {
              const BYTE op = *pFile;
              pFile++;
              switch(op)
              {
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
                pFile+=*(DWORD*)pFile*6+4;
                break;
              case 0x16:
                const DWORD key = *(DWORD*)pFile;
                pFile+=4;
                switch(key)
                {
                case 0x9F92BA78://78 BA 92 9F
                  script->Append(0x16);
                  script->Append(*(Checksum*)&key);
                  script->Append(0x7);
                  pFile++;
                  if(*pFile==0x2D)
                  {
                    script->Append(0x2D);
                    script->Append(0x16);
                    pFile++;
                  }
                  else if(*pFile==0x16)
                  {
                    script->Append(0x16);
                  }
                  else
                  {
                    while(*pFile!=0x16)
                    {
                      pFile++;
                    }
                  }
                  pFile++;
                  script->Append(*(Checksum*)pFile);
                  pFile+=4;
                  break;
                }
                break;
              }
            }
          case 0xAFA20E18:
          case 0x5151B2B1:
            script->Append(0x16);
            script->Append(*(Checksum*)&qbKey);
            if(*pFile==0x2D)
            {
              script->Append(0x2D);
              script->Append(0x16);
              pFile++;
            }
            else if(*pFile==0x16)
            {
              script->Append(0x16);
            }
            else
            {
              while(*pFile!=0x16)
              {
                pFile++;
              }
            }
            pFile++;
            DWORD checkc;
            checkc = *(DWORD*)pFile;
            if(checkc==0xB0A2A4C5)//C5 A4 A2 B0
              *(Checksum*)&checkc=Checksum("glasspane2x");
            script->Append(*(Checksum*)&checkc);
            LoadSound(*(Checksum*)&checkc, sounds);
            pFile+=4;
            script->EndScript();
            break;
          case 0xE5399FB2:
            DWORD start;
            /*bool foundEndId;
            foundEndId=false;*/
            start = script->size;
            script->Append(0x16);
            script->Append(Checksum("EndGap"));
            //script->Append(0x2C);
            while(true)
            {
              const BYTE op = *pFile;
              pFile++;
              switch(op)
              {
              case 0x0A:
                pFile++;
                script->InsertSub(start, Checksum("id"), *(DWORD*)pFile);
                pFile+=4;
                break;
              case 0x0B:
                pFile++;
                script->InsertAdd(start, Checksum("id"), *(DWORD*)pFile);
                pFile+=4;
                break;
              case 0x1B:
              case 0x1C:
                pFile += *(DWORD*)(pFile)+4;
                break;
              case 0x1:
                /*if(!foundEndId)
                {
                /*if(script->size-start)
                {
                FuckedEndGapSize = script->size-start;
                memcpy(FuckedEndGap, &script->func[start], FuckedEndGapSize);
                }
                script->size=start;*/

                /*    script->Append(0x16);
                script->Append(Checksum("GapId"));
                script->Append(0x07);
                script->Append(0x16);
                script->Append(Checksum("DefaultGapId"));

                /*found=true;
                break;*/
                //  }

                goto doneParsing;
                break;
              case 0x2:
                pFile+=4;
                /*if(!foundEndId)
                {

                script->Append(0x16);
                script->Append(Checksum("GapId"));
                script->Append(0x07);
                script->Append(0x16);
                script->Append(Checksum("DefaultGapId"));

                }*/
                goto doneParsing;
                break;
              case 0x17:
              case 0x1A:
              case 0x2E:
                pFile+=4;
                break;
              case 0x40:
              case 0x2F:
                pFile+=*(DWORD*)pFile*6+4;
                break;
              case 0x16:
                const DWORD key = *(DWORD*)pFile;
                pFile+=4;
                switch(key)
                {
                case 0xEC1CD520:
                  script->Append(0x16);
                  script->Append(Checksum("Continue"));
                  script->Append(0x07);
                  script->Append(0x3);
                  script->Append(0x16);
                  pFile+=2;
                  while(*pFile != 0x16) pFile++;
                  pFile++;
                  script->Append(*(Checksum*)pFile);
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
                    script->Append(0x16);
                    pFile++;
                    script->Append(*(Checksum*)pFile);
                    pFile+=4;
                  }
                  else
                  {
                    script->Append(0x16);
                    pFile++;
                    script->Append(*(Checksum*)pFile);
                    pFile+=4;
                  }
                  script->Append(0x4);
                  break;
                  /*signed char idb;
                  idb = 0;
                  while(true)
                  {
                  const BYTE ope = *pFile;
                  pFile++;
                  if(ope!=0x9 && ope!=0x0E && ope!=0x0F)
                  {
                  script->Append(ope);
                  switch(ope)
                  {
                  case 0x1B:
                  case 0x1C:
                  DWORD len;
                  len = *(DWORD*)(pFile)+4;
                  script->Append(pFile, len);
                  pFile+=len;
                  break;
                  case 0x0A:
                  case 0x0B:
                  script->size--;
                  pFile+=5;
                  break;
                  case 0x3:
                  idb++;
                  break;
                  case 0x4:
                  idb--;
                  break;
                  case 0x1:
                  script->size--;
                  if(idb>0)
                  {
                  MessageBox(0,"ids","",0);
                  script->Append(0x4);
                  }
                  goto doneParsing;
                  break;
                  case 0x2:
                  pFile+=4;
                  script->size--;
                  goto doneParsing;
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
                  const DWORD numRands = *(DWORD*)pFile;
                  script->Append(*(Checksum*)&numRands);
                  pFile+=numRands*2+4;
                  const DWORD size = numRands*4;
                  MessageBox(0, "adding RNd", "",0);
                  script->Append(pFile, size);
                  pFile+=size;
                  break;
                  }
                  }
                  }      
                  doneAlmost:
                  script->EndScript();*/
                  // break;
                case 0xCD66C8AE:
                  script->Append(0x16);
                  script->Append(Checksum("Score"));
                  script->Append(0x7);
                  pFile++;
                  if(*pFile==0x2D)
                  {
                    script->Append(0x2D);
                    script->Append(0x16);
                    pFile+=2;
                    script->Append(*(Checksum*)pFile);
                    pFile+=4;
                  }
                  else if(*pFile==0x17)
                  {
                    script->Append(0x17);
                    pFile++;
                    script->Append(*(Checksum*)pFile);
                    pFile+=4;
                  }
                  else if(*pFile==0x1A)
                  {
                    script->Append(0x1A);
                    pFile++;
                    script->Append(*(Checksum*)pFile);
                    pFile+=4;

                  }
                  else
                    MessageBox(0,"wtf","score",0);
                  break;

                case 0xC4745838:
                  script->Append(0x16);
                  script->Append(Checksum("text"));
                  script->Append(0x7);
                  pFile++;
                  if(*pFile==0x2D)
                  {
                    script->Append(0x2D);
                    script->Append(0x16);
                    pFile+=2;
                    script->Append(*(Checksum*)pFile);
                    pFile+=4;
                  }
                  else
                  {
                    script->Append(0x1B);
                    pFile++;
                    const DWORD len = *(DWORD*)pFile+4;
                    script->Append(pFile, len);
                    pFile+=len;
                  }
                  break;
                case 0x7C9E51AB:
                  /*DWORD starter;
                  starter=script->size;*/
                  script->Append(0x16);
                  script->Append(Checksum("gapscript"));
                  script->Append(0x7);
retry2:
                  pFile++;
                  if(*pFile==0x2D)
                  {
                    script->Append(0x2D);
                    script->Append(0x16);
                    pFile++;
                  }
                  else if(*pFile==0x16)
                  {
                    script->Append(0x16);
                  }
                  else if(*pFile==0x0E)
                  {
                    goto retry2;
                  }
                  else
                  {
                    MessageBox(0, "ao", "", 0);
                    while(*pFile!=0x16)
                    {
                      pFile++;
                    }
                  }
                  pFile++;
                  /*DWORD qb;
                  qb = *(DWORD*)pFile;
                  script->Append(*(Checksum*)&qb);

                  for(DWORD i=0, numScripts = scripts.size(); i<numScripts; i++)
                  {
                    if(qb==scripts[i].name.checksum)
                    {
                      if(!GetTrigger(qb))
                        triggers.push_back(qb);
                      DWORD name = script->name.checksum;
                      if(!GetScript(qb))
                        KnownScripts.push_back(scripts[i]);
                      goto foundIt;
                      break;
                    }
                  }
                  script->size=starter;
foundIt:*/
                  DWORD qb;
                  qb = *(DWORD*)pFile;
                  if(!GetTrigger(qb))
                    triggers.push_back(qb);
                  script->Append(*(Checksum*)&qb);
                  pFile+=4;
                  break;
                case 0x3B442E26://78 BA 92 9F
                  script->Append(0x16);
                  script->Append(Checksum("GapId"));
                  //foundEndId=true;
                  script->Append(0x7);
retry:
                  pFile++;
                  if(*pFile==0x2D)
                  {
                    script->Append(0x2D);
                    script->Append(0x16);
                    pFile++;
                  }
                  else if(*pFile==0x16)
                  {
                    script->Append(0x16);
                  }
                  else if(*pFile==0x0E)
                  {
                    goto retry;
                  }
                  else
                  {
                    MessageBox(0, "ao", "", 0);
                    while(*pFile!=0x16)
                    {
                      pFile++;
                    }
                  }
                  pFile++;
                  script->Append(*(Checksum*)pFile);
                  pFile+=4;
                  break;
                }
                break;
              }
            }
            goto doneParsing;
            signed char ide;
            ide=0;

            while(true)
            {
              const BYTE op = *pFile;
              pFile++;
              if(op!=0x9 && op!=0x0E && op!=0x0F)
              {
                script->Append(op);
                switch(op)
                {
                case 0x1B:
                case 0x1C:
                  DWORD len;
                  len = *(DWORD*)(pFile)+4;
                  script->Append(pFile, len);
                  pFile+=len;
                  break;
                case 0x0B:
                case 0x0A:
                  script->size--;
                  pFile+=5;
                  break;
                case 0x3:
                  ide++;
                  break;
                case 0x4:
                  ide--;
                  break;
                case 0x1:
                  script->size--;
                  if(ide>0)
                  {
                    MessageBox(0,"ide","",0);
                    script->Append(0x4);
                  }
                  goto doneParsing;
                  break;
                case 0x2:
                  pFile+=4;
                  script->size--;
                  goto doneParsing;
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
                  const DWORD numRands = *(DWORD*)pFile;
                  script->Append(*(Checksum*)&numRands);
                  pFile+=numRands*2+4;
                  const DWORD size = numRands*4;
                  MessageBox(0, "adding RNd", "",0);
                  script->Append(pFile, size);
                  pFile+=size;
                  break;
                }
              }
            }
            break;
          case 0x65B9EC39:
            DWORD starte;
            /*bool foundStartId;
            foundStartId=false;*/
            starte = script->size;
            script->Append(0x16);
            script->Append(Checksum("StartGap"));
            signed char ids;
            ids=0;
            bool added;
            added = false;

            while(true)
            {
              const BYTE op = *pFile;
              pFile++;
              if(op!=0x9 && op!=0x0E && op!=0x0F)
              {
                script->Append(op);
                switch(op)
                {
                case 0x1B:
                case 0x1C:
                  DWORD len;
                  len = *(DWORD*)(pFile)+4;
                  script->Append(pFile, len);
                  pFile+=len;
                  break;
                case 0x0A:
                  script->size--;
                  pFile++;
                  MessageBox(0, "sub", "", 0);
                  script->InsertSub(starte, Checksum("id"), *(DWORD*)pFile);
                  pFile+=4;
                  break;
                case 0x0B:
                  script->size--;
                  pFile++;
                  if(!added)
                  {
                    //MessageBox(0, "add", "", 0);
                    script->InsertAdd(starte, Checksum("id"), *(DWORD*)pFile);
                  }
                  pFile+=4;
                  break;
                case 0x3:
                  ids++;
                  break;
                case 0x4:
                  ids--;
                  break;
                case 0x1:
                  script->size--;
                  if(ids>0)
                  {
                    MessageBox(0,"ids","",0);
                    script->Append(0x4);
                  }
                  /*if(!foundStartId)
                  {
                  script->Append(0x16);
                  script->Append(Checksum("GapId"));
                  script->Append(0x07);
                  script->Append(0x16);
                  script->Append(Checksum("DefaultGapId"));
                  }*/
                  goto doneParsing;
                  break;
                case 0x2:
                  pFile+=4;
                  script->size--;
                  /*if(!foundStartId)
                  {
                  script->Append(0x16);
                  script->Append(Checksum("GapId"));
                  script->Append(0x07);
                  script->Append(0x16);
                  script->Append(Checksum("DefaultGapId"));
                  }*/
                  goto doneParsing;
                  break;
                  /*case 0x16:
                  DWORD k;
                  k = *(DWORD*)pFile;
                  if(k==0x3EAFA520)
                  {
                  pFile+=6;
                  pFile+=*(DWORD*)pFile;
                  }
                  else if(k==0xAD9FA37F)
                  {
                  pFile+=5;
                  }
                  else
                  script->Append(*(Checksum*)&k);
                  pFile+=4;
                  break;*/
                case 0x16:
                case 0x17:
                case 0x1A:
                case 0x2E:
                  script->Append((DWORD*)pFile);
                  /*if(*(DWORD*)pFile==0x3B442E26)
                  foundStartId=true;*/
                  pFile+=4;
                  break;
                case 0x40:
                case 0x2F:
                  const DWORD numRands = *(DWORD*)pFile;
                  script->Append(*(Checksum*)&numRands);
                  pFile+=numRands*2+4;
                  const DWORD size = numRands*4;
                  MessageBox(0, "adding RNd", "",0);
                  script->Append(pFile, size);
                  pFile+=size;
                  break;
                }
              }
            }
            break;
            /*case 0xB1058546:
            script->Append(0x16);
            script->Append(Checksum("Sk3_Killskater_Water"));

            while(true)
            {
            const BYTE op = *pFile;
            pFile++;
            switch(op)
            {
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
            pFile+=*(DWORD*)pFile*6+4;
            break;
            case 0x16:
            const DWORD key = *(DWORD*)pFile;
            pFile+=4;
            switch(key)
            {
            case 0xF6BA43E1:
            script->Append(0x16);
            script->Append(Checksum("Bail"));
            break;
            case 0x9F92BA78://78 BA 92 9F
            script->Append(0x16);
            script->Append(Checksum("TeleTo"));
            script->Append(0x7);
            pFile++;
            if(*pFile==0x2D)
            {
            script->Append(0x2D);
            script->Append(0x16);
            pFile++;
            }
            else if(*pFile==0x16)
            {
            script->Append(0x16);
            }
            else
            {
            while(*pFile!=0x16)
            {
            pFile++;
            }
            }
            pFile++;
            script->Append(*(Checksum*)pFile);
            pFile+=4;
            break;
            }
            break;
            }
            }*/
          case 0xB1058546:
          case 0xDEA3057B:
            script->Append(0x16);
            script->Append(*(Checksum*)&qbKey);

            while(true)
            {
              const BYTE op = *pFile;
              pFile++;
              switch(op)
              {
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
                pFile+=*(DWORD*)pFile*6+4;
                break;
              case 0x16:
                const DWORD key = *(DWORD*)pFile;
                pFile+=4;
                switch(key)
                {
                case 0xF6BA43E1:
                  script->Append(0x16);
                  script->Append(Checksum("Bail"));
                  break;
                case 0x9F92BA78://78 BA 92 9F
                  script->Append(0x16);
                  script->Append(Checksum("TeleTo"));
                  script->Append(0x7);
                  pFile++;
                  if(*pFile==0x2D)
                  {
                    script->Append(0x2D);
                    script->Append(0x16);
                    pFile++;
                  }
                  else if(*pFile==0x16)
                  {
                    script->Append(0x16);
                  }
                  else
                  {
                    while(*pFile!=0x16)
                    {
                      pFile++;
                    }
                  }
                  pFile++;
                  script->Append(*(Checksum*)pFile);
                  pFile+=4;
                  break;
                }
                break;
              }
            }
          case 0x7AAE1282:
            script->Append(0x16);
            script->Append(Checksum("Sk3_Killskater_Finish"));

            while(true)
            {
              const BYTE op = *pFile;
              pFile++;
              switch(op)
              {
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
                pFile+=*(DWORD*)pFile*6+4;
                break;
              case 0x16:
                const DWORD key = *(DWORD*)pFile;
                pFile+=4;
                switch(key)
                {
                case 0xF6BA43E1:
                  script->Append(0x16);
                  script->Append(Checksum("Bail"));
                  break;
                case 0x9F92BA78://78 BA 92 9F
                  script->Append(0x16);
                  script->Append(Checksum("TeleTo"));
                  script->Append(0x7);
                  pFile++;
                  if(*pFile==0x2D)
                  {
                    script->Append(0x2D);
                    script->Append(0x16);
                    pFile++;
                  }
                  else if(*pFile==0x16)
                  {
                    script->Append(0x16);
                  }
                  else
                  {
                    while(*pFile!=0x16)
                    {
                      pFile++;
                    }
                  }
                  pFile++;
                  script->Append(*(Checksum*)pFile);
                  pFile+=4;
                  break;
                }
                break;
              }
            }
          case 0x6E77719D:
            script->Append(0x16);
            script->Append(Checksum("ShatterAndDie"));
            //MessageBox(0,"script",(*(Checksum*)&qbKey).GetString(),0);
            signed char ibs;
            ibs=0;

            while(true)
            {
              const BYTE op = *pFile;
              pFile++;
              if(op!=0x9)
                script->Append(op);
              switch(op)
              {
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
                  MessageBox(0,"ibs","",0);
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
                goto doneParsing;
                break;
              case 0x16:
              case 0x17:
              case 0x1A:
              case 0x2E:
                if(*(DWORD*)pFile==0x7DC30BA6)
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
                const DWORD numRands = *(DWORD*)pFile;
                script->Append(*(Checksum*)&numRands);
                pFile+=numRands*2+4;
                const DWORD size = numRands*4;
                MessageBox(0, "adding RNd", "",0);
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
                script->Append(0x16);
                script->Append(*(Checksum*)&qbKey);
                pFile = AddParams(pFile, script);
                script->EndScript();
                if(!GetTrigger(qbKey))
                  triggers.push_back(*(Checksum*)&qbKey);
                if(!GetScript(qbKey))
                  KnownScripts.push_back(scripts[i]);
                break;
              }
            }
            break;
          }
          /*if(!found)
          {
          for(DWORD i=0, numScripts = scripts.size(); i<numScripts; i++)
          {
          if(qbKey==scripts[i].name.checksum)
          {
          //added=true;
          //MessageBox(0,"script",(*(Checksum*)&qbKey).GetString(),0);
          for(DWORD j=0, numTriggers = triggers.size(); j<numTriggers; j++)
          {
          if(triggers[j].checksum == qbKey)
          {
          found=true;
          break;
          }
          }
          if(!found)
          triggers.push_back(*(Checksum*)&qbKey);
          found=false;
          for(DWORD j=0, numKnown = KnownScripts.size(); j<numKnown; j++)
          {
          if(KnownScripts[j].name.checksum == qbKey)
          {
          found=true;
          break;
          }
          }
          if(!found)
          KnownScripts.push_back(scripts[i]);
          script->Append(0x16);
          script->Append(*(Checksum*)&qbKey);
          signed char id=0;
          //BYTE ids[12];
          while(true)
          {
          const BYTE op = *pFile;
          pFile++;
          script->Append(op);
          switch(op)
          {
          case 0x3:
          id++;
          break;
          case 0x4:
          id--;

          case 0x1:
          if(id>0)
          {
          MessageBox(0,"id","",0);
          script->size--;
          script->Append(0x4);
          script->size++;
          }
          goto dones;
          break;
          case 0x2:
          if(id>0)
          {
          MessageBox(0,"id","",0);
          script->size--;
          script->Append(0x4);
          script->size++;
          }
          pFile+=4;
          goto dones;
          break;
          case 0x16:
          case 0x17:
          case 0x1A:
          case 0x2E:
          //MessageBox(0, "adding 4 bytes", "",0);
          script->Append((DWORD*)pFile);
          pFile+=4;
          break;
          case 0x40:
          case 0x2F:
          const DWORD numRands = *(DWORD*)pFile;
          script->Append(*(Checksum*)&numRands);
          pFile+=numRands*2+4;
          const DWORD size = numRands*4;
          MessageBox(0, "adding RNd", "",0);
          script->Append(pFile, size);
          pFile+=size;
          break;
          }
          }
          dones:
          script->size--;
          script->EndScript();
          break;
          }
          }
          }
          else
          added=true;*/
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
        //MessageBox(0,"rand","",0);
        pFile+=*(DWORD*)pFile*6+4;
        break;
      case 0:
        goto done;
        /* default:
        char unk[5];
        sprintf(unk, "%X", opcode);
        MessageBox(0,"unk",unk,0);
        break;*/
      case 0x23:
        for(signed char i=ifCounter; i>=0; i--)
          ifs[ifCounter]=false;
        ifCounter=-1;
        //added=false;
        pFile++;
        scripts.push_back(KnownScript(*(Checksum*)pFile, NULL, 0));
        script = &scripts.back();
        pFile+=4;
        break;
      case 0x24:
        if(!script->size)
          scripts.pop_back();
        script = NULL;
        break;

      }
    }
done:
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
              KnownScripts.push_back(scripts[i]);
              break;
            }
          }
        }
      }
    }
    else
      MessageBox(0,"empty??","",0);
    delete [] pData;
  }
}

__declspec (noalias) void Scene::LoadThugNodeArray(register const BYTE const* __restrict pFile, const BYTE const* __restrict eof, vector<KnownScript> &scripts, vector<Script> &sounds)
{
  vector<Node> compressedNodes;
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
  bool nodesDone = false;
  //bool added=false;
  bool ifs[32];
  signed char ifCounter=-1;
  /*DWORD FuckedEndGapSize=0;
  BYTE FuckedEndGap[900];*/

  while(true)
  {
    const BYTE opcode = *pFile;
    pFile++;
    switch(opcode)
    {
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
        case 0x3A717382:
          script->Append(0x25);
          if(addNot)
          {
            script->Append(0x16);
            script->Append(Checksum("Not"));
          }
          script->Append(0x16);
          script->Append(*(Checksum*)&qbKey);
          if(*pFile==0x16)
          {
            pFile++;
            script->Append(0x16);
            script->Append(*(Checksum*)pFile);
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
        case 0xCF66717B:
        case 0xE565465B:
        case 0xF6C80F5A:
          script->Append(0x25);
          if(addNot)
          {
            script->Append(0x16);
            script->Append(Checksum("Not"));
          }
          script->Append(0x16);
          script->Append(*(Checksum*)&qbKey);
          if(*pFile==0x16)
          {
            pFile++;
            script->Append(0x16);
            script->Append(*(Checksum*)pFile);
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
      /*case 0x25:

      if(script)
      {
      script->Append(0x25);
      script->Append(0x16);
      while(*pFile != 0x16) pFile++;
      pFile++;
      script->Append(*(Checksum*)pFile);
      pFile+=4;
      script->EndScript();
      }
      break;
      case 0x27:
      if(script)
      {
      script->Append(0x27);
      script->Append(0x16);
      while(*pFile != 0x16) pFile++;
      pFile++;
      script->Append(*(Checksum*)pFile);
      pFile+=4;
      script->EndScript();
      }
      break;
      case 0x28:
      case 0x26:
      if(script)
      {
      script->Append(opcode);
      script->EndScript();
      }
      break;*/
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
      pFile+=*(DWORD*)pFile*6+4;
      break;
    case 0x16:
      const DWORD qbKey = *(DWORD*)pFile;
      pFile+=4;
      if(node)
      {
        switch(qbKey)
        {
        case 0x7F261953:
          //MessageBox(0,"found pos", "", 0);
          pFile+=2;
          node->Position = *(SimpleVertex*)pFile;
          node->Position.z *= -1;
          pFile+=sizeof(SimpleVertex);
          break;
        case 0x9D2D0915:
          pFile+=2;
          node->Angles = *(SimpleVertex*)pFile;
          //node->Angles.y*=-1;
          //node->Angles.x*=-1;
          //node->Angles.z*=-1;
          pFile+=sizeof(SimpleVertex);
          break;
        case 0xA1DC81F9:
          pFile+=2;
          node->Name = *(Checksum*)pFile;
          pFile+=sizeof(Checksum);
          break;
        case 0x1645B830:
          node->TrickObject=true;
          break;
        case 0x12B4E660:
          pFile+=2;
          node->Class = *(Checksum*)pFile;
          if(node->Class.checksum == 0x8E6B02AD)
          {
            char nodename[128];
            sprintf(nodename, "rn%u", numRails);
            node->Name = Checksum(nodename);
            numRails++;
          }
          pFile+=sizeof(Checksum);
          break;
        case 0x7321A8D6:
          pFile+=2;
          node->Type = *(Checksum*)pFile;
          if(node->Type.checksum==Checksum("PedAI").checksum || node->Type.checksum==Checksum("default").checksum)
          {
            node->Class.checksum=0;
          }
          pFile+=sizeof(Checksum);
          break;

        case 0x2CA8A299:
          pFile+=2;
          node->Trigger = *(Checksum*)pFile;
          pFile+=sizeof(Checksum);
          triggers.push_back(node->Trigger);
          break;

        case 0x1A3A966B:
          pFile+=2;
          node->Cluster = *(Checksum*)pFile;
          pFile+=sizeof(Checksum);
          break;

        case 0x68910AC6:
          node->AbsentInNetGames = true;
          break;

        case 0x20209C31:
          node->NetEnabled = true;
          break;

        case 0x23627FD7:
          node->Permanent = true;
          break;

        case 0x2E7D5EE7:
          pFile+=3;
          /*if(*(DWORD*)pFile>=0xFFFF)
          MessageBox(0, "...", "...", 0);*/
          if(node->Class.checksum==Checksum("Waypoint").checksum)
          {
            node->Class.checksum=0;
            if(node->Trigger.checksum)
              triggers.pop_back();
          }
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

        case 0x7C2552B9:
          node->CreatedAtStart = true;
          break;

        default:
          for(DWORD i=0, numComp = compressedNodes.size(); i<numComp; i++)
          {
            if(compressedNodes[i].Name.checksum == qbKey)
            {
              if(compressedNodes[i].Class.checksum && !node->Class.checksum)
              {
                node->Class = compressedNodes[i].Class;
                if(node->Class.checksum == 0x8E6B02AD)
                {
                  char nodename[128];
                  sprintf(nodename, "rn%u", numRails);
                  node->Name = Checksum(nodename);
                  numRails++;
                }
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
        //bool found = false;
        switch(qbKey)
        {
          /*case 0x70291F04:
          script->Append(0x16);
          script->Append(Checksum("Create"));

          while(true)
          {
          const BYTE op = *pFile;
          pFile++;
          switch(op)
          {
          case 0x1:
          case 0x2:
          goto doneParsing;
          break;
          case 0x17:
          case 0x1A:
          case 0x2E:
          pFile+=4;
          break;
          case 0x40:
          case 0x2F:
          pFile+=*(DWORD*)pFile*6+4;
          break;
          case 0x16:
          const DWORD key = *(DWORD*)pFile;
          pFile+=4;
          switch(key)
          {
          case 0xA1DC81F9://78 BA 92 9F
          script->Append(0x16);
          script->Append(Checksum("Name"));
          script->Append(0x7);
          pFile++;
          if(*pFile==0x2D)
          {
          script->Append(0x2D);
          script->Append(0x16);
          pFile++;
          }
          else if(*pFile==0x16)
          {
          script->Append(0x16);
          }
          else
          {
          while(*pFile!=0x16)
          {
          pFile++;
          }
          }
          pFile++;
          script->Append(*(Checksum*)pFile);
          pFile+=4;
          break;
          case 0x6C4E7971:
          script->Append(0x16);
          script->Append(Checksum("Prefix"));
          const DWORD size = *(DWORD*)(pFile+2)+6;
          script->Append(pFile, size);
          pFile+=size;
          break;
          }
          break;
          }
          }
          case 0x854F17A6:
          script->Append(0x16);
          script->Append(Checksum("Visible"));

          while(true)
          {
          const BYTE op = *pFile;
          pFile++;
          switch(op)
          {
          case 0x1:
          case 0x2:
          goto doneParsing;
          break;
          case 0x17:
          case 0x1A:
          case 0x2E:
          pFile+=4;
          break;
          case 0x40:
          case 0x2F:
          pFile+=*(DWORD*)pFile*6+4;
          break;
          case 0x16:
          const DWORD key = *(DWORD*)pFile;
          pFile+=4;
          switch(key)
          {
          case 0xA1DC81F9://78 BA 92 9F
          script->Append(0x16);
          script->Append(Checksum("Name"));
          script->Append(0x7);
          pFile++;
          if(*pFile==0x2D)
          {
          script->Append(0x2D);
          script->Append(0x16);
          pFile++;
          }
          else if(*pFile==0x16)
          {
          script->Append(0x16);
          }
          else
          {
          while(*pFile!=0x16)
          {
          pFile++;
          }
          }
          pFile++;
          script->Append(*(Checksum*)pFile);
          pFile+=4;
          break;
          case 0x6C4E7971:
          script->Append(0x16);
          script->Append(Checksum("Prefix"));
          const DWORD size = *(DWORD*)(pFile+2)+6;
          script->Append(pFile, size);
          pFile+=size;
          break;
          }
          break;
          }
          }
          case 0xCB8B5900:
          script->Append(0x16);
          script->Append(Checksum("Invisible"));

          while(true)
          {
          const BYTE op = *pFile;
          pFile++;
          switch(op)
          {
          case 0x1:
          case 0x2:
          goto doneParsing;
          break;
          case 0x17:
          case 0x1A:
          case 0x2E:
          pFile+=4;
          break;
          case 0x40:
          case 0x2F:
          pFile+=*(DWORD*)pFile*6+4;
          break;
          case 0x16:
          const DWORD key = *(DWORD*)pFile;
          pFile+=4;
          switch(key)
          {
          case 0xA1DC81F9://78 BA 92 9F
          script->Append(0x16);
          script->Append(Checksum("Name"));
          script->Append(0x7);
          pFile++;
          if(*pFile==0x2D)
          {
          script->Append(0x2D);
          script->Append(0x16);
          pFile++;
          }
          else if(*pFile==0x16)
          {
          script->Append(0x16);
          }
          else
          {
          while(*pFile!=0x16)
          {
          pFile++;
          }
          }
          pFile++;
          script->Append(*(Checksum*)pFile);
          pFile+=4;
          break;
          case 0x6C4E7971:
          script->Append(0x16);
          script->Append(Checksum("Prefix"));
          const DWORD size = *(DWORD*)(pFile+2)+6;
          script->Append(pFile, size);
          pFile+=size;
          break;
          }
          break;
          }
          }
          case 0xF8D6A996:
          script->Append(0x16);
          script->Append(Checksum("Kill"));

          while(true)
          {
          const BYTE op = *pFile;
          pFile++;
          switch(op)
          {
          case 0x1:
          case 0x2:
          goto doneParsing;
          break;
          case 0x17:
          case 0x1A:
          case 0x2E:
          pFile+=4;
          break;
          case 0x40:
          case 0x2F:
          pFile+=*(DWORD*)pFile*6+4;
          break;
          case 0x16:
          const DWORD key = *(DWORD*)pFile;
          pFile+=4;
          switch(key)
          {
          case 0xA1DC81F9://78 BA 92 9F
          script->Append(0x16);
          script->Append(Checksum("Name"));
          script->Append(0x7);
          pFile++;
          if(*pFile==0x2D)
          {
          script->Append(0x2D);
          script->Append(0x16);
          pFile++;
          }
          else if(*pFile==0x16)
          {
          script->Append(0x16);
          }
          else
          {
          while(*pFile!=0x16)
          {
          pFile++;
          }
          }
          pFile++;
          script->Append(*(Checksum*)pFile);
          pFile+=4;
          break;
          case 0x6C4E7971:
          script->Append(0x16);
          script->Append(Checksum("Prefix"));
          const DWORD size = *(DWORD*)(pFile+2)+6;
          script->Append(pFile, size);
          pFile+=size;
          break;
          }
          break;
          }
          }*/
        case 0x4562E4A6:
          script->Append(0x16);
          script->Append(*(Checksum*)&qbKey);
          script->EndScript();
          break;
          /*if(FuckedEndGapSize)
          {
          MessageBox(0,"exporting fucked","",0);
          script->Append(FuckedEndGap, FuckedEndGapSize);
          script->Append(0x16);
          script->Append(Checksum("GapId"));
          script->Append(0x07);
          script->Append(0x16);
          script->Append(Checksum("DefaultGapId"));
          script->EndScript();
          }
          else
          {
          script->Append(0x16);
          script->Append(Checksum("EndGap"));
          script->Append(0x16);
          script->Append(Checksum("GapId"));
          script->Append(0x07);
          script->Append(0x16);
          script->Append(Checksum("DuplicateTrigger"));
          script->EndScript();
          }*/
          break;
        case 0x9FBBD5F0:
        case 0x3F4B93F8:
          script->Append(0x16);
          script->Append(Checksum("Sk3_TeleportToNode"));

          while(true)
          {
            const BYTE op = *pFile;
            pFile++;
            switch(op)
            {
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
              pFile+=*(DWORD*)pFile*6+4;
              break;
            case 0x16:
              const DWORD key = *(DWORD*)pFile;
              pFile+=4;
              switch(key)
              {
              case 0x9F92BA78://78 BA 92 9F
                script->Append(0x16);
                script->Append(*(Checksum*)&key);
                script->Append(0x7);
                pFile++;
                if(*pFile==0x2D)
                {
                  script->Append(0x2D);
                  script->Append(0x16);
                  pFile++;
                }
                else if(*pFile==0x16)
                {
                  script->Append(0x16);
                }
                else
                {
                  while(*pFile!=0x16)
                  {
                    pFile++;
                  }
                }
                pFile++;
                script->Append(*(Checksum*)pFile);
                pFile+=4;
                break;
              }
              break;
            }
          }
        case 0xAFA20E18:
        case 0x5151B2B1:
          script->Append(0x16);
          script->Append(*(Checksum*)&qbKey);
          if(*pFile==0x2D)
          {
            script->Append(0x2D);
            script->Append(0x16);
            pFile++;
          }
          else if(*pFile==0x16)
          {
            script->Append(0x16);
          }
          else
          {
            while(*pFile!=0x16)
            {
              pFile++;
            }
          }
          pFile++;
          DWORD checkc;
          checkc = *(DWORD*)pFile;
          if(checkc==0xB0A2A4C5)//C5 A4 A2 B0
            *(Checksum*)&checkc=Checksum("glasspane2x");
          script->Append(*(Checksum*)&checkc);
          LoadSound(*(Checksum*)&checkc, sounds);
          pFile+=4;
          script->EndScript();
          break;
        case 0xE5399FB2:
          DWORD start;
          /*bool foundEndId;
          foundEndId=false;*/
          start = script->size;
          script->Append(0x16);
          script->Append(Checksum("EndGap"));
          signed char ide;
          ide=0;

          while(true)
          {
            const BYTE op = *pFile;
            pFile++;
            if(op!=0x9 && op!=0x0E && op!=0x0F)
            {
              script->Append(op);
              switch(op)
              {
              case 0x1B:
              case 0x1C:
                DWORD len;
                len = *(DWORD*)(pFile)+4;
                script->Append(pFile, len);
                pFile+=len;
                break;
              case 0x0A:
                script->size--;
                pFile++;
                script->InsertSub(start, Checksum("id"), *(DWORD*)pFile);
                pFile+=4;
                break;
              case 0x0B:
                script->size--;
                pFile++;
                script->InsertAdd(start, Checksum("id"), *(DWORD*)pFile);
                pFile+=4;
                break;
              case 0x3:
                ide++;
                break;
              case 0x4:
                ide--;
                break;
              case 0x1:
                script->size--;
                if(ide>0)
                {
                  MessageBox(0,"ide","",0);
                  script->Append(0x4);
                }
                /*if(!foundEndId)
                {
                if(script->size-start)
                {
                FuckedEndGapSize = script->size-start;
                memcpy(FuckedEndGap, &script->func[start], FuckedEndGapSize);
                }
                script->size=start;
                /*script->Append(0x16);
                script->Append(Checksum("GapId"));
                script->Append(0x07);
                script->Append(0x16);
                script->Append(Checksum("DefaultGapId"));
                found=true;
                break;
                }*/
                goto doneParsing;
                break;
              case 0x2:
                pFile+=4;
                script->size--;
                /*if(!foundEndId)
                {
                if(script->size-start)
                {
                FuckedEndGapSize = script->size-start;
                memcpy(FuckedEndGap, &script->func[start], FuckedEndGapSize);
                }
                script->size=start;
                /*script->Append(0x16);
                script->Append(Checksum("GapId"));
                script->Append(0x07);
                script->Append(0x16);
                script->Append(Checksum("DefaultGapId"));
                found=true;
                break;
                }*/
                goto doneParsing;
                break;

                /*case 0x16:
                DWORD k;
                k = *(DWORD*)pFile;
                if(k==0x3EAFA520)
                {
                pFile+=6;
                pFile+=*(DWORD*)pFile;
                }
                else if(k==0xAD9FA37F)
                {
                pFile+=5;
                }
                else
                script->Append(*(Checksum*)&k);
                pFile+=4;
                break;*/
              case 0x16:
              case 0x17:
              case 0x1A:
              case 0x2E:
                script->Append((DWORD*)pFile);
                /*if(*(DWORD*)pFile==0x3B442E26)
                foundEndId=true;*/
                pFile+=4;
                break;
              case 0x40:
              case 0x2F:
                const DWORD numRands = *(DWORD*)pFile;
                script->Append(*(Checksum*)&numRands);
                pFile+=numRands*2+4;
                const DWORD size = numRands*4;
                MessageBox(0, "adding RNd", "",0);
                script->Append(pFile, size);
                pFile+=size;
                break;
              }
            }
          }
          break;
        case 0x65B9EC39:
          DWORD startr;
          /*bool foundStartId;
          foundStartId=false;*/
          startr = script->size;
          script->Append(0x16);
          script->Append(Checksum("StartGap"));
          signed char idf;
          idf=0;

          while(true)
          {
            const BYTE op = *pFile;
            pFile++;
            if(op!=0x9 && op!=0x0E && op!=0x0F)
            {
              script->Append(op);
              switch(op)
              {
              case 0x1B:
              case 0x1C:
                DWORD len;
                len = *(DWORD*)(pFile)+4;
                script->Append(pFile, len);
                pFile+=len;
                break;
              case 0x0A:
                script->size--;
                pFile++;
                script->InsertSub(startr, Checksum("id"), *(DWORD*)pFile);
                pFile+=4;
                break;
              case 0x0B:
                script->size--;
                pFile++;
                script->InsertAdd(startr, Checksum("id"), *(DWORD*)pFile);
                pFile+=4;
                break;
              case 0x3:
                idf++;
                break;
              case 0x4:
                idf--;
                break;
              case 0x1:
                script->size--;
                if(idf>0)
                {
                  MessageBox(0,"idf","",0);
                  script->Append(0x4);
                }
                /*if(!foundStartId)
                {
                script->Append(0x16);
                script->Append(Checksum("GapId"));
                script->Append(0x07);
                script->Append(0x16);
                script->Append(Checksum("DefaultGapId"));
                }*/
                goto doneParsing;
                break;
              case 0x2:
                pFile+=4;
                script->size--;
                /*if(!foundStartId)
                {
                script->Append(0x16);
                script->Append(Checksum("GapId"));
                script->Append(0x07);
                script->Append(0x16);
                script->Append(Checksum("DefaultGapId"));
                }*/
                goto doneParsing;
                break;
                /*case 0x16:
                DWORD k;
                k = *(DWORD*)pFile;
                if(k==0x3EAFA520)
                {
                pFile+=6;
                pFile+=*(DWORD*)pFile;
                }
                else if(k==0xAD9FA37F)
                {
                pFile+=5;
                }
                else
                script->Append(*(Checksum*)&k);
                pFile+=4;
                break;*/
              case 0x16:
              case 0x17:
              case 0x1A:
              case 0x2E:
                script->Append((DWORD*)pFile);
                /*if(*(DWORD*)pFile==0x3B442E26)
                foundStartId=true;*/
                pFile+=4;
                break;
              case 0x40:
              case 0x2F:
                const DWORD numRands = *(DWORD*)pFile;
                script->Append(*(Checksum*)&numRands);
                pFile+=numRands*2+4;
                const DWORD size = numRands*4;
                MessageBox(0, "adding RNd", "",0);
                script->Append(pFile, size);
                pFile+=size;
                break;
              }
            }
          }
          break;
          /*case 0xB1058546:
          script->Append(0x16);
          script->Append(Checksum("Sk3_Killskater_Water"));

          while(true)
          {
          const BYTE op = *pFile;
          pFile++;
          switch(op)
          {
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
          pFile+=*(DWORD*)pFile*6+4;
          break;
          case 0x16:
          const DWORD key = *(DWORD*)pFile;
          pFile+=4;
          switch(key)
          {
          case 0xF6BA43E1:
          script->Append(0x16);
          script->Append(Checksum("Bail"));
          break;
          case 0x9F92BA78://78 BA 92 9F
          script->Append(0x16);
          script->Append(Checksum("TeleTo"));
          script->Append(0x7);
          pFile++;
          if(*pFile==0x2D)
          {
          script->Append(0x2D);
          script->Append(0x16);
          pFile++;
          }
          else if(*pFile==0x16)
          {
          script->Append(0x16);
          }
          else
          {
          while(*pFile!=0x16)
          {
          pFile++;
          }
          }
          pFile++;
          script->Append(*(Checksum*)pFile);
          pFile+=4;
          break;
          }
          break;
          }
          }*/
        case 0xB1058546:
        case 0xDEA3057B:
          script->Append(0x16);
          script->Append(*(Checksum*)&qbKey);

          while(true)
          {
            const BYTE op = *pFile;
            pFile++;
            switch(op)
            {
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
              pFile+=*(DWORD*)pFile*6+4;
              break;
            case 0x16:
              const DWORD key = *(DWORD*)pFile;
              pFile+=4;
              switch(key)
              {
              case 0xF6BA43E1:
                script->Append(0x16);
                script->Append(Checksum("Bail"));
                break;
              case 0x9F92BA78://78 BA 92 9F
                script->Append(0x16);
                script->Append(Checksum("TeleTo"));
                script->Append(0x7);
                pFile++;
                if(*pFile==0x2D)
                {
                  script->Append(0x2D);
                  script->Append(0x16);
                  pFile++;
                }
                else if(*pFile==0x16)
                {
                  script->Append(0x16);
                }
                else
                {
                  while(*pFile!=0x16)
                  {
                    pFile++;
                  }
                }
                pFile++;
                script->Append(*(Checksum*)pFile);
                pFile+=4;
                break;
              }
              break;
            }
          }
        case 0x7AAE1282:
          script->Append(0x16);
          script->Append(Checksum("Sk3_Killskater_Finish"));

          while(true)
          {
            const BYTE op = *pFile;
            pFile++;
            switch(op)
            {
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
              pFile+=*(DWORD*)pFile*6+4;
              break;
            case 0x16:
              const DWORD key = *(DWORD*)pFile;
              pFile+=4;
              switch(key)
              {
              case 0xF6BA43E1:
                script->Append(0x16);
                script->Append(Checksum("Bail"));
                break;
              case 0x9F92BA78://78 BA 92 9F
                script->Append(0x16);
                script->Append(Checksum("TeleTo"));
                script->Append(0x7);
                pFile++;
                if(*pFile==0x2D)
                {
                  script->Append(0x2D);
                  script->Append(0x16);
                  pFile++;
                }
                else if(*pFile==0x16)
                {
                  script->Append(0x16);
                }
                else
                {
                  while(*pFile!=0x16)
                  {
                    pFile++;
                  }
                }
                pFile++;
                script->Append(*(Checksum*)pFile);
                pFile+=4;
                break;
              }
              break;
            }
          }
        case 0x6E77719D:
          script->Append(0x16);
          script->Append(Checksum("ShatterAndDie"));
          //MessageBox(0,"script",(*(Checksum*)&qbKey).GetString(),0);
          signed char ibs;
          ibs=0;

          while(true)
          {
            const BYTE op = *pFile;
            pFile++;
            if(op!=0x9)
              script->Append(op);
            switch(op)
            {
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
                MessageBox(0,"ibs","",0);
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
              goto doneParsing;
              break;
            case 0x16:
            case 0x17:
            case 0x1A:
            case 0x2E:
              if(*(DWORD*)pFile==0x7DC30BA6)
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
              const DWORD numRands = *(DWORD*)pFile;
              script->Append(*(Checksum*)&numRands);
              pFile+=numRands*2+4;
              const DWORD size = numRands*4;
              MessageBox(0, "adding RNd", "",0);
              script->Append(pFile, size);
              pFile+=size;
              break;
            }
          }
doneParsing:
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
              script->Append(0x16);
              script->Append(*(Checksum*)&qbKey);
              pFile = AddParams(pFile, script);
              script->EndScript();
              if(!GetTrigger(qbKey))
                triggers.push_back(*(Checksum*)&qbKey);
              DWORD name=script->name.checksum;
              if(!GetScript(qbKey))
                KnownScripts.push_back(scripts[i]);
              script = GetScript(name);
              break;
            }
          }
          break;
        }
        /*if(!found)
        {
        for(DWORD i=0, numScripts = scripts.size(); i<numScripts; i++)
        {
        added=true;
        if(qbKey==scripts[i].name.checksum)
        {
        //MessageBox(0,"script",(*(Checksum*)&qbKey).GetString(),0);
        for(DWORD j=0, numTriggers = triggers.size(); j<numTriggers; j++)
        {
        if(triggers[j].checksum == qbKey)
        {
        found=true;
        break;
        }
        }
        if(!found)
        triggers.push_back(*(Checksum*)&qbKey);
        found=false;
        for(DWORD j=0, numKnown = KnownScripts.size(); j<numKnown; j++)
        {
        if(KnownScripts[j].name.checksum == qbKey)
        {
        found=true;
        break;
        }
        }
        if(!found)
        KnownScripts.push_back(scripts[i]);
        script->Append(0x16);
        script->Append(*(Checksum*)&qbKey);
        signed char id=0;
        //BYTE ids[12];
        while(true)
        {
        const BYTE op = *pFile;
        pFile++;
        script->Append(op);
        switch(op)
        {
        case 0x3:
        id++;
        break;
        case 0x4:
        id--;

        case 0x1:
        if(id>0)
        {
        MessageBox(0,"id","",0);
        script->size--;
        script->Append(0x4);
        script->size++;
        }
        goto dones;
        break;
        case 0x2:
        if(id>0)
        {
        MessageBox(0,"id","",0);
        script->size--;
        script->Append(0x4);
        script->size++;
        }
        pFile+=4;
        goto dones;
        break;
        case 0x16:
        case 0x17:
        case 0x1A:
        case 0x2E:
        //MessageBox(0, "adding 4 bytes", "",0);
        script->Append((DWORD*)pFile);
        pFile+=4;
        break;
        case 0x40:
        case 0x2F:
        const DWORD numRands = *(DWORD*)pFile;
        script->Append(*(Checksum*)&numRands);
        pFile+=numRands*2+4;
        const DWORD size = numRands*4;
        MessageBox(0, "adding RNd", "",0);
        script->Append(pFile, size);
        pFile+=size;
        break;
        }
        }
        dones:
        script->size--;
        script->EndScript();
        break;
        }
        }
        }
        else
        added=true;*/
      }
      break;
    }
  }
qbTable:
  Checksum("GapId");
  Checksum("Score");
  Checksum("Flags");
  Checksum("PURE_AIR");
  Checksum("text");
  compressedNodes.clear();
  for(DWORD i=0; i<nodes.size(); i++)
  {
#ifdef remove
    if(nodes[i].CreatedAtStart)
    {
#endif
      if(nodes[i].Class.checksum == 0xB7B3BD86)///*Checksum("LevelObject").checksum*/ || nodes[i].Class.checksum == 0xBF4D3536/*Checksum("LevelGeometry").checksum*/)
      {
        nodes[i].Class = Checksum("EnvironmentObject");
        for(DWORD j=0, numMeshes = meshes.size(); j<numMeshes; j++)
        {
          if(meshes[j].GetName().checksum == nodes[i].Name.checksum)
          {
            D3DXVECTOR3 center = meshes[j].GetCenter();
            /*if(center.x == 0 && center.y == 0 && center.z == 0)
            {
            MessageBox(0,nodes[i].Name.GetString(),"moving GameObj",0);*/
            meshes[j].UnSetFlag(MeshFlags::isVisible);
            meshes[j].SetFlag(MeshFlags::isDeleted);
            CloneMesh(&meshes[j]);
            Mesh &mesh = meshes.back();
            mesh.SetFlag(MeshFlags::isVisible);
            mesh.UnSetFlag(MeshFlags::isDeleted);
            D3DXVECTOR3 angle = *(D3DXVECTOR3*)&nodes[i].Angles;
            // angle.y*=-1;
            angle.y+=D3DX_PI;
            //angle.x+=D3DX_PI;
            mesh.SetPosition((D3DXVECTOR3*)&nodes[i].Position, &angle);
            /* }
            else
            MessageBox(0,nodes[i].Name.GetString(),"NOT moving GameObj",0);*/
            break;
          }
        }
      }
      else if(nodes[i].Class.checksum == 0xBF4D3536)
        nodes[i].Class = Checksum("EnvironmentObject");
      else if(nodes[i].Class.checksum == Checksum("RailNode").checksum)
      {
        if(!nodes[i].CreatedAtStart || nodes[i].AbsentInNetGames)
        {
          DeleteNode(i);
          i--;
        }
      }
      else if(nodes[i].Class.checksum != Checksum("EnvironmentObject").checksum/* && nodes[i].Class.checksum != Checksum("RailNode").checksum*/ && nodes[i].Class.checksum != Checksum("Restart").checksum && nodes[i].Class.checksum != Checksum("Waypoint").checksum)
      {
        DeleteNode(i);
        i--;
      }
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
        mesh->SetFlag(MeshFlags::isDeleted);
        mesh->UnSetFlag(MeshFlags::isVisible);
      }
      /*DeleteNode(i);
      i--;*/
    }
#endif
  }

  FixLinks();
  FixLinksFinal();
  if(nodes.size()>=0xFFFF)
    MessageBox(0, "NodeArray too big...", "", 0);
}

__declspec (noalias) void Scene::LoadProSkaterNodeArray(register const BYTE const* __restrict pFile, const BYTE const* __restrict eof)
{
  DWORD numRails = 0;
  BYTE ident=0;
  pFile += 0xC;
  Node* node = NULL;
  KnownScript* script = NULL;

  while(true)
  {
    const BYTE opcode = *pFile;
    pFile++;
    switch(opcode)
    {
    case 3:
      ident++;
      if(ident==1)
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
      pFile++;
      KnownScripts.push_back((KnownScript(*(Checksum*)pFile, NULL, 0)));
      pFile+=4;
      script = &KnownScripts.back();
      break;
    case 0x24:
      script=NULL;
      break;
    case 4:
      ident--;
      if(node==NULL)
        goto qbTable;
      if(ident==0)
        node = NULL;
      break;
    case 6:
      if(node==NULL)
        goto qbTable;
      break;
    case 0x16:
      const DWORD qbKey = *(DWORD*)pFile;
      pFile+=4;
      if(node)
      {
        switch(qbKey)
        {
        case 0xB9D31B0A:
          pFile+=2;
          node->Position = *(SimpleVertex*)pFile;
          pFile+=sizeof(SimpleVertex);
          break;
        case 0x9D2D0915:
          pFile+=2;
          node->Angles = *(SimpleVertex*)pFile;
          pFile+=sizeof(SimpleVertex);
          break;
        case 0xA1DC81F9:
          pFile+=2;
          node->Name = *(Checksum*)pFile;
          pFile+=sizeof(Checksum);
          break;
        case 0x1645B830:
          node->TrickObject=true;
          break;
        case 0x12B4E660:
          pFile+=2;
          node->Class = *(Checksum*)pFile;
          if(node->Class.checksum == 0x8E6B02AD)
          {
            char nodename[128];
            sprintf(nodename, "rn%u", numRails);
            node->Name = Checksum(nodename);
            numRails++;
          }
          pFile+=sizeof(Checksum);
          break;
        case 0x7321A8D6:
          pFile+=2;
          node->Type = *(Checksum*)pFile;
          pFile+=sizeof(Checksum);
          break;

        case 0x2CA8A299:
          pFile+=2;
          node->Trigger = *(Checksum*)pFile;
          pFile+=sizeof(Checksum);
          triggers.push_back(node->Trigger);
          break;

        case 0x1A3A966B:
          pFile+=2;
          node->Cluster = *(Checksum*)pFile;
          pFile+=sizeof(Checksum);
          break;

        case 0x68910AC6:
          node->AbsentInNetGames = true;
          break;

        case 0x20209C31:
          node->NetEnabled = true;
          break;

        case 0x23627FD7:
          node->Permanent = true;
          break;

        case 0x2E7D5EE7:
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

        case 0x7C2552B9:
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

      }
      break;
    }
  }
qbTable:
  ParseQBTable(pFile, eof);

  for(DWORD i=0; i<nodes.size(); i++)
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
      meshes[j].UnSetFlag(MeshFlags::isVisible);
      meshes[j].SetFlag(MeshFlags::isDeleted);
      CloneMesh(&meshes[j]);
      Mesh &mesh = meshes.back();
      mesh.SetFlag(MeshFlags::isVisible);
      mesh.UnSetFlag(MeshFlags::isDeleted);
      mesh.SetPosition((D3DXVECTOR3*)&nodes[i].Position, (D3DXVECTOR3*)&nodes[i].Angles);
      }
      break;
      }
      }
      }*/
      if(nodes[i].Class.checksum == 0xB7B3BD86)///*Checksum("LevelObject").checksum*/ || nodes[i].Class.checksum == 0xBF4D3536/*Checksum("LevelGeometry").checksum*/)
      {
        nodes[i].Class = Checksum("EnvironmentObject");
        for(DWORD j=0, numMeshes = meshes.size(); j<numMeshes; j++)
        {
          if(meshes[j].GetName().checksum == nodes[i].Name.checksum)
          {
            D3DXVECTOR3 center = meshes[j].GetCenter();
            /*if(center.x == 0 && center.y == 0 && center.z == 0)
            {
            MessageBox(0,nodes[i].Name.GetString(),"moving GameObj",0);*/
            meshes[j].UnSetFlag(MeshFlags::isVisible);
            meshes[j].SetFlag(MeshFlags::isDeleted);
            CloneMesh(&meshes[j]);
            Mesh &mesh = meshes.back();
            mesh.SetFlag(MeshFlags::isVisible);
            mesh.UnSetFlag(MeshFlags::isDeleted);
            D3DXVECTOR3 angle = *(D3DXVECTOR3*)&nodes[i].Angles;
            // angle.y*=-1;
            //  angle.y+=D3DX_PI;
            //angle.x+=D3DX_PI;
            mesh.SetPosition((D3DXVECTOR3*)&nodes[i].Position, &angle);
            /* }
            else
            MessageBox(0,nodes[i].Name.GetString(),"NOT moving GameObj",0);*/
            break;
          }
        }
      }
      else if(nodes[i].Class.checksum == 0xBF4D3536)
        nodes[i].Class = Checksum("EnvironmentObject");
      else if(nodes[i].Class.checksum == Checksum("RailNode").checksum)
      {
        if(!nodes[i].CreatedAtStart || nodes[i].AbsentInNetGames)
        {
          DeleteNode(i);
          i--;
        }
      }
      else if(nodes[i].Class.checksum != Checksum("EnvironmentObject").checksum/* && nodes[i].Class.checksum != Checksum("RailNode").checksum*/ && nodes[i].Class.checksum != Checksum("Restart").checksum && nodes[i].Class.checksum != Checksum("Waypoint").checksum)
      {
        DeleteNode(i);
        i--;
      }
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
        mesh->SetFlag(MeshFlags::isDeleted);
        mesh->UnSetFlag(MeshFlags::isVisible);
      }
      /*DeleteNode(i);
      i--;*/
    }
#endif
  }
#define MAX_LINK_DEPTH 7
  for(DWORD i=0; i<MAX_LINK_DEPTH; i++)
    FixLinks();
  FixLinksFinal();
  if(nodes.size()>=0xFFFF)
    MessageBox(0, "NodeArray too big...", "", 0);
}