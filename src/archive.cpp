#include "archive.h"
#include <cassert>

namespace Typhoon {

ArchiveElement::ArchiveElement(InputArchive& archive, const char* tagName)
    : archive(&archive)
    , isValid(archive.beginElement(tagName)) {
}

ArchiveElement::~ArchiveElement() {
	if (isValid) {
		archive->endElement();
	}
}

ArchiveElement::operator bool() const {
	return isValid;
}

WriteTag::WriteTag(OutputArchive& archive, const char* elementName)
    : archive(&archive)
    , isValid(archive.beginElement(elementName)) {
	assert(elementName);
}

WriteTag::~WriteTag() {
	if (isValid) {
		archive->endElement();
	}
}

WriteTag::operator bool() const {
	return isValid;
}

} // namespace Typhoon
