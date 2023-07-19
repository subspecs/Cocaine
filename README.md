# Cocaine
Are [steroids](https://github.com/subspecs/Steroids) not doing it for you anymore?<br>
Need that 'kick' in the mornings to get you through the day?

### Look no further because we have the solution for you!

### Meet <ins>**Cocaine**</ins>!
A multi-platform C library that can be used to accelerate <ins>large</ins> workloads/big data/<ins>anything really</ins> with the power of a GPU with ease. <br>
And you know what's the best part of it? You can even let grandma do a line.


### That's right!
The library uses ONLY the default OpenGL 4.3 or Open GL ES 3.1 features with no extensions.<br>
Meaning this thing works on toasters/solitaire/potato/trashcan machines that were made from 2010* and up.
<br>
<sub>*Some SOCs and GPUs like ATI/AMD/Intel didn't bother to update their API levels, tho a very small portion of them.</sub><br>

For what **methods** are available, their **documentation** and **signatures** are available at the [wiki](https://github.com/subspecs/Cocaine/wiki/API-Documentation).<br>
There are also comments on mostly everything**.<br><br>

### So, how does one use it?
For a simple multi-platform solution, there is a [Cocaine .NET](https://github.com/subspecs/CocaineNET) wrapper in the works.<br>
Cocaine uses [GLFW](https://github.com/glfw/glfw) under the hood since it can be compiled for many platforms. <br>
If <ins>compiling for windows</ins>, link against **libCocaine.a** and use the '**api_methods.h**', '**gpu_methods.h**' and '**os_methods.h**' header files, OR use the **Cocaine.dll** at your convenience, just don't forget to put **glfw3.dll** next to your executable.<br>
If <ins>compiling for other platforms</ins>, you just need the **libCocaine.(.so/.dynlib/etc.)** and the '**api_methods.h**', '**gpu_methods.h**' and '**os_methods.h**' header files.<br>

**That's it!**<br>
**[Example](https://github.com/subspecs/Cocaine/blob/master/examples/basicexample.c) Usage**:
```C++
#include "../include/api_methods.h"
#include "../include/gpu_methods.h"
#include "../include/os_methods.h"

char* ShaderCode = //Sample compute shader code for work.
"#version 430\n"
"layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;\n"
"layout(std430, binding = 0) buffer TestName1 { float BufferA[]; };\n" //Here we define our GPU Buffer A that we created using MemoryA memory buffer.
"layout(std430, binding = 1) buffer TestName2 { float BufferB[]; };\n" //Here we define our GPU Buffer B that we created using MemoryB memory buffer.
"layout(std430, binding = 2) buffer TestName3 { float BufferC[]; };\n" //Here we define our GPU Buffer C that we left NULL so it creates and empty buffer.

"unsigned int JobIndex = (gl_GlobalInvocationID.x * (gl_NumWorkGroups.y * gl_NumWorkGroups.z)) + (gl_GlobalInvocationID.y * gl_NumWorkGroups.z) + gl_GlobalInvocationID.z;\n" //A small piece of code that calculates where/which shader unit is at in the job/work count. (Array index essentially.)

"void main()\n"
"{\n"
"	BufferC[JobIndex] = BufferA[JobIndex] + BufferB[JobIndex];\n" //Math.
"}\n";

long long TestSize = 100; //The amount of variables to create. (Make sure you have 3x(4 bytes x TestSize) amount of RAM and GPU VRAM.)
long long TestSizeBufferByteCount; //Calculated the amount of bytes these variable will consume.

void SetBufferValues(float* Buffer, int Length, float Value) { int n = 0; while(n < Length) { Buffer[n] = Value; n++; } } //A function that helps us set our values.

int main()
{
	Initialize(); //Create a line(Initialize library) of Cocaine;

	GPUDevice* ComputeDevices = NULL;
	int DeviceCount = GetRawGPUDevices(&ComputeDevices); //Get the GPUs and their count that Cocaine has detected.

	if(CreateGPUContext(&ComputeDevices[0])) //Take the first GPU and create a context for it.
	{
		SetActiveGPUContext(&ComputeDevices[0]); //Make the current GPU context is active on the calling thread.

		int i = 0;
		while(i < GPUCount)
		{
			printf("GPU[%d]: %s\n", i, ComputeDevices[i].DisplayName); //Print out our GPU names because why not?
			i++;
		}

		TestSizeBufferByteCount = TestSize * sizeof(float); //Calculate the byte count each of our variables are going to consume. (Both on RAM and GPU VRAM)

		void* MemoryA = malloc(TestSizeBufferByteCount); //Create the A empty memory buffer of where we write values to be calculated.
		void* MemoryB = malloc(TestSizeBufferByteCount); //Create the B empty memory buffer of where we write values to be calculated.
		void* MemoryC = malloc(TestSizeBufferByteCount); //Create the C empty memory buffer of where we save values that are calculated.

		SetBufferValues((float*)MemoryA, TestSize, 34.5f); //Generate numbers to calculate for the A memory buffer to calculate.
		SetBufferValues((float*)MemoryB, TestSize, 34.5f); //Generate numbers to calculate for the B memory buffer to calculate.

		GPUBuffer BufferA = AllocateGPUBuffer(NoReadWrite, 0, MemoryA, TestSizeBufferByteCount); //Create a GPU buffer and write our memory buffer A contents to it.
		GPUBuffer BufferB = AllocateGPUBuffer(NoReadWrite, 1, MemoryB, TestSizeBufferByteCount); //Create a GPU buffer and write our memory buffer B contents to it.
		GPUBuffer BufferC = AllocateGPUBuffer(Read, 2, NULL, TestSizeBufferByteCount); //Create a GPU buffer and and leave the buffer input NULL, this signals the GPU that you just want to create an empty GPU buffer on it. (Saves time)

		bool IsPepsi = false; GLuint Program;
		if (!CompileProgram(ShaderCode, &Program)) //Compiles shader code that is atop our code.
		{
			IsPepsi = true; //If compilation failed. 
			CheckLogError(true, NULL, "main"); //Error Check why compilation failed.
		}

		if(!IsPepsi) //In-case the compilation didn't fail.
		{
			if(RunComputeProgram(Program, GPUDevices[0].GPUDeviceLimits, TestSize, false)) //We run our compiled program on the CURRENTLY ACTIVE GPU on the CURRENTLY ACTIVE THREAD.
			{
				ReadFromGPUBuffer(&BufferC, MemoryC, 0, TestSizeBufferByteCount); //When done, we'd like to read the results from GPU buffer back to our memory buffer. (BufferC => MemoryC)

				int n = TestSize - 10;
				while(n < TestSize)
				{
					printf("Output[%d]: %f\n", n, ((float*)MemoryC)[n]); //For kicks we print out the result of the last 10 jobs completed.
					n++;
				}
			}
			else { CheckLogError(true, NULL, "main"); } //Error Check why program run failed.
		}
	}
	else { CheckLogError(true, NULL, "main"); }  //Error Check why creating GPU context failed.

	return 0;
}
```

### So, how does one compile it?
You can use any C/C++ compiler you wish. I use mingw64 for development.<br><br>
To create an Visual Studio / Custom solution, simply create an empty c++ project, and add everything but the **tests**, **examples** and **outputs** folders.<br>
Don't forget to link against **libglfw3dll.a** in the project settings and keep the **glfw3.dll** next to the compiled exe if on windows, also don't forget to define **Cocaine_Win_64** or switch it to a [platform](https://github.com/subspecs/Cocaine/blob/master/include/platform.h) of your choice.<br><br>
**WARNING**: There's an **gles2.c** file in there, IF you intend to compile to a platform that uses opengl es, then DON'T compile **gl.c** and instead replace it with **gles2.c**, if your platform doesn't use opengl es then just compile the **gl.c** alone.<br><br>
Cocaine uses [GLFW](https://github.com/glfw/glfw) under the hood, and the basic libglfw3dll.a and glfw3.dll for windows x64 are already included in the source.<br>
<sub>*If you need static libraries for other platfroms, compile/get them from [GLFW](https://github.com/glfw/glfw) repo yourself, or pray for a release build.</sub><br><br>
You can check the latest compile arguments I use [here](https://github.com/subspecs/Cocaine/blob/master/-%20CompileGL_DllLib.bat), or look at these basic arguments(swap **Cocaine_Win_64** for anything you're [targeting](https://github.com/subspecs/Cocaine/blob/master/include/platform.h) and don't forget to change the static **lglfw3dll** library to match the platform you're compiling for.) here:
```
gcc -DCocaine_Win_64 -DCocaineCompile "src/glad/gl.c" "src/os_methods.c" "src/gpu_methods.c" "src/api_methods.c" -L "lib/Win/x64" -lglfw3dll -fPIC -shared -O3 -o "outputs/Cocaine.dll"
```

### **Want to [help me](https://www.patreon.com/subspecs) pay off my therapist? Might make updates more frequent!**


