#include <iostream>
#include <vector>
#include <cstdarg>
using namespace std;

class aaa
{
    vector<int> temp;

public:
    int &operator[](int first, ...)
    {
        cout << "aaa::operator[] called with " << first;

        va_list args;
        va_start(args, first);

        for (int i = 0; i < 2; i++) // assuming 2 more dimensions for example
        {
            int index = va_arg(args, int);
            cout << " " << index;
        }

        cout << endl;

        va_end(args);
    }
};

int main()
{
    aaa a;
    a[2, 3, 4] = 10;

    return 0;
}