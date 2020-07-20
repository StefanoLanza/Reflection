#pragma once

#include "allocator.h"

namespace Typhoon::Reflection {

namespace detail {

Allocator& getAllocator();
void*      alloc(size_t size, size_t alignment);
void       free(void* ptr, size_t size);

} // namespace detail

} // namespace Typhoon::Reflection
