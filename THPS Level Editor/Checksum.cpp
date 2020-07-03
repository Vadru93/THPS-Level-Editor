#include "stdafx.h"
#include "Structs.h"
#pragma unmanaged
using std::vector;

extern vector <Script> Scripts;
char temp[256] = "";



char* const Checksum::GetString() const
{
  for(DWORD i=0; i<Scripts.size(); i++)
  {
    if(checksum==Scripts[i].checksum)
      return Scripts[i].name;
  }
  return NULL;
}
char* const Checksum::GetString2() const
{
  for (DWORD i = 0; i<Scripts.size(); i++)
  {
    if (checksum == Scripts[i].checksum)
      return Scripts[i].name;
  }
  sprintf(temp, "0x%X", checksum);
  return temp;
}

void Checksum::SetString(char* string)
{
    for (DWORD i = 0; i < Scripts.size(); i++)
    {
        if (checksum == Scripts[i].checksum)
        {
            memcpy(Scripts[i].name, string, strlen(string) + 1);
        }
    }
}

unsigned long GetCrc(char* name)
{
  for (DWORD i = 0; i < Scripts.size(); i++)
  {
    if (!stricmp(name, Scripts[i].name))
      return Scripts[i].checksum;
  }
  return 0xFFFFFFFF;
}

Checksum GenerateCaseSensChecksum(const char* string)
{
  DWORD checksum = 0xFFFFFFFF;

  for(DWORD i=0; i<strlen(string); i++)
  {
    char c = string[i];

    //if(c >= 'A' && c <= 'Z') c += 32;
    if(c == '/') c = '\\';

    checksum = checksumTable[(BYTE)(checksum ^ c)] ^ (checksum >> 8);
  }
  return *(Checksum*)&checksum;
}

Checksum::Checksum(const char* const string, bool createTable)
{
    if (string == NULL)
    {
        this->checksum = Checksum("NULL_STRING").checksum;
    }
    else
    {
        this->checksum = 0xFFFFFFFF;

        for (DWORD i = 0; i < strlen(string); i++)
        {
            char c = string[i];

            if (c >= 'A' && c <= 'Z') c += 32;
            if (c == '/') c = '\\';

            this->checksum = checksumTable[(BYTE)(this->checksum ^ c)] ^ (this->checksum >> 8);
        }

        if (createTable)
        {
            if (!GetString())
                Scripts.push_back(Script(checksum, string));
        }
    }
}

Checksum::Checksum(const char* const string)
{
    if (string == NULL)
    {
        this->checksum = Checksum("NULL_STRING").checksum;
   }
    else
    {
        this->checksum = 0xFFFFFFFF;

        for (DWORD i = 0; i < strlen(string); i++)
        {
            char c = string[i];

            if (c >= 'A' && c <= 'Z') c += 32;
            if (c == '/') c = '\\';

            this->checksum = checksumTable[(BYTE)(this->checksum ^ c)] ^ (this->checksum >> 8);
        }

        if (!GetString())
            Scripts.push_back(Script(checksum, string));
    }
}