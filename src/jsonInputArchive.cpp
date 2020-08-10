#include "JSONInputArchive.h"

#if TY_REFLECTION_JSON

#include <cassert>

namespace Typhoon::Reflection {

JSONInputArchive::JSONInputArchive() {
}

JSONInputArchive::~JSONInputArchive() = default;

bool JSONInputArchive::initialize(const char* buffer) {
	return false;
}

std::string JSONInputArchive::getErrorDesc() const {
	return {};
}

const char* JSONInputArchive::currNodeText() {
	return nullptr;
}

void JSONInputArchive::endElement() {
}

bool JSONInputArchive::beginElement() {
	return false;
}

bool JSONInputArchive::beginElement(const char* name) {
	return false;
}

bool JSONInputArchive::iterateChild(ArchiveIterator& it) {
	return true;
}

bool JSONInputArchive::iterateChild(ArchiveIterator& it, const char* name) {
	return true;
}

bool JSONInputArchive::readAttribute(const char* name, const char** str) {
	return false;
}

bool JSONInputArchive::readAttribute(const char* name, bool& value) {
	return false;
}

bool JSONInputArchive::readAttribute(const char* name, char& value) {
	return false;
}

bool JSONInputArchive::readAttribute(const char* name, unsigned char& value) {
	return false;
}

bool JSONInputArchive::readAttribute(const char* name, int& value) {
	return false;
}

bool JSONInputArchive::readAttribute(const char* name, unsigned int& value) {
	return false;
}

bool JSONInputArchive::readAttribute(const char* name, short& value) {
	return false;
}

bool JSONInputArchive::readAttribute(const char* name, unsigned short& value) {
	return false;
}

bool JSONInputArchive::readAttribute(const char* name, float& value) {
	return false;
}

bool JSONInputArchive::readAttribute(const char* name, double& value) {
	return false;
}

} // namespace Typhoon::Reflection

#endif
