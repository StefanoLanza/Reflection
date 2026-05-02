#pragma once

#include <core/flags.h>

namespace Typhoon::Reflection {

enum Flag : uint32_t {
	none = 0,
	edit = 1,
	view = 2,
	readable = 4,
	writeable = 8,
	clonable = 16,
	editAndView = edit | view,
	readWrite = readable | writeable,
	all = edit | view | readable | writeable | clonable,
};

using Flags = Typhoon::Flags<Flag>;

} // namespace Typhoon::Reflection
