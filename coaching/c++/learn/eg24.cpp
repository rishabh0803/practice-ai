#include <iostream>
using namespace std;

class Proxy;
class aaa
{
private:
	int x[3];
public:
	aaa(int a, int b, int c);
	Proxy &operator[](int index);
	void set(int index, int value) { x[index] = value; }
	int get(int index) const { return x[index]; }
};

class Proxy
{
private:
	aaa *obj;
	int index;
public:
	int operator()(aaa *a, int index)
	{
		this->obj = a;
		this->index = index;
	}

	Proxy &operator=(int v)
	{
		cout << "Write Operation" << endl;
		this->obj->set(this->index, v);
		return *this;
	}

	Proxy &operator=(const Proxy &p)
	{
		cout << "Read,Write Operation" << endl;
		this->obj->set(this->index, p.obj->get(p.index));
		return *this;
	}

	operator int()
	{
		cout << "Read Operation" << endl;
		return this->obj->get(this->index);
	}
};

aaa::aaa(int a, int b, int c) : x{ a,b,c } {}

Proxy &aaa::operator[](int index)
{
	Proxy *p = new Proxy();
	(*p)(this, index);
	return *p;
}

int main() {
	aaa a(10, 20, 30);
	cout << a[0] << endl;
	cout << a[1] << endl;
	cout << a[2] << endl;

	a[1] = 200;
	a[0] = a[1];
	cout << a[0] << endl;
	cout << a[1] << endl;

	return 0;
}