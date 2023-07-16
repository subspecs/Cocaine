#include "../include/platform.h"
#include "../include/os_methods.h"
#include "../include/gpu_methods.h"
#include "../include/api_methods.h"

#include <math.h>

//Internal Methods ==============================================================================================


//Public Methods =================================================================================================

//Allocate a buffer on GPU memory and write in it on the currently active GPU context on the calling thread.
GPUBuffer AllocateGPUBuffer(GPUBufferTypes BufferType, int BufferID, void* ByteBuffer, long long ByteCount)
{
	GPUBuffer Buffer; 
	glGenBuffers(1, &Buffer.Buffer); //Generate empty buffer.
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Buffer.Buffer); //Bind empty buffer to current GPU context.
	switch(BufferType)
	{
		case NoReadWrite: { glBufferData(GL_SHADER_STORAGE_BUFFER, ByteCount, ByteBuffer, GL_STREAM_READ); } break; //Bufer is intended to be neither read nor written to outside of the shader program.
		case Read: { glBufferData(GL_SHADER_STORAGE_BUFFER, ByteCount, ByteBuffer, GL_STATIC_READ); } break; //Buffer is read-only and when read reads directly from GPU memory.
		case FastRead: { glBufferData(GL_SHADER_STORAGE_BUFFER, ByteCount, ByteBuffer, GL_STATIC_COPY); } break; //Buffer is read-only and the buffer contents are stored within a copy buffer in RAM, making reading fast but consumes 'ByteCount' bytes of extra RAM to create.
		case ReadWrite: { glBufferData(GL_SHADER_STORAGE_BUFFER, ByteCount, ByteBuffer, GL_DYNAMIC_DRAW); } break; //Buffer is in read-write mode, you can read-write at leasure, but note it will read-write to GPU memory directly so it might be slower.
	}
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, BufferID, Buffer.Buffer); //Bind the buffer index for the shader.
	Buffer.BufferID = BufferID; CheckLogError(true, NULL, "AllocateGPUBuffer"); return Buffer; //Save buffer id and return buffer.
}
//Deallocate a GPU buffer from GPU memory and optionally deletes the ByteBuffer on the currently active GPU context on the calling thread.
void DeallocateGPUBuffer(GPUBuffer* GPUBuffer)
{
	glDeleteBuffers(1, &GPUBuffer->Buffer); //Delete allocated buffer on the GPU side.
	CheckLogError(true, NULL, "DeallocateGPUBuffer");
}

//Read bytes from a GPU buffer on the currently active GPU context on the calling thread. (WARNING: If the buffer was NOT made on the current GPU, it WILL re-create/bind it there.)
void ReadFromGPUBuffer(GPUBuffer* GPUBuffer, void* Buffer, long long GPUBufferOffset,  long long Count)
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, GPUBuffer->Buffer); //Bind empty buffer to current GPU context.
#if !defined(Cocaine_GLES)
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, (GLintptr)GPUBufferOffset, (GLsizeiptr)Count, Buffer);
#else
	void* tmp = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, GPUBufferOffset, Count, GL_MAP_READ_BIT);
	BlockCopy(tmp, 0, Buffer, 0, Count);
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
#endif
	CheckLogError(true, NULL, "ReadFromGPUBuffer");
}
//Write bytes to a GPU buffer on the currently active GPU context on the calling thread. (WARNING: If the buffer was NOT made on the current GPU, it WILL re-create/bind it there.)
void WriteToGPUBuffer(GPUBuffer* GPUBuffer, void* Buffer, long long GPUBufferOffset,  long long Count)
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, GPUBuffer->Buffer); //Bind empty buffer to current GPU context.
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, (GLintptr)GPUBufferOffset, (GLsizeiptr)Count, Buffer);
	CheckLogError(true, NULL, "WriteToGPUBuffer");
}

