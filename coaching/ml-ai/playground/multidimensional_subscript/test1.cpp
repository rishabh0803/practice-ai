#include <iostream>

using namespace std;

class aaa
{
    int temp;

public:
    int &operator[](int a, int b, int c)
    {
        cout << "aaa::operator[] called with " << a << "," << b << "," << c << endl;
        return temp;
    }
};

int main()
{
    aaa a;
    a[2, 3, 4] = 10;
    return 0;
}