#include "../include/platform.h"
#include "../include/os_methods.h"

//Variables(Private) ============================================================================================
#if defined(Cocaine_Win_64) || defined(Cocaine_Win_32)
void InitTime();
LARGE_INTEGER FPC; //Frequency Performance Counter.
bool IsTimeMethodsInitialized;
double RawTimeFrequency, MiliTimeFrequency, MicroTimeFrequency, NanoTimeFrequency;
#endif

//Internal Methods ==============================================================================================
#if defined(Cocaine_Win_64) || defined(Cocaine_Win_32)
//Timing Methods.
void InitTime() { if(!IsTimeMethodsInitialized) { if (QueryPerformanceFrequency(&FPC)) { RawTimeFrequency = FPC.QuadPart; MiliTimeFrequency = FPC.QuadPart / 1000.0; MicroTimeFrequency = FPC.QuadPart / 1000000.0; NanoTimeFrequency = FPC.QuadPart / 1000000000.0; } else { RawTimeFrequency = 0; MiliTimeFrequency = 0; MicroTimeFrequency = 0; NanoTimeFrequency = 0; }; IsTimeMethodsInitialized = true; } }
long long GetCurrentTimestamp() { LARGE_INTEGER Counter; QueryPerformanceCounter(&Counter); return Counter.QuadPart; }
long long GetTimestampSeconds(long long Timestamp) { return (long long)((double)Timestamp / RawTimeFrequency); }
long long GetTimestampMiliseconds(long long Timestamp) { return (long long)((double)Timestamp / MiliTimeFrequency); }
long long GetTimestampMicroseconds(long long Timestamp) { return (long long)((double)Timestamp / MicroTimeFrequency); }
long long GetTimestampNanoseconds(long long Timestamp) { return (long long)((double)Timestamp / NanoTimeFrequency); }

//Window Methods.
void Win_HideWindowInTaskbar(void* win) { glfwHideWindow(win); SetWindowLong(glfwGetWin32Window(win), GWL_EXSTYLE, WS_EX_TOOLWINDOW); glfwShowWindow(win); }
#else
//Timing Methods.
#include <time.h>
void InitTime() { }
long long GetCurrentTimestamp() { struct timespec Counter; clock_gettime(CLOCK_REALTIME, &Counter); return (Counter.tv_sec * 1000000000LL) + Counter.tv_nsec; }
long long GetTimestampSeconds(long long Timestamp) { return (long long)((double)Timestamp / 1000000000.0); }
long long GetTimestampMiliseconds(long long Timestamp) { return (long long)((double)Timestamp / 1000000.0); }
long long GetTimestampMicroseconds(long long Timestamp) { return (long long)((double)Timestamp / 1000.0); }
long long GetTimestampNanoseconds(long long Timestamp) { return Timestamp; }
#endif
//Gets the current GLFW error, if any.
const char* GetCurrentGLFWErrorText(const char** Desc)
{
	switch(glfwGetError(Desc))
	{
	case GLFW_NO_ERROR: { return NULL; }
	case GLFW_NOT_INITIALIZED: { return "GLFW_NOT_INITIALIZED"; } //This occurs if a GLFW function was called that must not be called unless the library is initialized.
	case GLFW_NO_CURRENT_CONTEXT: { return "GLFW_NO_CURRENT_CONTEXT"; } //This occurs if a GLFW function was called that needs and operates on the current OpenGL or OpenGL ES context but no context is current on the calling thread. One such function is glfwSwapInterval.
	case GLFW_INVALID_ENUM: { return "GLFW_INVALID_ENUM"; } //One of the arguments to the function was an invalid enum value, for example requesting GLFW_RED_BITS with glfwGetWindowAttrib.
	case GLFW_INVALID_VALUE: { return "GLFW_INVALID_VALUE"; } //One of the arguments to the function was an invalid value, for example requesting a non-existent OpenGL or OpenGL ES version like 2.7. Requesting a valid but unavailable OpenGL or OpenGL ES version will instead result in a GLFW_VERSION_UNAVAILABLE error.
	case GLFW_OUT_OF_MEMORY: { return "GLFW_OUT_OF_MEMORY"; } //A memory allocation failed.
	case GLFW_API_UNAVAILABLE: { return "GLFW_API_UNAVAILABLE"; } //GLFW could not find support for the requested API on the system.
	case GLFW_VERSION_UNAVAILABLE: { return "GLFW_VERSION_UNAVAILABLE"; } //The requested OpenGL or OpenGL ES version (including any requested context or framebuffer hints) is not available on this machine.
	case GLFW_PLATFORM_ERROR: { return "GLFW_PLATFORM_ERROR"; } //A platform-specific error occurred that does not match any of the more specific categories.
	case GLFW_FORMAT_UNAVAILABLE: { return "GLFW_FORMAT_UNAVAILABLE"; } //If emitted during window creation, the requested pixel format is not supported. If emitted when querying the clipboard, the contents of the clipboard could not be converted to the requested format.
	case GLFW_NO_WINDOW_CONTEXT: { return "GLFW_NO_WINDOW_CONTEXT"; } //A window that does not have an OpenGL or OpenGL ES context was passed to a function that requires it to have one.
	default: { return "GLFW_UNKNOWN_ERROR_CODE"; }
	}
	
}

