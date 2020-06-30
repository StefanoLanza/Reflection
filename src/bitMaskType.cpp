#include "bitMaskType.h"
#include <cassert>
#include <cstring>

namespace Typhoon::Reflection {

BitMaskType::BitMaskType(TypeId typeID, size_t size, size_t alignment, const BitMaskConstant enumerators[], size_t numEnumerators)
    : Type(typeID, Subclass::BitMask, size, alignment, {})
    , enumerators(enumerators)
    , numEnumerators(numEnumerators) {
}

span<const BitMaskConstant> BitMaskType::getEnumerators() const {
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

} // namespace Typhoon
