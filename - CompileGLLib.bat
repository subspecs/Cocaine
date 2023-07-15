gcc -DCocaine_Win_64 "src/glad/gl.c" "src/os_methods.c" "src/gpu_methods.c" "src/api_methods.c" -L "lib/Win/x64" -lglfw3dll -fPIC -shared -O3 -o "Output/Cocaine.dll"
pause