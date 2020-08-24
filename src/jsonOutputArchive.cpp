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

bool JSONOutputArchive::writeBool(const char* key, bool value) {
	writer->Key(key);
	writer->Bool(value);
	return true;
}

bool JSONOutputArchive::writeInt(const char* key, int value) {
	writer->Key(key);
	writer->Int(value);
	return true;
}

bool JSONOutputArchive::writeUInt(const char* key, unsigned int value) {
	writer->Key(key);
	writer->Uint(value);
	return true;
}

bool JSONOutputArchive::writeFloat(const char* key, float value) {
	writer->Key(key);
	writer->Double(value);
	return true;
}

bool JSONOutputArchive::writeDouble(const char* key, double value) {
	writer->Key(key);
	writer->Double(value);
	return true;
}

bool JSONOutputArchive::writeString(const char* key, const char* str) {
	writer->Key(key);
	writer->String(str);
	return true;
}

void JSONOutputArchive::writeAttributeKey(const char* key) const {
	char tmp[64];
	tmp[0] = '@';
	strcpy_s(tmp + 1, sizeof(tmp) - 1, key);
	writer->Key(tmp);
}

} // namespace Typhoon::Reflection

#endif
