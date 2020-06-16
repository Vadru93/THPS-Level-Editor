#ifndef STRUCTS_H
#define STRUCTS_H

#include "Includes.h"
#include "Checksum.h"
#include <iostream>
#include <string>
#include <map>
#include <string>
#include <sstream>
#include <vector>
#pragma unmanaged
struct Script
{
  DWORD checksum;
  char name[128];

  Script(DWORD checksum, const char* string)
  {
    ZeroMemory(name, 128);
    this->checksum = checksum;

    DWORD i = 0;
    while (string[i])
    {
      if (string[i] == 0x20 || string[i] == 0x2D)
        name[i] = 0x5F;
      else
        name[i] = string[i];
      i++;
    }
    name[i] = '\0';
  }
};

typedef struct {
  char  idLength;
  char  colourMapType;
  char  dataTypeCode;
  //short colourMapOrigin;
  short colourMapLength;
  char  colourMapDepth;
  short x_origin;
  short y_origin;
  short width;
  short height;
  char  bitsPerPixel;
  char  imageDescriptor;
} TGAHEADER;

typedef struct {
  unsigned long magic;      // file id
  unsigned long structSize;    // 0x7C
  unsigned long flags;
  unsigned long height;      // image height
  unsigned long width;      // image width
  unsigned long imageSize;    // main image size (bytes)
  unsigned long depth;      // image bits per pixel???
  unsigned long mipMapNum;    // number of mipmaps
  unsigned long reserved1[11];
  unsigned long pixelFormat[8];  // pixel format (DXT type)
  unsigned long caps[4];      // capabilities???
  unsigned long reserved2;
} DDS_HEADER;

const DWORD checksumTable[256] =
{
  0x00000000, 0x77073096, 0xee0e612c, 0x990951ba,
  0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
  0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
  0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
  0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
  0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
  0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,
  0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
  0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
  0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
  0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940,
  0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
  0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116,
  0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
  0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
  0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
  0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a,
  0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
  0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818,
  0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
  0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
  0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
  0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c,
  0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
  0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
  0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
  0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
  0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
  0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086,
  0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
  0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4,
  0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
  0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
  0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
  0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
  0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
  0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe,
  0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
  0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
  0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
  0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252,
  0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
  0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60,
  0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
  0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
  0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
  0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04,
  0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
  0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,
  0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
  0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
  0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
  0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e,
  0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
  0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
  0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
  0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
  0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
  0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0,
  0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
  0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6,
  0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
  0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
  0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};
/*
template <const char* string, const DWORD idx> struct crc32
{
  static const DWORD prev = crc32<string, idx-1>::checksum;
  static const DWORD checksum = checksumTable[(BYTE)(prev ^ string[idx])] ^ (prev >> 8);
};

template <const char* string>
struct crc32<string, 0>
{
  static const DWORD checksum = checksumTable[(BYTE)(0xFFFFFFFF ^ string[0])] ^ (0xFFFFFFFF >> 8);
};

class file {
public:
    static const char arg[];
};
decltype(file::arg) file::arg = "environmentobject";

#define CHECKSUM(x) crc32<x, sizeof(x)-2>::checksum*/

struct SimpleFace
{
  WORD a;
  WORD b;
  WORD c;

  SimpleFace()
  {
    a = 0;
    b = 0;
    c = 0;
  }

  SimpleFace(WORD a, WORD b, WORD c)
  {
    this->a = a;
    this->b = b;
    this->c = c;
  }

  SimpleFace(SimpleFace* face)
  {
    a = face->a;
    b = face->b;
    c = face->c;
  }

  SimpleFace(SimpleFace* face, DWORD offset)
  {
    a = face->a + (WORD)offset;
    b = face->b + (WORD)offset;
    c = face->c + (WORD)offset;
  }

  void SetIndexOffset(DWORD offset)
  {
    a += (WORD)offset;
    b += (WORD)offset;
    c += (WORD)offset;
  }
};

struct ScriptFunction
{
  Checksum name;
  BYTE* rawData;
};

static std::string ident;
static DWORD pseudo;
static BYTE* data;
static std::string buffer;
static char* it;
static char* end;
static DWORD nestingCount = 0;
static DWORD insideStruct = 0;
static bool unmarkedNumbers = false;
static bool breakLine = true;
static bool thug;
static BYTE nestingCodes[128];

static void StartNesting(BYTE endCode)
{
  nestingCodes[nestingCount] = endCode;
  nestingCount++;

  if (nestingCount == 128)
    MessageBox(0, "warning too many {", 0, 0);
}

static bool addedLine = false;
static bool insideIf = false;

static void EndNesting()
{
  if (nestingCount == 0)
    MessageBox(0, "warning too many }", 0, 0);
  else
  {
    if (nestingCount == insideStruct)
      insideStruct = 0;
    nestingCount--;
    static const DWORD line = 0;
    BYTE code = nestingCodes[nestingCount];
    *data = code;
    data++;
    /*if(nestingCount==insideStruct && code==4)
    insideStruct=0;*/
    if (!insideIf && !insideStruct && (code == 0x28 || code == 0x24 || code == 0x6 || code == 0x4 || code == 0x21))
    {
      addedLine = true;
      *data = 0x1;
      data++;/*fputc(0x02,f);
                     fwrite(&line,4,1,f);//*/
    }
  }
}


static void DecreaseNesting()
{
  if (ident.length() > 1)
    ident = ident.substr(2);
}

static char* itoa(int val, int base) {
  bool negative = val < 0;
  if (val == 0)
    return "0";
  else
  {
    static char buf[32] = { 0 };

    char* ptr = &buf[31];

    //int i = 30;


    while (val)
    {
      int tmpVal = val;
      val /= base;
      *ptr-- = "FEDCBA9876543210123456789ABCDEF"[15 + (tmpVal - val * base)];
    }

    /*for(; val && i ; --i, val /= base)

    buf[i] = "FEDCBA9876543210123456789ABCDEF"[15 + (val * base)];*/

    if (negative)
    {
      *ptr = '-';
      return ptr;
    }
    else
      return ptr + 1;
  }
}

static bool FastCheck(char check)
{
  if (*it != check)
    return false;

  it++;
  return true;
}

static bool FastCheck(char* check)
{
  char* p = it;

  while (*check)
  {
    if (*p != *check)
      return false;
    p++;
    check++;
  }

  it = p;
  return true;
}

struct Node;
namespace Executer
{
  static const BYTE* __restrict pFile = NULL;
  static const BYTE* __restrict eof = NULL;
}

struct KnownScript
{
  Checksum name;
  DWORD size;
  BYTE func[13900];//325

  const std::string ParsePair() const
  {
    std::stringstream stream;
    stream.precision(12);
    stream << std::fixed << *(float*)data;
    stream << "; ";
    data += 4;
    stream << std::fixed << *(float*)data;
    data += 4;
    return stream.str();
  }

