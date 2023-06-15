#include "jsonOutputArchive.h"

#if TY_REFLECTION_JSON

#include <cassert>
#include <fstream>
#include <rapidjson/include/rapidjson/document.h>
#include <rapidjson/include/rapidjson/prettywriter.h>
#include <rapidjson/include/rapidjson/rapidjson.h>
#include <rapidjson/include/rapidjson/stringbuffer.h>

using namespace rapidjson;

namespace Typhoon::Reflection {

JSONOutputArchive::JSONOutputArchive(bool openRoot)
    : stream(std::make_unique<StringBuffer>())
    , writer(std::make_unique<PrettyWriter<StringBuffer>>(*stream))
    , endRoot { openRoot } {
	if (openRoot) {
		writer->StartObject(); // begin root
	}
}

JSONOutputArchive::~JSONOutputArchive() = default;

bool JSONOutputArchive::saveToFile(const char* fileName) {
	std::string   str = saveToString();
	std::ofstream file(fileName);
	if (file) {
		file.write(str.data(), str.size());
		file.close();
		return true;
	}
	return false;
}

std::string JSONOutputArchive::saveToString() {
	if (endRoot) {
		writer->EndObject();
		endRoot = false;
	}
	return stream->GetString();
}

void JSONOutputArchive::setKey(const char* name) {
	assert(name);
	writer->Key(name);
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

void JSONOutputArchive::write(bool value) {
	writer->Bool(value);
}

void JSONOutputArchive::write(int value) {
	writer->Int(value);
}

void JSONOutputArchive::write(unsigned int value) {
	writer->Uint(value);
}

void JSONOutputArchive::write(int64_t value) {
	writer->Int64(value);
}

void JSONOutputArchive::write(uint64_t value) {
	writer->Uint64(value);
}

void JSONOutputArchive::write(float value) {
	writer->Double(value);
}

void JSONOutputArchive::write(double value) {
	writer->Double(value);
}

void JSONOutputArchive::write(const char* str) {
	writer->String(str);
}

void JSONOutputArchive::write(std::string_view str) {
	writer->String(str.data(), static_cast<rapidjson::SizeType>(str.size()));
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
