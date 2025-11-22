#include <iostream>
#include <stdarg.h>
using namespace std;

// Original function for all integers
int sum(int count, ...)
{
    va_list args;
    va_start(args, count);

    int total = 0;
    for (int i = 0; i < count; ++i)
    {
        total += va_arg(args, int);
    }

    va_end(args);
    return total;
}

double sumWithDouble(int count, ...)
{
    va_list args;
    va_start(args, count);

    double total = 0.0;

    // Sum the first (count-1) integers
    for (int i = 0; i < count - 1; ++i)
    {
        total += va_arg(args, int);
    }

    // Add the last argument as double
    total += va_arg(args, double);

    va_end(args);
    return total;
}

int main()
{
    // Test original function with all integers
    cout << "All integers: " << sum(5, 1, 2, 3, 4, 5) << endl;
    cout << "All integers: " << sum(2, 1, 10) << endl;

    // Test new function with n-1 integers and 1 double (double is last)
    cout << "Mixed (4 ints + 1 double): " << sumWithDouble(5, 1, 2, 3, 4, 5.5) << endl;
    cout << "Mixed (1 int + 1 double): " << sumWithDouble(2, 10, 3.14) << endl;
    cout << "Mixed (3 ints + 1 double): " << sumWithDouble(4, 1, 2, 3, 4.7) << endl;

    return 0;
}