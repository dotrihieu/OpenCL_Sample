#pragma once
#include <stdint.h>
#pragma pack(push, 1)
struct PVRHeader
{
  uint32_t version;
  uint32_t flags;
  uint64_t pixelFormat;
  uint32_t colorSpace;
  uint32_t channelType;
  uint32_t height;
  uint32_t width;
  uint32_t depth;
  uint32_t numberOfSurfaces;
  uint32_t numberOfFaces;
  uint32_t mipmapCount;
  uint32_t metaDataSize;
};
#pragma pack(pop)