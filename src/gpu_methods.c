#include "../include/platform.h"
#include "../include/os_methods.h"
#include "../include/gpu_methods.h"

#include <string.h>

//Variables(Private) ============================================================================================
bool IsGladLoaded, IsInitialized;

//Internal Methods ==============================================================================================

//Creates an OpenGL context for that specific GPU connected on that specific monitor.
bool CreateContext(void* GPU, void** Window)
{
#if !defined(Cocaine_GLES)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //Sets the OpenGL version(4.3). 
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //Sets the OpenGL profile. 
#else
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
#endif

#if defined(__APPLE__) //fucking apple
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_COCOA_MENUBAR, GLFW_FALSE);
#endif

	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); //Hide the window.

	const GLFWvidmode* mode = glfwGetVideoMode(GPU); //Gets the current monitor video mode sets the window to it. (aka fullscreen-windowed mode)
	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

#if defined(Cocaine_Win_64) || defined(Cocaine_Win_32)
	void* CurrentForegroundWindow = GetForegroundWindow(); //Gets the current focused window.
#endif
	*Window = glfwCreateWindow(mode->width, mode->height, "", NULL, NULL); //Create window.  GPU
	if(*Window == NULL) { CheckLogError(true, NULL, "CreateContext"); return false; }
	if(!IsGladLoaded)
	{ 
		glfwMakeContextCurrent(*Window);
#if !defined(Cocaine_GLES)
		IsGladLoaded = gladLoadGL(glfwGetProcAddress);
#else
		IsGladLoaded = gladLoadGLES2(glfwGetProcAddress);
#endif
		if(!IsGladLoaded) { CheckLogError(true, "Error loading glad! (Is at least one GPU context loaded?)", "CreateContext"); return false; } 
	} //Tries to load GLAD, this is needed for anything related to this API.
#if defined(Cocaine_Win_64) || defined(Cocaine_Win_32)
	Win_HideWindowInTaskbar(*Window); //Hides the window in the taskbar. (Windows 10+ bug.)
	SetWindowPos(glfwGetWin32Window(*Window), HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE); //Set this window to be at the utmost back of the desktop.
	SetForegroundWindow(CurrentForegroundWindow); //Set back the last focused window. (This because creating a new window will focus on it, since it's invisible that would softlock the entire destop, so either this or ATL-TAB fixes it.)
#endif
	CheckLogError(true, NULL, "CreateContext"); //Check for errors.
	return *Window != NULL; //Check if any error has occured.
}
//Get GPU/Renderer names of already created contexts. WARNING: Use this BEFORE you do any work on the GPUs or BEFORE creating their associated threads.
char** GetGPUMonitors(void** GPUContexts, const int Count)
{
	void* OGContext = glfwGetCurrentContext(); //Get the calling threads GPU context if any.
	char** Names = malloc(sizeof(void*) * Count); //Allocated the pointers needed for each char array.
	int n = 0; while(n < Count) 
	{ 
		glfwMakeContextCurrent(GPUContexts[n]); //Hopyfully the GPU ISN'T being used by the API right now, since we switch it's context to the CALLING thread.
		const char* GPUName = glGetString(GL_RENDERER); //Actually get the GPU's/Renderes name.
		Names[n] = (char*)(GPUName == NULL ? "Unknown" : GPUName); //Does a sanity check on the name, since some crappy emulators/GPU's might not have this name stored.
		n++; 
	}
	if(OGContext != NULL) { glfwMakeContextCurrent(OGContext); } //If this thread had any GPU context, we restore it.
	CheckLogError(true, NULL, "GetCurrentGPUDevices"); //Check for errors.
	return Names; //Return the names of the GPU's.
}
//Creates GPU device contexts for all connected GPUs and returns them.
int GetCurrentGPUDevices(GPUDevice** GPUDevices)
{
	void* OGContext = glfwGetCurrentContext(); //Since we're going to be switching contexts, IF any already exists on the current thread then we save it to set it back later.
	int MonitorCount; GLFWmonitor** GPUMonitors = glfwGetMonitors(&MonitorCount); //Get all currently detected and plugged in monitors.

	if(MonitorCount > 0)
	{
		GPUDevice* Devices = malloc(sizeof(GPUDevice) * MonitorCount);
		int n = 0, x = 0, GPUCount = 0;
		while(n < MonitorCount)
		{
			GPUCount++;
			Devices[n].GPUMonitor = GPUMonitors[n]; //Get the monitor associated with the GPU.
			if(CreateContext(GPUMonitors[n], &Devices[n].GPUContext)) //Initialize a device context on the GPU that's connecte to the given monitor.
			{
				glfwMakeContextCurrent(Devices[n].GPUContext); //Make the context current so we can read GPU values.
				Devices[n].DisplayName = (char*)glGetString(GL_RENDERER); //Get the actual GPU/Renderer name.
				glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &Devices[n].GPUDeviceLimits[0]); //Get GPU die work group limits per X axis.
				glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &Devices[n].GPUDeviceLimits[1]); //Get GPU die work group limits per Y axis.
				glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &Devices[n].GPUDeviceLimits[2]); //Get GPU die work group limits per Z axis.
			}
			else { CheckLogError(true, NULL, "GetCurrentGPUDevices"); char tmp[1024]; strcpy(tmp, "Could not create GPU device context for: "); strcpy(tmp, glfwGetMonitorName(GPUMonitors[n])); CheckLogError(true, tmp, "GetCurrentGPUDevices"); GPUCount--; n++; continue;} //Report and move on if any issues arrise.

			x = 0;
			while(x < MonitorCount) 
			{
				if(x != n && strcmp(Devices[x].DisplayName, Devices[n].DisplayName) == 0) //Check if currently existing GPUs match the current one and ommit it if it does. (GPU dual+ monitor?)
				{ 
					glfwWaitEvents(); //Process ANY events for the GPU Context.
					glfwSetWindowShouldClose(Devices[n].GPUContext, true); //Flag the GPUContext for deletion.
					glfwDestroyWindow(Devices[n].GPUContext); //Delete GPUContext.
					Devices[n].GPUContext = NULL; //Set it to NULL so we can sort it out later.
					GPUCount--; //Remove from GPU counter.
					break; 
				};
				x++;
			}
			CheckLogError(true, NULL, "GetCurrentGPUDevices"); //Check for errors.
			n++;
		}

		*GPUDevices = malloc(sizeof(GPUDevice) * GPUCount); //Allocate space for GPUDevices.

		n = 0; x = 0;
		while(n < GPUCount)
		{
			if(Devices[x].GPUContext == NULL) { x++; continue; } //Check if GPUContext exists.

			glfwMakeContextCurrent(Devices[x].GPUContext); //Make the context current so we can read GPU values.

			glfwWaitEvents(); //Process ANY events for the GPU Context.
			glfwSetWindowShouldClose(Devices[x].GPUContext, true); //Flag the GPUContext for deletion.
			glfwWaitEvents(); //Process ANY events for the GPU Context.
			glfwDestroyWindow(Devices[x].GPUContext); //Delete GPUContext.
			CheckLogError(true, NULL, "GetCurrentGPUDevices"); //Check for errors.
			
			(*GPUDevices)[n] = Devices[x]; //If yes then copy it to the buffer.
			(*GPUDevices)[n].GPUContext = NULL; //Set GPU context to null.

			n++;
		}
		
		free(Devices); //Free the allocated memory.
		if(OGContext != NULL) { glfwMakeContextCurrent(OGContext); } //If this thread had any GPU context, we restore it.
		CheckLogError(true, NULL, "GetCurrentGPUDevices"); //Check for errors.
		return GPUCount;
	}
	else { return 0; }

}

