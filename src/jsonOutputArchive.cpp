#include "jsonOutputArchive.h"

#if TY_REFLECTION_JSON

#include <fstream>
#include <cassert>
#include <rapidjson/include/rapidjson/document.h>
#include <rapidjson/include/rapidjson/prettywriter.h>
#include <rapidjson/include/rapidjson/rapidjson.h>
#include <rapidjson/include/rapidjson/stringbuffer.h>

using namespace rapidjson;

namespace Typhoon::Reflection {

JSONOutputArchive::JSONOutputArchive()
    : stream(std::make_unique<StringBuffer>())
    , writer(std::make_unique<PrettyWriter<StringBuffer>>(*stream))
    , endRoot(true) {
	writer->StartObject(); // begin root
}

JSONOutputArchive::~JSONOutputArchive() = default;

bool JSONOutputArchive::saveToFile(const char* fileName) {
	std::string str;
	if (saveToString(str)) {
		std::ofstream file(fileName);
		if (file) {
			file.write(str.data(), str.size());
			file.close();
			return true;
		}
	}
	return false;
}

bool JSONOutputArchive::saveToString(std::string& string) {
	if (endRoot) {
		writer->EndObject();
		endRoot = false;
	}
	string = stream->GetString(); // TODO string_view
	return true;
}

std::string_view JSONOutputArchive::getString() {
	return { stream->GetString(), stream->GetSize() };
}

bool JSONOutputArchive::beginElement(const char* name) {
	assert(name);
	return writer->Key(name);
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

bool JSONOutputArchive::write(bool value) {
	return writer->Bool(value);
}

bool JSONOutputArchive::write(int value) {
	return writer->Int(value);
}

bool JSONOutputArchive::write(unsigned int value) {
	return writer->Uint(value);
}

bool JSONOutputArchive::write(int64_t value) {
	return writer->Int64(value);
}

bool JSONOutputArchive::write(uint64_t value) {
	return writer->Uint64(value);
}

bool JSONOutputArchive::write(float value) {
	return writer->Double(value);
}

bool JSONOutputArchive::write(double value) {
	return writer->Double(value);
}

bool JSONOutputArchive::write(const char* str) {
	return writer->String(str);
}

bool JSONOutputArchive::write(std::string_view str) {
	return writer->String(str.data(), static_cast<rapidjson::SizeType>(str.size()));
}

void JSONOutputArchive::writeAttributeKey(const char* key) const {
	char tmp[256];
	tmp[0] = '@';
#ifdef _MSC_VER
	strncpy_s(tmp + 1, sizeof(tmp) - 1, key, sizeof(tmp) - 2);
#else
	strncpy(tmp + 1, key, sizeof(tmp) - 2);
#endif
	tmp[sizeof(tmp) - 1] = 0; // null terminate
	writer->Key(tmp);
}

} // namespace Typhoon::Reflection

#endif
