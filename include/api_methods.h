#pragma once
#include "os_methods.h"
#include "gpu_methods.h"

//Enums =========================================================================================================
typedef enum GPUBufferTypes { NoReadWrite, Read, FastRead, ReadWrite } GPUBufferTypes;


//Structs =======================================================================================================
typedef struct GPUBuffer { unsigned int Buffer; int BufferID; } GPUBuffer;


//Public Methods =================================================================================================
#ifdef __cplusplus
extern "C" {
#endif

//Allocate a buffer on GPU memory and write in it.
GPUBuffer AllocateGPUBuffer(GPUBufferTypes BufferType, int BufferID, void* ByteBuffer, long long ByteCount);
//Deallocate a GPU buffer from GPU memory and optionally deletes the ByteBuffer.
void DeallocateGPUBuffer(GPUBuffer* GPUBuffer);

//Read bytes from a GPU buffer on the currently active GPU context on the calling thread. (WARNING: If the buffer was NOT made on the current GPU, it WILL re-create/bind it there.)
void ReadFromGPUBuffer(GPUBuffer* GPUBuffer, void* Buffer, long long GPUBufferOffset,  long long Count);
//Write bytes to a GPU buffer on the currently active GPU context on the calling thread. (WARNING: If the buffer was NOT made on the current GPU, it WILL re-create/bind it there.)
void WriteToGPUBuffer(GPUBuffer* GPUBuffer, void* Buffer, long long GPUBufferOffset,  long long Count);

//Compiles shader. NOTE: Only works in an active context.
bool CompileProgram(const char* const* ShaderCode, unsigned int* OutProgram);
//Load a ComputeProgram from memory.
bool LoadComputeProgram(unsigned char* Buffer, int Count, unsigned int* Program);
//Save a ComputeProgram to memory.
int SaveComputeProgram(unsigned int Program, unsigned char* Buffer);

//Binds the current GPU Device to the calling thread and makes the GPU context active on calling thread.
void SetActiveGPUContext(GPUDevice* Device);
//Will run the currently active Compute Program on the CURRENTLY ACTIVE GPU context on the CALLING THREAD.
bool RunComputeProgram(unsigned int Program, int* GPUDeviceLimits, long long ProcessCount, bool PreciseCycleCount);

#ifdef __cplusplus
}
#endif
