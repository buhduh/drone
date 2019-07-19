#ifndef MEMORY_ARENA_HPP
#define MEMORY_ARENA_HPP

#include <stddef.h>

#include "memory/memory.hpp"

namespace memory {

//The type for the number of allocations a specific arena will have made
//Can't imagine needing more than 2^16
typedef uint16_t block_count_t;
#define BLOCK_COUNT_MAX UINT16_MAX

class Arena {

	public:
	Arena(size_t);
	~Arena();
	void* allocate(size_t);
	void deallocate(void*);
	template <template <typename T> class allocator, typename T>
	allocator<T> requestAllocator();
	private:
	Block* blocks;
	void* arena;
	block_count_t blockCount;
	size_t size;
};

}
#endif
