#include "memoryUtil.h"
#include <cassert>

namespace Typhoon {

namespace {

Allocator*       gHeapAllocator = nullptr;
LinearAllocator* gScratchAllocator = nullptr;
LinearAllocator* gFrameAllocator = nullptr;

} // namespace

void setGlobalHeapAllocator(Allocator* allocator) {
	gHeapAllocator = allocator;
}

Allocator& getGlobalHeapAllocator() {
	return *gHeapAllocator;
}

void setGlobalScratchAllocator(LinearAllocator* allocator) {
	gScratchAllocator = allocator;
}

LinearAllocator& getGlobalScratchAllocator() {
	return *gScratchAllocator;
}

void setGlobalFrameAllocator(LinearAllocator* allocator) {
	gFrameAllocator = allocator;
}

LinearAllocator& getGlobalFrameAllocator() {
	return *gFrameAllocator;
}

void* scratchAlloc(size_t size, size_t alignment) {
	assert(gScratchAllocator);
	return gScratchAllocator->Allocate(size, alignment);
}

void scratchFree(void* ptr) {
	if (ptr) {
		assert(gScratchAllocator);
		gScratchAllocator->Rewind(ptr);
	}
}

void* frameAlloc(size_t size, size_t alignment) {
	assert(gFrameAllocator);
	return gFrameAllocator->Allocate(size, alignment);
}

MemoryScope::MemoryScope(LinearAllocator& allocator) : allocator(&allocator), ptr(allocator.GetOffset()) {
}

MemoryScope::MemoryScope() : allocator(&getGlobalScratchAllocator()), ptr(allocator->GetOffset()) {
}

MemoryScope::~MemoryScope() {
	allocator->Rewind(ptr);
}

} // namespace Typhoon
