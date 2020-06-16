#ifndef SCN_HEADER_H
#define SCN_HEADER_H
#define TEXFLAG_UNKNOWN 1
#define TEXFLAG_EXTENSION 2
#define TEXFLAG_EXTENSION2 2048
#pragma unmanaged
#define MESHFLAG_TEXCOORD 1
#define MESHFLAG_COLOUR 2
#define MESHFLAG_NORMAL 4
#define MESHFLAG_SKIN 16
#define MESHFLAG_EXTENSION 2048
#define MESHFLAG_HEADEREXTENSION 8388608

__forceinline bool GetBit(const DWORD &value, const BYTE &bit)
{
  return(value & 1<<bit);
}
#endif