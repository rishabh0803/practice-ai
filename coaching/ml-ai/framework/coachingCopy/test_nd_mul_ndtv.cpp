#include<NDArray.h>
#include<iomanip> // for setw
#include<iostream>
#include<NDArrayTransposedView.h>
using namespace std;

void print_2d_matrix(NDArrayTransposedView &n)
{
	auto dimensions=n.get_dimensions();
	if(dimensions.size()!=2)
	{
		cout<<"Not a 2 d array"<<endl;
		return;
	}
	auto rows=dimensions[0];
	auto columns=dimensions[1];
	for(int r=0;r<rows;++r)
	{
		for(int c=0;c<columns;++c)
		{
			cout<<setw(10)<<n.get(r,c);
		}
		cout<<endl;
	}
}

void print_2d_matrix(NDArray &n)
{
	auto dimensions=n.get_dimensions();
	if(dimensions.size()!=2)
	{
		cout<<"Not a 2 d array"<<endl;
		return;
	}
	auto rows=dimensions[0];
	auto columns=dimensions[1];
	for(int r=0;r<rows;++r)
	{
		for(int c=0;c<columns;++c)
		{
			cout<<setw(10)<<n[r][c]<<" ";
		}
		cout<<endl;
	}
}

int main()
{
	NDArray n({3});
	n[0] = 0.119815666446268;
	n[1] = 0.0345136230173435;
	n[2] = 0.0161133270624231;


	NDArray n2({5});
	n2[0] = 1;
	n2[1] = 0.1;
	n2[2] = 0.2;
	n2[3] = 0.3;
	n2[4] = 0.4;

	NDArrayTransposedView n2_tv=n2.get_transposed_view(true);


	NDArray n3({3, 5});
	n3 = n * n2_tv;

	n3.print();

	return 0;
}
