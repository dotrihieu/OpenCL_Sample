#pragma once
#include "CL/cl.h"

class Device_OpenCL
{
  static Device_OpenCL* instance;
  cl_platform_id platformID;
  cl_device_id deviceID;
  cl_context context;
  cl_command_queue queue;
public:
  ~Device_OpenCL();
  static Device_OpenCL* GetInstance()
  {
    return instance;
  }
  static bool Init();
  static void Release();
};