//Public Methods ================================================================================================

//Registers the method callback to use when an internal GLFW error occurs.
void RegisterOnGLFWErrorMethod(void* Method)
{
	OnGLFWError = (void (*)(const char*,const char*,const char*))Method;
}
//Registers the method callback to use when an Compute Program error occurs.
void RegisterOnProgramRunErrorMethod(void* Method)
{
	OnProgramRunError = (void (*)(const char*))Method;
}
//Registers the method callback to use when an shader compilation error occurs.
void RegisterOnShaderCompileErrorMethod(void* Method)
{
	OnShaderCompileError = (void (*)(const char*))Method;
}

//Reads and logs errors in memory for later reading, optionally prints them.
void CheckLogError(bool PrintError, const char* AppendCustomError, const char* FunctionName)
{
	const char* ErrorDesc = NULL;
	const char* GLError = GetCurrentGLFWErrorText(&ErrorDesc);
	if(GLError != NULL) 
	{
		while(GLError != NULL)
		{
#if defined(DEBUG)
			if(PrintError) { printf("GLFW Error in '%s' - %s: %s\n", FunctionName == NULL ? "" : FunctionName, GLError, ErrorDesc); }
#endif
			if(OnGLFWError != NULL) { OnGLFWError(GLError, FunctionName, ErrorDesc); }
			GLError = GetCurrentGLFWErrorText(&ErrorDesc);
		}
	}
	if (AppendCustomError != NULL) 
	{ 
#if defined(DEBUG)
		if(PrintError) { printf("Error '%s': %s\n", FunctionName == NULL ? "" : FunctionName, AppendCustomError); }
#endif
		if (OnGLFWError != NULL) { OnGLFWError("Custom", FunctionName, AppendCustomError); } 
	}
}
//Perform a byte block copy from Src to Dest.
void BlockCopy(void* Src, long long SrcOffset, void* Dest, long long DestOffset, long long Count)
{
	long long Cycles = Count / sizeof(Bit256Block), ExtraOffset = Cycles * sizeof(Bit256Block), Extra = Count - ExtraOffset, n = 0; //Calculate cycles.
	Bit256Block* SrcCycleBlocks = (Bit256Block*)(Src + SrcOffset); Bit256Block* DestCycleBlocks = (Bit256Block*)(Dest + DestOffset); //Cast both to blocks.
	while(n < Cycles) { DestCycleBlocks[n] = SrcCycleBlocks[n]; n++; }; 
	if(Extra > 0) { unsigned char* S = (unsigned char*)(Src + SrcOffset + ExtraOffset); unsigned char* D = (unsigned char*)(Dest + DestOffset + ExtraOffset); n = 0; while(n < Extra) { D[n] = S[n]; n++; } }
}
