#include "stdafx.h"
#include "Imagehandling.h"
#include "Drawer.h"
//#include "Debug.h"
#pragma unmanaged

inline void DecompressDXT1alpha(const BYTE *src, unsigned int width, unsigned int height, BYTE *g_tgaBuf, unsigned int size)
{
  unsigned long tgaCnt = 0;//, tCnt=0;
  unsigned long lp1 = 0, lp2 = 0;
  unsigned long cntW=0, cntH=0;
  unsigned short rC1= 0, rC2= 0;
  unsigned char c1[4], c2[4], c3[4], c4[4];
  unsigned char tmp = 0, tmpW=0, tmpH=0;

  for(lp1=0;lp1<(size);lp1+=8) {

    // read in colors
    rC1 = src[lp1] + (src[lp1+1]*256);
    rC2 = src[lp1+2] + (src[lp1+3]*256);
    tmpH=0;

    // convert to 32bit
    c1[3] = (rC1&0x1);
    c1[2] = (rC1&0xF800)>>12;	// red
    c1[2] = (c1[2] << 3) | (c1[2] >> 2);
    c1[1] = (rC1&0x7C0)>>6;		// green
    c1[1] = (c1[1] << 3) | (c1[1] >> 2);
    c1[0] = (rC1&0x003E)>>1;  // blue
    c1[0] = (c1[0] << 3) | (c1[0] >> 2);
    

    c2[3] = (rC2&0x1);
    c2[2] = (rC2&0xF800)>>12;	// red
    c2[2] = (c2[2] << 3) | (c2[2] >> 2);
    c2[1] = (rC2&0x7C0)>>6;		// green
    c2[1] = (c2[1] << 3) | (c2[1] >> 2);
    c2[0] = (rC2&0x003E)>>1;  // blue
    c2[0] = (c2[0] << 3) | (c2[0] >> 2);

    //printf("rC1: %X, rC2:%X\n", rC1, rC2);
    //printf("c1: r%u g%u b%u, c2: r%u g%u b%u\n", c1[0], c1[1], c1[2], c2[0], c2[1], c2[2]);

    if(rC1 > rC2) {
      c3[0] = ((c1[0]/3)*2) + (c2[0]/3);
      c3[1] = ((c1[1]/3)*2) + (c2[1]/3);
      c3[2] = ((c1[2]/3)*2) + (c2[2]/3);
      c3[3] = c1[3] + c2[3];
      c4[0] = (c1[0]/3) + ((c2[0]/3)*2);
      c4[1] = (c1[1]/3) + ((c2[1]/3)*2);
      c4[2] = (c1[2]/3) + ((c2[2]/3)*2);
      c4[3] = c1[3] + c2[3];
    }
    else {
      c3[0] = (c1[0]/2) + (c2[0]/2);
      c3[1] = (c1[1]/2) + (c2[1]/2);
      c3[2] = (c1[2]/2) + (c2[2]/2);
      c3[3] = (c1[3] + c2[3]);
      c4[0] = 0;
      c4[1] = 0;
      c4[2] = 0;
      c4[3] = 0;
      
    }

    for(lp2=0;lp2<16;lp2++) {

      switch(lp2)
      {
      case 0:
      case 4:
      case 8:
      case 12:
        tmpW = 3;
        tmp = src[(lp1+4)+(lp2/4)] & 0xC0; // 11000000
        tmp >>= 6;
        break;
      case 1:
      case 5:
      case 9:
      case 13:
        tmpW = 2;
        tmp = src[(lp1+4)+(lp2/4)] & 0x30; // 00110000
        tmp >>= 4;
        break;
      case 2:
      case 6:
      case 10:
      case 14:
        tmpW = 1;
        tmp = src[(lp1+4)+(lp2/4)] & 0x0C; // 00001100
        tmp >>= 2;
        break;
      case 3:
      case 7:
      case 11:
      case 15:
        tmpW = 0;
        tmp = src[(lp1+4)+(lp2/4)] & 0x03; // 00000011
        break;
      }

      tgaCnt = (((cntH+tmpH)*width)+(cntW+tmpW))*4; // set tga offset
      switch(tmp)
      {
      case 0x00: // 00
        //g_tgaBuf[((cntH+tmpH)*tiH.width)+(cntW+tmpW)] = c1;
        g_tgaBuf[tgaCnt] = c1[0];
        g_tgaBuf[tgaCnt+1] = c1[1];
        g_tgaBuf[tgaCnt+2] = c1[2];
        if(c1[3])
          g_tgaBuf[tgaCnt+3] = 0xFF;
        else
          g_tgaBuf[tgaCnt+3] = 0;
        //printf("c1: %X\n", c1);
        //tCnt++;
        break;
      case 0x01: // 01
        //g_tgaBuf[((cntH+tmpH)*tiH.width)+(cntW+tmpW)] = c2;
        g_tgaBuf[tgaCnt] = c2[0];
        g_tgaBuf[tgaCnt+1] = c2[1];
        g_tgaBuf[tgaCnt+2] = c2[2];
        if(c2[3])
          g_tgaBuf[tgaCnt+3] = 0xFF;
        else
          g_tgaBuf[tgaCnt+3] = 0;
        //printf("c2: %X\n", c2);
        //tCnt++;
        break;
      case 0x02: // 10
        //g_tgaBuf[((cntH+tmpH)*tiH.width)+(cntW+tmpW)] = c3;
        g_tgaBuf[tgaCnt] = c3[0];
        g_tgaBuf[tgaCnt+1] = c3[1];
        g_tgaBuf[tgaCnt+2] = c3[2];
        if(c3[3])
          g_tgaBuf[tgaCnt+3] = 0xFF;
        else
          g_tgaBuf[tgaCnt+3] = 0;
        //printf("c3: %X\n", c3);
        //tCnt++;
        break;
      case 0x03: // 11
        //g_tgaBuf[((cntH+tmpH)*tiH.width)+(cntW+tmpW)] = c4;
        g_tgaBuf[tgaCnt] = c4[0];
        g_tgaBuf[tgaCnt+1] = c4[1];
        g_tgaBuf[tgaCnt+2] = c4[2];
        if(c1[3])
          g_tgaBuf[tgaCnt+3] = 0xFF;
        else
          g_tgaBuf[tgaCnt+3] = 0;
        //printf("c4: %X\n", c4);
        //tCnt++;
        break;
      }

      //printf("tmp: %u\n", tmp);
      if(tmpW == 0) tmpH++;
    }

    // update height counter
    cntW += 4;
    if(cntW >= width) cntW=0, cntH+=4;
  }
}

