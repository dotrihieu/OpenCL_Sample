#include "Device_OpenCL.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>

Device_OpenCL* Device_OpenCL::instance = NULL;

bool Device_OpenCL::Init()
{
  if (instance)
  {
    return true;
  }
  cl_int result;
  cl_uint numberOfPlatform;
  result = clGetPlatformIDs(0, NULL, &numberOfPlatform);
  if (result != CL_SUCCESS)
  {
    return false;
  }
  cl_platform_id* platformList = (cl_platform_id*)malloc(numberOfPlatform * sizeof(cl_platform_id));
  result = clGetPlatformIDs(numberOfPlatform, platformList, NULL);
  if (result != CL_SUCCESS)
  {
    return false;
  }
  for (int i = 0; i < numberOfPlatform; i++)
  {
    char vendor[128];
    result = clGetPlatformInfo(platformList[i], CL_PLATFORM_VENDOR, sizeof(vendor), vendor, NULL);
    if (result != CL_SUCCESS)
    {
      sprintf(vendor, "Unknown");
    }
    printf("Vendor: %s\n", vendor);

    char platformName[128];
    result = clGetPlatformInfo(platformList[i], CL_PLATFORM_NAME, sizeof(platformName), platformName, NULL);
    if (result != CL_SUCCESS)
    {
      sprintf(platformName, "Unknown");
    }
    printf("Platform name: %s\n", platformName);

    char platformProfile[128];
    result = clGetPlatformInfo(platformList[i], CL_PLATFORM_PROFILE, sizeof(platformProfile), platformProfile, NULL);
    if (result != CL_SUCCESS)
    {
      sprintf(platformProfile, "Unknown");
    }
    printf("Platform profile: %s\n", platformProfile);

    char platformVersion[128];
    result = clGetPlatformInfo(platformList[i], CL_PLATFORM_VERSION, sizeof(platformVersion), platformVersion, NULL);
    if (result != CL_SUCCESS)
    {
      sprintf(platformVersion, "Unknown");
    }
    printf("Platform version: %s\n", platformVersion);

    char platformExtension[512];
    result = clGetPlatformInfo(platformList[i], CL_PLATFORM_EXTENSIONS, sizeof(platformExtension), platformExtension, NULL);
    if (result != CL_SUCCESS)
    {
      sprintf(platformExtension, "Unknown");
    }
    printf("Platform extension: %s\n", platformExtension);

    cl_uint numberOfDevice;
    result = clGetDeviceIDs(platformList[i], CL_DEVICE_TYPE_GPU, NULL, NULL, &numberOfDevice);
    if (result != CL_SUCCESS)
    {
      printf("Platform's devices not found\n");
      continue;
    }
    cl_device_id* deviceList = (cl_device_id*)malloc(numberOfDevice * sizeof(cl_device_id));
    result = clGetDeviceIDs(platformList[i], CL_DEVICE_TYPE_GPU, numberOfDevice, deviceList, NULL);
    if (result != CL_SUCCESS)
    {
      printf("Can't get device ID\n");
      continue;
    }
    if (instance == NULL && numberOfDevice > 0 && strstr(vendor, "Advanced Micro Devices") != NULL)
    {
      instance = new Device_OpenCL();
      instance->platformID = platformList[i];
      instance->deviceID = deviceList[0];
    }
    for (int j = 0; j < numberOfDevice; j++)
    {
      char deviceName[128];
      result = clGetDeviceInfo(deviceList[j], CL_DEVICE_NAME, sizeof(deviceName), deviceName, NULL);
      if (result != CL_SUCCESS)
      {
        sprintf(deviceName, "Unknown");
      }
      printf("\tDevice name: %s\n", deviceName);

      char deviceVersion[128];
      result = clGetDeviceInfo(deviceList[j], CL_DEVICE_VERSION, sizeof(deviceVersion), deviceVersion, NULL);
      if (result != CL_SUCCESS)
      {
        sprintf(deviceVersion, "Unknown");
      }
      printf("\tDevice version: %s\n", deviceVersion);

      char driverVersion[128];
      result = clGetDeviceInfo(deviceList[j], CL_DEVICE_VERSION, sizeof(driverVersion), driverVersion, NULL);
      if (result != CL_SUCCESS)
      {
        sprintf(driverVersion, "Unknown");
      }
      printf("\tDriver version: %s\n", driverVersion);
    }
  }
  if (instance == NULL)
  {
    return false;
  }
  instance->context = clCreateContext(NULL, 1, &(instance->deviceID), NULL, NULL, NULL);
  if (instance->context == NULL)
  {
    printf("Create context failed\n");
    return false;
  }
  instance->queue = clCreateCommandQueue(instance->context, instance->deviceID, 0, NULL);
  if (instance->queue == NULL)
  {
    printf("Create command queue failed\n");
    return false;
  }
  printf("OpenCL init successfully ----------------------------\n");
  return true;
}

Device_OpenCL::~Device_OpenCL()
{
  for (int i = 0; i < kernelList.size(); i++)
  {
    clReleaseKernel(kernelList[i]);
  }
  kernelList.clear();

  for (int i = 0; i < programList.size(); i++)
  {
    clReleaseProgram(programList[i]);
  }
  programList.clear();

  for (int i = 0; i < bufferList.size(); i++)
  {
    clReleaseMemObject(bufferList[i]);
  }
  bufferList.clear();

  clReleaseCommandQueue(queue);
  clReleaseContext(context);
}

void Device_OpenCL::Release()
{
  if (instance)
  {
    delete instance;
    instance = NULL;
  }
}

cl_program Device_OpenCL::CreateAndBuildProgramFromSrc(const char* src)
{
  cl_program program = clCreateProgramWithSource(instance->context,
                                                  1,
                                                  &src,
                                                  NULL, NULL);
  clBuildProgram(program, 1, &(instance->deviceID), NULL, NULL, NULL);
  instance->programList.push_back(program);
  return program;
}

cl_kernel Device_OpenCL::CreateKernel(cl_program program, const char* functionName)
{
  cl_kernel kernel = clCreateKernel(program, functionName, NULL);
  instance->kernelList.push_back(kernel);
  return kernel;
}

cl_mem Device_OpenCL::CreateBuffer(cl_mem_flags flags, size_t size, void *initData)
{
  cl_int error;
  cl_mem buffer = clCreateBuffer(instance->context,
                                  flags,
                                  size,
                                  initData, &error);
  if (buffer == NULL)
  {
    printf("Create buffer failed: %x\n", error);
  }
  instance->bufferList.push_back(buffer);
  return buffer;
}

void Device_OpenCL::RunKernel(cl_kernel kernel, const size_t *global_work_size)
{
  cl_int status = clEnqueueNDRangeKernel(instance->queue, kernel, 1, NULL, global_work_size, NULL, 0, NULL, NULL);
}

void Device_OpenCL::GetDataFromBuffer(cl_mem bufferCL, size_t size, void* buffer)
{
  cl_int status = clEnqueueReadBuffer(instance->queue, bufferCL, CL_TRUE, 0, size, buffer, 0, NULL, NULL);
}