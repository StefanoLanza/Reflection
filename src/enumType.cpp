#include "enumType.h"

namespace Typhoon::Reflection {

EnumType::EnumType(TypeId typeID, const char* name, size_t size, size_t alignment, const Enumerator enumConstants[], size_t count)
    : Type(typeID, Subclass::Enum, size, alignment, {})
    , name(name)
    , enumerators(enumConstants)
    , numEnumerators(count) {
}

const char* EnumType::getName() const {
	return name;
}

span<const Enumerator> EnumType::getEnumerators() const {
	return { enumerators, numEnumerators };
}

const Enumerator* EnumType::findEnumeratorByValue(const void* value, size_t valueSize) const {
	for (size_t i = 0; i < numEnumerators; ++i) {
		if (std::memcmp(enumerators[i].value, value, valueSize) == 0) {
			return &enumerators[i];
		}
	}
	return nullptr;
}

const Enumerator* EnumType::findEnumeratorByName(const char* constantName) const {
	for (size_t i = 0; i < numEnumerators; ++i) {
		if (strcmp(enumerators[i].name, constantName) == 0) {
			return &enumerators[i];
		}
	}
	return nullptr;
}

} // namespace Typhoon::Reflection
