#pragma once

#include "type.h"
#include <core/bitMask.h>
#include <core/span.h>
#include <cstdint>

namespace Typhoon::Reflection {

using BitMaskStorageType = uint64_t; // largest type that can represent bitmasks

struct BitMaskConstant {
	const char*        name;
	BitMaskStorageType mask;
};

class BitMaskType : public Type {
public:
	BitMaskType(const char* typeName, TypeId typeID, const Type* underlyingType, const BitMaskConstant enumerators[],
	            size_t numEnumerators);

	span<const BitMaskConstant> getEnumerators() const;
	const char*                 findConstantByValue(BitMaskStorageType value) const;
	BitMaskStorageType          findConstantByName(const char* enumeratorName) const;
	const Type&                 getUnderlyingType() const;

private:
	const Type*            underlyingType;
	const BitMaskConstant* enumerators;
	size_t                 numEnumerators;
};

} // namespace Typhoon::Reflection
