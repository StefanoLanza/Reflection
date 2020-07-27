#pragma once

#include "type.h"

namespace Typhoon::Reflection {

class BuiltinType : public Type {
public:
	BuiltinType(TypeId typeID, size_t size, size_t alignment, const MethodTable& methods);
};

} // namespace Typhoon::Reflection
