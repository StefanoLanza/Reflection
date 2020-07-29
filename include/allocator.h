#pragma once

#include "config.h"

namespace Typhoon::Reflection {

/**
 * @brief Allocator
 */
class Allocator {
public:
	virtual ~Allocator() = default;

	virtual void* alloc(size_t size, size_t alignment) = 0;
	virtual void  free(void* ptr, size_t size) = 0;

	template <class T>
	void* alloc() { return alloc(sizeof(T), alignof(T)); }
};

} // namespace Typhoon::Reflection