inline void DecompressDXT1(const BYTE *src, unsigned int width, unsigned int height, BYTE *g_tgaBuf,unsigned int size)
{
  unsigned long tgaCnt = 0;//, tCnt=0;
  unsigned long lp1 = 0, lp2 = 0;
  unsigned long cntW=0, cntH=0;
  unsigned short rC1= 0, rC2= 0;
  bool a;
  unsigned char c1[3], c2[3], c3[3], c4[3];
  unsigned char tmp = 0, tmpW=0, tmpH=0;

  for(lp1=0;lp1<(size);lp1+=8) {

    // read in colors
    rC1 = src[lp1] + (src[lp1+1]*256);
    rC2 = src[lp1+2] + (src[lp1+3]*256);
    tmpH=0;

    // convert to 24bit
    c1[2] = ((float)((rC1&0xF800)>>11)/0x1F)*255;	// red
    c1[1] = ((float)((rC1&0x7E0)>>5)/0x3F)*255;		// green
    c1[0] = ((float)(rC1&0x1F)/0x1F)*255;			// blue

    c2[2] = ((float)((rC2&0xF800)>>11)/0x1F)*255;	// red
    c2[1] = ((float)((rC2&0x7E0)>>5)/0x3F)*255;		// green
    c2[0] = ((float)(rC2&0x1F)/0x1F)*255;			// blue

    //printf("rC1: %X, rC2:%X\n", rC1, rC2);
    //printf("c1: r%u g%u b%u, c2: r%u g%u b%u\n", c1[0], c1[1], c1[2], c2[0], c2[1], c2[2]);

    if(rC1 > rC2) {
      c3[0] = ((c1[0]/3)*2) + (c2[0]/3);
      c3[1] = ((c1[1]/3)*2) + (c2[1]/3);
      c3[2] = ((c1[2]/3)*2) + (c2[2]/3);
      c4[0] = (c1[0]/3) + ((c2[0]/3)*2);
      c4[1] = (c1[1]/3) + ((c2[1]/3)*2);
      c4[2] = (c1[2]/3) + ((c2[2]/3)*2);
      a=true;
    }
    else {
      c3[0] = (c1[0]/2) + (c2[0]/2);
      c3[1] = (c1[1]/2) + (c2[1]/2);
      c3[2] = (c1[2]/2) + (c2[2]/2);
      c4[0] = 0;
      c4[1] = 0;
      c4[2] = 0;
      a=false;
    }

    for(lp2=0;lp2<16;lp2++) {

      switch(lp2)
      {
      case 0:
      case 4:
      case 8:
      case 12:
        tmpW = 3;
        tmp = src[(lp1+4)+(lp2/4)] & 0xC0; // 11000000
        tmp >>= 6;
        break;
      case 1:
      case 5:
      case 9:
      case 13:
        tmpW = 2;
        tmp = src[(lp1+4)+(lp2/4)] & 0x30; // 00110000
        tmp >>= 4;
        break;
      case 2:
      case 6:
      case 10:
      case 14:
        tmpW = 1;
        tmp = src[(lp1+4)+(lp2/4)] & 0x0C; // 00001100
        tmp >>= 2;
        break;
      case 3:
      case 7:
      case 11:
      case 15:
        tmpW = 0;
        tmp = src[(lp1+4)+(lp2/4)] & 0x03; // 00000011
        break;
      }

      tgaCnt = (((cntH+tmpH)*width)+(cntW+tmpW))*4; // set tga offset
      switch(tmp)
      {
      case 0x00: // 00
        //g_tgaBuf[((cntH+tmpH)*tiH.width)+(cntW+tmpW)] = c1;
        g_tgaBuf[tgaCnt] = c1[0];
        g_tgaBuf[tgaCnt+1] = c1[1];
        g_tgaBuf[tgaCnt+2] = c1[2];
        g_tgaBuf[tgaCnt+3] = 0xFF;
        //printf("c1: %X\n", c1);
        //tCnt++;
        break;
      case 0x01: // 01
        //g_tgaBuf[((cntH+tmpH)*tiH.width)+(cntW+tmpW)] = c2;
        g_tgaBuf[tgaCnt] = c2[0];
        g_tgaBuf[tgaCnt+1] = c2[1];
        g_tgaBuf[tgaCnt+2] = c2[2];
        g_tgaBuf[tgaCnt+3] = 0xFF;
        //printf("c2: %X\n", c2);
        //tCnt++;
        break;
      case 0x02: // 10
        //g_tgaBuf[((cntH+tmpH)*tiH.width)+(cntW+tmpW)] = c3;
        g_tgaBuf[tgaCnt] = c3[0];
        g_tgaBuf[tgaCnt+1] = c3[1];
        g_tgaBuf[tgaCnt+2] = c3[2];
        g_tgaBuf[tgaCnt+3] = 0xFF;
        //printf("c3: %X\n", c3);
        //tCnt++;
        break;
      case 0x03: // 11
        //g_tgaBuf[((cntH+tmpH)*tiH.width)+(cntW+tmpW)] = c4;
        g_tgaBuf[tgaCnt] = c4[0];
        g_tgaBuf[tgaCnt+1] = c4[1];
        g_tgaBuf[tgaCnt+2] = c4[2];
        g_tgaBuf[tgaCnt+3] = (BYTE)a*0xFF;
        //printf("c4: %X\n", c4);
        //tCnt++;
        break;
      }

      //printf("tmp: %u\n", tmp);
      if(tmpW == 0) tmpH++;
    }

    // update height counter
    cntW += 4;
    if(cntW >= width) cntW=0, cntH+=4;
  }
}

