#include "bitMaskType.h"
#include <cassert>
#include <cstring>

namespace Typhoon::Reflection {

BitMaskType::BitMaskType(const char* typeName, TypeId typeID, const Type* underlyingType, const BitMaskConstant enumerators[], size_t numEnumerators,
                         Allocator& allocator)
    : Type(typeName, typeID, Subclass::BitMask, underlyingType->getSize(), underlyingType->getAlignment(), {}, allocator)
    , underlyingType(underlyingType)
    , enumerators(enumerators)
    , numEnumerators(numEnumerators) {
}

std::span<const BitMaskConstant> BitMaskType::getEnumerators() const {
	return { enumerators, numEnumerators };
}

const char* BitMaskType::findConstantByValue(BitMaskStorageType value) const {
	for (size_t i = 0; i < numEnumerators; ++i) {
		if (enumerators[i].mask == value) {
			return enumerators[i].name;
		}
	}
	return nullptr;
}

BitMaskStorageType BitMaskType::findConstantByName(const char* enumeratorName) const {
	assert(enumeratorName);
	for (size_t i = 0; i < numEnumerators; ++i) {
		if (strcmp(enumerators[i].name, enumeratorName) == 0) {
			return enumerators[i].mask;
		}
	}
	return 0;
}

const Type& BitMaskType::getUnderlyingType() const {
	return *underlyingType;
}

} // namespace Typhoon::Reflection
