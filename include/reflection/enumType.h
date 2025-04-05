#pragma once

#include "type.h"

#include <cassert>
#include <cstring>
#include <span>

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
	EnumType(const char* typeName, TypeId typeID, size_t size, size_t alignment, const Enumerator enumConstants[], size_t count,
	         const Type* underlyingType, Allocator& allocator);

	std::span<const Enumerator> getEnumerators() const;
	const Enumerator*           findEnumeratorByValue(ConstDataPtr value, size_t valueSize) const;
	const Enumerator*           findEnumeratorByName(const char* constantName) const;
	const Type&                 getUnderlyingType() const;

private:
	const Enumerator* enumerators;
	size_t            numEnumerators;
	const Type*       underlyingType;
};

} // namespace Typhoon::Reflection
