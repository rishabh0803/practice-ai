rm *.o
gcc -c ..\src\error\*.c -I ..\include -I ..\include\error\ -I ..\include\matrix -I ..\private\include
gcc -c ..\src\matrix\*.c -I ..\include -I ..\include\error\ -I ..\include\matrix -I ..\private\include

rm ..\lib\librikai.a
ar rcs ..\lib\librikai.a *.o
