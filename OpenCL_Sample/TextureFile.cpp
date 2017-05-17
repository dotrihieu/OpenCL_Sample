#include "TextureFile.h"
#include <string>
#include "Common.h"

TextureFile::TextureFile(const char *fileInputPath)
{
  int length;
  ReadFileToMemory("fileInputPath", "rb", (char**)&fileData, &length);

  if (length < 3)
  {
    return;
  }
  //detect header
  if (strncmp((char*)fileData, "PVR", 3) == 0)
  {
    PVRHeader header;
    memcpy(&header, fileData, sizeof(PVRHeader));
    this->width = header.width;
    this->height = header.height;
    this->openGLFormat = header.pixelFormat;
    this->mipmapCount = header.mipmapCount;
    if (header.pixelFormat >= 27 && header.pixelFormat <= 40) //ASTC 2D
    {
      if (header.colorSpace == 0) //linear RGB
      {
        this->openGLFormat = COMPRESSED_RGBA_ASTC_4x4_KHR + (header.pixelFormat - 27);
      }
      else if (header.colorSpace == 1) //sRGB
      {
        this->openGLFormat = COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR + (header.pixelFormat - 27);
      }
    }
    else if (header.pixelFormat >= 41 && header.pixelFormat <= 50) //ASTC 3D
    {
      if (header.colorSpace == 0) //linear RGB
      {
        this->openGLFormat = COMPRESSED_RGBA_ASTC_3x3x3_OES + (header.pixelFormat - 41);
      }
      else if (header.colorSpace == 1) //sRGB
      {
        this->openGLFormat = COMPRESSED_SRGB8_ALPHA8_ASTC_3x3x3_OES + (header.pixelFormat - 41);
      }
    }
    this->dataBeginPtr = fileData + sizeof(PVRHeader) + header.metaDataSize;
  }
}
TextureFile::~TextureFile()
{
  delete[] fileData;
}
void TextureFile::DecompressToTGA(const char *fileOutputPath)
{
	uint8_t* decompressData = new uint8_t[this->width*this->height*4];
  if (this->openGLFormat >= COMPRESSED_RGBA_ASTC_4x4_KHR && this->openGLFormat <= COMPRESSED_RGBA_ASTC_12x12_KHR)
  {
    DecompressASTC(decompressData);
  }
  delete[] decompressData;
}

void TextureFile::DecompressASTC(const uint8_t *buffer)
{
	uint8_t Bw = static_cast <int> (std::ceil(this->width));
	uint8_t Bh = static_cast <int> (std::ceil(this->height));
  uint32_t numberOfBlock;
  if (this->openGLFormat == COMPRESSED_RGBA_ASTC_6x6_KHR)
  {
    numberOfBlock = int((this->width + 5) / 6) * int((this->height + 5) / 6);
  }
  uint32_t numberOfByte = numberOfBlock * 16;
  for (int i = 0; i < numberOfBlock; i++)
  {
		uint8_t* blockData = this->dataBeginPtr + 16 * i;
		uint8_t weightWidth, weightHeight;
		uint8_t weightRange; //from 0
		bool dualWeightPlane_D = GetBitLittle(blockData[1], 2);
		bool precisionBit_H = GetBitLittle(blockData[1], 1);
  }
}