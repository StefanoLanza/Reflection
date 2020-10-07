#include "pointerType.h"
#include <cassert>

namespace Typhoon::Reflection {

PointerType::PointerType(const char* typeName, TypeId typeID, size_t size, size_t alignment, const Type* pointedType)
    : Type { typeName, typeID, Subclass::Pointer, size, alignment, {} }
    , pointedType { pointedType } {
	assert(pointedType);
}

const Type& PointerType::getPointedType() const {
	return *pointedType;
}

namespace detail {

RawPointerType::RawPointerType(const char* typeName, TypeId typeID, size_t size, size_t alignment, const Type* pointedType)
    : PointerType { typeName, typeID, size, alignment, pointedType } {
}

const void* RawPointerType::resolvePointer(const void* data) const {
	const void* pointer = nullptr;
	std::memcpy(&pointer, data, sizeof pointer);
	return pointer;
}

void* RawPointerType::resolvePointer(void* data) const {
	void* pointer = nullptr;
	std::memcpy(&pointer, data, sizeof pointer);
	return pointer;
}

} // namespace detail

} // namespace Typhoon::Reflection
