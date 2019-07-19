#include "memory/arena.hpp"

int main() {
	memory::Arena arena = memory::Arena(1024);
	void* one = arena.allocate(sizeof(void*));
	void* two = arena.allocate(sizeof(void*));
	void* three = arena.allocate(sizeof(void*));
	arena.deallocate(two);
	arena.allocate(sizeof(void*));
	int i = 2;
}
