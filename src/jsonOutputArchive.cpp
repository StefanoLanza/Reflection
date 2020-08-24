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
}

JSONOutputArchive::~JSONOutputArchive() {
}

bool JSONOutputArchive::saveToFile(const char* fileName) {
	return false;
}

bool JSONOutputArchive::saveToString(std::string& string) {
	if (! saved) {
		// writer->EndObject();
		saved = true;
	}
	string = stream->GetString(); // TODO string_view
	return true;
}

std::string_view JSONOutputArchive::getString() {
	return { stream->GetString(), stream->GetSize() };
}

void JSONOutputArchive::beginRoot() {
	writer->StartObject();
}

void JSONOutputArchive::endRoot() {
	writer->EndObject();
}

bool JSONOutputArchive::beginElement(const char* name) {
	assert(name);
	return writer->Key(name, static_cast<SizeType>(strlen(name)));
}

void JSONOutputArchive::endElement() {
}

bool JSONOutputArchive::beginObject() {
	return writer->StartObject();
}

void JSONOutputArchive::endObject() {
	writer->EndObject();
}

bool JSONOutputArchive::beginArray() {
	return writer->StartArray();
}

void JSONOutputArchive::endArray() {
	writer->EndArray();
}

void JSONOutputArchive::writeAttribute(const char* name, bool value) {
	writeAttributeKey(name);
	writer->Bool(value);
}

void JSONOutputArchive::writeAttribute(const char* name, int value) {
	writeAttributeKey(name);
	writer->Int(value);
}

void JSONOutputArchive::writeAttribute(const char* name, unsigned int value) {
	writeAttributeKey(name);
	writer->Int64(value);
}

void JSONOutputArchive::writeAttribute(const char* name, float value) {
	writeAttributeKey(name);
	writer->Double(value);
}

void JSONOutputArchive::writeAttribute(const char* name, double value) {
	writeAttributeKey(name);
	writer->Double(value);
}

void JSONOutputArchive::writeAttribute(const char* name, const char* str) {
	writeAttributeKey(name);
	writer->String(str);
}

bool JSONOutputArchive::write(const char* text) {
	return writer->String(text, static_cast<SizeType>(strlen(text)));
}

void JSONOutputArchive::writeAttributeKey(const char* key) const {
	char tmp[64];
	tmp[0] = '@';
	strcpy_s(tmp + 1, sizeof(tmp) - 1, key);
	writer->Key(tmp);
}

} // namespace Typhoon::Reflection

#endif
