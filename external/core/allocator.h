#pragma once

namespace Typhoon {

/**
 * @brief Allocator
 */
class Allocator {
public:
	virtual ~Allocator() = default;

	virtual void* alloc(size_t size, size_t alignment) = 0;
	virtual void  free(void* ptr, size_t size) = 0;

	template <class T>
	void* alloc() {
		return alloc(sizeof(T), alignof(T));
	}
};

/**
 * @brief Heap allocator
 */
class HeapAllocator : public Allocator {
public:
	void* alloc(size_t size, size_t alignment) override;
	void  free(void* ptr, size_t size) override;
};

/**
 * @brief Linear allocator
 */
class LinearAllocator : public Allocator {
public:
	LinearAllocator(char* buffer, size_t bufferSize, Allocator* backup);

	void* alloc(size_t size, size_t alignment) override;
	void  free(void* ptr, size_t size) override;

private:
	char*      buffer;
	void*      offset;
	size_t     bufferSize;
	size_t     freeSize;
	Allocator* backup;
};

} // namespace Typhoon::Reflection