unsigned long PackRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
  return ((r << 16) | (g << 8) | (b) | a << 24);
}

void DecompressBlockDXT5(unsigned long x, unsigned long y, unsigned long width, const BYTE *blockStorage, unsigned long *image)
{
  unsigned char alpha0 = *reinterpret_cast<const unsigned char *>(blockStorage);
  unsigned char alpha1 = *reinterpret_cast<const unsigned char *>(blockStorage + 1);

  const unsigned char *bits = blockStorage + 2;
  unsigned long alphaCode1 = bits[2] | (bits[3] << 8) | (bits[4] << 16) | (bits[5] << 24);
  unsigned short alphaCode2 = bits[0] | (bits[1] << 8);

  unsigned short color0 = *reinterpret_cast<const unsigned short *>(blockStorage + 8);
  unsigned short color1 = *reinterpret_cast<const unsigned short *>(blockStorage + 10);	

  unsigned long temp;

  temp = (color0 >> 11) * 255 + 16;
  unsigned char r0 = (unsigned char)((temp/32 + temp)/32);
  temp = ((color0 & 0x07E0) >> 5) * 255 + 32;
  unsigned char g0 = (unsigned char)((temp/64 + temp)/64);
  temp = (color0 & 0x001F) * 255 + 16;
  unsigned char b0 = (unsigned char)((temp/32 + temp)/32);

  temp = (color1 >> 11) * 255 + 16;
  unsigned char r1 = (unsigned char)((temp/32 + temp)/32);
  temp = ((color1 & 0x07E0) >> 5) * 255 + 32;
  unsigned char g1 = (unsigned char)((temp/64 + temp)/64);
  temp = (color1 & 0x001F) * 255 + 16;
  unsigned char b1 = (unsigned char)((temp/32 + temp)/32);

  unsigned long code = *reinterpret_cast<const unsigned long *>(blockStorage + 12);

  for (int j=0; j < 4; j++)
  {
    for (int i=0; i < 4; i++)
    {
      int alphaCodeIndex = 3*(4*j+i);
      int alphaCode;

      if (alphaCodeIndex <= 12)
      {
        alphaCode = (alphaCode2 >> alphaCodeIndex) & 0x07;
      }
      else if (alphaCodeIndex == 15)
      {
        alphaCode = (alphaCode2 >> 15) | ((alphaCode1 << 1) & 0x06);
      }
      else // alphaCodeIndex >= 18 && alphaCodeIndex <= 45
      {
        alphaCode = (alphaCode1 >> (alphaCodeIndex - 16)) & 0x07;
      }

      unsigned char finalAlpha;
      if (alphaCode == 0)
      {
        finalAlpha = alpha0;
      }
      else if (alphaCode == 1)
      {
        finalAlpha = alpha1;
      }
      else
      {
        if (alpha0 > alpha1)
        {
          finalAlpha = ((8-alphaCode)*alpha0 + (alphaCode-1)*alpha1)/7;
        }
        else
        {
          if (alphaCode == 6)
            finalAlpha = 0;
          else if (alphaCode == 7)
            finalAlpha = 255;
          else
            finalAlpha = ((6-alphaCode)*alpha0 + (alphaCode-1)*alpha1)/5;
        }
      }

      unsigned char colorCode = (code >> 2*(4*j+i)) & 0x03;

      unsigned long finalColor;
      switch (colorCode)
      {
      case 0:
        finalColor = PackRGBA(r0, g0, b0, finalAlpha);
        break;
      case 1:
        finalColor = PackRGBA(r1, g1, b1, finalAlpha);
        break;
      case 2:
        finalColor = PackRGBA((2*r0+r1)/3, (2*g0+g1)/3, (2*b0+b1)/3, finalAlpha);
        break;
      case 3:
        finalColor = PackRGBA((r0+2*r1)/3, (g0+2*g1)/3, (b0+2*b1)/3, finalAlpha);
        break;
      }
      if (x + i < width)
        image[(y + j)*width + (x + i)] = finalColor;
    }
  }
}

