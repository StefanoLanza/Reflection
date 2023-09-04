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
	const rapidjson::ParseResult result = document->Parse<rapidjson::kParseCommentsFlag | rapidjson::kParseTrailingCommasFlag>(buffer);
	if (! result.IsError()) {
		stack.push(document.get());
	}
	return { ! result.IsError(), GetParseError_En(result.Code()), static_cast<int>(result.Offset()) };
}

bool JSONInputArchive::read(bool& b) const {
	if (auto value = stack.top(); value->IsBool()) {
		b = value->GetBool();
		return true;
	}
	return false;
}

bool JSONInputArchive::read(int& i) const {
	if (auto value = stack.top(); value->IsInt()) {
		i = value->GetInt();
		return true;
	}
	return false;
}

bool JSONInputArchive::read(unsigned int& ui) const {
	if (auto value = stack.top(); value->IsUint()) {
		ui = value->GetUint();
		return true;
	}
	return false;
}

bool JSONInputArchive::read(int64_t& i64) const {
	if (auto value = stack.top(); value->IsInt64()) {
		i64 = value->GetInt64();
		return true;
	}
	return false;
}

bool JSONInputArchive::read(uint64_t& ui64) const {
	if (auto value = stack.top(); value->IsUint64()) {
		ui64 = value->GetUint64();
		return true;
	}
	return false;
}

bool JSONInputArchive::read(float& f) const {
	if (auto value = stack.top(); value->IsFloat()) {
		f = value->GetFloat();
		return true;
	}
	return false;
}

bool JSONInputArchive::read(double& d) const {
	if (auto value = stack.top(); value->IsDouble()) {
		d = value->GetDouble();
		return true;
	}
	return false;
}

bool JSONInputArchive::read(const char*& str) const {
	if (auto value = stack.top(); value->IsString()) {
		str = value->GetString();
		return true;
	}
	return false;
}

bool JSONInputArchive::read(std::string_view& sv) const {
	if (auto value = stack.top(); value->IsString()) {
		sv = { value->GetString(), value->GetStringLength() };
		return true;
	}
	return false;
}

bool JSONInputArchive::beginElement(const char* name) const {
	assert(! stack.empty());
	const StackItem& top = stack.top();
	if (top->IsObject()) {
		if (auto memberItr = name ? top->FindMember(name) : top->MemberBegin(); memberItr != top->MemberEnd()) {
			stack.push(&memberItr->value);
			return true;
		}
		else {
			return false;
		}
	}
	return false;
}

void JSONInputArchive::endElement() const {
	stack.pop();
}

// TODO beginObject(name). This operates on the stack. value(name) returns a string to be parsed
bool JSONInputArchive::beginObject() const {
	StackItem& top = stack.top();
	if (top->IsObject()) {
		return true;
	}
	else {
		return false;
	}
}

void JSONInputArchive::endObject() const {
	[[maybe_unused]] const StackItem& top = stack.top();
	assert(top->IsObject());
}

bool JSONInputArchive::beginArray() const {
	const StackItem& top = stack.top();
	return top->IsArray();
}

void JSONInputArchive::endArray() const {
	[[maybe_unused]] const StackItem& top = stack.top();
	assert(top->IsArray());
	stack.pop();
}

bool JSONInputArchive::beginObject(const char* key) const {
	bool res = false;
	if (beginElement(key)) {
		res = beginObject();
		if (! res) {
			endElement();
		}
	}
	return res;
}

bool JSONInputArchive::beginArray(const char* key) const {
	bool res = false;
	if (beginElement(key)) {
		res = beginArray();
		if (! res) {
			endElement();
		}
	}
	return res;
}

