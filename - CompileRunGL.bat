gcc -DCocaine_Win_64 -DCocaineCompile "src/glad/gl.c" "src/os_methods.c" "src/gpu_methods.c" "src/api_methods.c" "console_tests.c" -L "lib/Win/x64" -lglfw3dll -O3 -o "outputs/Console.exe"
"outputs/Console.exe"
pause