  const std::string ParseVec() const
  {
    std::stringstream stream;
    stream.precision(12);
    stream << std::fixed << *(float*)data;
    stream << "; ";
    data += 4;
    stream << std::fixed << *(float*)data;
    stream << "; ";
    data += 4;
    stream << std::fixed << *(float*)data;
    data += 4;
    return stream.str();
  }

  const std::string ParseRnd() const
  {
    std::stringstream stream;
    int count = *(int*)data;
    data += 4;
    if (*(int*)data == 65537 || (count == 1 && *(WORD*)data == 1))
    {
      data += 2 * count;
      stream << "THUG";
    }
    stream << "[";
    stream << count;
    stream << "][";

    for (int i = 0; i < count; i++)
    {
      stream << *(int*)data;
      data += 4;
      if (i + 1 < count)
        stream << "; ";
    }
    return stream.str();
  }

  const std::string ParseFloat() const
  {
    const float f = *(float*)data;
    data += 4;
    if (f == 0.0f)
      return "0.0";
    else
    {
      std::stringstream stream;
      stream.precision(12);
      stream << std::fixed << f;
      return stream.str();
    }
  }

  bool NextToken()
  {
    while (it != end && (*it == ' ' || *it == '\r' || *it == '\n' || *it == ')' || *it == '('))
      it++;

    if (it == end)
      return false;

    if (FastCheck("=="))
    {
      *data = 0x11;
      data++;
    }
    else if (FastCheck("IF"))
    {
      StartNesting(0x28);
      *data = 0x25;
      data++;
      breakLine = false;
      insideIf = true;
    }
    else if (FastCheck("GOTO IF"))
    {
      *data = 0x47;
      data++;
    }
    else if (FastCheck("ELSE IF"))
    {
      *data = 0x27;
      data++;
    }
    else if (FastCheck("ELSE"))
    {
      data -= 2;
      StartNesting(0x28);
      *data = 0x26;
      data++;
      breakLine = false;
    }
    /*else if(FastCheck("END IF"))
    {
    fputc(0x28,f);
    }*/
    else if (FastCheck("NOT"))
    {
      *data = 0x39;
      data++;
    }
    else if (FastCheck("AND"))
    {
      *data = 0x33;
      data++;
    }
    else if (FastCheck("OR"))
    {
      *data = 0x32;
      data++;
    }
    else if (FastCheck("<="))
    {
      *data = 0x13;
      data++;
    }
    else if (FastCheck(">="))
    {
      *data = 0x15;
      data++;
    }
    else if (FastCheck('<'))
    {
      *data = 0x12;
      data++;
    }
    else if (FastCheck('>'))
    {
      *data = 0x14;
      data++;
    }
    else if (FastCheck("="))
    {
      *data = 0x7;
      data++;
    }
    else if (FastCheck('+'))
    {
      *data = 0xB;
      data++;
    }
    else if (FastCheck('/'))
    {
      *data = 0xD;
      data++;
    }
    else if (FastCheck('*'))
    {
      *data = 0xC;
      data++;
    }
    else if (FastCheck(','))
    {
      *data = 0x42;
      data++;
    }
    else if (FastCheck("FUNCTION"))
    {
      StartNesting(0x24);
      *data = 0x23;
      data++;
      breakLine = false;
    }
    /*else if(FastCheck("END FUNCTION"))
    {
    fputc(0x24,f);
    }*/
    else if (FastCheck("LOOP"))
    {
      StartNesting(0x21);
      *data = 0x20;
      data++;
      breakLine = false;
    }
    /*else if(FastCheck("END LOOP"))
    {
    fputc(0x21,f);
    }*/
    else if (FastCheck("BREAK"))
    {
      *data = 0x22;
      data++;
      static const DWORD line = 0;
      *data = 0x01;
      data++;
    }
    else if (FastCheck("RETURN"))
    {
      *data = 0x29;
      data++;
    }
    else if (FastCheck("JUMP"))
    {
      *data = 0x2E;
      data++;
    }
    else if (FastCheck("NULL"))
    {
      *data = 0x2C;
      data++;
    }
    else if (FastCheck("ARRAY{"))
    {
      StartNesting(6);
      if (!insideStruct)
        insideStruct = nestingCount;
      *data = 0x5;
      data++;
    }
    else if (FastCheck('}'))
    {
      EndNesting();
      if (it >= end)
        return false;
      else
        return true;
    }
    else if (FastCheck("VECTOR"))
    {
      *data = 0x1E;
      data++;
    }
    else if (FastCheck("PAIR"))
    {
      *data = 0x1F;
      data++;
    }
    else if (FastCheck("STRUCT{"))
    {
      StartNesting(4);
      if (!insideStruct)
        insideStruct = nestingCount;
      *data = 0x3;
      data++;
    }
    else if (FastCheck("RANDOM4THUG"))
    {
      *data = 0x41;
      data++;
      thug = true;
    }
    else if (FastCheck("RANDOM3THUG"))
    {
      *data = 0x40;
      data++;
      thug = true;
    }
    else if (FastCheck("RANDOM2THUG"))
    {
      *data = 0x37;
      data++;
      thug = true;
    }
    else if (FastCheck("RANDOMTHUG"))
    {
      *data = 0x2F;
      data++;
      thug = true;
    }
    else if (FastCheck("RANDOM4"))
    {
      *data = 0x41;
      data++;
    }
    else if (FastCheck("RANDOM3"))
    {
      *data = 0x40;
      data++;
    }
    else if (FastCheck("RANDOM2"))
    {
      *data = 0x37;
      data++;
    }
    else if (FastCheck("RANDOM"))
    {
      *data = 0x2F;
      data++;
    }
    else if (FastCheck("BETWEEN"))
    {
      *data = 0x30;
      data++;
    }
    else if (FastCheck("SWITCH"))
    {
      *data = 0x3C;
      data++;
    }
    else if (FastCheck("END SWITCH"))
    {
      *data = 0x3D;
      data++;
    }
    else if (FastCheck("CASE"))
    {
      *data = 0x3E;
      data++;
    }
    else if (FastCheck("DEFAULT"))
    {
      *data = 0x3F;
      data++;
    }
    else if (FastCheck("GLOBAL."))
    {
      *data = 0x2D;
      data++;
    }
    else if (FastCheck(" ( "))
    {
      *data = 0x0E;
      data++;
    }
    else if (FastCheck(" ) "))
    {
      *data = 0x0F;
      data++;
    }
    /*else if(FastCheck('('))
    {
    }
    else if(FastCheck(')'))
    {
    }*/
    else if (FastCheck('{'))
    {
      insideIf = false;
      if (!breakLine)
      {
        static const DWORD line = 0;
        *data = 0x01;
        data++;/*fputc(0x02,f);
                       fwrite(&line,4,1,f);//*/
        breakLine = true;
      }
    }
    else if (FastCheck(';'))
    {
      if (!unmarkedNumbers && !addedLine)
      {
        static const DWORD line = 0;
        *data = 0x01;
        data++;/*fputc(0x02,f);
                       fwrite(&line,4,1,f);//*/
      }
      else
        addedLine = false;
    }
    else if (FastCheck('['))
    {
      unmarkedNumbers = true;
      breakLine = false;
    }
    else if (FastCheck(']'))
    {
      unmarkedNumbers = false;
      if (FastCheck(')'))
        breakLine = false;
      else
        breakLine = true;
    }
    else if (FastCheck('"'))
    {
      DWORD len = 0;
      char* datax = it;

      while (*it != '\"')
      {
        /*fputc(*it,f);
        it++;*/
        len++;
        it++;
      }
      it++;
      *data = 0x1B;
      data++;
      DWORD nLen = len + 1;
      *(DWORD*)data = nLen;
      data += 4;
      memcpy(data, datax, len);
      data += len;
      *data = 0x0;
      data++;
    }
    else if (FastCheck('\''))
    {
      DWORD len = 0;
      char* datax = it;

      while (*it != '\'')
      {
        /*fputc(*it,f);
        it++;*/
        len++;
        it++;
      }
      it++;
      DWORD nLen = len + 1;
      *data = 0x1C;
      data++;
      *(DWORD*)data = nLen;
      memcpy(data, datax, len);
      *data = 0x0;
      data++;
    }
    else if (FastCheck("0x"))
    {
      int total = 0;
      while (*it != ' ' && *it != ';' && *it != ')' && *it != '\0')
      {
        total = total * 16 + *it - '0';
        it++;
      }
      *(DWORD*)data = total;
      data += 4;
    }
    else if (*it == '.')
    {
      it++;
      *data = 0x8;
      data++;
    }
    else if (*it == '-')
    {
      it++;
      if (*it == ' ')
      {
        *data = 0x0A;
        data++;
      }
      else
      {
        bool isFloat = false;
        std::stringstream stream;
        char* old = it;

        while (*it != ' ' && *it != ';' && *it != ']' && *it != ')' && *it != '\0')
        {
          stream << *it;
          if (*it == '.')
            isFloat = true;
          it++;
        }
        if (!unmarkedNumbers)
        {
          if (isFloat)
          {
            *data = 0x1A;
            data++;
          }
          else
          {
            *data = 0x17;
            data++;
          }
        }

        if (isFloat)
        {
          float val;
          stream >> val;
          val = -val;
          *(DWORD*)data = val;
          data += 4;
        }
        else
        {
          it = old;
          int total = 0;
          while (*it >= '0' && *it <= '9'    /**it!=' ' && *it!= ';' && *it!= ')' && *it!='\0'*/)
          {
            total = total * 10 + *it - '0';
            it++;
          }
          total = -total;
          if (thug)
          {
            thug = false;
            for (DWORD j = 0; j < total; j++)
            {
              *data = 0x1;
              data++;
              *data = 0x0;
              data++;
            }
          }
          *(DWORD*)data = total;
          data += 4;
        }
        stream.clear();
      }
    }
    else if (*it <= '9' && *it >= '0')//*it == '0' || *it == '1' || *it == '2' || *it == '3' || *it == '4' || *it == '5' || *it == '6' || *it == '7' || *it == '8' || *it == '9')
    {
      bool isFloat = false;
      std::stringstream stream;
      char* old = it;

      while (*it != ' ' && *it != ';' && *it != ']' && *it != ')' && *it != '\0')
      {
        stream << *it;
        if (*it == '.')
          isFloat = true;
        it++;
      }
      if (!unmarkedNumbers)
      {
        if (isFloat)
        {
          *data = 0x1A;
          data++;
        }
        else
        {
          *data = 0x17;
          data++;
        }
      }

      if (isFloat)
      {
        float val;
        stream >> val;
        *(DWORD*)data = val;
        data += 4;
      }
      else
      {
        it = old;
        int total = 0;
        while (*it >= '0' && *it <= '9'    /**it!=' ' && *it!= ';' && *it!= ')' && *it!='\0'*/)
        {
          total = total * 10 + *it - '0';
          it++;
        }
        if (thug)
        {
          thug = false;
          for (DWORD j = 0; j < total; j++)
          {
            *data = 0x1;
            data++;
            *data = 0x0;
            data++;
          }
        }
        *(DWORD*)data = total;
        data += 4;
      }
      stream.clear();
    }
    /*else if (FastCheck("pseudo"))
    {
      it -= 6;
      typedef std::map<int, char*>::iterator iter;

      iter end = qbTable.end();
      iter item = qbTable.begin();
      bool found = false;

      while (item != end)
      {
        DWORD i = 0;
        char* c = item->second;
        //char* p = qbKeyName;

        while (*it != ';' && *it != ')' && *it != '(' && *it != ' ' && *it != '\0')//while(*p)
        {
          i++;
          if (*c++ != *it++)
            goto Next;
        }
        found = true;
        fputc(0x16, f);
        fwrite(&item->second, 4, 1, f);
        break;
      Next:
        it -= i;
        item++;
      }
      if (!found)
      {
        static const unsigned long crc32_table[256] =
        {
          0x00000000, 0x77073096, 0xee0e612c, 0x990951ba,
          0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
          0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
          0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
          0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
          0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
          0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,
          0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
          0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
          0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
          0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940,
          0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
          0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116,
          0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
          0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
          0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
          0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a,
          0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
          0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818,
          0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
          0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
          0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
          0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c,
          0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
          0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
          0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
          0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
          0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
          0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086,
          0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
          0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4,
          0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
          0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
          0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
          0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
          0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
          0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe,
          0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
          0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
          0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
          0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252,
          0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
          0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60,
          0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
          0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
          0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
          0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04,
          0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
          0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,
          0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
          0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
          0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
          0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e,
          0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
          0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
          0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
          0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
          0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
          0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0,
          0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
          0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6,
          0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
          0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
          0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
        };

        unsigned long crc = 0xFFFFFFFF;

        while (*it != ';' && *it != ')' && *it != '(' && *it != ' ' && *it != '\0')
        {
          char c = *it;
          if (c == '/') c = '\\';

          else if (c >= 'A' && c <= 'Z') c += 32;

          crc = crc32_table[(unsigned char)(crc ^ c)] ^ (crc >> 8);
          it++;
        }
        fputc(0x16, f);
        fwrite(&crc, 4, 1, f);
      }
    }*/
    else
    {
      static const unsigned long crc32_table[256] =
      {
        0x00000000, 0x77073096, 0xee0e612c, 0x990951ba,
        0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
        0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
        0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
        0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
        0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
        0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,
        0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
        0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
        0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
        0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940,
        0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
        0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116,
        0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
        0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
        0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
        0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a,
        0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
        0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818,
        0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
        0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
        0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
        0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c,
        0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
        0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
        0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
        0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
        0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
        0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086,
        0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
        0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4,
        0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
        0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
        0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
        0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
        0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
        0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe,
        0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
        0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
        0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
        0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252,
        0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
        0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60,
        0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
        0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
        0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
        0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04,
        0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
        0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,
        0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
        0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
        0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
        0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e,
        0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
        0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
        0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
        0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
        0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
        0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0,
        0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
        0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6,
        0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
        0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
        0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
      };

      //unsigned long crc = 0xFFFFFFFF;
      char* pos = it;

      char name[MAX_PATH];
      DWORD i = 0;
      while (*pos != ';' && *pos != ')' && *pos != '(' && *pos != ' ' && *pos != '\0')
      {
        name[i] = *pos;
        i++;
        pos++;
      }
      name[i] = '\0';
      extern unsigned long GetCrc(char* name);
      unsigned long crc = GetCrc(name);
      if (crc == 0xFFFFFFFF)
      {
        crc = Checksum((char*)name).checksum;
      }
      it = pos;

      /*while (*it != ';' && *it != ')' && *it != '(' && *it != ' ' && *it != '\0')
      {
        char c = *it;
        if (c == '/') c = '\\';

        else if (c >= 'A' && c <= 'Z') c += 32;

        crc = crc32_table[(unsigned char)(crc ^ c)] ^ (crc >> 8);
        it++;
      }*/
      *data = 0x16;
      data++;
      *(DWORD*)data = crc;
      data += 4;

      /*char name[MAX_PATH];
      DWORD i = 0;
      while (*pos != ';' && *pos != ')' && *pos != '(' && *pos != ' ' && *pos != '\0')
      {
        name[i] = *pos;
        i++;
        pos++;
      }
      name[i] = '\0';
      Checksum((char*)name);*///newKeys.insert(std::pair<int, char*>(crc, name));
      //delete[] name;
    }

    addedLine = false;

    if (it >= end)
      return false;
    else
      return true;
  }

