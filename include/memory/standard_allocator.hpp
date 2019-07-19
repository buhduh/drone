#ifndef MEMORY_STANDARD_ALLOCATOR_HPP
#define MEMORY_STANDARD_ALLOCATOR_HPP

template <typename T>
class StandardAllocator {
public:
	StandardAllocator(Arena arena);
	T* allocate(std::size_t);
	void deallocate(T*);
private:
	Arena arena;
};

#endif
