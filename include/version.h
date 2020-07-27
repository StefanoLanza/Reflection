#pragma once

#include <cstdint>

namespace Typhoon::Reflection {

#define TY_REFLECTION_MAJOR_VERSION 1
#define TY_REFLECTION_MINOR_VERSION 0
#define TY_REFLECTION_PATCHLEVEL    0

// This macro turns the version numbers into a numeric value
#define TY_REFLECTION_VERSIONNUM(X, Y, Z) ((X)*1000 + (Y)*100 + (Z))

#define TY_REFLECTION_COMPILEDVERSION TY_REFLECTION_VERSIONNUM(TY_REFLECTION_MAJOR_VERSION, TY_REFLECTION_MINOR_VERSION, TY_REFLECTION_PATCHLEVEL)

struct Version {
	uint8_t major; /**< major version */
	uint8_t minor; /**< minor version */
	uint8_t patch; /**< update version */
};

Version     getVersion();
const char* getVersionString();

} // namespace Typhoon::Reflection
