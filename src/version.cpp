#include "version.h"

namespace Typhoon::Reflection {

Version getVersion() {
	return { TY_REFLECTION_MAJOR_VERSION, TY_REFLECTION_MINOR_VERSION, TY_REFLECTION_PATCHLEVEL };
}

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

const char* getVersionString() {
	return STR(TY_REFLECTION_MAJOR_VERSION) "." STR(TY_REFLECTION_MINOR_VERSION) "." STR(TY_REFLECTION_PATCHLEVEL);
}

} // namespace Typhoon::REFLECTION
