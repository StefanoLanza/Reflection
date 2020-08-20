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

} // namespace Typhoon::Reflection
