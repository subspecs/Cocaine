gcc -DCocaine_Win_64 -DCocaineCompile -c "src/glad/gl.c" -c "src/os_methods.c" -c "src/gpu_methods.c" -c "src/api_methods.c" -L "lib/Win/x64" -lglfw3dll -O3
ar rcs "outputs/libCocaine.a" "gl.o" "os_methods.o" "gpu_methods.o" "api_methods.o"
del "gl.o"
del "os_methods.o"
del "gpu_methods.o"
del "api_methods.o"
pause