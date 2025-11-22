#include <iostream>
#include <type_traits>
#include <tuple>
#include <initializer_list>
using namespace std;

// Modern C++ approach using variadic templates (C++14 compatible)
template<typename T>
double sumMixedHelper(T arg) {
    return static_cast<double>(arg);
}

template<typename T, typename... Args>
double sumMixedHelper(T first, Args... rest) {
    return static_cast<double>(first) + sumMixedHelper(rest...);
}

template<typename... Args>
auto sumMixed(Args... args) -> double
{
    return sumMixedHelper(args...);
}

// Alternative: More explicit type handling (C++14 compatible)
template<typename T>
double sumMixedExplicit(T first)
{
    return static_cast<double>(first);
}

template<typename First, typename... Rest>
double sumMixedExplicit(First first, Rest... rest)
{
    return static_cast<double>(first) + sumMixedExplicit(rest...);
}

template<typename... Args>
void processArguments(Args... args, double lastArg) {
    cout << "Processing arguments: ";
    // C++14 compatible way to print all args
    auto dummy = {(cout << args << " ", 0)...};
    (void)dummy; // suppress unused variable warning
    cout << "and " << lastArg << endl;
}

// Correct implementation: Function that accepts n parameters where first n-1 are uint32_t and last is double
// Version 1: Using recursion to handle the constraint properly (C++14 compatible)

template<typename T>
double processUintAndDoubleImpl(T&& last) {
    static_assert(std::is_same<typename std::decay<T>::type, double>::value, "Last parameter must be double");
    return static_cast<double>(last);
}

template<typename First, typename... Rest>
double processUintAndDoubleImpl(First&& first, Rest&&... rest) {
    static_assert(std::is_same<typename std::decay<First>::type, uint32_t>::value, "Non-last parameters must be uint32_t");
    cout << "Processing uint32_t: " << first << " ";
    return static_cast<double>(first) + processUintAndDoubleImpl(std::forward<Rest>(rest)...);
}

// Wrapper function
template<typename... Args>
auto processUintAndDouble(Args&&... args) -> double {
    static_assert(sizeof...(args) >= 2, "At least one uint32_t and one double required");
    cout << "Starting processing: ";
    double result = processUintAndDoubleImpl(std::forward<Args>(args)...);
    cout << "\nSum result: " << result << endl;
    return result;
}

// Version 2: Using std::initializer_list for uint32_t and separate double parameter
double processUintListAndDouble(std::initializer_list<uint32_t> uintArgs, double lastArg) {
    cout << "Processing uint32_t list: ";
    double sum = 0.0;
    for (auto val : uintArgs) {
        cout << val << " ";
        sum += static_cast<double>(val);
    }
    cout << "and double: " << lastArg << endl;
    return sum + lastArg;
}

// Version 3: Constexpr version (C++14 compatible)
template<uint32_t First>
constexpr double processConstexprUintsHelper() {
    return static_cast<double>(First);
}

template<uint32_t First, uint32_t Second, uint32_t... Rest>
constexpr double processConstexprUintsHelper() {
    return static_cast<double>(First) + processConstexprUintsHelper<Second, Rest...>();
}

template<uint32_t... Values>
constexpr auto processConstexprUints(double lastArg) -> double
{
    return processConstexprUintsHelper<Values...>() + lastArg;
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
    
    cout << "\n--- Functions with uint32_t parameters and double ---" << endl;
    
    // Test the new function with uint32_t parameters and double
    uint32_t a = 10, b = 20, c = 30;
    double d = 5.5;
    
    // Version 1: Recursive template approach
    cout << "\nRecursive template approach:" << endl;
    processUintAndDouble(a, b, c, d);
    processUintAndDouble(uint32_t(5), uint32_t(15), 3.14);
    processUintAndDouble(uint32_t(100), 2.71);
    
    // Version 2: Using initializer_list
    cout << "\nInitializer list approach:" << endl;
    cout << "List result: " << processUintListAndDouble({10, 20, 30}, 5.5) << endl;
    cout << "List result: " << processUintListAndDouble({5, 15}, 3.14) << endl;
    cout << "List result: " << processUintListAndDouble({100}, 2.71) << endl;
    
    // Version 3: Constexpr version
    cout << "\nConstexpr processing:" << endl;
    cout << "Constexpr result: " << processConstexprUints<10, 20, 30>(5.5) << endl;
    cout << "Constexpr result: " << processConstexprUints<5, 15>(3.14) << endl;
    
    return 0;
}