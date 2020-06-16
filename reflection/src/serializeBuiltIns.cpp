#include "serializeBuiltIns.h"
#include "archive.h"
#include <cassert>
#include <cstdio>

namespace Typhoon {

namespace {

template <class T>
bool scan(T* data, const char* fmt, InputArchive& archive) {
	bool res = false;
	if (const char* str = archive.currNodeText(); str) {
		res = (sscanf_s(str, fmt, data) == 1);
	}
	return res;
}

template <class T>
bool sprintf(T data, const char* fmt, OutputArchive& archive) {
	char tmp[128];
	bool ok = sprintf_s(tmp, fmt, data) >= 0;
	ok = ok && archive.write(tmp);
	return ok;
}

} // namespace

bool serialize(char& data, InputArchive& archive) {
	int  i = 0;
	bool res = scan(&i, "%d", archive);
	res = res && (i >= -128 && i <= 127);
	data = static_cast<char>(i);
	return res;
}

bool serialize(unsigned char& data, InputArchive& archive) {
	int  i = 0;
	bool res = scan(&i, "%i", archive);
	res = res && (i >= 0 && i <= 255);
	data = static_cast<unsigned char>(i);
	return res;
}

bool serialize(short& data, InputArchive& archive) {
	return scan(&data, "%h", archive);
}

bool serialize(unsigned short& data, InputArchive& archive) {
	return scan(&data, "%hu", archive);
}

bool serialize(int& data, InputArchive& archive) {
	return scan(&data, "%d", archive);
}

bool serialize(unsigned int& data, InputArchive& archive) {
	return scan(&data, "%u", archive);
}

bool serialize(long& data, InputArchive& archive) {
	return scan(&data, "%ld", archive);
}

bool serialize(unsigned long& data, InputArchive& archive) {
	return scan(&data, "%lu", archive);
}

bool serialize(long long& data, InputArchive& archive) {
	return scan(&data, "%lld", archive);
}

bool serialize(unsigned long long& data, InputArchive& archive) {
	return scan(&data, "%llu", archive);
}

bool serialize(bool& data, InputArchive& archive) {
	bool res = false;
	if (const char* str = archive.currNodeText(); str) {
		data = (! _stricmp(str, "true")) || (std::atoi(str) != 0);
		res = true;
	}
	return res;
}

bool serialize(float& data, InputArchive& archive) {
	return scan(&data, "%f", archive);
}

bool serialize(double& data, InputArchive& archive) {
	return scan(&data, "%lf", archive);
}

bool serialize(bool data, OutputArchive& archive) {
	return archive.write(data ? "true" : "false");
}

bool serialize(char data, OutputArchive& archive) {
	return sprintf(static_cast<int>(data), "%d", archive);
}

bool serialize(unsigned char data, OutputArchive& archive) {
	return sprintf(static_cast<int>(data), "%d", archive);
}

bool serialize(short data, OutputArchive& archive) {
	return sprintf(static_cast<int>(data), "%d", archive);
}

bool serialize(unsigned short data, OutputArchive& archive) {
	return sprintf(static_cast<unsigned int>(data), "%u", archive);
}

bool serialize(int data, OutputArchive& archive) {
	return sprintf(data, "%d", archive);
}

bool serialize(unsigned int data, OutputArchive& archive) {
	return sprintf(data, "%u", archive);
}

bool serialize(long data, OutputArchive& archive) {
	return sprintf(data, "%ld", archive);
}

bool serialize(unsigned long data, OutputArchive& archive) {
	return sprintf(data, "%lu", archive);
}

bool serialize(long long data, OutputArchive& archive) {
	return sprintf(data, "%lld", archive);
}

bool serialize(unsigned long long data, OutputArchive& archive) {
	return sprintf(data, "%llu", archive);
}

bool serialize(float data, OutputArchive& archive) {
	return sprintf(data, "%f", archive);
}

bool serialize(double data, OutputArchive& archive) {
	return sprintf(data, "%lf", archive);
}

bool serialize(const char* str, OutputArchive& archive) {
	assert(str);
	return archive.write(str);
}

} // namespace Typhoon
