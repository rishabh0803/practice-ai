#include <iostream>
#include <thread>
using namespace std;

void kalia(int from, int to)
{
    for (int i = from; i <= to; i++)
    {
        cout << "Kalia eating mango " << i << endl;
    }
}

int main()
{
    int x, y;
    x = 1;
    y = 200;
    thread t(kalia, x, y);

    t.join();
    return 0;
}

