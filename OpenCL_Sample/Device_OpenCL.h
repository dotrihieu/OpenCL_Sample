#pragma once
#include "CL/cl.h"
#include <vector>

class Device_OpenCL
{
  static Device_OpenCL* instance;
  cl_platform_id platformID;
  cl_device_id deviceID;
  cl_context context;
  cl_command_queue queue;
  std::vector<cl_program> programList;
  std::vector<cl_kernel> kernelList;
  std::vector<cl_mem> bufferList;
public:
  ~Device_OpenCL();
  static Device_OpenCL* GetInstance()
  {
    return instance;
  }
  static bool Init();
  static void Release();
  static cl_program CreateAndBuildProgramFromSrc(const char* src);
  static cl_kernel CreateKernel(cl_program program, const char* functionName);
  static cl_mem CreateBuffer(cl_mem_flags flags, size_t size, void *initData);
  static void RunKernel(cl_kernel kernel, const size_t *global_work_size);
  static void GetDataFromBuffer(cl_mem bufferCL, size_t size, void* buffer);
};