#ifndef CHECKSUM_H
#define CHECKSUM_H
#pragma unmanaged
#define TERRAIN_DEFAULT = 0 

#define TERRAIN_CONCSMOOTH = 1 

#define TERRAIN_CONCROUGH = 2 

#define TERRAIN_METALSMOOTH = 3 

#define TERRAIN_METALROUGH = 4 

#define TERRAIN_METALCORRUGATED = 5 

#define TERRAIN_METALGRATING = 6 

#define TERRAIN_METALTIN = 7 

#define TERRAIN_WOOD = 8 

#define TERRAIN_WOODMASONITE = 9 

#define TERRAIN_WOODPLYWOOD = 10 

#define TERRAIN_WOODFLIMSY = 11 

#define TERRAIN_WOODSHINGLE = 12 

#define TERRAIN_WOODPIER = 13 

#define TERRAIN_BRICK = 14 

#define TERRAIN_TILE = 15 

#define TERRAIN_ASPHALT = 16 

#define TERRAIN_ROCK = 17 

#define TERRAIN_GRAVEL = 18 

#define TERRAIN_SIDEWALK = 19 

#define TERRAIN_GRASS = 20 

#define TERRAIN_GRASSDRIED = 21 

#define TERRAIN_DIRT = 22 

#define TERRAIN_DIRTPACKED = 23 

#define TERRAIN_WATER = 24 

#define TERRAIN_ICE = 25 

#define TERRAIN_SNOW = 26 

#define TERRAIN_SAND = 27 

#define TERRAIN_PLEXIGLASS = 28 

#define TERRAIN_FIBERGLASS = 29 

#define TERRAIN_CARPET = 30 

#define TERRAIN_CONVEYOR = 31 

#define TERRAIN_CHAINLINK = 32 

#define TERRAIN_METALFUTURE = 33 

#define TERRAIN_GENERIC1 = 34 

#define TERRAIN_GENERIC2 = 35 

#define TERRAIN_WHEELS = 36 

#define TERRAIN_WETCONC = 37 

#define TERRAIN_METALFENCE = 38 

#define TERRAIN_GRINDTRAIN = 39 

#define TERRAIN_GRINDROPE = 40

struct Checksum
{
  DWORD checksum;

  char* const GetString() const;
  char* const GetString2() const;

  Checksum()
  {
    this->checksum = 0;
  }

  Checksum(const char* string);
  Checksum(const char* string, bool createTable);


  Checksum(DWORD &checksum)
  {
    this->checksum = checksum;
  }

  void SetString(char* string);

  /*Checksum operator=(const DWORD &crc)
  {
    checksum = crc;
    return *this;
  }*/

};
Checksum GenerateCaseSensChecksum(const char* string);
#endif