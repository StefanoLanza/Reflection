#include "archive.h"
#include <cassert>

namespace Typhoon::Reflection {

namespace detail {

Context& getContext();

}

InputArchive::InputArchive()
    : context { detail::getContext() } {
}

bool InputArchive::read(void* data, TypeId typeId) const {
	bool res = false;
	if (auto type = context.typeDB->tryGetType(typeId); type) {
		res = readAny(data, *type);
	}
	return res;
}

bool InputArchive::read(const char* key, void* data, TypeId typeId) const {
	bool res = false;
	if (beginElement(key)) {
		if (auto type = context.typeDB->tryGetType(typeId); type) {
			res = readAny(data, *type);
		}
		endElement();
	}
	return res;
}

bool InputArchive::readAny(void* data, const Type& type) const {
	return detail::readData(data, type, *this, context);
}

OutputArchive::OutputArchive()
    : context { detail::getContext() } {
}

void OutputArchive::write(const char* key, const void* data, TypeId typeId) {
	setKey(key);
	write(data, typeId);
}

void OutputArchive::write(const void* data, TypeId typeId) {
	if (auto type = context.typeDB->tryGetType(typeId); type) {
		detail::writeData(data, *type, *this, context);
	}
}

void OutputArchive::write(const char* key, const char* str) {
	setKey(key);
	write(str);
}

ArrayReadScope::ArrayReadScope(InputArchive& archive, const char* key)
    : archive { archive }
    , hasKey { archive.beginElement(key) }
    , isValid { hasKey && archive.isArray() } {
}

ArrayReadScope::ArrayReadScope(InputArchive& archive)
    : archive { archive }
    , hasKey { false }
    , isValid { archive.isArray() } {
}

ArrayReadScope::~ArrayReadScope() {
	if (hasKey) {
		archive.endElement();
	}
}

ArrayReadScope::operator bool() const {
	return isValid;
}

ObjectReadScope::ObjectReadScope(const InputArchive& archive, const char* key)
    : archive { archive }
    , hasKey { archive.beginElement(key) }
    , isValid { hasKey && archive.isObject() } {
}

ObjectReadScope::ObjectReadScope(const InputArchive& archive)
    : archive { archive }
    , hasKey { false }
    , isValid { archive.isObject() } {
}

ObjectReadScope::~ObjectReadScope() {
	if (hasKey) {
		archive.endElement();
	}
}

ObjectReadScope::operator bool() const {
	return isValid;
}

ArrayWriteScope::ArrayWriteScope(OutputArchive& archive, const char* key)
    : archive { archive } {
	if (key) {
		archive.setKey(key);
	}
	archive.beginArray();
}

ArrayWriteScope::~ArrayWriteScope() {
	archive.endArray();
}

ObjectWriteScope::ObjectWriteScope(OutputArchive& archive, const char* key)
    : archive { archive } {
	if (key) {
		archive.setKey(key);
	}
	archive.beginObject();
}

ObjectWriteScope::~ObjectWriteScope() {
	archive.endObject();
}

} // namespace Typhoon::Reflection
