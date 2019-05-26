#include <iostream>

#include "model.hpp"

int main() {
	Model m("/home/dale/local/drone/models/bin/cbetriangles.bin");
	if(m.flags.test(BAD_READ)) {
		std::cout << "yay" << std::endl;
	}
	for(auto i : m.indeces) {
		std::cout << i << std::endl;
	}
}
