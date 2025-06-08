if exist %1.exe del %1.exe
gcc %1.c -I ..\include -L ..\lib -lrikai -lm -o %1.exe
