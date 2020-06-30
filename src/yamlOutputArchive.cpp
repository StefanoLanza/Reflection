#include "yamlOutputArchive.h"

#if TY_REFLECTION_YAML

#include <cassert>

namespace Typhoon::Reflection {

YAMLOutputArchive::YAMLOutputArchive() {
}

YAMLOutputArchive::~YAMLOutputArchive() = default;

bool YAMLOutputArchive::saveToFile(const char* fileName) {
	return false;
}

bool YAMLOutputArchive::saveToString(std::string& string) {
	return false;
}

bool YAMLOutputArchive::beginElement(const char* name) {
	return false;
}

void YAMLOutputArchive::endElement() {
}

bool YAMLOutputArchive::writeAttribute(const char* name, const char* str) {
	return false;
}

bool YAMLOutputArchive::writeAttribute(const char* name, bool value) {
	return false;
}

bool YAMLOutputArchive::writeAttribute(const char* name, char value) {
	return false;
}

bool YAMLOutputArchive::writeAttribute(const char* name, unsigned char value) {
	return false;
}

bool YAMLOutputArchive::writeAttribute(const char* name, int value) {
	return false;
}

bool YAMLOutputArchive::writeAttribute(const char* name, unsigned int value) {
	return false;
}

bool YAMLOutputArchive::writeAttribute(const char* name, short value) {
	return false;
}

bool YAMLOutputArchive::writeAttribute(const char* name, unsigned short value) {
	return false;
}

bool YAMLOutputArchive::writeAttribute(const char* name, float value) {
	return false;
}

bool YAMLOutputArchive::writeAttribute(const char* name, double value) {
	return false;
}

bool YAMLOutputArchive::write(const char* text) {
	return false;
}

} // namespace Typhoon

#endif
