#pragma once

#include "type.h"

namespace Typhoon {

class BuiltinType : public Type {
public:
	BuiltinType(TypeId typeID, size_t size, size_t alignment, const MethodTable& methods);
};

} // namespace Typhoon
