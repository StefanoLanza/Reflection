#include <include/allocator.h>
#include <new>

class CustomAllocator : public Typhoon::Reflection::Allocator {
public:
	void* alloc(size_t size, size_t /*alignment*/) override {
		totalAlloc += size;
		return ::malloc(size);
	}

	void free(void* ptr, size_t /*size*/) override {
		::free(ptr);
	}

	size_t getTotalAlloc() const {
		return totalAlloc;
	}

private:
	size_t totalAlloc = 0;
};
