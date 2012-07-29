#include <limits>
#include <cstdlib>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>


#include "obstack.hpp"

namespace boost {
namespace obstack {
namespace detail {

void free_marker_dtor(void*) {
}
void array_of_primitives_dtor(void*) {
}

static size_t seed_from_heap_memory() {
	size_t const len = 64*1024 / sizeof(size_t);
	size_t * const mem = new size_t[len];
	size_t seed = reinterpret_cast<size_t>(mem);
	
	for(size_t i=0; i<len; i++) {
		seed ^= mem[i];
	}
	
	delete[] mem;
	return seed;
}

static size_t make_strong_seed() {
	size_t seed = 0;
	seed ^= seed_from_heap_memory();
	//TODO add more seed sources	
	return seed;
}

static void* init_fptr_cookie() {
	
	boost::mt19937 gen;
	size_t const seed = make_strong_seed();

	gen.seed(seed);
	boost::uniform_int<size_t> dist(0, std::numeric_limits<size_t>::max());

	size_t const cookie = dist(gen);
	
	return reinterpret_cast<void*>(cookie);
}

const void * const fptr_cookie = init_fptr_cookie();


void* malloc_allocator::alloc(size_t size) {
	return malloc(size);
}

void malloc_deallocator::dealloc(void*p) {
	free(p);
}

malloc_allocator global_malloc_allocator;
malloc_deallocator global_malloc_deallocator;
null_deallocator global_null_deallocator;

} //namespace detail
} //namespace obstack
} //namespace boost

