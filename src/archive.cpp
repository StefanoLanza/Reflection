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

WriteTag::WriteTag(OutputArchive& archive, const char* tag)
    : archive { archive }
    , isValid { archive.beginElement(tag) } {
	assert(tag);
}

WriteTag::~WriteTag() {
	if (isValid) {
		archive.endElement();
	}
}

WriteTag::operator bool() const {
	return isValid;
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

bool OutputArchive::write(const char* key, const void* data, TypeId typeId) {
	bool res = false;
	if (beginElement(key)) {
		res = write(data, typeId);
		endElement();
	}
	return res;
}

bool OutputArchive::write(const void* data, TypeId typeId) {
	auto type = context.typeDB->tryGetType(typeId);
	if (type) {
		return detail::writeData(data, *type, *this, context);
	}
	return false;
}

bool OutputArchive::write(const char* key, const char* str) {
	bool res = false;
	if (beginElement(key)) {
		res = write(str);
		endElement();
	}
	return res;
}

} // namespace Typhoon::Reflection
