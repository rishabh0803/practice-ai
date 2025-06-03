rm *.o
gcc -c ..\src\error\*.c -I ..\include -I ..\private\include  -Wall 
gcc -c ..\src\matrix\*.c -I ..\include -I ..\private\include -Wall
gcc -c ..\src\vector\*.c -I ..\include -I ..\private\include -Wall
gcc -c ..\src\math\*.c -I ..\include -I ..\private\include -Wall

rm ..\lib\librikai.a
ar rcs ..\lib\librikai.a *.o
