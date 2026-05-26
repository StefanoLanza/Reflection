#pragma once

#include "type.h"

namespace Typhoon::Reflection {

class VariantType : public Type {
public:
	explicit VariantType(ArenaAllocator& allocator);
};

} // namespace Typhoon::Reflection
