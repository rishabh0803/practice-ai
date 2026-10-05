#include <NDArray.h>
#include <iostream>
using namespace std;
int main()
{
    NDArray b({2, 3});
    NDArray c({2, 3});
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
    c[1][0] = 40;
    c[1][1] = 50;
    c[1][2] = 60;

    a = sigmoid(b);

    a.print_for_testing();

    return 0;
}
