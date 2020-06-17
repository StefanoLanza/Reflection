#include "referenceType.h"
#include <cassert>

namespace Typhoon {

ReferenceType::ReferenceType(TypeId typeID, const Type* referencedType, bool bIsConst)
    : Type(typeID, Subclass::Reference, 0, 0, {})
    , referencedType(referencedType)
    , bIsConst(bIsConst) {
}

void* ReferenceType::resolvePointer(void* data) const {
	void* pointer = nullptr;
	std::memcpy(&pointer, &data, sizeof pointer);
	assert(pointer); // cannot have a null reference
	return pointer;
}

} // namespace Typhoon
