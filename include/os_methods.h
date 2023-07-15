#pragma once
#include "platform.h"

//Function Pointers =============================================================================================
void (*OnShaderCompileError)(const char* Error);
void (*OnProgramRunError)(const char* Error);
void (*OnGLFWError)(const char* ErrorName, const char* FunctionName, const char* ErrorDesc);


//Structs =======================================================================================================
typedef struct Bit256Block { int i1, i2, i3, i4, i5, i6, i7, i8; } Bit256Block;


//Variables(Private) ============================================================================================
#if defined(Cocaine_Win_64) || defined(Cocaine_Win_32)
void InitTime();
LARGE_INTEGER FPC; //Frequency Performance Counter.
bool IsTimeMethodsInitialized;
double RawTimeFrequency, MiliTimeFrequency, MicroTimeFrequency, NanoTimeFrequency;
#endif


//Internal Methods ==============================================================================================
void Win_HideWindowInTaskbar(GLFWwindow* win);


//Public Methods ================================================================================================

#ifdef __cplusplus
extern "C" {
#endif

//Gets current date/time timestamp.
long long GetCurrentTimestamp();
//Get the total amount of Seconds in given timestamp.
long long GetTimestampSeconds(long long Timestamp);
//Get the total amount of Miliseconds in given timestamp.
long long GetTimestampMiliseconds(long long Timestamp);
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

//Reads and logs errors in memory for later reading, optionally prints them.
void CheckLogError(bool PrintError, const char* AppendCustomError, const char* FunctionName);
//Perform a byte block copy from Src to Dest.
void BlockCopy(void* Src, long long SrcOffset, void* Dest, long long DestOffset, long long Count);

#ifdef __cplusplus
}
#endif