bool JSONInputArchive::iterateChild(ArchiveIterator& it) const {
	if (it.hasValidIndex()) {
		stack.pop();

		const StackItem& top = stack.top();
		const size_t     index = it.getIndex() + 1;
		if (top->IsArray()) {
			auto array = top->GetArray();
			if (index >= array.Size()) {
				it.reset();
				return false;
			}
			it.setIndex(index);
			auto sindex = static_cast<rapidjson::SizeType>(index);
			stack.push(&array[sindex]);
		}
		else if (top->IsObject()) {
			auto object = top->GetObject();
			if (index >= object.MemberCount()) {
				it.reset();
				return false;
			}
			it.setIndex(index);
			rapidjson::GenericMemberIterator memberIt = object.MemberBegin() + index;
			stack.push(&memberIt->value);
			it.setKey(memberIt->name.GetString());
		}
		else {
			assert(false);
		}
	}
	else {
		const StackItem& top = stack.top();
		if (top->IsArray()) {
			auto array = top->GetArray();
			if (array.Empty()) {
				return false;
			}
			it.setIndex(0);
			stack.push(&array[0]);
		}
		else if (top->IsObject()) {
			auto object = top->GetObject();
			if (object.MemberCount() == 0) {
				return false;
			}
			it.setIndex(0);
			rapidjson::GenericMemberIterator memberIt = object.MemberBegin();
			stack.push(&memberIt->value);
			it.setKey(memberIt->name.GetString());
		}
		else {
			assert(false); // unsupported
		}
	}

	return true;
}

// Legacy
bool JSONInputArchive::iterateChild(ArchiveIterator&, const char*) const {
	assert(false);
	return false;
}

bool JSONInputArchive::readAttribute(const char* name, bool& value) const {
	bool res = false;
	if (beginAttribute(name)) {
		const StackItem& top = stack.top();
		if (top->IsBool()) {
			value = top->GetBool();
			res = true;
		}
		endElement();
	}
	return res;
}

bool JSONInputArchive::readAttribute(const char* name, int& value) const {
	bool res = false;
	if (beginAttribute(name)) {
		const StackItem& top = stack.top();
		if (top->IsInt()) {
			value = top->GetInt();
			res = true;
		}
		endElement();
	}
	return res;
}

bool JSONInputArchive::readAttribute(const char* name, unsigned int& value) const {
	bool res = false;
	if (beginAttribute(name)) {
		const StackItem& top = stack.top();
		if (top->IsUint()) {
			value = top->GetUint();
			res = true;
		}
		endElement();
	}
	return res;
}

bool JSONInputArchive::readAttribute(const char* name, float& value) const {
	bool res = false;
	if (beginAttribute(name)) {
		const StackItem& top = stack.top();
		if (top->IsFloat()) {
			value = top->GetFloat();
			res = true;
		}
		endElement();
	}
	return res;
}

bool JSONInputArchive::readAttribute(const char* name, double& value) const {
	bool res = false;
	if (beginAttribute(name)) {
		const StackItem& top = stack.top();
		if (top->IsDouble()) {
			value = top->GetDouble();
			res = true;
		}
		endElement();
	}
	return res;
}

bool JSONInputArchive::readAttribute(const char* name, const char*& str) const {
	bool res = false;
	if (beginAttribute(name)) {
		const StackItem& top = stack.top();
		if (top->IsString()) {
			str = top->GetString();
			res = true;
		}
		endElement();
	}
	return res;
}

bool JSONInputArchive::readAttribute(const char* name, std::string_view& sv) const {
	bool res = false;
	if (beginAttribute(name)) {
		const StackItem& top = stack.top();
		if (top->IsString()) {
			sv = { top->GetString(), top->GetStringLength() };
			res = true;
		}
		endElement();
	}
	return res;
}

bool JSONInputArchive::beginAttribute(const char* name) const {
	char tmp[256];
	tmp[0] = '@';
#ifdef _MSC_VER
	strncpy_s(tmp + 1, sizeof(tmp) - 1, name, sizeof(tmp) - 2);
	tmp[sizeof(tmp) - 1] = 0;
#else
	strncpy(tmp + 1, name, sizeof(tmp) - 2);
	tmp[sizeof(tmp) - 1] = 0; // null terminate
#endif
	return beginElement(tmp);
}

const rapidjson::Value& JSONInputArchive::getValue(const char* key) const {
	const StackItem& top = stack.top();
	if (top->IsObject()) {
		if (auto memberItr = top->FindMember(key); memberItr != top->MemberEnd()) {
			return memberItr->value;
		}
	}
	static rapidjson::Value nullValue;
	return nullValue;
}

} // namespace Typhoon::Reflection

#endif
