#include "TextureFile.h"
#include <string>
#include "Common.h"

TextureFile::TextureFile(const char *fileInputPath)
{
  int length;
  ReadFileToMemory(fileInputPath, "rb", (char**)&fileData, &length);

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
		
		uint8_t bit_0 = GetBitLittle(blockData[0], 0);
		uint8_t bit_1 = GetBitLittle(blockData[0], 1);
		uint8_t bit_2 = GetBitLittle(blockData[0], 2);
		uint8_t bit_3 = GetBitLittle(blockData[0], 3);
		uint8_t bit_4 = GetBitLittle(blockData[0], 4);
		uint8_t bit_5 = GetBitLittle(blockData[0], 5);
		uint8_t bit_6 = GetBitLittle(blockData[0], 6);
		uint8_t bit_7 = GetBitLittle(blockData[0], 7);
		uint8_t bit_8 = GetBitLittle(blockData[1], 0);
		uint8_t bit_9 = GetBitLittle(blockData[1], 1);
		uint8_t bit_10 = GetBitLittle(blockData[1], 2);
		uint8_t bit_11 = GetBitLittle(blockData[1], 3);
		uint8_t bit_12 = GetBitLittle(blockData[1], 4);
		
		bool dualWeightPlane_D = bit_10;
		bool precisionBit_H = bit_9;
		uint8_t R0, R1, R2, A, B, Width, Height;
		R0 = bit_4;
		uint8_t numberOfPartition = ((bit_12 << 1) | bit_11) + 1;

		if (bit_0 || bit_1) //5 first case
		{
			R1 = bit_0;
			R2 = bit_1;
			A = (bit_6 << 1) | bit_5;
			
			if ((bit_2 & bit_3) == 0) //3 first case
			{
				B = (bit_8 << 1) | bit_7;
				if (bit_3 == 0)
				{
					Height = A + 2;
					if (bit_2 == 0)
					{
						Width = B + 4;
					}
					else
					{
						Width = B + 8;
					}
				}
				else
				{
					Width = A + 2;
					Height = B + 8;
				}
			}
			else
			{
				B = bit_7;
				if (bit_8) //case 5
				{
					Width = B + 2;
					Height = A + 8;
				}
				else //case 4
				{
					Width = A + 2;
					Height = B + 6;
				}
			}
		}
		else //case 6 -> case 13
		{
			R1 = bit_2;
			R2 = bit_3;
			
			if (bit_8 == 0) //case 6, case 7
			{
				A = (bit_6 << 1) | bit_5;
				if (bit_7 == 0) //case 6
				{
					Width = 12;
					Height = A + 2;
				}
				else
				{
					Width = A + 2;
					Height = 12;
				}
			}
			else //case 8 -> case 13
			{
				if (bit_7 == 0) //case 10
				{
					A = (bit_6 << 1) | bit_5;
					B = (bit_10 << 1) | bit_9;
					Width = A + 6;
					Height = B + 6;
					dualWeightPlane_D = 0;
					precisionBit_H = 0;
				}
				else //case 8, case 9, case 11, 12, 13
				{
					if ((bit_2 & bit_3 & bit_4 & bit_5 & bit_6) == 1) //void
					{

					}
					else if (bit_5 == 0)
					{
						Width = 6;
						Height = 10;
					}
					else
					{
						Width = 10;
						Height = 6;
					}
				}
			}
		}
  }
}