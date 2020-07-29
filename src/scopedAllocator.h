#pragma once

#include "allocator.h"
#include <type_traits>

namespace Typhoon::Reflection {

struct Finalizer {
	void (*fn)(void* ptr);
	void*      obj;
	Finalizer* next;
};

template <typename T>
void destructorCall(void* ptr) {
	static_cast<T*>(ptr)->~T();
}

class ScopedAllocator {
public:
	ScopedAllocator(Allocator& allocator);

	~ScopedAllocator();

	template <class T, class... ArgTypes>
	T* make(ArgTypes... args) {
		void* ptr = allocator.alloc(sizeof(T), alignof(T));
		if constexpr (! std::is_trivially_default_constructible_v<T>) {
			Finalizer* f = new (allocator.alloc(sizeof(Finalizer), alignof(Finalizer))) Finalizer;
			f->fn = destructorCall<T>;
			f->obj = ptr;
			f->next = finalizerHead;
			finalizerHead = f;
		}
		return new (ptr) T(std::forward<ArgTypes>(args)...);
	}

private:
	Allocator& allocator;
	Finalizer* finalizerHead;
};

} // namespace Typhoon::Reflection
