#include "JSONInputArchive.h"

#if TY_REFLECTION_JSON

#include <cassert>
#include <rapidjson-master/include/rapidjson/document.h>
#include <rapidjson-master/include/rapidjson/error/en.h>

using namespace rapidjson;

namespace Typhoon::Reflection {

JSONInputArchive::JSONInputArchive()
    : document(std::make_unique<Document>()) {
}

JSONInputArchive::~JSONInputArchive() = default;

ParseResult JSONInputArchive::initialize(const char* buffer) {
	const rapidjson::ParseResult result = document->Parse(buffer);
	if (! result.IsError()) {
		stack.push({ document.get(), StackState::beforeStart, 0 });
	}
	return { ! result.IsError(), GetParseError_En(result.Code()), static_cast<int>(result.Offset()) };
}

const char* JSONInputArchive::currNodeText() {
	assert(! stack.empty());
	return stack.top().value->GetString();
}

bool JSONInputArchive::beginElement(const char* name) {
	assert(! stack.empty());
	const StackItem& top = stack.top();
	if (top.value->IsObject() && top.state == StackState::started) {
		auto memberItr = top.value->FindMember(name);
		if (memberItr != top.value->MemberEnd()) {
			stack.push({ &memberItr->value, StackState::beforeStart, 0 });
		}
		else {
			return false;
		}
	}
	return true;
}

void JSONInputArchive::endElement() {
	stack.pop();
	// TODO array
}

// TODO beginObject(name). This operates on the stack. value(name) returns a string to be parsed

bool JSONInputArchive::beginObject() {
	StackItem& top = stack.top();
	if (top.value->IsObject() && top.state == StackState::beforeStart) {
		top.state = StackState::started;
		return true;
	}
	else {
		return false;
	}
}

void JSONInputArchive::endObject() {
	[[maybe_unused]] const StackItem& top = stack.top();
	assert(top.value->IsObject() && top.state == StackState::started);
	stack.pop();
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
