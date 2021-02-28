#include "allocator.h"
#include "ptrUtil.h"

#include <cassert>
#include <memory>

namespace Typhoon {

void* HeapAllocator::alloc(size_t size, [[maybe_unused]] size_t alignment) {
#ifdef _MSC_VER
	return _aligned_malloc(size, alignment);
#else
	return ::malloc(size);
#endif
}

void HeapAllocator::free(void* ptr, size_t /*size*/) {
#ifdef _MSC_VER
	::_aligned_free(ptr);
#else
	::free(ptr);
#endif
}

void* HeapAllocator::realloc(void* ptr, size_t bytes, [[maybe_unused]] size_t alignment) {
#ifdef _MSC_VER
	return _aligned_realloc(ptr, bytes, alignment);
#else
	return ::realloc(ptr, bytes);
#endif
}

BufferAllocator::BufferAllocator(char* buffer, size_t bufferSize)
    : buffer(buffer)
    , offset(buffer)
    , bufferSize(bufferSize)
    , freeSize(bufferSize) {
}

void* BufferAllocator::alloc(size_t size, size_t alignment) {
	void* result = std::align(alignment, size, offset, freeSize);
	if (result) {
		offset = advancePointer(result, size);
		freeSize = reinterpret_cast<uintptr_t>(buffer) + bufferSize - reinterpret_cast<uintptr_t>(offset);
	}
	return result;
}

void BufferAllocator::rewind() {
	offset = buffer;
	freeSize = bufferSize;
}

void BufferAllocator::rewind(void* ptr) {
	if (ptr) {
		if (ptr >= buffer && ptr < static_cast<const char*>(buffer) + bufferSize) {
			offset = ptr;
			freeSize = reinterpret_cast<uintptr_t>(buffer) + bufferSize - reinterpret_cast<uintptr_t>(ptr);
			assert(freeSize <= bufferSize);
		}
	}
}

PagedAllocator::PagedAllocator(Allocator& parentAllocator, size_t pageSize)
    : allocator(&parentAllocator)
    , pageSize(pageSize)
    , rootPage(nullptr)
    , currPage(nullptr)
    , freeSize(0) {
}

PagedAllocator::~PagedAllocator() {
	for (Page* page = rootPage; page; page = page->next) {
		allocator->free(page->buffer, page->size);
	}
}

void* PagedAllocator::alloc(size_t size, size_t alignment) {
	if (! rootPage) {
		rootPage = allocPage();
		if (! rootPage) {
			return nullptr;
		}
		currPage = rootPage;
	}
	if (void* result = allocFromPage(*currPage, size, alignment); result) {
		return result;
	}
	if (size > pageSize) {
		return nullptr;
	}
	Page* newPage = allocPage();
	if (newPage) {
		newPage->prev = currPage;
		currPage->next = newPage;
		currPage = newPage;
		if (void* result = allocFromPage(*newPage, size, alignment); result) {
			return result;
		}
	}
	return nullptr;
}

void PagedAllocator::rewind() {
	for (Page* page = currPage; page; page = page->prev) {
		page->offset = page->buffer;
	}
	if (rootPage) {
		freeSize = rootPage->size;
	}
	currPage = rootPage;
}

void PagedAllocator::rewind(void* ptr) {
	if (ptr) {
		for (Page* page = currPage; page != nullptr; page = page->prev) {
			if (ptr >= page->buffer && ptr < static_cast<const char*>(page->buffer) + page->size) {
				page->offset = ptr;
				freeSize = reinterpret_cast<uintptr_t>(page->buffer) + page->size - reinterpret_cast<uintptr_t>(ptr);
				assert(freeSize <= page->size);
				currPage = page;
				break;
			}
		}
	}
}

PagedAllocator::Page* PagedAllocator::allocPage() {
	void* buffer = allocator->alloc(pageSize, Allocator::defaultAlignment);
	if (buffer) {
		Page newPage;
		newPage.next = nullptr;
		newPage.prev = nullptr;
		newPage.buffer = buffer;
		newPage.offset = static_cast<char*>(buffer) + sizeof(Page);
		newPage.size = pageSize - sizeof(Page);
		freeSize = pageSize;
		std::memcpy(buffer, &newPage, sizeof newPage);
		return static_cast<Page*>(buffer);
	}
	return nullptr;
}

void* PagedAllocator::allocFromPage(Page& page, size_t size, size_t alignment) {
	void* result = std::align(alignment, size, page.offset, freeSize);
	if (result) {
		page.offset = advancePointer(result, size);
		freeSize = reinterpret_cast<uintptr_t>(page.buffer) + page.size - reinterpret_cast<uintptr_t>(page.offset);
	}
	return result;
}

} // namespace Typhoon
