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
Personally I use windows 10, so I compile this with mingw64.<br>
Cocaine uses [GLFW](https://github.com/glfw/glfw) under the hood, and the basic libglfw3dll.a and glfw3.dll for windows x64 are already included in the source.<br>
So in short, all dependencies(GLFW) are already present, one only needs to compile the code.<br>
You can check the latest compile arguments I use [here](https://github.com/subspecs/Cocaine/blob/master/-%20CompileGLLib.bat), or look at these basic arguments(swap Cocaine_Win_64 for anything you're [targeting]) here:
```
gcc -DCocaine_Win_64 -DCocaineCompile "src/glad/gl.c" "src/os_methods.c" "src/gpu_methods.c" "src/api_methods.c" -L "lib/Win/x64" -lglfw3dll -fPIC -shared -O3 -o "outputs/Cocaine.dll"
```
