#include <iostream>
#include <thread>
#include <chrono>
using namespace std;

void kalia(int from, int to)
{
    for (int i = from; i <= to; i++)
    {
        cout << i << " ";
    }
}

int main()
{
    int x, y;
    x = 1;
    y = 100000;
    auto start = chrono::high_resolution_clock::now();

    thread t1(kalia, x, y);
    t1.join();

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;

    cout << "\nTime taken: " << duration.count() << " seconds" << endl;
    return 0;
}