  void Compile(char* script)
  {
    nestingCount = 0;
    insideStruct = 0;
    unmarkedNumbers = false;
    breakLine = true;
    addedLine = false;
    insideIf = false;
    ZeroMemory(nestingCodes, 128);// nestingCodes = new BYTE[128];//bracetMeaning.reserve(120);
    thug = false;

    buffer.reserve(strlen(script) + 1);
    buffer.assign(script);
    data = (BYTE*)func;
    it = &buffer.front();
    end = &buffer.back() + 1;

    while (NextToken());

    size = data - (BYTE*)func;
    buffer.clear();
    return;
  }

  KnownScript(char* data, char* name)
  {
    this->name = Checksum(name);
    Compile(data);
  }

  const char* Decompile()
  {
    buffer = "";
    ident = "";
    pseudo = 0;
    static char result[249] = { 0 };
    float x, y, z;
    int iX, iY, iZ;
    //static char tempX[80];
    static char temp[300];
    char* const __restrict tempX = temp + 1;
    char* const __restrict tempY = temp + 81;
    char* const __restrict tempZ = temp + 161;
    /*static char tempY[80];
    static char tempZ[80];*/
    bool breakLineAfterThis = false;
    bool requestNewLineAfterNext = false;
    bool openBracet = false;
    bool newLine = true;
    bool addedLine = false;
    bool addRounded = true;
    bool addedRounded = false;
    DWORD inStructure = 0;
    bool addRoundAfterThis = false;
    bool addedRoundLast = false;
    bool breaker = false;
    bool lastCall = false;
    //bool newLine = true;
    data = (BYTE*)func;
    BYTE* lastByte = data + size;

    while (data < lastByte)
    {
      BYTE code = *data;
      data++;
      if (lastCall)
        lastCall = false;
      else
        breaker = false;

      switch (code)
      {
      case 0x1:
        addRounded = true;
        /*if(newLine)
        buffer.append("\r\n  " + ident);
        else
        newLine=true;*/
        if (openBracet)
        {
          if (addedRounded)
          {
            if (addedLine)
              buffer.erase(buffer.end() - (5 + ident.length()), buffer.end());
            else if (!addedRoundLast /*&& buffer.length() > 1*/)
              buffer.pop_back();
            buffer.append(")");
            addedRounded = false;
          }
          buffer.append("\r\n" + ident + "{\r\n  " + ident);
          openBracet = false;
        }
        else if (newLine)
        {
          if (addedLine)
            buffer.erase(buffer.end() - (5 + ident.length()), buffer.end());
          else if (!addedRoundLast && buffer.length() > 1)
            buffer.pop_back();

          if (addedRounded)
          {
            buffer.append(");\r\n  " + ident);
            addedRounded = false;
          }
          else if (breaker)
            buffer.append(" \r\n  " + ident);
          else
            buffer.append(";\r\n  " + ident);
        }
        else
        {
          if (addedRounded)
          {
            if (addedLine)
              buffer.erase(buffer.end() - (4 + ident.length()), buffer.end());
            else if (!addedRoundLast /*&& buffer.length() > 1*/)
              buffer.pop_back();
            buffer.append(");\r\n  " + ident);
            addedRounded = false;
          }
          else if (!addedLine)
            buffer.append("\r\n  " + ident);
          newLine = true;
        }
        break;

      case 0x2:
        addRounded = true;
        /*if(newLine)
        buffer.append("\r\n  " + ident);
        else
        newLine=true;*/
        if (openBracet)
        {
          if (addedRounded)
          {
            if (addedLine)
              buffer.erase(buffer.end() - (5 + ident.length()), buffer.end());
            else if (!addedRoundLast /*&& buffer.length() > 1*/)
              buffer.pop_back();
            buffer.append(")");
            addedRounded = false;
          }
          buffer.append("\r\n" + ident + "{\r\n  " + ident);
          openBracet = false;
        }
        else if (newLine)
        {
          if (addedLine)
            buffer.erase(buffer.end() - (5 + ident.length()), buffer.end());
          else if (!addedRoundLast && buffer.length() > 1)
            buffer.pop_back();

          if (addedRounded)
          {
            buffer.append(");\r\n  " + ident);
            addedRounded = false;
          }
          else if (breaker)
            buffer.append(" \r\n  " + ident);
          else
            buffer.append(";\r\n  " + ident);
        }
        else
        {
          if (addedRounded)
          {
            if (addedLine)
              buffer.erase(buffer.end() - (4 + ident.length()), buffer.end());
            else if (!addedRoundLast /*&& buffer.length() > 1*/)
              buffer.pop_back();
            buffer.append(");\r\n  " + ident);
            addedRounded = false;
          }
          else if (!addedLine)
            buffer.append("\r\n  " + ident);
          newLine = true;
        }
        data += 4;
        break;

      case 0x3:
        ident += "  ";
        buffer.append("STRUCT{\r\n  " + ident);
        breakLineAfterThis = false;
        if (!inStructure)
          inStructure = ident.length();
        break;

      case 0x4:
        if (inStructure == ident.length())
          inStructure = 0;
        DecreaseNesting();
        if (addedLine)
          buffer.erase(buffer.end() - (5 + ident.length()), buffer.end());
        buffer.append("\r\n  " + ident + "} ");
        //newLine=false;
        breakLineAfterThis = true;
        break;

      case 0x5:
        ident += "  ";
        buffer.append("ARRAY{\r\n  " + ident);
        breakLineAfterThis = false;
        if (!inStructure)
          inStructure = ident.length();
        //buffer.append("\r\n  " );
        //newLine=false;
        break;

      case 0x6:
        if (inStructure == ident.length())
          inStructure = 0;
        DecreaseNesting();
        if (addedLine)
          buffer.erase(buffer.end() - (5 + ident.length()), buffer.end());
        buffer.append("\r\n  " + ident + "} ");
        newLine = false;
        breakLineAfterThis = true;
        break;

      case 0x7:
        buffer.append("= ");
        requestNewLineAfterNext = true;
        addRounded = false;
        break;

      case 0x8:
        buffer.append(".");
        break;

      case 0x9:
        buffer.append(", ");
        break;

      case 0xA:
        buffer.append("- ");
        break;

      case 0xB:
        buffer.append("+ ");
        break;

      case 0xC:
        buffer.append("* ");
        break;

      case 0xD:
        buffer.append("/ ");
        break;

      case 0xE:
        buffer.append("( ");
        break;

      case 0xF:
        buffer.append(") ");
        break;

      case 0x11:
        buffer.append("== ");
        break;

      case 0x12:
        buffer.append("< ");
        break;

      case 0x13:
        buffer.append("<= ");
        break;

      case 0x14:
        buffer.append("> ");
        break;

      case 0x15:
        buffer.append(">= ");
        break;

      case 0x16:
        char* s;
        s = (*(Checksum*)data).GetString();
        if (s == NULL)
        {
          s = new char[256];
          sprintf(s, "pseudo%u", pseudo);
          pseudo++;
          buffer.append(s);
          delete[] s;
        }
        else
          buffer.append(s);

        data += 4;
        if (addRounded && !inStructure && *data != 7)
        {
          if (!addedRounded)
          {
            addRoundAfterThis = true;
            addedRounded = true;
          }
          else
            buffer.append(" ");
        }
        else
        {
          addRounded = true;
          // addedRounded=false;
          buffer.append(" ");
        }
        break;

      case 0x17:
        buffer.append(itoa(*(int*)data, 10));
        buffer.append(" ");
        data += 4;
        break;

      case 0x18:
        buffer.append("0x18 ");
        break;

      case 0x19:
        buffer.append("0x19 ");
        break;

      case 0x1A:
        x = *(float*)data;
        data += 4;
        iX = (int)x;
        sprintf(tempX - 1, "%.6g", fabs(x) - abs(iX));
        if (*(tempX) == '\0')
          sprintf(result, "%d.0 ", iX);
        else
          sprintf(result, "%d%s ", iX, tempX);
        buffer.append(result);
        //buffer.append(ParseFloat() + " ");//ftoa(*(float*)data));//
        addRounded = false;
        //data+=4;
        //buffer.append(" ");
        break;

      case 0x1B:
        buffer.append("\"");
        buffer.append(((char*)data + 4));
        buffer.append("\" ");
        data += *(int*)data + 4;
        break;

      case 0x1C:
        buffer.append("'");
        buffer.append(((char*)data + 4));
        buffer.append("' ");
        data += *(int*)data + 4;
        break;

      case 0x1D:
        buffer.append("0x1D ");
        break;

      case 0x1E:

        /*x = *(float*)data;
        data+=4;
        y = *(float*)data;
        data+=4;
        z = *(float*)data;
        data+=4;
        sprintf(result, "VECTOR[%.0f.%.10g; %.0f.%.10g; %.0f.%.10g] ", x, fabs(x) - abs((int)x), y, fabs(y) - abs((int)y), z, fabs(z) - abs((int)z));
        buffer.append(result);*/
        x = *(float*)data;
        data += 4;
        iX = (int)x;
        y = *(float*)data;
        data += 4;
        iY = (int)y;
        z = *(float*)data;;
        data += 4;
        iZ = (int)z;
        sprintf(tempX - 1, "%.6g", fabs(x) - abs(iX));
        sprintf(tempY - 1, "%.6g", fabs(y) - abs(iY));
        sprintf(tempZ - 1, "%.6g", fabs(z) - abs(iZ));

        if (*(tempX) == '\0')
        {
          if (*(tempY) == '\0')
          {
            if (*(tempZ) == '\0')
              sprintf(result, "VECTOR[%d.0; %d.0; %d.0] ", iX, iY, iZ);
            else
              sprintf(result, "VECTOR[%d.0; %d.0; %d%s] ", iX, iY, iZ, tempZ);
          }
          else if (*(tempZ) == '\0')
            sprintf(result, "VECTOR[%d.0; %d%s; %d.0] ", iX, iY, tempY, iZ);
          else
            sprintf(result, "VECTOR[%d.0; %d%s; %d%s] ", iX, iY, tempY, iZ, tempZ);
        }
        else if (*(tempY) == '\0')
        {
          if (*(tempZ) == '\0')
            sprintf(result, "VECTOR[%d%s; %d.0; %.0] ", iX, tempX, iY, iZ);
          else
            sprintf(result, "VECTOR[%d%s; %d.0; %d%s] ", iX, tempX, iY, iZ, tempZ);
        }
        else if (*(tempZ) == '\0')
          sprintf(result, "VECTOR[%d%s; %d%s; %d.0] ", iX, tempX, iY, tempY, iZ);
        else
          sprintf(result, "VECTOR[%d%s; %d%s; %d%s] ", iX, tempX, iY, tempY, iZ, tempZ);
        buffer.append(result);
        //buffer.append("VECTOR[" + ParseVec() + "] ");
        addRounded = false;
        break;

      case 0x1F:

        /*x = *(float*)data;
        data+=4;
        y = *(float*)data;
        data+=4;
        sprintf(result, "PAIR[%.0f.%.10g; %.0f.%.10g] ", x, fabs(x) - abs((int)x), y, fabs(y) - abs((int)y));
        buffer.append(result);*/
        x = *(float*)data;
        data += 4;
        iX = (int)x;
        y = *(float*)data;
        data += 4;
        iY = (int)y;
        sprintf(tempX - 1, "%.6g", fabs(x) - abs(iX));
        sprintf(tempY - 1, "%.6g", fabs(y) - abs(iY));
        if (*(tempX) == '\0')
        {
          if (*(tempY) == '\0')
            sprintf(result, "PAIR[%d.0; %d.0] ", iX, iY);
          else
            sprintf(result, "PAIR[%d.0; %d%s] ", iX, iY, tempY);
        }
        else if (*(tempY) == '\0')
        {
          sprintf(result, "PAIR[%d%s; %d.0] ", iX, tempX, iY);
        }
        else
          sprintf(result, "PAIR[%d%s; %d%s] ", iX, tempX, iY, tempY);
        buffer.append(result);
        //buffer.append("PAIR[" + ParsePair() + "] ");

        break;

      case 0x20:
        /*ident += "  ";
        buffer.append("LOOP ");*/
        buffer.append("LOOP ");
        ident += "  ";
        openBracet = true;
        break;

      case 0x21:
        buffer.erase(buffer.end() - 2, buffer.end());
        /*buffer.append("END LOOP ");
        DecreaseNesting();*/
        DecreaseNesting();
        buffer.append("}\r\n");
        newLine = false;
        break;

      case 0x22:
        buffer.append("BREAK\r\n  " + ident);
        breaker = true;
        lastCall = true;
        break;

      case 0x23:
        buffer.append("FUNCTION ");
        ident += "  ";
        addRounded = false;
        //buffer.append("\r\n  "  + ident);
        //newLine=false;
        //requestNewLineAfterNext = true;
        openBracet = true;
        break;

      case 0x24:
        buffer.erase(buffer.end() - 2, buffer.end());
        buffer.append("}\r\n");//          " + ident);
        DecreaseNesting();
        //buffer.append("\r\n  " + ident);
        //newLine=false;
        newLine = false;
        break;

      case 0x25:
        buffer.append("IF ");
        ident += "  ";
        openBracet = true;
        break;

      case 0x26:
        // DecreaseNesting();
        buffer.erase(buffer.end() - 2, buffer.end());
        buffer.append("}\r\n" + ident + "ELSE ");
        newLine = false;

        openBracet = true;
        /*buffer.erase(buffer.end()-2, buffer.end());
        buffer.append("ELSE ");
        newLine=false;*/
        break;

      case 0x27:
        buffer.append("ELSEIF ");
        break;

      case 0x28:
        DecreaseNesting();
        buffer.erase(buffer.end() - 2, buffer.end());
        buffer.append("}\r\n");
        newLine = false;
        break;

      case 0x29:
        buffer.append("RETURN ");
        break;

      case 0x2A:
        buffer.append("UNDEFINED ");
        break;

      case 0x2B:
        data = NULL;
        break;

      case 0x2C:
        buffer.append("NULL ");
        break;

      case 0x2D:
        buffer.append("GLOBAL.");
        breakLineAfterThis = false;
        requestNewLineAfterNext = true;
        addRounded = false;
        break;

      case 0x2E:
        buffer.append("JUMP 0x");
        buffer.append(itoa(*(int*)data, 16));
        data += 4;
        buffer.append(" ");
        break;

      case 0x2F:
        buffer.append("RANDOM" + ParseRnd() + "]\r\n  " + ident);
        break;

      case 0x30:
        buffer.append("BETWEEN ");
        breakLineAfterThis = false;
        break;

      case 0x31:
        buffer.append("AT ");
        break;

      case 0x32:
        buffer.append("OR ");
        break;

      case 0x33:
        buffer.append("AND ");
        break;

      case 0x34:
        buffer.append("XOR ");
        break;

      case 0x35:
        buffer.append("<< ");
        break;

      case 0x36:
        buffer.append(">> ");
        break;

      case 0x37:
        buffer.append("RANDOM2" + ParseRnd() + "]\r\n  " + ident);
        break;

      case 0x38:
        MessageBox(0, "UnknownCode 0x38", "", 0);
        buffer.append("0x38 ");
        break;

      case 0x39:
        buffer.append("NOT ");
        break;

      case 0x3A:
        buffer.append("& ");
        break;

      case 0x3B:
        buffer.append("| ");
        break;

      case 0x3C:
        buffer.append("SWITCH ");
        break;

      case 0x3D:
        buffer.append("END SWITCH ");
        break;

      case 0x3E:
        buffer.append("CASE ");
        break;

      case 0x3F:
        buffer.append("DEFAULT ");
        break;

      case 0x40:
        buffer.append("RANDOM3" + ParseRnd() + "]\r\n  " + ident);
        break;

      case 0x41:
        buffer.append("RANDOM4" + ParseRnd() + "]\r\n  " + ident);
        break;

      case 0x42:
        buffer.append(": ");
        break;

      case 0x43:
        MessageBox(0, "UnknownCode 0x43", "", 0);
        buffer.append("0x43 ");
        break;

      case 0x44:
        MessageBox(0, "UnknownCode 0x44", "", 0);
        buffer.append("0x44 ");
        break;

      case 0x45:
        MessageBox(0, "UnknownCode 0x45", "", 0);
        buffer.append("0x45 ");
        break;

      case 0x46:
        MessageBox(0, "UnknownCode 0x46", "", 0);
        buffer.append("0x46 ");
        break;

      case 0x47:
        buffer.append("GOTO IF 0x");
        buffer.append(itoa((int)*(short*)data, 16));
        data += 2;
        buffer.append(" ");
        break;

      case 0x48:
        buffer.append("GOTO ELSE 0x");
        buffer.append(itoa((int)*(short*)data, 16));
        data += 2;
        buffer.append(" ");
        break;

      case 0x49:
        buffer.append("GOTO BREAK 0x");
        buffer.append(itoa((int)*(short*)data, 16));
        data += 2;
        buffer.append(" ");
        break;

      default:
        char txt[2];
        txt[0] = code;
        txt[1] = '0x00';
        MessageBox(0, "Unknown", txt, 0);
        buffer.append("UNKNOWN(" + std::string(txt) + "\r\n  " + ident);
        break;
      }

      if (breakLineAfterThis)
      {
        buffer.append("\r\n  " + ident);
        breakLineAfterThis = false;
        addedLine = true;
      }
      else if (addedLine)
        addedLine = false;
      else if (addRoundAfterThis)
      {
        buffer.append("(");
        addRoundAfterThis = false;
        addedRoundLast = true;
      }
      else if (addedRoundLast)
        addedRoundLast = false;
      if (requestNewLineAfterNext) {
        requestNewLineAfterNext = false;
        breakLineAfterThis = true;
      }
    }
    return buffer.c_str();
  }

