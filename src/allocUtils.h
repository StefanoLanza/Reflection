#pragma once

namespace Typhoon::Reflection {


class Allocator;

namespace detail {

Allocator& getAllocator();
void* alloc(size_t size, size_t alignment);
void  free(void* ptr, size_t size);

template <class T, class... ArgTypes>
T* make(ArgTypes... args) {
	void* ptr = alloc(sizeof(T), alignof(T));
	return new (ptr) T(std::forward<ArgTypes>(args)...);
}

}

}
