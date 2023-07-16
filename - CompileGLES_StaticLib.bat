gcc -DCocaine_Win_ARM64 -DCocaineCompile -c "src/glad/gles2.c" -c "src/os_methods.c" -c "src/gpu_methods.c" -c "src/api_methods.c" -L "lib/Win/x64" -lglfw3dll -O3
ar rcs "outputs/libCocaine.a" "gles2.o" "os_methods.o" "gpu_methods.o" "api_methods.o"
del "gles2.o"
del "os_methods.o"
del "gpu_methods.o"
del "api_methods.o"
pause