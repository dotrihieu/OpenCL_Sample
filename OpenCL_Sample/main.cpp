#include <stdio.h>
#include "Device_OpenCL.h"

void ReadFileToMemory(const char* fileName, const char* readMode, char** buffer, int* length)
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
  }
  else
  {
    printf("File %s not found", fileName);
  }
}
void main()
{
  Device_OpenCL::Init();
  char* src;
  int length;
  ReadFileToMemory("HelloWorld.cl", "rb", &src, &length);
  cl_program prg = Device_OpenCL::CreateAndBuildProgramFromSrc(src);
  cl_kernel func = Device_OpenCL::CreateKernel(prg, "helloworld");
  char input[1024];
  for (int i = 0; i < 1024; i++)
  {
    input[i] = i;
  }
  cl_mem inputBuffer = Device_OpenCL::CreateBuffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, (1024) * sizeof(char), (void *)input);
  cl_mem outputBuffer = Device_OpenCL::CreateBuffer(CL_MEM_WRITE_ONLY, (1024) * sizeof(char), NULL);

  clSetKernelArg(func, 0, sizeof(cl_mem), (void *)&inputBuffer);
  clSetKernelArg(func, 1, sizeof(cl_mem), (void *)&outputBuffer);

  size_t global_work_size[1] = { 1024 };
  Device_OpenCL::RunKernel(func, global_work_size);

  char output[1024];
  Device_OpenCL::GetDataFromBuffer(outputBuffer, 1024, output);

  Device_OpenCL::Release();
}