//Compiles shader and returns compile status and error message if any. NOTE: Only works in an active context.
bool CompileProgram(const char* const* ShaderCode, GLuint* OutProgram)
{
	GLuint Shader = glCreateShader(GL_COMPUTE_SHADER); //Create an empty shader.
	glShaderSource(Shader, 1, ShaderCode, NULL); glCompileShader(Shader); //Bind the shader code to the shader object and compile it.
	bool IsCompiled = false; glGetShaderiv(Shader, GL_COMPILE_STATUS, &IsCompiled); //Retreive the status of the shaders compilation.
	if(!IsCompiled)
	{
		int ShaderErrorMessageLength = 0; glGetShaderiv(Shader, GL_INFO_LOG_LENGTH, &ShaderErrorMessageLength); //Retreive the error messages lenth in bytes.
		if(ShaderErrorMessageLength > 0)
		{
			char* ShaderErrorMessage = malloc(sizeof(char) * ShaderErrorMessageLength); glGetShaderInfoLog(Shader, ShaderErrorMessageLength, NULL, ShaderErrorMessage); //Allocated a char* for specified error message and retreive it.
			if(OnShaderCompileError != NULL)
			{
				OnShaderCompileError(ShaderErrorMessage);
			}
#if defined(DEBUG)
			printf("Shader Error: %s\n", ShaderErrorMessage); 
#endif
			free(ShaderErrorMessage);
		}
		glDeleteShader(Shader); return IsCompiled; //Delete the failed shader and return compile status and error message. (Don't forget to DELETE error message afterwards.)
	}
	*OutProgram = glCreateProgram();
	glAttachShader(*OutProgram, Shader);
	glDeleteShader(Shader);
	CheckLogError(true, NULL, "CompileProgram"); //Check for errors.
	return IsCompiled; //Return compile status.
}
//Load a ComputeProgram from memory.
bool LoadComputeProgram(unsigned char* Buffer, int Count, unsigned int* Program)
{
	if(*Buffer == 1)
	{
		*Program = glCreateProgram();
		glProgramBinary(*Program, (GLenum)*(unsigned int*)(Buffer + 1), Buffer + 5, Count - 5);
		CheckLogError(true, NULL, "LoadComputeProgram");
		return true;
	}
	else
	{
		GLuint Shader;
		if(!CompileProgram((const char**)(Buffer + 1), Program))
		{
			//ERROR.
			return false;
		}
		CheckLogError(true, NULL, "LoadComputeProgram");
		return true;
	}
}
//Save a ComputeProgram to memory.
int SaveComputeProgram(unsigned int Program, unsigned char* Buffer)
{
	if(GL_NUM_PROGRAM_BINARY_FORMATS > 0)
	{
		int ByteCount = 0; unsigned int BinaryFormat = 0; *Buffer = 1;
		glGetProgramBinary(Program, GL_PROGRAM_BINARY_LENGTH, &ByteCount, &BinaryFormat, Buffer + 5);
		*(int*)(Buffer + 1) = BinaryFormat; CheckLogError(true, NULL, "SaveComputeProgram"); return ByteCount + 5;
	}
	else
	{
		int Counter = 0; GLuint Shader; GLchar Source[MaxProgramShaderSourceSize]; *Buffer = 0;
		glGetAttachedShaders(Program, 1, &Counter, &Shader);
		glGetShaderSource(Shader, MaxProgramShaderSourceSize, &Counter, Source);
		BlockCopy(Source, 0, Buffer, 1, Counter); CheckLogError(true, NULL, "SaveComputeProgram"); return Counter + 1;
	}
}

//Gets the condensed GPU device limitations.
int* GetGPUDeviceLimits(GPUDevice* Device)
{
	return Device->GPUDeviceLimits;
}
//Binds the current GPU Device to the calling thread and makes the GPU context active on calling thread.
void SetActiveGPUContext(GPUDevice* Device)
{
	glfwMakeContextCurrent(NULL); //Set the calling threads GPU context to NULL.
	glfwMakeContextCurrent(Device->GPUContext); //Make this GPU the current threads GPU context.
	CheckLogError(true, NULL, "SetActiveGPUContext"); //Error Check.
}
//Will run the currently active Compute Program on the CURRENTLY ACTIVE GPU context on the CALLING THREAD.
bool RunComputeProgram(unsigned int Program, int* GPUDeviceLimits, long long ProcessCount, bool PreciseCycleCount)
{
	glLinkProgram(Program); //Link shader program to the current GPU context.
	glUseProgram(Program); //Register the shader program for use with the current GPU context.
	CheckLogError(true, NULL, "RunComputeProgram"); //Error Check.

	bool AllGood = true; //A simple check if all is good, no errors etc.
	long long JobCountCBRT = ceil(cbrt(ProcessCount)); //The the cube root of ProcessCount.
	if(GPUDeviceLimits[0] < ProcessCount && !PreciseCycleCount && JobCountCBRT > 1) 
	{ 
		if(GPUDeviceLimits[0] >= JobCountCBRT && GPUDeviceLimits[1] >= JobCountCBRT && GPUDeviceLimits[2] >= JobCountCBRT) 
		{ 
			glDispatchCompute(JobCountCBRT, JobCountCBRT, JobCountCBRT); //Execute work.
		}
		else { AllGood = false; CheckLogError(true, "Work Group Count higher than supported! (WorkGroupPerXYZCount = cuberoot(ProcessCount))", "RunComputeProgram"); }
	} 
	else if(GPUDeviceLimits[0] >= ProcessCount) { glDispatchCompute(ProcessCount, 1, 1); } else { CheckLogError(true, "WorkGroupX < ProcessCount", "RunComputeProgram"); } //Execute work.
	CheckLogError(true, NULL, "RunComputeProgram"); //Error Check.

	int LogSize = 0; glGetProgramiv(Program, GL_INFO_LOG_LENGTH, &LogSize); 
	if(LogSize > 0)
	{
		char* LogMessage = malloc(sizeof(char) * LogSize);
		glGetProgramInfoLog(Program, MaxProgramShaderSourceSize, &LogSize, LogMessage);
		if(OnProgramRunError != NULL)
		{
			OnProgramRunError(LogMessage);
		}
#if defined(DEBUG)
		printf("Compute Program: %s\n", LogMessage);
#endif
		free(LogMessage);
	}
	
	glMemoryBarrier(GL_ALL_BARRIER_BITS); //Wait for GPU to complete task;
	glfwWaitEvents(); //Wait for window events to process, otherwise OS will think it's dead weight.
	CheckLogError(true, NULL, "RunComputeProgram"); //Error Check.
	return AllGood;
}
