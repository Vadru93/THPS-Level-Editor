#include <Stdlib.h>
#include <Stdio.h>
#include <String.h>

typedef signed char			sbyte;
typedef unsigned char		ubyte;
typedef signed short		sword;
typedef signed int			sdword;
typedef signed __int64		sqword;
typedef unsigned __int64	uqword;
typedef float				sfloat;

#define RELEASE(x)		{ if (x != null) delete x;		x = null; }
#define RELEASEARRAY(x)	{ if (x != null) delete []x;	x = null; }

inline void ZeroMemory(void* addr, unsigned int size)
{
  memset(addr, 0, size);
}

inline void CopyMemory(void* dest, const void* src, unsigned int size)
{
  memcpy(dest, src, size);
}

inline void FillMemory(void* dest, unsigned int size, ubyte val)
{
  memset(dest, val, size);
}

#include "RevisitedRadix.h"
#include "CustomArray.h"
#include "Adjacency.h"
#include "Striper.h"