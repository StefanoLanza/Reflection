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

bool InputArchive::readString(const char* key, const char*& str) {
	bool res = false;
	if (beginElement(key)) {
		res = readString(str);
		endElement();
	}
	return res;
}

bool OutputArchive::writeString(const char* key, const char* str) {
	bool res = false;
	if (beginElement(key)) {
		writeString(str);
		endElement();
	}
	return res;
}

} // namespace Typhoon::Reflection
