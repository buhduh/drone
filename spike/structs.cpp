#include <iostream>

struct MyStruct {
	int a;
	int b;
	int c;	
	friend std::ostream& operator<<(std::ostream& os, const MyStruct& ms) {
		os << "{" << ms.a << "," << ms.b << "," << ms.c << "}";
	}
};

int main(void) {
	MyStruct foo = MyStruct{1,2,3};
	std::cout << foo << std::endl;
}