  __declspec (noalias) const BYTE* __restrict GetParam(Checksum param, const BYTE* __restrict params) const;
  __declspec (noalias) void Execute(const Node* __restrict node = NULL, const BYTE* __restrict param = NULL) const;

  void Append(DWORD appendSize, DWORD dataSize, BYTE* data)
  {
    BYTE tmpData[8];
    DWORD overSize = dataSize - appendSize;

    DWORD appendIndex = this->size - (overSize);

    if (overSize)
      memcpy(tmpData, &this->func[appendIndex], overSize);
    this->size += appendSize;
    memcpy(&this->func[appendIndex], data, dataSize);
    if (overSize)
      memcpy(&this->func[this->size], tmpData, overSize);
    if (this->size > 13900)
      MessageBox(0, "Script Too Big...", NULL, 0);
  }

  bool Contains(DWORD checksum)
  {
    if (size < 4)
      return false;
    DWORD dSize = size - 3;
    for (DWORD i = 0; i < dSize; i++)
    {
      if (*(DWORD*)&func[i] == checksum)
        return true;
    }
    return false;
  }

  void InsertByte(DWORD index, BYTE byte)
  {
    if (size >= 13868)
      MessageBox(0, "Script Too Big...", name.GetString(), 0);
    else
    {
      BYTE tmpData[13869];
      const DWORD overSize = size - index;
      //MessageBox(0, "", "", 0);
      size++;
      memcpy(tmpData, &func[index], overSize);
      //MessageBox(0, "", "", 0);
      func[index] = byte;
      //MessageBox(0, "", "", 0);
      index++;
      memcpy(&func[index], tmpData, overSize);
      //MessageBox(0, "", "", 0);
    }
  }

