#include<NDArray.h>
#include<iostream>
using namespace std;
int main()
{
NDArray n({2,5});
NDArray n2({5});
NDArray::copy(n2,{0},n,{1,0},{1,4});
return 0;
}
