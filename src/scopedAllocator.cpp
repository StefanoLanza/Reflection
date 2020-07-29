#include "scopedAllocator.h"

namespace Typhoon::Reflection {

ScopedAllocator::ScopedAllocator(Allocator& allocator)
    : allocator(allocator)
    , finalizerHead(nullptr) {
}

ScopedAllocator::~ScopedAllocator() {
	for (Finalizer* f = finalizerHead; f; f = f->next) {
		f->fn(f->obj);
	}
}

} // namespace Typhoon::Reflection