inline void DecompressDXT5(unsigned long width, unsigned long height, const BYTE *blockStorage, unsigned long *image)
{
  unsigned long blockCountX = (width + 3) / 4;
  unsigned long blockCountY = (height + 3) / 4;
  unsigned long blockWidth = (width < 4) ? width : 4;
  unsigned long blockHeight = (height < 4) ? height : 4;

  for (unsigned long j = 0; j < blockCountY; j++)
  {
    for (unsigned long i = 0; i < blockCountX; i++) DecompressBlockDXT5(i*4, j*4, width, blockStorage + i * 16, image);
    blockStorage += blockCountX * 16;
  }
}
/*
static void DecompressBlockDXT1Internal (const BYTE* block,
unsigned long* output,
unsigned long outputStride,
const BYTE* alphaValues)
{
unsigned long temp, code;

unsigned short color0, color1;
BYTE r0, g0, b0, r1, g1, b1;

int i, j;

color0 = *(const BYTE*)(block);
color1 = *(const BYTE*)(block + 2);

temp = (color0 >> 11) * 255 + 16;
r0 = (BYTE)((temp/32 + temp)/32);
temp = ((color0 & 0x07E0) >> 5) * 255 + 32;
g0 = (BYTE)((temp/64 + temp)/64);
temp = (color0 & 0x001F) * 255 + 16;
b0 = (BYTE)((temp/32 + temp)/32);

temp = (color1 >> 11) * 255 + 16;
r1 = (BYTE)((temp/32 + temp)/32);
temp = ((color1 & 0x07E0) >> 5) * 255 + 32;
g1 = (BYTE)((temp/64 + temp)/64);
temp = (color1 & 0x001F) * 255 + 16;
b1 = (BYTE)((temp/32 + temp)/32);

code = *(const BYTE*)(block + 4);

if (color0 > color1) {
for (j = 0; j < 4; ++j) {
for (i = 0; i < 4; ++i) {
unsigned long finalColor, positionCode;
BYTE alpha;

alpha = alphaValues [j*4+i];

finalColor = 0;
positionCode = (code >>  2*(4*j+i)) & 0x03;

switch (positionCode) {
case 0:
finalColor = PackRGBA(r0, g0, b0, alpha);
break;
case 1:
finalColor = PackRGBA(r1, g1, b1, alpha);
break;
case 2:
finalColor = PackRGBA((2*r0+r1)/3, (2*g0+g1)/3, (2*b0+b1)/3, alpha);
break;
case 3:
finalColor = PackRGBA((r0+2*r1)/3, (g0+2*g1)/3, (b0+2*b1)/3, alpha);
break;
}

output [j*outputStride + i] = finalColor;
}
}
} else {
for (j = 0; j < 4; ++j) {
for (i = 0; i < 4; ++i) {
unsigned long finalColor, positionCode;
unsigned long alpha;

alpha = alphaValues [j*4+i];

finalColor = 0;
positionCode = (code >>  2*(4*j+i)) & 0x03;

switch (positionCode) {
case 0:
finalColor = PackRGBA(r0, g0, b0, alpha);
break;
case 1:
finalColor = PackRGBA(r1, g1, b1, alpha);
break;
case 2:
finalColor = PackRGBA((r0+r1)/2, (g0+g1)/2, (b0+b1)/2, alpha);
break;
case 3:
finalColor = PackRGBA(0, 0, 0, alpha);
break;
}

output [j*outputStride + i] = finalColor;
}
}
}
}

void DecompressBlockDXT3(unsigned long x, unsigned long y, unsigned long width,
const BYTE* blockStorage,
unsigned long* image)
{
int i;

BYTE alphaValues [16] = { 0 };

for (i = 0; i < 4; ++i) {
const BYTE* alphaData = (const BYTE*) (blockStorage);

alphaValues [i*4 + 0] = (((*alphaData) >> 0) & 0xF ) * 17;
alphaValues [i*4 + 1] = (((*alphaData) >> 4) & 0xF ) * 17;
alphaValues [i*4 + 2] = (((*alphaData) >> 8) & 0xF ) * 17;
alphaValues [i*4 + 3] = (((*alphaData) >> 12) & 0xF) * 17;

blockStorage += 2;
}

DecompressBlockDXT1Internal (blockStorage,
image + x + (y * width), width, alphaValues);
}
/*
void DecompressDXT3(unsigned long width, unsigned long height, const BYTE *blockStorage, unsigned long *image)
{
unsigned long blockCountX = (width + 3) / 4;
unsigned long blockCountY = (height + 3) / 4;
unsigned long blockWidth = (width < 4) ? width : 4;
unsigned long blockHeight = (height < 4) ? height : 4;

for (unsigned long j = 0; j < blockCountY; j++)
{
for (unsigned long i = 0; i < blockCountX; i++) DecompressBlockDXT3(i*4, j*4, width, blockStorage + i * 16, image);
blockStorage += blockCountX * 16;
}
}
*/

