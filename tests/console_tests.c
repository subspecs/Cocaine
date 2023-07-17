#include "../include/api_methods.h"
#include "../include/gpu_methods.h"
#include "../include/os_methods.h"

const char* ShaderCode = //Sample shader code for work.
"#version 430\n"
"layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;\n"
"layout(std430, binding = 0) buffer TestName1 { float BufferA[]; };\n"
"layout(std430, binding = 1) buffer TestName2 { float BufferB[]; };\n"
"layout(std430, binding = 2) buffer TestName3 { float BufferC[]; };\n"

"unsigned int JobIndex = (gl_GlobalInvocationID.x * (gl_NumWorkGroups.y * gl_NumWorkGroups.z)) + (gl_GlobalInvocationID.y * gl_NumWorkGroups.z) + gl_GlobalInvocationID.z;\n"

"void main()\n"
"{\n"
//"	BufferC[JobIndex] = BufferA[JobIndex] + BufferB[JobIndex];\n"
"}\n";

long long TestSize = 550000000; //The amount of variables to create.
long long TestSizeBufferByteCount; //The amount of bytes these variable will consume.

void SetBufferValues(float* Buffer, int Length, float Value) { int n = 0; while(n < Length) { Buffer[n] = Value; n++; } }

int main()
{
	Initialize(); //Create a line(Initialize library) of Cocaine;

	GPUDevice* ComputeDevices = NULL;
	int DeviceCount = GetRawGPUDevices(&ComputeDevices); //Get the GPUs and their count that Cocaine has detected.

	if(CreateGPUContext(&ComputeDevices[0])) //Take the first GPU and create a context for it.
	{
		SetActiveGPUContext(&ComputeDevices[0]); //Make the current GPU context active on the calling thread.

		int i = 0;
		while(i < GPUCount)
		{
			printf("GPU[%d]: %s\n", i, ComputeDevices[i].DisplayName); //Print out our GPU names because why not?
			i++;
		}

		TestSizeBufferByteCount = TestSize * sizeof(float); //Calculate the byte count our variables are going to consume. (Both on RAM and GPU VRAM)

		printf("Creating Bufffers... "); long long Start1 = GetCurrentTimestamp();
		void* MemoryA = malloc(TestSizeBufferByteCount); //Create the A empty memory buffer of where we write values to be calculated.
		void* MemoryB = malloc(TestSizeBufferByteCount); //Create the B empty memory buffer of where we write values to be calculated.
		void* MemoryC = malloc(TestSizeBufferByteCount); //Create the C empty memory buffer of where we save values that are calculated.
		long long End1 = GetCurrentTimestamp(); printf("%lldms\n", GetTimestampMilliseconds(End1 - Start1));

		printf("Filling Bufffers... "); long long Start2 = GetCurrentTimestamp();
		SetBufferValues((float*)MemoryA, TestSize, 34.5f); //Generate numbers to calculate for the A memory buffer to calculate.
		SetBufferValues((float*)MemoryB, TestSize, 34.5f); //Generate numbers to calculate for the B memory buffer to calculate.
		long long End2 = GetCurrentTimestamp(); printf("%lldms\n", GetTimestampMilliseconds(End2 - Start2));

		printf("Send buffers to gpu... "); long long Start3 = GetCurrentTimestamp();
		GPUBuffer BufferA = AllocateGPUBuffer(NoReadWrite, 0, MemoryA, TestSizeBufferByteCount); //Create a GPU buffer and write our memory buffer A contents to it.
		GPUBuffer BufferB = AllocateGPUBuffer(NoReadWrite, 1, MemoryB, TestSizeBufferByteCount); //Create a GPU buffer and write our memory buffer B contents to it.
		GPUBuffer BufferC = AllocateGPUBuffer(FastRead, 2, NULL, TestSizeBufferByteCount); //Create a GPU buffer and and leave the buffer input NULL, this signals the GPU that you just want to create an empty GPU buffer on it. (Saves time)
		long long End3 = GetCurrentTimestamp(); printf("%lldms\n", GetTimestampMilliseconds(End3 - Start3));

		bool IsPepsi = false; GLuint Program;
		if (!CompileProgram(&ShaderCode, &Program)) //Compiles shader code that a top this file.
		{
			IsPepsi = true; //If compilation failed. (There are error/method callbacks that you'll be able to use, too lazy to write here.)
		}

		if(!IsPepsi) //In-case the compilation didn't fail.
		{
			printf("Running programm #1... "); long long Start4 = GetCurrentTimestamp();
			RunComputeProgram(Program, GPUDevices[0].GPUDeviceLimits, TestSize, false); //We run our compiled program on the CURRENTLY ACTIVE GPU on the CURRENTLY ACTIVE THREAD. ex: If you run a program on a GPU context that's owned by thread B ON thread A(which owns a different context per say), you'll have a bad bad time.
			long long End4 = GetCurrentTimestamp(); printf("%lldms\n", GetTimestampMilliseconds(End4 - Start4));

			printf("Running programm #2... "); long long Start5 = GetCurrentTimestamp();
			RunComputeProgram(Program, GPUDevices[0].GPUDeviceLimits, TestSize, false); //We run our compiled program on the CURRENTLY ACTIVE GPU on the CURRENTLY ACTIVE THREAD. ex: If you run a program on a GPU context that's owned by thread B ON thread A(which owns a different context per say), you'll have a bad bad time.
			long long End5 = GetCurrentTimestamp(); printf("%lldns\n", GetTimestampNanoseconds(End5 - Start5));

			printf("Reading GPU into RAM memory#1... "); long long Start6 = GetCurrentTimestamp();
			ReadFromGPUBuffer(&BufferC, MemoryC, 0, TestSizeBufferByteCount); //When done, we'd like to read the results from GPU buffer back to our memory buffer. (BufferC => MemoryC)
			long long End6 = GetCurrentTimestamp(); printf("%lldms\n", GetTimestampMilliseconds(End6 - Start6));

			printf("Reading GPU into RAM memory#2... "); long long Start8 = GetCurrentTimestamp();
			ReadFromGPUBuffer(&BufferC, MemoryC, 0, TestSizeBufferByteCount); //When done, we'd like to read the results from GPU buffer back to our memory buffer. (BufferC => MemoryC)
			long long End8 = GetCurrentTimestamp(); printf("%lldms\n", GetTimestampMilliseconds(End8 - Start8));

			long long Start7 = GetCurrentTimestamp();
			int n = TestSize - 10;
			while(n < TestSize)
			{
				printf("Output[%d]: %f\n", n, ((float*)MemoryC)[n]); //For kicks we print out the result of the last 10 jobs completed.
				n++;
			}
			printf("Printing Data... "); long long End7 = GetCurrentTimestamp(); printf("%lldms\n", GetTimestampMilliseconds(End7 - Start7));
		}
	}
	return 0;
}
