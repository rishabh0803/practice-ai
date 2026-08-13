/*
 * Contains code discussed in the lecture.
 * Maily to understand the how the expression tree is built
 * and how the operator overloading works.
 */

#include <iostream>

using namespace std;

class Number
{
private:
    int num;
    char name;

public:
    Number()
    {
        this->num = 0;
        this->name = (char)0;
    }

    Number(int num, char name)
    {
        this->num = num;
        this->name = name;
    }

    Number &operator*(Number &other)
    {
        cout << this->name << " * " << other.name << endl;
        return *this;
    }

    Number &operator+(Number &other)
    {
        cout << this->name << " + " << other.name << endl;
        return *this;
    }

    Number &operator-(Number &other)
    {
        cout << this->name << " - " << other.name << endl;
        return *this;
    }

    Number &operator/(Number &other)
    {
        cout << this->name << " / " << other.name << endl;
        return *this;
    }
};

int main()
{
    Number a;
    Number b(2, 'b');
    Number c(3, 'c');
    Number d(4, 'd');
    Number e(5, 'e');
    Number f(6, 'f');

    a = b + c * d / e - f;

    return 0;
}