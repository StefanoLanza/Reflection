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
		res = (ui >= 0 && ui <= 255);
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
	// InputArchive::read(unsigned long) overload does not exist
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
	int64_t i64;
	bool    res = false;
	if (archive.read(i64)) {
		data = static_cast<long long>(i64);
		res = true;
	}
	return res;
}

bool read(unsigned long long& data, const InputArchive& archive) {
	static_assert(std::is_same_v<unsigned long long, uint64_t>);
	return archive.read(data);
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
	return archive.write(static_cast<unsigned int>(data));
}

bool write(int data, OutputArchive& archive) {
	return archive.write(data);
}

bool write(unsigned int data, OutputArchive& archive) {
	return archive.write(data);
}

bool write(long data, OutputArchive& archive) {
	// OutputArchive::write(long) overload does not exist
	if constexpr (std::is_same_v<long, int64_t>) {
		return archive.write(static_cast<int64_t>(data));
	}
	else {
		return archive.write(static_cast<int>(data));
	}
}

bool write(unsigned long data, OutputArchive& archive) {
	// OutputArchive::write(unsigned long) overload does not exist
	if constexpr (std::is_same_v<unsigned long, uint64_t>) {
		return archive.write(static_cast<uint64_t>(data));
	}
	else {
		return archive.write(static_cast<unsigned int>(data));
	}
}

bool write(long long data, OutputArchive& archive) {
	static_assert(std::is_same_v<long long, int64_t>);
	return archive.write(data);
}

bool write(unsigned long long data, OutputArchive& archive) {
	static_assert(std::is_same_v<unsigned long long, uint64_t>);
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
