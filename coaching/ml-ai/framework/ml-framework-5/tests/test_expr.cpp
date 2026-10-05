#include <NDArray.h>
#include <iostream>
using namespace std;
int main()
{
    NDArray b({2, 3});
    NDArray c({2, 3});
    NDArray d({2, 3});
    NDArray e({2, 3});
    NDArray a({2, 3});
    b[0][0] = 1;
    b[0][1] = 2;
    b[0][2] = 3;
    b[1][0] = 4;
    b[1][1] = 5;
    b[1][2] = 6;

    c[0][0] = 10;
    c[0][1] = 20;
    c[0][2] = 30;
    c[1][0] = 40;
    c[1][1] = 50;
    c[1][2] = 60;
    d[0][0] = 100;
    d[0][1] = 200;
    d[0][2] = 300;
    d[1][0] = 400;
    d[1][1] = 500;
    d[1][2] = 600;
    e[0][0] = 1000;
    e[0][1] = 2000;
    e[0][2] = 3000;
    e[1][0] = 4000;
    e[1][1] = 5000;
    e[1][2] = 6000;
    a = b + c + d + e;

    cout << a[0][0] << endl;
    cout << a[0][1] << endl;
    cout << a[0][2] << endl;
    cout << a[1][0] << endl;
    cout << a[1][1] << endl;
    cout << a[1][2] << endl;

    return 0;
}
