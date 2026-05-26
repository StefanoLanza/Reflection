#include <core/allocator.h>
#include <cstdlib>
#include <new>

class CustomAllocator final : public Typhoon::HeapAllocator {
public:
	void* alloc(size_t size, [[maybe_unused]] size_t alignment) override {
		totalAlloc += size;
		return ::malloc(size);
	}

	void free(void* ptr, [[maybe_unused]] size_t size) override {
		::free(ptr);
	}

	void* realloc(void* ptr, [[maybe_unused]] size_t currSize, size_t newSize, [[maybe_unused]] size_t alignment) override {
		return ::realloc(ptr, newSize);
	}

	size_t getTotalAlloc() const {
		return totalAlloc;
	}

private:
	size_t totalAlloc = 0;
};
