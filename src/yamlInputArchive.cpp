#include "YAMLInputArchive.h"

#if TY_REFLECTION_YAML

#include <cassert>

namespace Typhoon::Reflection {

YAMLInputArchive::YAMLInputArchive() {
}

YAMLInputArchive::~YAMLInputArchive() = default;

bool YAMLInputArchive::initialize(const char* buffer) {
	return false;
}

std::string YAMLInputArchive::getErrorDesc() const {
	return {};
}

const char* YAMLInputArchive::currNodeText() {
	return nullptr;
}

void YAMLInputArchive::endElement() {
}

bool YAMLInputArchive::beginElement() {
	return false;
}

bool YAMLInputArchive::beginElement(const char* name) {
	return false;
}

bool YAMLInputArchive::iterateChild(ArchiveIterator& it) {
	return true;
}

bool YAMLInputArchive::iterateChild(ArchiveIterator& it, const char* name) {
	return true;
}

bool YAMLInputArchive::readAttribute(const char* name, const char** str) {
	return false;
}

bool YAMLInputArchive::readAttribute(const char* name, bool& value) {
	return false;
}

bool YAMLInputArchive::readAttribute(const char* name, char& value) {
	return false;
}

bool YAMLInputArchive::readAttribute(const char* name, unsigned char& value) {
	return false;
}

bool YAMLInputArchive::readAttribute(const char* name, int& value) {
	return false;
}

bool YAMLInputArchive::readAttribute(const char* name, unsigned int& value) {
	return false;
}

bool YAMLInputArchive::readAttribute(const char* name, short& value) {
	return false;
}

bool YAMLInputArchive::readAttribute(const char* name, unsigned short& value) {
	return false;
}

bool YAMLInputArchive::readAttribute(const char* name, float& value) {
	return false;
}

bool YAMLInputArchive::readAttribute(const char* name, double& value) {
	return false;
}

} // namespace Typhoon::Reflection

#endif