  void InsertSub(DWORD index, Checksum name, DWORD value)
  {
    if (size >= 13868)
      MessageBox(0, "Script Too Big...", name.GetString(), 0);
    else
    {
      BYTE tmpData[13869];
      const DWORD overSize = size - index;
      size += 32;
      memcpy(tmpData, &func[index], overSize);
      func[index] = 0x16;
      index++;
      *(Checksum*)&func[index] = Checksum("SubToGlobal");
      index += 4;
      func[index] = 0x16;
      index++;
      (*(Checksum*)(&func[index])) = Checksum("Name");
      index += 4;
      func[index] = 0x7;
      index++;
      func[index] = 0x16;
      index++;
      (*(Checksum*)(&func[index])) = name;
      index += 4;
      func[index] = 0x16;
      index++;
      *(Checksum*)&func[index] = Checksum("value");
      index += 4;
      func[index] = 0x07;
      index++;
      func[index] = 0x17;
      index++;
      *(DWORD*)&func[index] = value;
      index += 4;
      func[index] = 0x2;
      index++;
      *(DWORD*)&func[index] = 0x00000001;
      index += 4;
      memcpy(&func[index], tmpData, overSize);
    }
  }

  void InsertAdd(DWORD index, Checksum name, DWORD value)
  {
    if (size >= 13868)
      MessageBox(0, "Script Too Big...", name.GetString(), 0);
    else
    {
      BYTE tmpData[13869];
      const DWORD overSize = size - index;
      size += 32;
      memcpy(tmpData, &func[index], overSize);
      func[index] = 0x16;
      index++;
      *(Checksum*)&func[index] = Checksum("AddToGlobal");
      index += 4;
      func[index] = 0x16;
      index++;
      (*(Checksum*)(&func[index])) = Checksum("Name");
      index += 4;
      func[index] = 0x7;
      index++;
      func[index] = 0x16;
      index++;
      (*(Checksum*)(&func[index])) = name;
      index += 4;
      func[index] = 0x16;
      index++;
      *(Checksum*)&func[index] = Checksum("value");
      index += 4;
      func[index] = 0x07;
      index++;
      func[index] = 0x17;
      index++;
      *(DWORD*)&func[index] = value;
      index += 4;
      func[index] = 0x2;
      index++;
      *(DWORD*)&func[index] = 0x00000001;
      index += 4;
      memcpy(&func[index], tmpData, overSize);
    }
  }

