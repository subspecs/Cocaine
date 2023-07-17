#pragma once
#include "os_methods.h"

//Structs =======================================================================================================
typedef struct GPUDevice { void* GPUContext; void* GPUMonitor; char* DisplayName; char* MonitorName; int GPUDeviceLimits[3]; } GPUDevice; //A simple holder for a GPU device.


//Variables(Private) ============================================================================================
int GPUCount;
GPUDevice* GPUDevices;


//Internal Methods ==============================================================================================


//Public Methods =================================================================================================

#ifdef __cplusplus
extern "C" {
#endif

//Initialize the library.
void Initialize();
//Refreshes the GPU list.
void RefreshGPUList();
//Release all resources held by the library.
void ReleaseResources();

//Initializes the GPU context on the calling thread.
bool CreateGPUContext(GPUDevice* Device);
//Disposes the GPU context on the calling thread.
void DisposeGPUContext(GPUDevice* Device);

//Gets the Raw GPUDevices pointer.
int GetRawGPUDevices(GPUDevice** OutDevices);

#ifdef __cplusplus
}
#endif
