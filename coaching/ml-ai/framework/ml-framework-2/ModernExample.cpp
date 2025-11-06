#include <iostream>
using namespace std;

// Modern C++ approach using variadic templates
template<typename... Args>
auto sumMixed(Args... args) -> double
{
    return (static_cast<double>(args) + ...); // C++17 fold expression
}

// Alternative: More explicit type handling
template<typename First, typename... Rest>
double sumMixedExplicit(First first, Rest... rest)
{
    if constexpr (sizeof...(rest) == 0) {
        return static_cast<double>(first);
    } else {
        return static_cast<double>(first) + sumMixedExplicit(rest...);
    }
}

int main()
{
    // Modern approach - automatically handles mixed types
    cout << "Modern mixed sum: " << sumMixed(1, 2, 3, 4, 5.5) << endl;
    cout << "Modern mixed sum: " << sumMixed(10, 3.14) << endl;
    cout << "Modern mixed sum: " << sumMixed(1.5, 2, 3, 4.7) << endl;
    
    // Explicit version
    cout << "Explicit mixed sum: " << sumMixedExplicit(1, 2, 3, 4, 5.5) << endl;
    cout << "Explicit mixed sum: " << sumMixedExplicit(10, 3.14) << endl;
    
    return 0;
}