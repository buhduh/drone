#include <iostream>

typedef const char* file_name;

class Foo {
	public:
	//Foo(file_name);
	Foo(const char*);
};

//Foo::Foo(const file_name) {
	//std::cout << "const file_name constructor." << std::endl;
//}

Foo::Foo(const char*) {
	std::cout << "const char* constructor" << std::endl;
}

int main() {
	//file_name foo("foo");
	//Foo(foo);
	Foo("foo");
}
