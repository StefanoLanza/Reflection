#include "bitMask.h"

#if 0

void test() {
	enum class Actions : uint8_t {
		move = 0x1,
		run = 0x2,
		jump = 0x4,
	};

	enum class Colors : uint8_t {
		red = 0x1,
		green = 0x2,
	};

	using ActionBitmask = Typhoon::Bitmask<Actions>;
	using ColorBitmask = Typhoon::Bitmask<Colors>;

	ActionBitmask actionBitmask { Actions::jump, Actions::move };
	ActionBitmask actionBitmask2 { Actions::jump };
	ColorBitmask  colorBitmask { Colors::red };

	actionBitmask.set(Actions::run);
	actionBitmask.unset(Actions::jump);
	[[maybe_unused]] bool jumping = actionBitmask.isSet(Actions::jump);
	actionBitmask.flip(Actions::jump);
	// actionBitmask.set(WrongEnum::red);
	actionBitmask | actionBitmask2;
	actionBitmask & actionBitmask2;
	actionBitmask ^ actionBitmask2;
	actionBitmask | Actions::run;
	actionBitmask |= Actions::run;
	ActionBitmask::Enum::jump;
}
#endif