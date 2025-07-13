if exist %1.exe del %1.exe
gcc %1.c -I ..\include -I ..\private\include -L ..\lib -lrikai -o %1.exe
