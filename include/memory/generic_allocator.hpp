#ifndef MEMORY_GENERIC_ALLOCATOR_HPP
#define MEMORY_GENERIC_ALLOCATOR_HPP
/*
A very stupid allocator.  No fragmentation checking, mostly as an experiment.  Probably shouldn't be used
for "real" code.
*/

namespace memory {
class GenericAllocator {
	public:
	allocate(size_t);
};
}
  
#endif
