#include "archive.h"
#include "serializeBuiltIns.h"
#include <cassert>
#include <cstdio>

namespace Typhoon::Reflection {

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

bool read(char& data, InputArchive& archive) {
	int  i = 0;
	bool res = scan(&i, "%d", archive);
	res = res && (i >= -128 && i <= 127);
	data = static_cast<char>(i);
	return res;
}

bool read(unsigned char& data, InputArchive& archive) {
	int  i = 0;
	bool res = scan(&i, "%i", archive);
	res = res && (i >= 0 && i <= 255);
	data = static_cast<unsigned char>(i);
	return res;
}

bool read(short& data, InputArchive& archive) {
	return scan(&data, "%h", archive);
}

bool read(unsigned short& data, InputArchive& archive) {
	return scan(&data, "%hu", archive);
}

bool read(int& data, InputArchive& archive) {
	return scan(&data, "%d", archive);
}

bool read(unsigned int& data, InputArchive& archive) {
	return scan(&data, "%u", archive);
}

bool read(long& data, InputArchive& archive) {
	return scan(&data, "%ld", archive);
}

bool read(unsigned long& data, InputArchive& archive) {
	return scan(&data, "%lu", archive);
}

bool read(long long& data, InputArchive& archive) {
	return scan(&data, "%lld", archive);
}

bool read(unsigned long long& data, InputArchive& archive) {
	return scan(&data, "%llu", archive);
}

bool read(bool& data, InputArchive& archive) {
	bool res = false;
	if (const char* str = archive.currNodeText(); str) {
		data = (! _stricmp(str, "true")) || (std::atoi(str) != 0);
		res = true;
	}
	return res;
}

bool read(float& data, InputArchive& archive) {
	return scan(&data, "%f", archive);
}

bool read(double& data, InputArchive& archive) {
	return scan(&data, "%lf", archive);
}

bool read(const char*& data, InputArchive& archive) {
	data = archive.currNodeText();
	return data != nullptr;
}

bool write(bool data, OutputArchive& archive) {
	return archive.write(data ? "true" : "false");
}

bool write(char data, OutputArchive& archive) {
	return sprintf(static_cast<int>(data), "%d", archive);
}

bool write(unsigned char data, OutputArchive& archive) {
	return sprintf(static_cast<int>(data), "%d", archive);
}

bool write(short data, OutputArchive& archive) {
	return sprintf(static_cast<int>(data), "%d", archive);
}

bool write(unsigned short data, OutputArchive& archive) {
	return sprintf(static_cast<unsigned int>(data), "%u", archive);
}

bool write(int data, OutputArchive& archive) {
	return sprintf(data, "%d", archive);
}

bool write(unsigned int data, OutputArchive& archive) {
	return sprintf(data, "%u", archive);
}

bool write(long data, OutputArchive& archive) {
	return sprintf(data, "%ld", archive);
}

bool write(unsigned long data, OutputArchive& archive) {
	return sprintf(data, "%lu", archive);
}

bool write(long long data, OutputArchive& archive) {
	return sprintf(data, "%lld", archive);
}

bool write(unsigned long long data, OutputArchive& archive) {
	return sprintf(data, "%llu", archive);
}

bool write(float data, OutputArchive& archive) {
	return sprintf(data, "%f", archive);
}

bool write(double data, OutputArchive& archive) {
	return sprintf(data, "%lf", archive);
}

bool write(const char* str, OutputArchive& archive) {
	assert(str);
	return archive.write(str);
}

} // namespace Typhoon::Reflection
