#include "jsonInputArchive.h"

#if TY_REFLECTION_JSON

#include <cassert>
#include <rapidjson/include/rapidjson/document.h>
#include <rapidjson/include/rapidjson/error/en.h>

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

bool JSONInputArchive::readBool(bool& b) const {
	if (auto value = stack.top().value; value->IsBool()) {
		b = value->GetBool();
		return true;
	}
	return false;
}

bool JSONInputArchive::readInt(int& i) const {
	if (auto value = stack.top().value; value->IsInt()) {
		i = value->GetInt();
		return true;
	}
	return false;
}

bool JSONInputArchive::readUInt(unsigned int& ui) const {
	if (auto value = stack.top().value; value->IsUint()) {
		ui = value->GetUint();
		return true;
	}
	return false;
}

bool JSONInputArchive::readInt64(int64_t& i64) const {
	if (auto value = stack.top().value; value->IsInt64()) {
		i64 = value->GetInt64();
		return true;
	}
	return false;
}

bool JSONInputArchive::readUInt64(uint64_t& ui64) const {
	if (auto value = stack.top().value; value->IsUint64()) {
		ui64 = value->GetUint64();
		return true;
	}
	return false;
}

bool JSONInputArchive::readFloat(float& f) const {
	if (auto value = stack.top().value; value->IsFloat()) {
		f = value->GetFloat();
		return true;
	}
	return false;
}

bool JSONInputArchive::readDouble(double& d) const{
	if (auto value = stack.top().value; value->IsDouble()) {
		d = value->GetDouble();
		return true;
	}
	return false;
}

bool JSONInputArchive::readString(const char*& str) const{
	if (auto value = stack.top().value; value->IsString()) {
		str = value->GetString();
		return true;
	}
	return false;
}

bool JSONInputArchive::beginElement(const char* name) {
	assert(! stack.empty());
	const StackItem& top = stack.top();
	if (top.value->IsObject()) {
		if (auto memberItr = name ? top.value->FindMember(name) : top.value->MemberBegin(); memberItr != top.value->MemberEnd()) {
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

const rapidjson::Value& JSONInputArchive::getValue(const char* key) const {
	const StackItem& top = stack.top();
	if (top.value->IsObject()) {
		if (auto memberItr = top.value->FindMember(key); memberItr != top.value->MemberEnd()) {
			return memberItr->value;
		}
	}
	static rapidjson::Value nullValue;
	return nullValue;
}

} // namespace Typhoon::Reflection

#endif
