#include <iostream>

#include <stdlib.h>

#include "memory/memory.hpp"

typedef uint8_t FLAG;
static const FLAG flag1 = (1 << 0);
static const FLAG flag2 = (1 << 1);
static const FLAG flag3 = (1 << 2);
static const FLAG flag4 = (1 << 3);
static const FLAG flag5 = (1 << 4);
static const FLAG flag6 = (1 << 5);
static const FLAG flag7 = (1 << 6);
static const FLAG flag8 = (1 << 7);

FLAG doIt() {
	//FLAG out(flag1);
	FLAG toRet(flag1);
	return toRet | flag3 | flag8;
	//memory::HeapAllocator ha = memory::HeapAllocator();
	//memory::Block<int> blk = ha.allocate<int>(sizeof(int));
	//memory::Block<int> blk = ha.allocate<int>(0);
	//if(blk.ptr == nullptr) {
		//std::cout << "yay" << std::endl;
	//}
	//std::cout << "kabloom: " << *blk.ptr << std::endl;
	//ha.deallocate(blk);
}

int main() {
	auto foo = doIt();
	char buff[33];
	sprintf(buff, "%d", foo);
	std::cout << buff << std::endl;
}
