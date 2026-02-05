#include <iostream>
#include <vector>
using namespace std;

class aaa
{

public:
    vector<int> temp;

    aaa()
    {
        temp.resize(1000); // assuming we need space for 1000 elements
    }

    template <typename... TT>
    int &operator[](TT... args)
    {
        cout << "aaa::operator[] called with ";
        vector<uint32_t> indices;
        double value;

        (indices.push_back((uint32_t)args), ...);

        uint64_t flat_index = 0;
        uint32_t stride = 1;
        for (int i = indices.size() - 1; i >= 0; --i)
        {
            cout << indices[i] << " ";
            flat_index += indices[i] * stride;
            stride *= 10; // assuming max dimension size is 10 for simplicity
        }

        return temp[flat_index];
    }
};

int main()
{
    aaa a;
    a[2, 3, 4] = 10;

    cout << a.temp[2 * 100 + 3 * 10 + 4] << endl; // should output 10
    return 0;
}
