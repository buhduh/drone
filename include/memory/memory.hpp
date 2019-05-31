#ifndef MEMORY_MEMORY_HPP
#define MEMORY_MEMORY_HPP
namespace memory {

template <typename T>
struct Block {
	size_t size;
	T* ptr;
};

/*
Requests memory directly from the heap with malloc.  Probably should be used sparringly, if at all...
Also probably the "base" allocator.
*/
class HeapAllocator {
	public:
	template<typename T>
	Block<T> allocate(size_t);
	template<typename T>
	void deallocate(Block<T>&);

	private:
};

template<typename T>
Block<T> HeapAllocator::allocate(size_t size) {
	void* ptr = malloc(size);
	size_t toRet(size);
	if(ptr == nullptr) {
		toRet = 0;
	}
	return Block<T>{toRet, reinterpret_cast<T*>(ptr)};
}

template<typename T>
void HeapAllocator::deallocate(Block<T>& blk) {
	blk.size = 0;
	free(blk.ptr);
	blk.ptr = nullptr;
}

//manages internal state with an ordered list of initialized pointers
//allocations search memory space for first block found of appropriate size
//deallocations simply remove the ptr for the ordered list
class SimpleAllocator {
	public:
};

}
#endif
