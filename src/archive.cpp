#include "archive.h"
#include <cassert>

namespace Typhoon::Reflection {

ArchiveElement::ArchiveElement(InputArchive& archive, const char* tag)
    : archive(&archive)
    , isValid(archive.beginElement(tag)) {
}

ArchiveElement::~ArchiveElement() {
	if (isValid) {
		archive->endElement();
	}
}

ArchiveElement::operator bool() const {
	return isValid;
}

WriteTag::WriteTag(OutputArchive& archive, const char* tag)
    : archive(&archive)
    , isValid(archive.beginElement(tag)) {
	assert(tag);
}

WriteTag::~WriteTag() {
	if (isValid) {
		archive->endElement();
	}
}

WriteTag::operator bool() const {
	return isValid;
}

bool InputArchive::read(const char* key, void* data, TypeId typeId) {
	bool res = false;
	if (beginElement(key)) {
		res = detail::readData(data, typeId, *this);
		endElement();
	}
	return res;
}

bool InputArchive::read(void* data, TypeId typeId) {
	return detail::readData(data, typeId, *this);
}

bool OutputArchive::write(const char* key, const void* data, TypeId typeId) {
	bool res = false;
	if (beginElement(key)) {
		res = detail::writeData(data, typeId, *this);
		endElement();
	}
	return res;
}

bool OutputArchive::write(void* data, TypeId typeId) {
	return detail::writeData(data, typeId, *this);
}

bool OutputArchive::write(const char* key, const char* str) {
	bool res = false;
	if (beginElement(key)) {
		res = write(str);
		endElement();
	}
	return false;
}

} // namespace Typhoon::Reflection
