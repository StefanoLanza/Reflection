#pragma once

#include "type.h"
#include <cassert>
#include <core/span.h>
#include <cstring>

namespace Typhoon::Reflection {

struct Enumerator {
	const char* name;
	char        value[sizeof(long long)];
};

template <class T>
inline Enumerator makeEnumerator(const char* name, T value) {
	assert(name);
	Enumerator enumerator;
	enumerator.name = name;
	static_assert(sizeof enumerator.value >= sizeof value, "Increase storage");
	std::memcpy(&enumerator.value, &value, sizeof value);
	return enumerator;
}

class EnumType final : public Type {
public:
	EnumType(const char* typeName, TypeId typeID, size_t size, size_t alignment, const Enumerator enumConstants[], size_t count);

	span<const Enumerator> getEnumerators() const;
	const Enumerator*      findEnumeratorByValue(const void* value, size_t valueSize) const;
	const Enumerator*      findEnumeratorByName(const char* constantName) const;

private:
	const Enumerator* enumerators;
	size_t            numEnumerators;
};

} // namespace Typhoon::Reflection
