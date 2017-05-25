#include "TextureFile.h"
#include <string>
#include "Common.h"
#include "TGAHeader.h"
#include <math.h>
#include <algorithm> 

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

uint8_t CEM_to_n[16] =
{
	2,
	2,
	2,
	2,
	4,
	4,
	4,
	4,
	6,
	6,
	6,
	6,
	8,
	8,
	8,
	8
};

void TextureFile::DecompressToTGA(const char *fileOutputPath)
{
	uint8_t* decompressData = new uint8_t[this->width*this->height*4]; //BGRA data
  if (this->openGLFormat >= COMPRESSED_RGBA_ASTC_4x4_KHR && this->openGLFormat <= COMPRESSED_RGBA_ASTC_12x12_KHR)
  {
    DecompressASTC(decompressData);
  }
  TGAHeader header;
  header.idlength = 0;
  header.colourmaptype = 0;
  header.datatypecode = 2;
  header.colourmaporigin = 0;
  header.colourmaplength = 0;
  header.colourmapdepth = 0;
  header.x_origin = 0;
  header.y_origin = 0;
  header.width = this->width;
  header.height = this->height;
  header.bitsperpixel = 32;
  header.imagedescriptor = 0x8;
  FILE *f = fopen(fileOutputPath, "wb");
  if (f)
  {
		//Flip
		uint8_t *temp = new uint8_t[this->width*4];
		for (int i = 0; i < this->height / 2; i++)
		{
			memcpy(temp, decompressData + i * this->width * 4, this->width * 4);
			memcpy(decompressData + i * this->width * 4, decompressData + (this->height - i - 1) * this->width * 4, this->width * 4);
			memcpy(decompressData + (this->height - i - 1) * this->width * 4, temp, this->width * 4);
		}
		delete[] temp;
    fwrite(&header, sizeof(TGAHeader), 1, f);
    fwrite(decompressData, 1, this->width*this->height * 4, f);
    fclose(f);
  }
  delete[] decompressData;
}

void TextureFile::DecompressASTC(uint8_t *buffer)
{
  uint8_t *output = buffer;
	uint8_t Bw, Bh;
  
  if (this->openGLFormat == COMPRESSED_RGBA_ASTC_6x6_KHR)
  {
		Bw = 6;
		Bh = 6;
  }
	else if (this->openGLFormat == COMPRESSED_RGBA_ASTC_8x8_KHR)
	{
		Bw = 8;
		Bh = 8;
	}
	const uint16_t bytePerUncompressedBlock = Bw * Bh * 4;
	uint32_t numberOfBlock = int((this->width + Bw - 1) / Bw) * int((this->height + Bh - 1) / Bh);
  uint32_t numberOfByte = numberOfBlock * 16;
	int8_t blockRowCount = (this->height + Bh - 1) / Bh;
	int8_t blockColumnCount = (this->width + Bw - 1) / Bw;
  for (int i = 0; i < numberOfBlock; i++)
  {
		int8_t blockRow = i / blockColumnCount;
		int8_t blockColumn = i % blockColumnCount;
		uint8_t* blockData = this->dataBeginPtr + 16 * i;
		uint8_t weightWidth, weightHeight;
		uint8_t weightRange; //from 0
		
		uint8_t bit_0 = GetBitLittleFromByte(blockData[0], 0);
		uint8_t bit_1 = GetBitLittleFromByte(blockData[0], 1);
		uint8_t bit_2 = GetBitLittleFromByte(blockData[0], 2);
		uint8_t bit_3 = GetBitLittleFromByte(blockData[0], 3);
		uint8_t bit_4 = GetBitLittleFromByte(blockData[0], 4);
		uint8_t bit_5 = GetBitLittleFromByte(blockData[0], 5);
		uint8_t bit_6 = GetBitLittleFromByte(blockData[0], 6);
		uint8_t bit_7 = GetBitLittleFromByte(blockData[0], 7);
		uint8_t bit_8 = GetBitLittleFromByte(blockData[1], 0);
		uint8_t bit_9 = GetBitLittleFromByte(blockData[1], 1);
		uint8_t bit_10 = GetBitLittleFromByte(blockData[1], 2);
		uint8_t bit_11 = GetBitLittleFromByte(blockData[1], 3);
		uint8_t bit_12 = GetBitLittleFromByte(blockData[1], 4);
		
		bool dualWeightPlane_D = bit_10;
		bool precisionBit_H = bit_9;
		uint8_t R0, R1, R2, A, B, Width, Height;
		R0 = bit_4;
		uint8_t numberOfPartition = ((bit_12 << 1) | bit_11) + 1;
		uint8_t CEM;

		if (bit_8 && blockData[0] == 0xFC) //void
		{
			bool dynamicRange_D = bit_9;
			if (dynamicRange_D) //HDR
			{

			}
			else //LDR
			{
				uint16_t minS = Get16BitLittleFromByteArray(blockData, 12, 24);
				uint16_t maxS = Get16BitLittleFromByteArray(blockData, 25, 37);
				uint16_t minT = Get16BitLittleFromByteArray(blockData, 38, 50);
				uint16_t maxT = Get16BitLittleFromByteArray(blockData, 51, 63);

				if (minS == 0x1FFF
					&& maxS == 0x1FFF
					&& minT == 0x1FFF
					&& maxT == 0x1FFF) //constant color
				{
          uint16_t ColorR, ColorG, ColorB, ColorA;
          ColorR = Get16BitLittleFromByteArray(blockData, 64, 71);
          ColorG = Get16BitLittleFromByteArray(blockData, 80, 87);
          ColorB = Get16BitLittleFromByteArray(blockData, 96, 103);
          ColorA = Get16BitLittleFromByteArray(blockData, 112, 119);
          uint32_t byteBeginBlock = ((blockRow * Bh) * this->width + blockColumn * Bw) * 4;
          for (int j = 0; j < Bw; j++)
          {
            uint32_t byteBeginBlockRow = byteBeginBlock + j * this->width * 4;
            for (int k = 0; k < Bh; k++)
            {
							
              output[byteBeginBlockRow + k * 4] = ColorB;
              output[byteBeginBlockRow + k * 4 + 1] = ColorG;
              output[byteBeginBlockRow + k * 4 + 2] = ColorR;
              output[byteBeginBlockRow + k * 4 + 3] = ColorA;
            }
          }
				}
				else
				{

				}
			}
		}
		else
		{
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
					else //case 8, case 9, case 12, 13
					{
						if (bit_5 == 0)
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
			if (numberOfPartition == 1)
			{
				CEM = Get8BitLittleFromByteArray(blockData, 13, 16);
				uint8_t n = CEM_to_n[CEM];
				uint8_t *v = new uint8_t[n];
				if (CEM == 8) //LDR RGB, direct 
				{
					/*
					s0 = v0 + v2 + v4; 
					s1 = v1 + v3 + v5;
					if (s1 >= s0) 
					{
						e0 = (v0, v2, v4, 0xFF);
						e1 = (v1, v3, v5, 0xFF);
					}
					else 
					{
						e0 = blue_contract(v1, v3, v5, 0xFF);
						e1 = blue_contract(v0, v2, v4, 0xFF);
					}
					*/
				}
				delete[] v;
			}
			else
			{
				uint8_t CEM2LowestBits = Get8BitLittleFromByteArray(blockData, 23, 24);
				if (CEM2LowestBits)
				{
					CEM = Get8BitLittleFromByteArray(blockData, 25, 28);
				}
				else
				{

				}
			}
		}
  }
}