#include "serializeBuiltIns.h"
#include "archive.h"
#include <cassert>

namespace Typhoon::Reflection {

bool read(char& data, const InputArchive& archive) {
	int  i = 0;
	bool res = false;
	if (archive.read(i)) {
		// check overflow
		res = (i >= -128 && i <= 127);
		data = static_cast<char>(i);
	}
	return res;
}

bool read(unsigned char& data, const InputArchive& archive) {
	unsigned int ui = 0;
	bool         res = false;
	if (archive.read(ui)) {
		// check overflow
		res = (ui <= 255);
		data = static_cast<unsigned char>(ui);
	}
	return res;
}

bool read(short& data, const InputArchive& archive) {
	int i = 0;
	if (archive.read(i)) {
		// TODO check overflow
		data = static_cast<short>(i);
		return true;
	}
	return false;
}

bool read(unsigned short& data, const InputArchive& archive) {
	unsigned int ui = 0;
	if (archive.read(ui)) {
		// TODO check overflow
		data = static_cast<unsigned short>(ui);
		return true;
	}
	return false;
}

bool read(int& data, const InputArchive& archive) {
	return archive.read(data);
}

bool read(unsigned int& data, const InputArchive& archive) {
	return archive.read(data);
}

bool read(long& data, const InputArchive& archive) {
	// InputArchive::read(long) overload does not exist
	if constexpr (std::is_same_v<long, int64_t>) {
		int64_t i64 = 0;
		if (archive.read(i64)) {
			data = static_cast<long>(i64);
			return true;
		}
	}
	else {
		int i = 0;
		if (archive.read(i)) {
			data = static_cast<long>(i);
			return true;
		}
	}
	return false;
}

bool read(unsigned long& data, const InputArchive& archive) {
	// InputArchive::read(unsigned long) overload does not exist
	if constexpr (std::is_same_v<unsigned long, uint64_t>) {
		uint64_t ui64 = 0;
		if (archive.read(ui64)) {
			data = static_cast<unsigned long>(ui64);
			return true;
		}
	}
	else {
		unsigned int ui = 0;
		if (archive.read(ui)) {
			data = static_cast<unsigned long>(ui);
			return true;
		}
	}
	return false;
}

bool read(long long& data, const InputArchive& archive) {
	bool res = false;
	if (int64_t i64 = 0; archive.read(i64)) {
		data = static_cast<long long>(i64);
		res = true;
	}
	return res;
}

bool read(unsigned long long& data, const InputArchive& archive) {
	if (uint64_t ui64 = 0; archive.read(ui64)) {
		data = static_cast<unsigned long long>(ui64);
		return true;
	}
	return false;
}

bool read(bool& data, const InputArchive& archive) {
	return archive.read(data);
}

bool read(float& data, const InputArchive& archive) {
	return archive.read(data);
}

bool read(double& data, const InputArchive& archive) {
	return archive.read(data);
}

bool read(const char*& data, const InputArchive& archive) {
	return archive.read(data);
}

void write(bool data, OutputArchive& archive) {
	archive.write(data);
}

void write(char data, OutputArchive& archive) {
	archive.write(static_cast<int>(data));
}

void write(unsigned char data, OutputArchive& archive) {
	archive.write(static_cast<unsigned int>(data));
}

void write(short data, OutputArchive& archive) {
	archive.write(static_cast<int>(data));
}

void write(unsigned short data, OutputArchive& archive) {
	archive.write(static_cast<unsigned int>(data));
}

void write(int data, OutputArchive& archive) {
	archive.write(data);
}

void write(unsigned int data, OutputArchive& archive) {
	archive.write(data);
}

void write(long data, OutputArchive& archive) {
	// OutputArchive::write(long) overload does not exist
	if constexpr (std::is_same_v<long, int64_t>) {
		archive.write(static_cast<int64_t>(data));
	}
	else {
		archive.write(static_cast<int>(data));
	}
}

void write(unsigned long data, OutputArchive& archive) {
	// OutputArchive::write(unsigned long) overload does not exist
	if constexpr (std::is_same_v<unsigned long, uint64_t>) {
		archive.write(static_cast<uint64_t>(data));
	}
	else {
		archive.write(static_cast<unsigned int>(data));
	}
}

void write(long long data, OutputArchive& archive) {
	static_assert(sizeof(long long) <= sizeof(int64_t));
	int64_t i64 = data;
	archive.write(i64);
}

void write(unsigned long long data, OutputArchive& archive) {
	static_assert(sizeof(unsigned long long) <= sizeof(uint64_t));
	uint64_t ui64 = data;
	archive.write(ui64);
}

void write(float data, OutputArchive& archive) {
	archive.write(data);
}

void write(double data, OutputArchive& archive) {
	archive.write(data);
}

void write(const char* str, OutputArchive& archive) {
	assert(str);
	archive.write(str);
}

} // namespace Typhoon::Reflection
