#ifndef MEMORY_MEMORY_HPP
#define MEMORY_MEMORY_HPP

#include <stddef.h>
#include <stdint.h>

namespace memory {

struct Block {
	size_t size;
	uintptr_t address;
};

}
#endif
