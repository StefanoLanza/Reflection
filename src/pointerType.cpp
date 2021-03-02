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

ConstDataPtr RawPointerType::resolvePointer(ConstDataPtr data) const {
	ConstDataPtr pointer = nullptr;
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