static int Unpack565( BYTE const* packed, BYTE* colour )
{
  // build the packed value
  int value = ( int )packed[0] | ( ( int )packed[1] << 8 );

  // get the components in the stored range
  BYTE red = ( BYTE )( ( value >> 11 ) & 0x1f );
  BYTE green = ( BYTE )( ( value >> 5 ) & 0x3f );
  BYTE blue = ( BYTE )( value & 0x1f );

  // scale up to 8 bits
  colour[0] = ( blue << 3 ) | ( blue >> 2 );
  colour[1] = ( green << 2 ) | ( green >> 4 );
  colour[2] = ( red << 3 ) | ( red >> 2 );
  colour[3] = 255;

  // return the value
  return value;
}

void DecompressBlockDXT3(BYTE* rgba, const BYTE* blockStorage)
{
  const BYTE* colourBlock = reinterpret_cast< BYTE const* >( blockStorage ) + 8;


  BYTE codes[16];
  int a = Unpack565( colourBlock, codes );
  int b = Unpack565( colourBlock + 2, codes + 4 );

  // generate the midpoints
  for( int i = 0; i < 3; ++i )
  {
    int c = codes[i];
    int d = codes[4 + i];

    codes[8 + i] = ( BYTE )( ( 2*c + d )/3 );
    codes[12 + i] = ( BYTE )( ( c + 2*d )/3 );
  }

  // fill in alpha for the intermediate values
  codes[8 + 3] = 255;
  codes[12 + 3] = 255;

  // unpack the indices
  BYTE indices[16];
  for( int i = 0; i < 4; ++i )
  {
    BYTE* ind = indices + 4*i;
    BYTE packed = colourBlock[4 + i];

    ind[0] = packed & 0x3;
    ind[1] = ( packed >> 2 ) & 0x3;
    ind[2] = ( packed >> 4 ) & 0x3;
    ind[3] = ( packed >> 6 ) & 0x3;
  }

  // store out the colours
  for( int i = 0; i < 16; ++i )
  {
    BYTE offset = 4*indices[i];
    for( int j = 0; j < 4; ++j )
      rgba[4*i + j] = codes[offset + j];
  }

  // unpack the alpha values pairwise
  for( int i = 0; i < 8; ++i )
  {
    // quantise down to 4 bits
    BYTE quant = blockStorage[i];

    // unpack the values
    BYTE lo = quant & 0x0f;
    BYTE hi = quant & 0xf0;

    // convert back up to bytes
    rgba[8*i + 3] = lo | ( lo << 4 );
    rgba[8*i + 7] = hi | ( hi >> 4 );
  }
}
inline void DecompressDXT3(unsigned long width, unsigned long height, const BYTE *blockStorage, BYTE *image)
{
  for( int y = 0; y < height; y += 4 )
  {
    for( int x = 0; x < width; x += 4 )
    {
      // decompress the block
      BYTE targetRgba[4*16];
      DecompressBlockDXT3( targetRgba, blockStorage);

      // write the decompressed pixels to the correct image locations
      BYTE const* sourcePixel = targetRgba;
      for( int py = 0; py < 4; ++py )
      {
        for( int px = 0; px < 4; ++px )
        {
          // get the target location
          int sx = x + px;
          int sy = y + py;
          if( sx < width && sy < height )
          {
            BYTE* targetPixel = image + 4*( width*sy + sx );

            // copy the rgba value
            for( int i = 0; i < 4; ++i )
              *targetPixel++ = *sourcePixel++;
          }
          else
          {
            // skip this pixel as its outside the image
            sourcePixel += 4;
          }
        }
      }

      // advance
      blockStorage += 16;
    }
  }
}

