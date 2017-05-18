#include <stdio.h>
#include <stdint.h>
#include <string.h>

bool ReadFileToMemory(const char* fileName, const char* readMode, char** buffer, int* length)
{
  FILE *file = fopen(fileName, readMode);
  if (file)
  {
    *length = 0;
    fseek(file, 0, SEEK_END);
    *length = ftell(file);
    fseek(file, 0, SEEK_SET);
    *buffer = new char[*length + 1];
    //memset(*buffer, 0, *length + 1);
    (*buffer)[*length] = 0;
    fread(*buffer, 1, *length, file);
    fclose(file);
    return true;
  }
  else
  {
    length = 0;
    *buffer = NULL;
    printf("File %s not found", fileName);
    return false;
  }
}

uint8_t GetBitLittleFromByte(const uint8_t &value, const int &index)
{
	return (value >> index) & 0x1;
}

uint8_t GetBitLittleFromByteArray(const uint8_t *value, const int &index)
{
	int byteIndex = index / 8;
	int bitIndex = index % 8;
	return GetBitLittleFromByte(value[byteIndex], bitIndex);
}

uint16_t Get16BitLittleFromByteArray(const uint8_t *value, const int &lowIndex, const int &highIndex)
{
	int lowByteIndex = lowIndex / 8;
	int highByteIndex = highIndex / 8;
	uint8_t temp[3];
	memcpy(temp , value + lowByteIndex, highByteIndex - lowByteIndex + 1);
	uint16_t result;

}