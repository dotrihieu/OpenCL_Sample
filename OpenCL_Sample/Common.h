#pragma once
#include <stdint.h>

extern bool ReadFileToMemory(const char* fileName, const char* readMode, char** buffer, int* length);
extern uint8_t GetBitLittle(const uint8_t &value, const int &index);