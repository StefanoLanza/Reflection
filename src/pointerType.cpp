#include "pointerType.h"

#include <cassert>
#include <cstring>
#include <bit>

namespace Typhoon::Reflection {

PointerType::PointerType(const char* typeName, TypeId typeID, size_t size, size_t alignment, const Type* pointedType, ArenaAllocator& allocator)
    : Type { typeName, typeID, Subclass::Pointer, size, alignment, {}, allocator }
    , pointedType { pointedType } {
	assert(pointedType);
}

const Type& PointerType::getPointedType() const {
	return *pointedType;
}

namespace detail {

RawPointerType::RawPointerType(const char* typeName, TypeId typeID, size_t size, size_t alignment, const Type* pointedType, ArenaAllocator& allocator)
    : PointerType { typeName, typeID, size, alignment, pointedType, allocator } {
}

const void* RawPointerType::resolvePointer(const void* data) const {
	const void* pointer = nullptr;
	std::memcpy(&pointer, data, sizeof pointer);
	return pointer;
}

DataPtr RawPointerType::resolvePointer(DataPtr data) const {
	DataPtr pointer = nullptr;
	std::memcpy(&pointer, data, sizeof pointer);
	return pointer;
}

} // namespace detail

} // namespace Typhoon::Reflection
