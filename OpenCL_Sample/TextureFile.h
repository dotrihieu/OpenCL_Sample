#pragma once
#include "PVRHeader.h"
#include "ASTC.h"

class TextureFile
{
public:
  uint32_t openGLFormat;
  uint32_t width;
  uint32_t height;
  uint32_t mipmapCount;
	uint8_t* fileData;
	uint8_t* dataBeginPtr;
  TextureFile(const char *fileInputPath);
  ~TextureFile();
  void DecompressToTGA(const char *fileOutputPath);
protected:
  void DecompressASTC(uint8_t *buffer);
};

struct WeightRange
{
  uint8_t Range;
  uint8_t Trits;
  uint8_t Quints;
  uint8_t Bits;
  WeightRange(uint8_t r, uint8_t t, uint8_t q, uint8_t b): Range(r), Trits(t), Quints(q), Bits(b)
  {

  }
};