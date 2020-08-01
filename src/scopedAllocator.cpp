#include "scopedAllocator.h"

namespace Typhoon::Reflection {

struct ScopedAllocator::Finalizer {
	void (*fn)(void* ptr);
	void*      obj;
	Finalizer* next;
	size_t     objSize;
};

ScopedAllocator::ScopedAllocator(Allocator& allocator)
    : allocator(allocator)
    , finalizerHead(nullptr) {
}

ScopedAllocator::~ScopedAllocator() {
	for (Finalizer *f = finalizerHead, *next = nullptr; f; f = next) {
		if (f->fn) {
			f->fn(f->obj);
		}
		next = f->next;
		void* obj = f->obj;
		size_t objSize = f->objSize;
		allocator.free(f, sizeof(Finalizer));
		allocator.free(obj, objSize);
	}
}

void ScopedAllocator::registerObject(void* obj, size_t objSize, Destructor destructor) {
	Finalizer* f = static_cast<Finalizer*>(allocator.alloc(sizeof(Finalizer), alignof(Finalizer)));
	f->fn = destructor;
	f->obj = obj;
	f->objSize = objSize;
	f->next = finalizerHead;
	finalizerHead = f;
}

} // namespace Typhoon::Reflection
