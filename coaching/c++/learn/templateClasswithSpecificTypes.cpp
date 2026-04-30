#include <type_traits>
#include <iostream>

using namespace std;

// Step 1: Define allowed types
class A
{
};
class B
{
};
class C
{
};

template <typename T>
constexpr bool is_allowed_v =
    is_same<T, A>::value ||
    is_same<T, B>::value;

template <typename T>
class MyClass
{
    static_assert(is_allowed_v<T>, "Type not allowed for MyClass");

public:
    void print()
    {
        cout << "Allowed type\n";
    }
};

int main()
{
    MyClass<A> obj1;
    obj1.print();
    MyClass<B> obj2;
    obj2.print();
    // MyClass<C> obj3;
}