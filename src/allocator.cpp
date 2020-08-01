#include "allocator.h"
#include <core/ptrUtil.h>

#include <cassert>
#include <memory>

namespace Typhoon::Reflection {

void* HeapAllocator::alloc(size_t size, size_t /*alignment*/) {
	return ::malloc(size);
}

void HeapAllocator::free(void* ptr, size_t /*size*/) {
	::free(ptr);
}

LinearAllocator::LinearAllocator(char* buffer, size_t bufferSize)
    : buffer(buffer)
    , offset(buffer)
    , bufferSize(bufferSize)
    , freeSize(bufferSize) {
}

void* LinearAllocator::alloc(size_t size, size_t alignment) {
	void* const result = std::align(alignment, size, offset, freeSize);
	assert(result);
	offset = advancePointer(result, size);
	freeSize = reinterpret_cast<uintptr_t>(buffer) + bufferSize - reinterpret_cast<uintptr_t>(offset);
	return result;
}

void LinearAllocator::free(void* ptr, size_t size) {
	if (static_cast<char*>(ptr) + size == offset) {
		offset = advancePointer(offset, -static_cast<ptrdiff_t>(size));
		freeSize += size;
	}
}

} // namespace Typhoon::Reflection
