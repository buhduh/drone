#include <iostream>

#include "modelparser/modelparser.hpp"

extern "C" int foo() {
	MP_Vertex v = {0.0, 0.0, 0.0};
	std::cout << v << std::endl;
	return 0;
}

int main() {
	MP_Vertex v = {1.0, 2.0, 3.0};
	std::cout << v << std::endl;
	foo();
	return 0;
}

