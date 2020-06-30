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
	BitMaskType(TypeId typeID, size_t size, size_t alignment, const BitMaskConstant enumerators[], size_t numEnumerators);

	span<const BitMaskConstant> getEnumerators() const;
	const char*                 findConstantByValue(BitMaskStorageType value) const;
	BitMaskStorageType          findConstantByName(const char* enumeratorName) const;

private:
	const BitMaskConstant* enumerators;
	size_t                 numEnumerators;
};

} // namespace Typhoon
