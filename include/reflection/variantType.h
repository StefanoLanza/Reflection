#pragma once

#include "type.h"

namespace Typhoon::Reflection {

class VariantType : public Type {
public:
	explicit VariantType(Allocator& allocator);
};

} // namespace Typhoon::Reflection
