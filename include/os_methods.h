#pragma once
#if defined(CocaineCompile)
#include "platform.h"
#endif

//Very helpful defines.
#define false 0
#define true 1
typedef int bool;
typedef unsigned char byte;

//Work Constants.
#define MaxProgramShaderSourceSize 512 * 1024 //512kb.

//Function Pointers =============================================================================================
void (*OnShaderCompileError)(const char* Error);
void (*OnProgramRunError)(const char* ErrorDesc);
void (*OnGLFWError)(const char* ErrorName, const char* FunctionName, const char* ErrorDesc);


//Structs =======================================================================================================
typedef struct Bit256Block { int i1, i2, i3, i4, i5, i6, i7, i8; } Bit256Block;


//Internal Methods ==============================================================================================
void InitTime();
void Win_HideWindowInTaskbar(void* win);
void CheckLogError(bool PrintError, const char* AppendCustomError, const char* FunctionName);
void BlockCopy(void* Src, long long SrcOffset, void* Dest, long long DestOffset, long long Count);

//Public Methods ================================================================================================

#ifdef __cplusplus
extern "C" {
#endif

//Gets current date/time timestamp.
long long GetCurrentTimestamp();
//Get the total amount of Seconds in given timestamp.
long long GetTimestampSeconds(long long Timestamp);
//Get the total amount of Miliseconds in given timestamp.
long long GetTimestampMilliseconds(long long Timestamp);
//Get the total amount of Microseconds in given timestamp.
long long GetTimestampMicroseconds(long long Timestamp);
//Get the total amount of Nanoseconds in given timestamp.
long long GetTimestampNanoseconds(long long Timestamp);

//Registers the method callback to use when an internal GLFW error occurs.
void RegisterOnGLFWErrorMethod(void* Method);
//Registers the method callback to use when an Compute Program error occurs.
void RegisterOnProgramRunErrorMethod(void* Method);
//Registers the method callback to use when an shader compilation error occurs.
void RegisterOnShaderCompileErrorMethod(void* Method);


#ifdef __cplusplus
}
#endif
