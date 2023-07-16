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
<sub>*Some SOCs and GPUs like ATI/AMD/Intel didn't bother to update their API levels, tho a very small portion of them.</sub>

### So, how does one use it?
For a simple multi-platform solution, there is a [Cocaine .NET]() wrapper.(COMING SOON!)<br>
For what I am using, here's an windows x64 example:<br>
1) Look at the example [here](https://github.com/subspecs/Cocaine/blob/master/examples/basicexample.c).
2) a) If you're going to link a **static library(.lib/.a)** then the only headers you'll need are '**api_methods.h**', '**gpu_methods.h**' and '**os_methods.h**'.<br>
b) If you're going to link/load against a **dynamic library(.dll)**, then you only need the **Cocaine.dll** and **glfw3.dll** files in the same directory as the executable.<br>

**That's it!**<br>
For what **methods** are available, their **documentation** and **signatures** are available at the wiki(SOON).<br>
There are also comments on mostly everything**.

### So, how does one compile it?
Personally I use windows 10, so I compile this with mingw64.<br>
Cocaine uses [GLFW](https://github.com/glfw/glfw) under the hood, and the basic libglfw3dll.a and glfw3.dll for windows x64 are already included in the source.<br><br>
So in short, all* dependencies(GLFW) are already present, one only needs to compile the code. (You sill need to alter the GLAD header file if switching to ES 3.1, for now.)<br>
<sub>*If you need other platfroms, compile/get them from [GLFW](https://github.com/glfw/glfw) repo yourself, or pray for a release build.</sub><br><br>
You can check the latest compile arguments I use [here](https://github.com/subspecs/Cocaine/blob/master/-%20CompileGLLib.bat), or look at these basic arguments(swap **Cocaine_Win_64** for anything you're [targeting](https://github.com/subspecs/Cocaine/blob/master/include/platform.h)) here:
```
gcc -DCocaine_Win_64 -DCocaineCompile "src/glad/gl.c" "src/os_methods.c" "src/gpu_methods.c" "src/api_methods.c" -L "lib/Win/x64" -lglfw3dll -fPIC -shared -O3 -o "outputs/Cocaine.dll"
```

### **Want to [help me](https://www.patreon.com/subspecs) pay off my therapist? Might make updates more frequent!**


