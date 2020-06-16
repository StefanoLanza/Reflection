#pragma once

#include <iomanip>
#include <sstream>
#include <vector>

namespace Typhoon {

//! Converts a value to a string
template <typename T>
const std::string toString(const T& v, std::streamsize precision = 4) {
	std::ostringstream os;
	os << std::fixed << std::setprecision(precision) << v;
	return os.str();
}

//! Converts a boolean value to a string
inline const std::string toString(bool v) {
	return v ? "true" : "false";
}

//! Converts a string to a primitive value
template <typename T>
inline void StringTo(const std::string& str, T& prim) {
	std::istringstream is(str);
	is >> prim;
}

bool      StringToBool(const char* str);
double    StringToDouble(const char* str);
int       StringToInt(const char* str);
double    StringToDouble(const char* str);
long      StringToLong(const char* str);
long long StringToLongLong(const char* str);

void StringTo(const std::string& str, bool& b);

} // namespace Typhoon
