gcc -DCocaine_Win_64 -DCocaineCompile "src/glad/gl.c" "src/os_methods.c" "src/gpu_methods.c" "src/api_methods.c" -L "lib/Win/x64" -lglfw3dll -O3 -fPIC -shared -o "outputs/Cocaine.dll"
pause