//Public Methods =================================================================================================

//Initialize the library.
void Initialize()
{
	if(!IsInitialized)
	{
		InitTime(); //Initialize Timing Methods. (Windows Only)
		if (!glfwInit()) { CheckLogError(true, NULL, "Initialize"); exit(EXIT_FAILURE); } //Initialize library if haven't already.
		GPUDevices = NULL; GPUCount = GetCurrentGPUDevices(&GPUDevices); //Create and get GPUs and their contexts.

		CheckLogError(true, NULL, "Initialize");
		IsInitialized = true;
	}
}
//Release all resources held by the library.
void ReleaseResources()
{
	glfwWaitEvents(); //Process window events for all gpu contexts.
	glfwTerminate(); //Release all GL resources.
}
//Refreshes the GPU list. WARNING: Dispose ALL open GPU contexts BEFORE calling this!
void RefreshGPUList()
{
	free(GPUDevices); //Delete all GPUs info.
	GPUDevices = NULL;
	GPUCount = GetCurrentGPUDevices(&GPUDevices); //Create and get GPUs and their contexts.
}

//Initializes the GPU context on the calling thread.
void CreateGPUContext(GPUDevice* Device)
{
	if(Device->GPUContext == NULL)
	{
		if(!CreateContext(Device->GPUMonitor, &Device->GPUContext)) //Initialize a device context on the GPU that's connecte to the given monitor.
		{
			char tmp[1024]; strcpy(tmp, "Could not create GPU device context for: "); strcpy(tmp, glfwGetMonitorName(Device->GPUMonitor)); CheckLogError(true, tmp, "CreateGPUContext"); //Report and move on if any issues arrise.
		}
	}
	CheckLogError(true, NULL, "CreateGPUContext"); 
}
//Disposes the GPU context on the calling thread.
void DisposeGPUContext(GPUDevice* Device)
{
	if(Device->GPUContext != NULL)
	{
		glfwMakeContextCurrent(Device->GPUContext); //Make the context current so we can read GPU values.
		glfwWaitEvents(); //Process ANY events for the GPU Context.
		glfwSetWindowShouldClose(Device->GPUContext, true); //Flag the GPUContext for deletion.
		glfwWaitEvents(); //Process ANY events for the GPU Context.
		glfwDestroyWindow(Device->GPUContext); //Delete GPUContext.
		Device->GPUContext == NULL; //Set it to NULL so we can sort it out later.
		CheckLogError(true, NULL, "AllocateGPUBuffer");
	}
}

//Gets the Raw GPUDevices pointer.
int GetRawGPUDevices(GPUDevice** OutDevices)
{
	*OutDevices = GPUDevices;
	return GPUCount;
}