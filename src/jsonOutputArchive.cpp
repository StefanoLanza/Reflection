#include "JSONOutputArchive.h"

#if TY_REFLECTION_JSON

#include <cassert>
#include <rapidjson-master/include/rapidjson/document.h>
#include <rapidjson-master/include/rapidjson/prettywriter.h>
#include <rapidjson-master/include/rapidjson/rapidjson.h>
#include <rapidjson-master/include/rapidjson/stringbuffer.h>

using namespace rapidjson;

namespace Typhoon::Reflection {

JSONOutputArchive::JSONOutputArchive()
    : stream(std::make_unique<StringBuffer>())
    , writer(std::make_unique<PrettyWriter<StringBuffer>>(*stream))
    , saved(false) {
	writer->StartObject();
}

JSONOutputArchive::~JSONOutputArchive() {
}

bool JSONOutputArchive::saveToFile(const char* fileName) {
	return false;
}

bool JSONOutputArchive::saveToString(std::string& string) {
	if (! saved) {
		writer->EndObject();
		saved = true;
	}
	string = stream->GetString(); // TODO string_view
	return true;
}

std::string_view JSONOutputArchive::getString() {
	return { stream->GetString(), stream->GetSize() };
}

bool JSONOutputArchive::beginElement(const char* name) {
	writer->Key(name, static_cast<SizeType>(strlen(name)));
	return true;
}

void JSONOutputArchive::endElement() {
}

void JSONOutputArchive::beginObject() {
	writer->StartObject();
}

void JSONOutputArchive::endObject() {
	writer->EndObject();
}

bool JSONOutputArchive::writeAttribute(const char* name, const char* str) {
	return false;
}

bool JSONOutputArchive::writeAttribute(const char* name, bool value) {
	return false;
}

bool JSONOutputArchive::writeAttribute(const char* name, char value) {
	return false;
}

bool JSONOutputArchive::writeAttribute(const char* name, unsigned char value) {
	return false;
}

bool JSONOutputArchive::writeAttribute(const char* name, int value) {
	return false;
}

bool JSONOutputArchive::writeAttribute(const char* name, unsigned int value) {
	return false;
}

bool JSONOutputArchive::writeAttribute(const char* name, short value) {
	return false;
}

bool JSONOutputArchive::writeAttribute(const char* name, unsigned short value) {
	return false;
}

bool JSONOutputArchive::writeAttribute(const char* name, float value) {
	return false;
}

bool JSONOutputArchive::writeAttribute(const char* name, double value) {
	return false;
}

bool JSONOutputArchive::write(const char* text) {
	writer->String(text, static_cast<SizeType>(strlen(text)));
	return true;
}

} // namespace Typhoon::Reflection

#endif
