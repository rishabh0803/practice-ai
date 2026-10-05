#include <NDArray.h>
#include <iostream>
using namespace std;
int main()
{
    NDArray b({2, 3});
    NDArray c({3, 4});
    NDArray d({4, 2});

    NDArray a;
    b[0][0] = 1;
    b[0][1] = 2;
    b[0][2] = 3;
    b[1][0] = 4;
    b[1][1] = 5;
    b[1][2] = 6;

    c[0][0] = 10;
    c[0][1] = 20;
    c[0][2] = 30;
    c[0][3] = 30;
    c[1][0] = 40;
    c[1][1] = 50;
    c[1][2] = 60;
    c[1][3] = 30;
    c[2][0] = 40;
    c[2][1] = 50;
    c[2][2] = 60;
    c[2][3] = 30;

    d[0][0] = 5;
    d[0][1] = 6;
    d[1][0] = 7;
    d[1][1] = 8;
    d[2][0] = 9;
    d[2][1] = 10;
    d[3][0] = 11;
    d[3][1] = 12;

    a = b * (c * d);

    a.print_for_testing();

    return 0;
}
