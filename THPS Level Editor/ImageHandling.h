#include "windows.h"
#include <iostream>
#include <conio.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#pragma unmanaged

//void Unswizzle(const void *src, unsigned int depth, unsigned int width, unsigned int height, void *dest);
bool DecompressDXT(const BYTE *src, unsigned int size, unsigned int width, unsigned int height, void *dest, int dxt);