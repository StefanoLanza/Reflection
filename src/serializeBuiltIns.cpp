#include "serializeBuiltIns.h"
#include "archive.h"
#include <cassert>

namespace Typhoon::Reflection {

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
	int64_t i64;
	bool    res = false;
	if (archive.readInt64(i64)) {
		data = static_cast<long>(i64);
		res = true;
	}
	return res;
}

bool read(unsigned long& data, InputArchive& archive) {
	uint64_t i64;
	bool     res = false;
	if (archive.readUInt64(i64)) {
		data = static_cast<unsigned long>(i64);
		res = true;
	}
	return res;
}

bool read(long long& data, InputArchive& archive) {
	int64_t i64;
	bool    res = false;
	if (archive.readInt64(i64)) {
		data = static_cast<long long>(i64);
		res = true;
	}
	return res;
}

bool read(unsigned long long& data, InputArchive& archive) {
	uint64_t i64;
	bool     res = false;
	if (archive.readUInt64(i64)) {
		data = static_cast<unsigned long long>(i64);
		res = true;
	}
	return res;
}

bool read(bool& data, InputArchive& archive) {
	return archive.readBool(data);
}

bool read(float& data, InputArchive& archive) {
	return archive.readFloat(data);
}

bool read(double& data, InputArchive& archive) {
	return archive.readDouble(data);
}

bool read(const char*& data, InputArchive& archive) {
	return archive.readString(data);
}

bool write(bool data, OutputArchive& archive) {
	return archive.write(data);
}

bool write(char data, OutputArchive& archive) {
	return archive.write(static_cast<int>(data));
}

bool write(unsigned char data, OutputArchive& archive) {
	return archive.write(static_cast<unsigned int>(data));
}

bool write(short data, OutputArchive& archive) {
	return archive.write(static_cast<int>(data));
}

bool write(unsigned short data, OutputArchive& archive) {
	return archive.write(static_cast<unsigned short>(data));
}

bool write(int data, OutputArchive& archive) {
	return archive.write(data);
}

bool write(unsigned int data, OutputArchive& archive) {
	return archive.write(data);
}

bool write(long data, OutputArchive& archive) {
	if constexpr (sizeof(long) == sizeof(int64_t)) {
		return archive.write(static_cast<int64_t>(data));
	}
	else {
		return archive.write(static_cast<int>(data));
	}
}

bool write(unsigned long data, OutputArchive& archive) {
	if constexpr (sizeof(long) == sizeof(uint64_t)) {
		return archive.write(static_cast<uint64_t>(data));
	}
	else {
		return archive.write(static_cast<unsigned int>(data));
	}
}

bool write(long long data, OutputArchive& archive) {
	return archive.write(data);
}

bool write(unsigned long long data, OutputArchive& archive) {
	return archive.write(data);
}

bool write(float data, OutputArchive& archive) {
	return archive.write(data);
}

bool write(double data, OutputArchive& archive) {
	return archive.write(data);
}

bool write(const char* str, OutputArchive& archive) {
	assert(str);
	return archive.write(str);
}

} // namespace Typhoon::Reflection
