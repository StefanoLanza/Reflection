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

LinearAllocator::LinearAllocator(char* buffer, size_t bufferSize, Allocator& allocator, size_t pageSize)
    : allocator(&allocator)
    , pageSize(pageSize)
	, currPage(&rootPage)
    , freeSize(bufferSize)
{
	rootPage.buffer = buffer;
	rootPage.offset = buffer;
	rootPage.next = nullptr;
	rootPage.prev = nullptr;
	rootPage.size = bufferSize;
}

LinearAllocator::LinearAllocator(char* buffer, size_t bufferSize)
	: allocator(nullptr)
    , pageSize(0)
	, currPage(&rootPage)
    , freeSize(bufferSize)
{
	rootPage.buffer = buffer;
	rootPage.offset = buffer;
	rootPage.next = nullptr;
	rootPage.prev = nullptr;
	rootPage.size = bufferSize;
}

   LinearAllocator::LinearAllocator(Allocator& allocator, size_t pageSize)
    : allocator(&allocator)
    , pageSize(pageSize)
    , currPage(&rootPage)
    , freeSize(0)
 {
	rootPage.buffer = nullptr;
	rootPage.offset = nullptr;
	rootPage.next = nullptr;
	rootPage.prev = nullptr;
	rootPage.size = 0;
}

LinearAllocator::~LinearAllocator() {
	for (Page* page = rootPage.next; page; page = page->next) {
		allocator->free(page->buffer, page->size);
	}
}

void* LinearAllocator::alloc(size_t size, size_t alignment) {
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
		freeSize = pageSize;
		if (void* result = allocFromPage(*newPage, size, alignment); result) {
			return result;
		}
	}
	return nullptr;
}

void LinearAllocator::rewind() {
	for (Page* page = currPage; page; page = page->prev) {
		page->offset = page->buffer;
	}
	freeSize = rootPage.size;
	currPage = &rootPage;
}

void LinearAllocator::rewind(void* ptr) {
	if (ptr) {
		for (Page* page = currPage; page; page = page->prev) {
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

void* LinearAllocator::getBuffer() const {
	return currPage->buffer;
}

LinearAllocator::Page* LinearAllocator::allocPage() {
	void* buffer = allocator->alloc(pageSize, Allocator::defaultAlignment);
	if (buffer) {
		Page newPage;
		newPage.next = nullptr;
		newPage.prev = nullptr;
		newPage.buffer = buffer;
		newPage.offset = static_cast<char*>(buffer) + sizeof(Page);
		newPage.size = pageSize - sizeof(Page);
		std::memcpy(buffer, &newPage, sizeof newPage);
		return static_cast<Page*>(buffer);
	}
	return nullptr;
}

void* LinearAllocator::allocFromPage(Page& page, size_t size, size_t alignment) {
	void* result = std::align(alignment, size, page.offset, freeSize);
	if (result) {
		page.offset = advancePointer(result, size);
		freeSize = reinterpret_cast<uintptr_t>(page.buffer) + page.size - reinterpret_cast<uintptr_t>(page.offset);
	}
	return result;
}


} // namespace Typhoon
