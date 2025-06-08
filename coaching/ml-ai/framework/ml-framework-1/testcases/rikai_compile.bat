if exist test.exe del %1.exe
gcc %1.c -I ..\include -L ..\lib -lrikai -o %1.exe
