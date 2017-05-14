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
  char* fileData;
  char* dataBeginPtr;
  TextureFile(const char *fileInputPath);
  ~TextureFile();
  void DecompressToTGA(const char *fileOutputPath);
protected:
  void DecompressASTC(const char *buffer);
};