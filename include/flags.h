#pragma once

#include <cstdint>

namespace Typhoon {

struct ReflFlags {
	using Type = uint32_t;
	enum : Type {
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
};

} // namespace Typhoon