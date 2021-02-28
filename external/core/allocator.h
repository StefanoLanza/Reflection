#pragma once

#include <type_traits>
#include <cstddef>

namespace Typhoon {

/**
 * @brief Allocator
 */
class Allocator {
public:
	virtual ~Allocator() = default;

	virtual void* alloc(size_t size, size_t alignment) = 0;
	virtual void  free(void* ptr, size_t size) = 0;
	virtual void* realloc(void* ptr, size_t bytes, size_t alignment) = 0;

	template <class T>
	T* alloc() {
		return static_cast<T*>(alloc(sizeof(T), alignof(T)));
	}

	template <class T>
	T* allocArray(size_t count) {
		static_assert(std::is_pod_v<T>);
		return static_cast<T*>(alloc(sizeof(T) * count, alignof(T)));	
	}

	static constexpr size_t defaultAlignment = alignof(void *);
};

/**
 * @brief Heap allocator
 */
class HeapAllocator final : public Allocator {
public:
	void* alloc(size_t size, size_t alignment) override;
	void  free(void* ptr, size_t size) override;
	void* realloc(void* ptr, size_t bytes, size_t alignment) override;
};

/**
 * @brief Linear allocator
 */
class LinearAllocator {
public:
	virtual ~LinearAllocator() = default;

	virtual void*  alloc(size_t size, size_t alignment) = 0;
	virtual void  rewind() = 0;
	virtual void   rewind(void* ptr) = 0;
};

/**
 * @brief Buffer allocator
 */
class BufferAllocator : public LinearAllocator {
public:
	BufferAllocator(char* buffer, size_t bufferSize);

	void* alloc(size_t size, size_t alignment) override;
	void  rewind() override;
	void  rewind(void* ptr) override;
	void* getBuffer() const;
	void* getOffset() const;

private:
	void*      buffer;
	void*  offset;
	size_t bufferSize;
	size_t     freeSize;
};

inline void* BufferAllocator::getBuffer() const {
	return buffer;
}

inline void* BufferAllocator::getOffset() const {
	return offset;
}

/**
 * @brief Paged allocator
 */

class PagedAllocator : public LinearAllocator {
public:
	PagedAllocator(Allocator& parentAllocator, size_t pageSize);
	~PagedAllocator();

	void* alloc(size_t size, size_t alignment) override;
	void  rewind() override;
	void  rewind(void* ptr) override;

	static constexpr size_t defaultPageSize = 65536;

private:
	struct Page;
	Page* allocPage();
	void* allocFromPage(Page& page, size_t size, size_t alignment);

private:
	struct Page {
		Page*  next;
		Page*  prev;
		void*  buffer;
		void*  offset;
		size_t size;
	};
	Allocator* allocator;
	size_t     pageSize;
	Page*      rootPage;
	Page*      currPage;
	size_t     freeSize;
};

} // namespace Typhoon
