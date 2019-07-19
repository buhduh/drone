#ifndef MEMORY_FREE_ALLOCATOR_HPP
#define MEMORY_FREE_ALLOCATOR_HPP

#include <memory>

namespace memory {

template <typename T>
class FreeAllocator : allocator_traits<FreeAllocator<T>> {
	public:
	FreeAllocator<T> allocator_type;
	T value_type;
	size_t size_type;
	static T* allocate(FreeAllocator<T>&, allocator_type::size_type);
	static void deallocate(FreeAllocator&, T*, 
};

}
#endif
