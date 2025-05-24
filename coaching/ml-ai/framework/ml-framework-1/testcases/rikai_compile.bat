if exist test.exe del test.exe
gcc %1 -I ..\include -L ..\lib -lrikai -o test.exe
