#include "archive.h"
#include <cassert>

namespace Typhoon::Reflection {

namespace detail {

Context& getContext();

}

ArchiveElement::ArchiveElement(InputArchive& archive, const char* tag)
    : archive { archive }
    , isValid { archive.beginElement(tag) } {
}

ArchiveElement::~ArchiveElement() {
	if (isValid) {
		archive.endElement();
	}
}

ArchiveElement::operator bool() const {
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
InputArchive::InputArchive()
    : context(detail::getContext()) {
}

bool InputArchive::read(void* data, TypeId typeId) {
	bool res = false;
	auto type = context.typeDB->tryGetType(typeId);
	if (type) {
		res = readAny(data, *type);
	}
	return res;
}

bool InputArchive::read(const char* key, void* data, TypeId typeId) {
	bool res = false;
	if (beginElement(key)) {
		auto type = context.typeDB->tryGetType(typeId);
		if (type) {
			res = readAny(data, *type);
		}
		endElement();
	}
	return res;
}

bool InputArchive::readAny(void* data, const Type& type) {
	return detail::readData(data, type, *this, context);
}

OutputArchive::OutputArchive()
    : context(detail::getContext()) {
}

void OutputArchive::write(const char* key, const void* data, TypeId typeId) {
	setKey(key);
	write(data, typeId);
}

void OutputArchive::write(const void* data, TypeId typeId) {
	auto type = context.typeDB->tryGetType(typeId);
	assert(type);
	detail::writeData(data, *type, *this, context);
}

void OutputArchive::write(const char* key, const char* str) {
	setKey(key);
	write(str);
}

} // namespace Typhoon::Reflection
