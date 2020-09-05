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

LinearAllocator::LinearAllocator(char* buffer, size_t bufferSize, Allocator* backup)
    : buffer(buffer)
    , offset(buffer)
    , bufferSize(bufferSize)
    , freeSize(bufferSize)
    , backup(backup) {
}

void* LinearAllocator::alloc(size_t size, size_t alignment) {
	void* result = std::align(alignment, size, offset, freeSize);
	if (result) {
		offset = advancePointer(result, size);
		freeSize = reinterpret_cast<uintptr_t>(buffer) + bufferSize - reinterpret_cast<uintptr_t>(offset);
	}
	else if (backup) {
		result = backup->alloc(size, alignment);
	}
	assert(result);
	return result;
}

void LinearAllocator::free(void* ptr, size_t size) {
	if (ptr >= buffer && ptr < buffer + bufferSize) {
		if (static_cast<char*>(ptr) + size == offset) {
			offset = advancePointer(offset, -static_cast<ptrdiff_t>(size));
			freeSize += size;
		}
	}
	else if (backup) {
		backup->free(ptr, size);
	}
	else {
		assert(false); // Not allocated by this allocator or its backup
	}
}

} // namespace Typhoon::Reflection