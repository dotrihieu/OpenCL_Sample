#include <stdio.h>
#include "Device_OpenCL.h"
#include "TextureFile.h"
#include "Common.h"
#include "BasicTimer.h"

BasicTimer *m_BasicTimer;
std::vector<int> m_myArrayInt;
std::vector<int> m_myArrayIntDest;
float m_time_print;

void createData(std::vector<int> &source, int total)
{
	float starttime = m_BasicTimer->GetCurrentTimeInSecond();
	source.resize(total);
	srand(1);
	for (int i = 0; i < total; i++)
	{
		source[i] = 7919;// rand() % total;
	}
	float delta = m_BasicTimer->GetCurrentTimeInSecond() - starttime;
	printf("Create Data finished in :%f second\n", delta);
}

void main()
{
	/*
	m_BasicTimer = new BasicTimer();
  Device_OpenCL::Init();
  char* src;
  int length;
  ReadFileToMemory("HelloWorld.cl", "rb", &src, &length);
  cl_program prg = Device_OpenCL::CreateAndBuildProgramFromSrc(src);
  delete[] src;
  cl_kernel func = Device_OpenCL::CreateKernel(prg, "helloworld");
	std::vector<int> input;
	int NUM_DATA = 1000000;
	createData(std::ref(input), NUM_DATA);

  cl_mem inputBuffer = Device_OpenCL::CreateBuffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, (NUM_DATA) * sizeof(int), (void *)&(input[0]));
  cl_mem outputBuffer = Device_OpenCL::CreateBuffer(CL_MEM_WRITE_ONLY, (NUM_DATA) * sizeof(char), NULL);

  clSetKernelArg(func, 0, sizeof(cl_mem), (void *)&inputBuffer);
  clSetKernelArg(func, 1, sizeof(cl_mem), (void *)&outputBuffer);

  size_t global_work_size[1] = { NUM_DATA };
	float starttime = m_BasicTimer->GetCurrentTimeInSecond();
  Device_OpenCL::RunKernel(func, global_work_size);

  char *output = new char[NUM_DATA];
  Device_OpenCL::GetDataFromBuffer(outputBuffer, NUM_DATA, output);
	m_time_print = m_BasicTimer->GetCurrentTimeInSecond() - starttime;
	printf("Print Data finished in :%f\n", m_time_print);
  Device_OpenCL::Release();
	delete[] output;
	*/
  //Decode ASTC 6x6 linear RGB

  TextureFile *tex = new TextureFile("8x8Checkerboard.pvr");
	tex->DecompressToTGA("output.tga");
  delete tex;
	
  getchar();
}