  void Append(const BYTE* data, DWORD dataSize)
  {
    if (size + dataSize > 13900)
    {
      char err[256];
      sprintf(err, "Script Too Big.%X.%X.%Xvoid Append(const BYTE* data, DWORD dataSize)", (DWORD*)data[0], (DWORD*)data[1], dataSize);
      MessageBox(0, err, (char*)data, 0);
    }
    else
    {
      memcpy(&this->func[size], data, dataSize);
      size += dataSize;
    }
  }
  void Append(const BYTE opcode)
  {
    if (size >= 13900)
      MessageBox(0, "Script Too Big...void Append(const BYTE opcode)", NULL, 0);
    else
    {
      this->func[size] = opcode;
      size++;
    }
  }

  void Script(DWORD qbKey)
  {
    if (size >= 13896)
      MessageBox(0, "Script Too Big... void StartScript(DWORD)", NULL, 0);
    else
    {
      this->func[size] = 0x16;
      size++;
      *(DWORD*)&this->func[size] = qbKey;
      size += 4;
    }
  }

  void Remove(BYTE* offset, BYTE size)
  {
    DWORD keepSize = this->size - (DWORD)func - (DWORD)offset - size;
    this->size -= size;
    memcpy(offset, offset + size, keepSize);
  }

  void Script(Checksum qbKey)
  {
    if (size >= 13896)
      MessageBox(0, "Script Too Big... void StartScript(Checksum)", NULL, 0);
    else
    {
      this->func[size] = 0x16;
      size++;
      *(Checksum*)&this->func[size] = qbKey;
      size += 4;
    }
  }

