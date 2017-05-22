#pragma once
#include <stdint.h>

extern bool ReadFileToMemory(const char* fileName, const char* readMode, char** buffer, int* length);
extern uint8_t GetBitLittleFromByte(const uint8_t &value, const int &index);
extern uint8_t GetBitLittleFromByteArray(const uint8_t *value, const int &index);
extern uint16_t Get16BitLittleFromByteArray(const uint8_t *value, const int &lowIndex, const int &highIndex);
extern uint8_t Get8BitLittleFromByteArray(const uint8_t *value, const int &lowIndex, const int &highIndex);