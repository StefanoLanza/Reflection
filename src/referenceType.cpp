#include "referenceType.h"
#include <cassert>
#include <cstring>

namespace Typhoon::Reflection {

ReferenceType::ReferenceType(TypeId typeID, const Type* referencedType, bool bIsConst, Allocator& allocator)
    : Type("&", typeID, Subclass::Reference, 0, 0, {}, allocator)
    , referencedType(referencedType)
    , bIsConst(bIsConst) {
}

DataPtr ReferenceType::resolvePointer(DataPtr data) const {
	DataPtr pointer = nullptr;
	std::memcpy(&pointer, &data, sizeof pointer);
	assert(pointer); // cannot have a null reference
	return pointer;
}

ConstDataPtr ReferenceType::resolvePointer(ConstDataPtr data) const {
	ConstDataPtr pointer = nullptr;
	std::memcpy(&pointer, &data, sizeof pointer);
	assert(pointer); // cannot have a null reference
	return pointer;
}

} // namespace Typhoon::Reflection