  void Script(char* name)
  {
    if (size >= 13896)
      MessageBox(0, "Script Too Big... void StartScript(char*)", NULL, 0);
    else
    {
      this->func[size] = 0x16;
      size++;
      *(Checksum*)&this->func[size] = Checksum(name);
      size += 4;
    }
  }

  void EndScript()
  {
    if (size >= 13896)
      MessageBox(0, "Script Too Big... void EndScript()", NULL, 0);
    else
    {
      this->func[size] = 2;
      size++;
      *(DWORD*)&this->func[size] = 0x00000001;
      size += 4;
    }
  }

  void Append(const DWORD* data)
  {
    if (size >= 13897)
      MessageBox(0, "Script Too Big...void Append(const DWORD* data)", NULL, 0);
    else
    {
      //memcpy(&this->func[size], data, 4);
      *(DWORD*)&this->func[size] = *data;
      size += 4;
    }
  }

  void Append(Checksum chc)
  {
    if (size >= 13897)
      MessageBox(0, "Script Too Big...void Append(Checksum chc)", chc.GetString(), 0);
    else
    {
      //memcpy(&this->func[size], &chc, 4);
      *(Checksum*)&this->func[size] = chc;
      size += 4;
    }
  }

  KnownScript(Checksum name, void* data, DWORD size)
  {
    if (size > 13900)
      MessageBox(0, "Script Too Big...KnownScript(Checksum name, void* data, DWORD size)", NULL, 0);
    else
    {
      this->name = name;
      if (size)
        memcpy(func, data, size);
      this->size = size;
    }
  }
};

struct FaceInfo
{
  WORD sound;
  WORD flags;

  FaceInfo()
  {
    sound = 0;
    flags = 0;
  }
};

struct Face
{
  WORD a;
  WORD b;
  WORD c;
  WORD sound;
  WORD flags;

  Face(SimpleFace* face, FaceInfo* info)
  {
    a = face->a;
    b = face->b;
    c = face->c;

    sound = 0;//sound = info->sound;
    flags = info->flags;
  }

  Face(WORD a, WORD b, WORD c)
  {
    this->a = a;
    this->b = b;
    this->c = c;
    this->sound = 0;
    this->flags = 0x0510;//no shadow/no shadow wall/no skatable
  }

  Face(SimpleFace* face, FaceInfo* info, WORD offset)
  {
    a = (face->a + offset);
    b = (face->b + offset);
    c = (face->c + offset);

    sound = 0;//sound = info->sound;
    flags = info->flags;
  }

  Face(SimpleFace* face, FaceInfo* info, DWORD* offset)
  {

    a = offset[face->a];
    b = offset[face->b];
    c = offset[face->c];

    sound = 0;//sound = info->sound;
    flags = info->flags;
  }

  Face()
  {
    a = 0;
    b = 0;
    c = 0;
    sound = 0;
    flags = 0;
  }
};

struct Colour
{
  BYTE r;
  BYTE g;
  BYTE b;
  union
  {
    BYTE a;
    BYTE blendCount;
  };

