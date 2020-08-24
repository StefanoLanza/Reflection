#include "JSONInputArchive.h"

#if TY_REFLECTION_JSON

#include <cassert>
#include <rapidjson-master/include/rapidjson/document.h>
#include <rapidjson-master/include/rapidjson/error/en.h>

namespace Typhoon::Reflection {

JSONInputArchive::JSONInputArchive()
    : document(std::make_unique<rapidjson::Document>()) {
}

JSONInputArchive::~JSONInputArchive() = default;

ParseResult JSONInputArchive::initialize(const char* buffer) {
	const rapidjson::ParseResult result = document->Parse(buffer);
	if (! result.IsError()) {
		stack.push({ document.get() });
	}
	return { ! result.IsError(), GetParseError_En(result.Code()), static_cast<int>(result.Offset()) };
}

const char* JSONInputArchive::currNodeText() {
	assert(! stack.empty());
	return stack.top().value->GetString();
}

bool JSONInputArchive::readBool(const char* key, bool& value) {
	assert(false);
	return false;
}

bool JSONInputArchive::readInt(const char* key, int& value) {
	assert(false);
	return false;
}

bool JSONInputArchive::readUInt(const char* key, unsigned int& value) {
	assert(false);
	return false;
}

bool JSONInputArchive::readFloat(const char* key, float& value) {
	assert(false);
	return false;
}

bool JSONInputArchive::readDouble(const char* key, double& value) {
	assert(false);
	return false;
}

bool JSONInputArchive::readString(const char* key, const char*& str) {
	bool             res = false;
	const StackItem& top = stack.top();
	if (top.value->IsObject()) {
		if (auto memberItr = top.value->FindMember(key); memberItr != top.value->MemberEnd()) {
			str = memberItr->value.GetString();
			res = true;
		}
	}
	return res;
}

bool JSONInputArchive::beginElement(const char* name) {
	assert(! stack.empty());
	const StackItem& top = stack.top();
	if (top.value->IsObject()) {
		auto memberItr = name ? top.value->FindMember(name) : top.value->MemberBegin();
		if (memberItr != top.value->MemberEnd()) {
			stack.push({ &memberItr->value });
			return true;
		}
		else {
			return false;
		}
	}
	return false;
}

void JSONInputArchive::endElement() {
	stack.pop();
}

// TODO beginObject(name). This operates on the stack. value(name) returns a string to be parsed
bool JSONInputArchive::beginObject() {
	StackItem& top = stack.top();
	if (top.value->IsObject()) {
		return true;
	}
	else {
		return false;
	}
}

void JSONInputArchive::endObject() {
	[[maybe_unused]] const StackItem& top = stack.top();
	assert(top.value->IsObject());
}

bool JSONInputArchive::beginArray() {
	const StackItem& top = stack.top();
	return top.value->IsArray();
}

void JSONInputArchive::endArray() {
	[[maybe_unused]] const StackItem& top = stack.top();
	assert(top.value->IsArray());
	stack.pop();
}

bool JSONInputArchive::beginObject(const char* key) {
	bool res = false;
	if (beginElement(key)) {
		res = beginObject();
		if (! res) {
			endElement();
		}
	}
	return res;
}

bool JSONInputArchive::beginArray(const char* key) {
	bool res = false;
	if (beginElement(key)) {
		res = beginArray();
		if (! res) {
			endElement();
		}
	}
	return res;
}

bool JSONInputArchive::iterateChild(ArchiveIterator& it) {
	if (it.getIndex() != static_cast<size_t>(-1)) {
		stack.pop();

		const StackItem& top = stack.top();
		assert(top.value->IsArray());
		auto array = top.value->GetArray();

		size_t index = it.getIndex() + 1;
		if (index >= array.Size()) {
			it.reset();
			return false;
		}
		auto sindex = static_cast<rapidjson::SizeType>(index);
		it.reset(index);
		stack.push({ &array[sindex] });
	}
	else {
		const StackItem& top = stack.top();
		assert(top.value->IsArray());
		auto array = top.value->GetArray();
		if (array.Empty()) {
			return false;
		}
		size_t index = 0;
		it.reset(index);
		stack.push({ &array[0] });
	}

	return true;
}

#if TY_REFLECTION_DEPRECATED

bool JSONInputArchive::iterateChild(ArchiveIterator& it, const char* name) {
	assert(false && "Not supported");
	return false;
}

#endif

bool JSONInputArchive::readAttribute(const char* name, bool& value) {
	bool res = false;
	if (beginAttribute(name)) {
		const StackItem& top = stack.top();
		if (top.value->IsBool()) {
			value = top.value->GetBool();
			res = true;
		}
		endElement();
	}
	return res;
}

bool JSONInputArchive::readAttribute(const char* name, int& value) {
	bool res = false;
	if (beginAttribute(name)) {
		const StackItem& top = stack.top();
		if (top.value->IsInt()) {
			value = top.value->GetInt();
			res = true;
		}
		endElement();
	}
	return res;
}

bool JSONInputArchive::readAttribute(const char* name, unsigned int& value) {
	bool res = false;
	if (beginAttribute(name)) {
		const StackItem& top = stack.top();
		if (top.value->IsUint()) {
			value = top.value->GetUint();
			res = true;
		}
		endElement();
	}
	return res;
}

bool JSONInputArchive::readAttribute(const char* name, float& value) {
	bool res = false;
	if (beginAttribute(name)) {
		const StackItem& top = stack.top();
		if (top.value->IsFloat()) {
			value = top.value->GetFloat();
			res = true;
		}
		endElement();
	}
	return res;
}

bool JSONInputArchive::readAttribute(const char* name, double& value) {
	bool res = false;
	if (beginAttribute(name)) {
		const StackItem& top = stack.top();
		if (top.value->IsDouble()) {
			value = top.value->GetDouble();
			res = true;
		}
		endElement();
	}
	return res;
}

bool JSONInputArchive::readAttribute(const char* name, const char*& str) {
	bool res = false;
	if (beginAttribute(name)) {
		const StackItem& top = stack.top();
		if (top.value->IsString()) {
			str = top.value->GetString();
			res = true;
		}
		endElement();
	}
	return res;
}

bool JSONInputArchive::beginAttribute(const char* name) {
	char tmp[64];
	tmp[0] = '@';
	strcpy_s(tmp + 1, sizeof(tmp) - 1, name);
	return beginElement(tmp);
}

} // namespace Typhoon::Reflection

#endif
