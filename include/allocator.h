#pragma once

#include "config.h"

#include <utility>

namespace Typhoon::Reflection {

/**
 * @brief Allocator
 */
class Allocator {
public:
	virtual ~Allocator() = default;

	virtual void* alloc(size_t size, size_t alignment) = 0;
	virtual void  free(void* ptr, size_t size) = 0;

	template <class T, class... ArgTypes>
	T* make(ArgTypes... args) {
		void* ptr = alloc(sizeof(T), alignof(T));
		return new (ptr) T(std::forward<ArgTypes>(args)...);
	}

	template <class T>
	void destroy(T* ptr) {
		if (ptr) {
			ptr->~T();
			free(ptr, sizeof(T));
		}
	}
};

} // namespace Typhoon::Reflection