  DWORD GetBGR()
  {
    return b | g << 8 | r << 16 | a << 24;
  }

  bool operator!= (const Colour& rhs)
  {
    return (*(DWORD*)&*this != *(DWORD*)&rhs);
  }

  Colour()
  {
    r = 0;
    g = 0;
    b = 0;
    a = 0;
  }

  Colour(BYTE r, BYTE g, BYTE b)
  {
    this->blendCount = 0;
    this->r = r;
    this->g = g;
    this->b = b;
  }

  Colour(Colour* color)
  {
    r = color->r;
    g = color->g;
    b = color->b;
    a = color->a;
  }

  void blend(Colour blend, bool modify = true, float bias = 0.5)
  {
    if (modify)
    {
      blendCount++;

      bias /= blendCount;
    }
    this->r = BYTE((float)this->r * (1.0 - bias) + (float)blend.r * bias);
    this->g = BYTE((float)this->g * (1.0 - bias) + (float)blend.g * bias);
    this->b = BYTE((float)this->b * (1.0 - bias) + (float)blend.b * bias);
  }

  /*void blend(Colour blend)
  {
  this->r = (this->r + blend.r) / 2;
  this->g = (this->g + blend.g) / 2;
  this->b = (this->b + blend.b) / 2;
  }*/
};

struct TexCoord
{
  float u;
  float v;
};

struct ByteTexCoord
{
  byte u;
  byte v;
};

struct Th4TexCoord
{
  DWORD numTexCoords;
  TexCoord* texCoords;
};

struct SimpleVertex;

struct Vertex
{
  float x;
  float y;
  float z;

  Colour colour;

  TexCoord tUV[1];

  Vertex()
  {
    x = 0.0f;
    y = 0.0f;
    z = 0.0f;
    //rgba = 0.0f;
    for (DWORD i = 0; i < 1; i++)
    {
      tUV[i].u = 0.0f; tUV[i].v = 0.0f;
    }
    //memset(&uv,0,32);
  }
  Vertex(BYTE r, BYTE g, BYTE b)
  {
    x = 0.0f;
    y = 0.0f;
    z = 0.0f;
    for (DWORD i = 0; i < 4; i++)
    {
      tUV[i].u = 0.0f; tUV[i].v = 0.0f;
    }
    colour.a = 255;
    colour.r = r;
    colour.g = g;
    colour.b = b;
  }
};

struct SimpleVertex
{
  float x;
  float y;
  float z;

  float DistanceTo(SimpleVertex & other)
  {
    float dx = this->x - other.x;
    float dy = this->y - other.y;
    float dz = this->z - other.z;

    return (dx*dx + dy*dy + dz*dz);
  }
};
#pragma pack(push)
#pragma pack(1)
struct Th2FixedVert
{
  signed int x;
  signed int y;
  signed int z;

  Th2FixedVert()
  {
    x = 0;
    y = 0;
    z = 0;
  }
};

struct Th2Data
{
  Colour* colour;
  float u;
  float v;

  Th2Data()
  {
    colour = NULL;
    u = 0;
    v = 0;
  }

  /*Th2Data operator=(Colour* colour)
  {
  this->colour = colour;
  }*/
};

struct Th2Vertex
{
  signed short x;
  signed short y;
  signed short z;
  signed short align;
};
#pragma pack(pop)
struct BBox
{
  SimpleVertex Min;
  SimpleVertex Max;

  void refitBBox(const BBox &other)
  {
    if (Max.x < other.Max.x)
      Max.x = other.Max.x;
    if (Max.y < other.Max.y)
      Max.y = other.Max.y;
    if (Max.z < other.Max.z)
      Max.z = other.Max.z;

    if (Min.x > other.Min.x)
      Min.x = other.Min.x;
    if (Min.y > other.Min.y)
      Min.y = other.Min.y;
    if (Min.z > other.Min.z)
      Min.z = other.Min.z;
  }

  void refit(const Face* faces, const DWORD* faceIndices, const DWORD numFaces, const Vertex* vertices)
  {
    Max.x = vertices[faces[faceIndices[0]].a].x;
    Max.y = vertices[faces[faceIndices[0]].a].y;
    Max.z = vertices[faces[faceIndices[0]].a].z;
    Min.x = vertices[faces[faceIndices[0]].a].x;
    Min.y = vertices[faces[faceIndices[0]].a].y;
    Min.z = vertices[faces[faceIndices[0]].a].z;
    for (DWORD i = 1; i<numFaces; i++)
    {
      if (vertices[faces[faceIndices[i]].a].x>Max.x)
        Max.x = vertices[faces[faceIndices[i]].a].x;
      else if (vertices[faces[faceIndices[i]].a].x < Min.x)
        Min.x = vertices[faces[faceIndices[i]].a].x;

      if (vertices[faces[faceIndices[i]].a].y > Max.y)
        Max.y = vertices[faces[faceIndices[i]].a].y;
      else if (vertices[faces[faceIndices[i]].a].y < Min.y)
        Min.y = vertices[faces[faceIndices[i]].a].y;

      if (vertices[faces[faceIndices[i]].a].x > Max.z)
        Max.z = vertices[faces[faceIndices[i]].a].z;
      else if (vertices[faces[faceIndices[i]].a].z < Min.z)
        Min.z = vertices[faces[faceIndices[i]].a].z;

      if (vertices[faces[faceIndices[i]].b].x > Max.x)
        Max.x = vertices[faces[faceIndices[i]].b].x;
      else if (vertices[faces[faceIndices[i]].b].x < Min.x)
        Min.x = vertices[faces[faceIndices[i]].b].x;

      if (vertices[faces[faceIndices[i]].b].y > Max.y)
        Max.y = vertices[faces[faceIndices[i]].b].y;
      else if (vertices[faces[faceIndices[i]].b].y < Min.y)
        Min.y = vertices[faces[faceIndices[i]].b].y;

      if (vertices[faces[faceIndices[i]].b].x > Max.z)
        Max.z = vertices[faces[faceIndices[i]].b].z;
      else if (vertices[faces[faceIndices[i]].b].z < Min.z)
        Min.z = vertices[faces[faceIndices[i]].b].z;

      if (vertices[faces[faceIndices[i]].c].x > Max.x)
        Max.x = vertices[faces[faceIndices[i]].c].x;
      else if (vertices[faces[faceIndices[i]].c].x < Min.x)
        Min.x = vertices[faces[faceIndices[i]].c].x;

      if (vertices[faces[faceIndices[i]].c].y > Max.y)
        Max.y = vertices[faces[faceIndices[i]].c].y;
      else if (vertices[faces[faceIndices[i]].c].y < Min.y)
        Min.y = vertices[faces[faceIndices[i]].c].y;

      if (vertices[faces[faceIndices[i]].c].x > Max.z)
        Max.z = vertices[faces[faceIndices[i]].c].z;
      else if (vertices[faces[faceIndices[i]].c].z < Min.z)
        Min.z = vertices[faces[faceIndices[i]].c].z;
    }
  }
};
#endif