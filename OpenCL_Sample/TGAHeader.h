#pragma once
#include <stdint.h>
#pragma pack(push, 1)
struct TGAHeader {
  uint8_t  idlength;
  uint8_t  colourmaptype;
  uint8_t  datatypecode;
  uint16_t colourmaporigin;
  uint16_t colourmaplength;
  uint8_t  colourmapdepth;
  uint16_t x_origin;
  uint16_t y_origin;
  uint16_t width;
  uint16_t height;
  uint8_t  bitsperpixel;
  uint8_t  imagedescriptor;
};
#pragma pack(pop)