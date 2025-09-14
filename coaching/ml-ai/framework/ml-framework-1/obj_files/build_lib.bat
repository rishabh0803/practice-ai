rm *.o
gcc -c ..\src\error\*.c -I ..\include -I ..\private\include  -Wall -lm
gcc -c ..\src\matrix\*.c -I ..\include -I ..\private\include -Wall -lm
gcc -c ..\src\vector\*.c -I ..\include -I ..\private\include -Wall -lm
gcc -c ..\src\math\*.c -I ..\include -I ..\private\include -Wall -lm
gcc -c ..\src\ml_math\*.c -I ..\include -I ..\private\include -Wall -lm
gcc -c ..\src\data_provider\*.c -I ..\include -I ..\private\include -Wall -lm
gcc -c ..\src\linear_regression\*.c -I ..\include -I ..\private\include -Wall -lm

rm ..\lib\librikai.a
ar rcs ..\lib\librikai.a *.o