/*bool ExtractResource(const HINSTANCE hInstance, WORD resourceID, LPCTSTR szOutputFilename)
{
bool bSuccess = false; 
try
{
// First find and load the required resource
HRSRC hResource = FindResource(hInstance, MAKEINTRESOURCE(resourceID), "BINARY");
HGLOBAL hFileResource = LoadResource(hInstance, hResource);

// Now open and map this to a disk file
LPVOID lpFile = LockResource(hFileResource);
DWORD dwSize = SizeofResource(hInstance, hResource);            

// Open the file and filemap
HANDLE hFile = CreateFile(szOutputFilename, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
HANDLE hFilemap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, dwSize, NULL);            LPVOID lpBaseAddress = MapViewOfFile(hFilemap, FILE_MAP_WRITE, 0, 0, 0);            

// Write the file

CopyMemory(lpBaseAddress, lpFile, dwSize);            

// Unmap the file and close the handles

UnmapViewOfFile(lpBaseAddress);
CloseHandle(hFilemap);
CloseHandle(hFile);
}
catch(...)
{
// Ignore all type of errors
} 
return bSuccess;

}
bool dll = false;*/
bool DecompressDXT(const BYTE *src, unsigned int size, unsigned int width, unsigned int height, void *dest, int dxt)
{
  /*if(!dll)
  {
  if(ExtractResource(0,IDR_DLL_BIN,"msvcr100.dll"))
  dll = true;

  }*/
  /*DEBUGSTART()
  {*/
  /*if(!Console)
  {
  CreateConsole();
  }
  printf("Inside DecompressDXT\n");*/
  if(dxt==1)
  {
    //printf("decompressing DXT1\n");
    DecompressDXT1(src,width,height,(BYTE*)dest,size);
    //printf("DONE returning\n");
    return true;
  }
  else if(dxt == 2 || dxt == 3)
  {
    //printf("Testing DXT2 || DXT3\n");
    DecompressDXT3(width, height, src, (BYTE*)dest);

    /*for(DWORD i=0; i<width*height*4; i+=4)
    {
    BYTE tmp = ((BYTE*)dest)[i];
    ((BYTE*)dest)[i] = ((BYTE*)dest)[i+2];
    ((BYTE*)dest)[i+2] = tmp;
    }*/
    //printf("DONE returning\n");
    return true;
  }
  else if(dxt == 4 || dxt == 5)
  {
    //printf("decompressing DXT4 || DXT5\n");
    DecompressDXT5(width,height,src,(unsigned long*)dest);
    //printf("DONE returning\n");
    return true;
  }
  else
    return false;
  /*}
  DEBUGEND()*/
}
/*void Unswizzle(const void *src, unsigned int depth, unsigned int width, unsigned int height, void *dest)
{
for (UINT y = 0; y < height; y++)
{
UINT sy = 0;
if (y < width)
{
for (int bit = 0; bit < 16; bit++)
sy |= ((y >> bit) & 1) << (2*bit);
sy <<= 1; // y counts twice
}
else
{
UINT y_mask = y % width;
for (int bit = 0; bit < 16; bit++)
sy |= ((y_mask >> bit) & 1) << (2*bit);
sy <<= 1; // y counts twice
sy += (y / width) * width * width;
}
BYTE *d = (BYTE *)dest + y * width * depth;
for (UINT x = 0; x < width; x++)
{
UINT sx = 0;
if (x < height * 2)
{
for (int bit = 0; bit < 16; bit++)
sx |= ((x >> bit) & 1) << (2*bit);
}
else
{
int x_mask = x % (2*height);
for (int bit = 0; bit < 16; bit++)
sx |= ((x_mask >> bit) & 1) << (2*bit);
sx += (x / (2 * height)) * 2 * height * height;
}
BYTE *s = (BYTE *)src + (sx + sy)*depth;
for (unsigned int i = 0; i < depth; ++i)
*d++ = *s++;
}
}
}*/