#include "stringUtils.h"
#include <cassert>

namespace Typhoon {

void StringTo(const std::string& str, bool& b) {
	if (str == "TRUE" || str == "true") {
		b = true;
	}
	else if (str == "FALSE" || str == "false") {
		b = false;
	}
	else {
		std::istringstream is(str);
		is >> b;
	}
}

bool StringToBool(const char* str) {
	assert(str);
	return (! _stricmp(str, "true")) || (std::atoi(str) != 0);
}

double StringToDouble(const char* str) {
	assert(str);
	return std::atof(str);
}

int StringToInt(const char* str) {
	assert(str);
	return std::atoi(str);
}

long StringToLong(const char* str) {
	assert(str);
	return std::atol(str);
}

long long StringToLongLong(const char* str) {
	assert(str);
	return std::atoll(str);
}

} // namespace Typhoon
