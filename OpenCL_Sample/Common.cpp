#include <stdio.h>
#include <stdint.h>

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

uint8_t GetBitLittle(const uint8_t &value, const int &index)
{
	return (value >> index) & 0x1;
}
