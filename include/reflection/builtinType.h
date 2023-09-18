#pragma once

#include "type.h"

namespace Typhoon::Reflection {

class BuiltinType : public Type {
public:
	BuiltinType(const char* typeName, TypeId typeID, size_t size, size_t alignment, const MethodTable& methods, Allocator& allocator);
};

} // namespace Typhoon::Reflection
