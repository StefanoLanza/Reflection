#pragma once

#include "semantics.h"
#include <cstdint>

namespace Typhoon {

class Type;

struct Field {
	const char* name;
	const Type* type;
	size_t      offset;
	uint32_t    flags;
	Semantic    semantic;
};

} // namespace Typhoon
