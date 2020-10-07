#include "serializeBuiltIns.h"
#include "archive.h"
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
	if (ok) {
		archive.write(tmp);
	}
	return ok;
}

} // namespace

bool read(char& data, InputArchive& archive) {
	int  i = 0;
	bool res = false;
	if (archive.readInt(i)) {
		// check overflow
		res = (i >= -128 && i <= 127);
		data = static_cast<char>(i);
	}
	return res;
}

bool read(unsigned char& data, InputArchive& archive) {
	unsigned int ui = 0;
	bool         res = false;
	if (archive.readUInt(ui)) {
		// check overflow
		res = (ui >= 0 && ui <= 255);
		data = static_cast<unsigned char>(ui);
	}
	return res;
}

bool read(short& data, InputArchive& archive) {
	int i = 0;
	if (archive.readInt(i)) {
		// TODO check overflow
		data = static_cast<short>(i);
		return true;
	}
	return false;
}

bool read(unsigned short& data, InputArchive& archive) {
	unsigned int ui = 0;
	if (archive.readUInt(ui)) {
		// TODO check overflow
		data = static_cast<unsigned short>(ui);
		return true;
	}
	return false;
}

bool read(int& data, InputArchive& archive) {
	return archive.readInt(data);
}

bool read(unsigned int& data, InputArchive& archive) {
	return archive.readUInt(data);
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
	return archive.readBool(data);
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
	archive.writeBool(data);
	return true;
}

bool write(char data, OutputArchive& archive) {
	archive.writeInt(static_cast<int>(data));
	return true;
}

bool write(unsigned char data, OutputArchive& archive) {
	archive.writeUInt(static_cast<unsigned int>(data));
	return true;
}

bool write(short data, OutputArchive& archive) {
	archive.writeInt(static_cast<int>(data));
	return true;
}

bool write(unsigned short data, OutputArchive& archive) {
	archive.writeUInt(static_cast<unsigned short>(data));
	return true;
}

bool write(int data, OutputArchive& archive) {
	archive.writeInt(data);
	return true;
}

bool write(unsigned int data, OutputArchive& archive) {
	archive.writeUInt(data);
	return true;
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
	archive.write(str);
	return true;
}

} // namespace Typhoon::Reflection
