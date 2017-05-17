#pragma once
#include <stdint.h>

extern bool ReadFileToMemory(const char* fileName, const char* readMode, char** buffer, int* length);
extern uint8_t GetBitLittle(uint8_t value, int index);