#include <core/allocator.h>
#include <cstdlib>
#include <new>

class CustomAllocator final : public Typhoon::Allocator {
public:
	void* alloc(size_t size, size_t /*alignment*/) override {
		totalAlloc += size;
		return ::malloc(size);
	}

	void free(void* ptr, size_t /*size*/) override {
		::free(ptr);
	}

	void* realloc(void* ptr, size_t bytes, size_t /*alignment*/) override {
		return ::realloc(ptr, bytes);
	}

	size_t getTotalAlloc() const {
		return totalAlloc;
	}

private:
	size_t totalAlloc = 0;
};
