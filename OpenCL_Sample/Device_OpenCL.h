#pragma once
#include "CL/cl.h"

class Device_OpenCL
{
  static Device_OpenCL* instance;
  cl_platform_id platform;
public:
  static Device_OpenCL* GetInstance()
  {
    return instance;
  }
  static bool Init();
};