gcc -DCocaine_Win_64 "src/glad/gl.c" "src/os_methods.c" "src/gpu_methods.c" "src/api_methods.c" "main.c" -L "lib/Win/x64" -lglfw3dll -O3 -o "Output/Console.exe"
"Output/Console.exe"
pause