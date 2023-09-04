#include "XMLInputArchive.h"

#if TY_REFLECTION_XML

#include <TinyXML/tinyxml2.h>
#include <cassert>

namespace Typhoon::Reflection {

XMLInputArchive::XMLInputArchive()
    : document(std::make_unique<tinyxml2::XMLDocument>()) {
}

XMLInputArchive::~XMLInputArchive() {
	assert(typeStack.empty());
}

ParseResult XMLInputArchive::initialize(const char* buffer) {
	assert(buffer);
	const auto error = document->Parse(buffer);
	if (error == tinyxml2::XML_SUCCESS) {
		currentNode = document.get();
		beginElement("root"); // might fail
		return { true, nullptr, 0 };
	}
	else {
		return { false, document->ErrorName(), document->ErrorLineNum() };
	}
}

bool XMLInputArchive::read(const void* element, bool& value) const {
	bool res = false;
	if (auto elm = static_cast<const tinyxml2::XMLNode*>(element)->ToElement(); element) {
		if (auto error = elm->QueryBoolText(&value); error == tinyxml2::XML_SUCCESS) {
			res = true;
		}
	}
	return res;
}

bool XMLInputArchive::read(const void* element, int& value)const  {
	bool res = false;
	if (auto elm = static_cast<const tinyxml2::XMLNode*>(element)->ToElement(); element) {
		if (auto error = elm->QueryIntText(&value); error == tinyxml2::XML_SUCCESS) {
			res = true;
		}
	}
	return res;
}

bool XMLInputArchive::read(const void* element, unsigned int& value)const  {
	bool res = false;
	if (auto elm = static_cast<const tinyxml2::XMLNode*>(element)->ToElement(); element) {
		if (auto error = elm->QueryUnsignedText(&value); error == tinyxml2::XML_SUCCESS) {
			res = true;
		}
	}
	return res;
}

bool XMLInputArchive::read(const void* element, int64_t& value)const  {
	bool res = false;
	if (auto elm = static_cast<const tinyxml2::XMLNode*>(element)->ToElement(); element) {
		if (auto error = elm->QueryInt64Text(&value); error == tinyxml2::XML_SUCCESS) {
			res = true;
		}
	}
	return res;
}

bool XMLInputArchive::read(const void* element, uint64_t& value) const {
	bool res = false;
	if (auto elm = static_cast<const tinyxml2::XMLNode*>(element)->ToElement(); element) {
		if (auto error = elm->QueryUnsigned64Text(&value); error == tinyxml2::XML_SUCCESS) {
			res = true;
		}
	}
	return res;
}
bool XMLInputArchive::read(const void* element, float& value)const  {
	bool res = false;
	if (auto elm = static_cast<const tinyxml2::XMLNode*>(element)->ToElement(); element) {
		if (auto error = elm->QueryFloatText(&value); error == tinyxml2::XML_SUCCESS) {
			res = true;
		}
	}
	return res;
}

bool XMLInputArchive::read(const void* element, double& value)const  {
	bool res = false;
	if (auto elm = static_cast<const tinyxml2::XMLNode*>(element)->ToElement(); element) {
		if (auto error = elm->QueryDoubleText(&value); error == tinyxml2::XML_SUCCESS) {
			res = true;
		}
	}
	return res;
}

bool XMLInputArchive::read(const void* element, const char*& str)const  {
	bool res = false;
	if (auto elm = static_cast<const tinyxml2::XMLNode*>(element)->ToElement(); element) {
		if (const char* text = elm->GetText(); text) {
			str = text;
			res = true;
		}
	}
	return res;
}

bool XMLInputArchive::read(const void* element, std::string_view& sv)const  {
	if (const char* str = nullptr; read(element, str)) {
		sv = str;
		return true;
	}
	return false;
}

void XMLInputArchive::endElement() {
}

bool XMLInputArchive::beginObject() {
	if (const char* type = nullptr; readAttribute("type", type)) {
		if (strcmp(type, "object")) {
			return false;
		}
	}
	typeStack.push(Type::object);
	return true;
}

void XMLInputArchive::endObject() {
	assert(typeStack.top() == Type::object);
	typeStack.pop();
}

bool XMLInputArchive::beginArray() {
	if (const char* type = nullptr; readAttribute("type", type)) {
		if (strcmp(type, "array")) {
			return false;
		}
	}
	typeStack.push(Type::array);
	return true;
}

void XMLInputArchive::endArray() {
	assert(typeStack.top() == Type::array);
	typeStack.pop();
}

bool XMLInputArchive::beginObject(const char* key) {
	bool res = false;
	if (beginElement(key)) {
		res = beginObject();
		if (! res) {
			endElement();
		}
	}
	return res;
}

bool XMLInputArchive::beginArray(const char* key) {
	bool res = false;
	if (beginElement(key)) {
		res = beginArray();
		if (! res) {
			endElement();
		}
	}
	return res;
}

InputArchiveElement XMLInputArchive::beginElement(const void* element, const char* name) {
	if (auto elm = static_cast<const tinyxml2::XMLNode*>(element)->ToElement(); element) {
		return { *this, elm };
	}
	else {
		return {*this };
	}
}

#if 0
bool XMLInputArchive::iterateChild(ArchiveIterator& it) {
	tinyxml2::XMLNode* childIt = nullptr;
	if (it.getNode()) {
		childIt = static_cast<tinyxml2::XMLNode*>(it.getNode())->NextSibling();
	}
	else {
		childIt = currentNode->FirstChildElement();
		if (! childIt) {
			return false;
		}
	}

	// Discard comments
	while (childIt && childIt->ToComment()) {
		childIt = childIt->NextSibling();
	}

	if (childIt == nullptr) {
		// Back to parent node
		currentNode = static_cast<tinyxml2::XMLNode*>(it.getNode())->Parent();
		it.reset();
		return false;
	}

	currentNode = childIt;
	it.setNode(childIt);
	return true;
}

bool XMLInputArchive::iterateChild(ArchiveIterator& it, const char* name) {
	tinyxml2::XMLNode* childIt = nullptr;
	if (it.getNode()) {
		childIt = static_cast<tinyxml2::XMLNode*>(it.getNode())->NextSiblingElement(name);
		if (childIt == nullptr) {
			currentNode = static_cast<tinyxml2::XMLNode*>(it.getNode())->Parent();
			it.reset();
			return false;
		}
	}
	else {
		childIt = currentNode->FirstChildElement(name);
		if (! childIt) {
			return false;
		}
	}
	currentNode = childIt;
	it.setNode(childIt);
	return true;
}
#endif

bool XMLInputArchive::readAttribute(const void* element, const char* name, bool& value) const {
	if (auto elm = static_cast<const tinyxml2::XMLNode*>(element)->ToElement(); elm) {
		if (auto error = elm->QueryBoolAttribute(name, &value); error == tinyxml2::XML_SUCCESS) {
			return true;
		}
	}
	return false;
}

bool XMLInputArchive::readAttribute(const void* element, const char* name, int& value) const {
	if (auto elm = static_cast<const tinyxml2::XMLNode*>(element)->ToElement(); elm) {
		if (auto error = elm->QueryIntAttribute(name, &value); error == tinyxml2::XML_SUCCESS) {
			return true;
		}
	}
	return false;
}

bool XMLInputArchive::readAttribute(const void* element, const char* name, unsigned int& value) const {
	if (auto elm = static_cast<const tinyxml2::XMLNode*>(element)->ToElement(); elm) {
		if (auto error = elm->QueryUnsignedAttribute(name, &value); error == tinyxml2::XML_SUCCESS) {
			return true;
		}
	}
	return false;
}

bool XMLInputArchive::readAttribute(const void* element, const char* name, float& value) const {
	if (auto elm = static_cast<const tinyxml2::XMLNode*>(element)->ToElement(); elm) {
		if (auto error = elm->QueryFloatAttribute(name, &value); error == tinyxml2::XML_SUCCESS) {
			return true;
		}
	}
	return false;
}

bool XMLInputArchive::readAttribute(const void* element, const char* name, double& value) const {
	if (auto elm = static_cast<const tinyxml2::XMLNode*>(element)->ToElement(); elm) {
		if (auto error = elm->QueryDoubleAttribute(name, &value); error == tinyxml2::XML_SUCCESS) {
			return true;
		}
	}
	return false;
}

bool XMLInputArchive::readAttribute(const void* element, const char* name, const char*& str) const {
	if (auto elm = static_cast<const tinyxml2::XMLNode*>(element)->ToElement(); elm) {
		if (auto error = elm->QueryStringAttribute(name, &str); error == tinyxml2::XML_SUCCESS) {
			return true;
		}
	}
	return false;
}

bool XMLInputArchive::readAttribute(const void* element, const char* name, std::string_view& sv) const {
	if (const char* str = nullptr; readAttribute(element, name, str)) {
		sv = str;
		return true;
	}
	return false;
}

bool  XMLInputArchive::readAttribute(const void* element, const char* attributeName, const char*& str) const {
	if (auto error = static_cast<const tinyxml2::XMLElement*>(element)->QueryStringAttribute(attributeName, &str); error == tinyxml2::XML_SUCCESS) {
		return true;
	}
	return false;
}

bool XMLInputArchive::isObject(const void* element) const {
	if (const char* type = nullptr; readAttribute(element, "type", type)) {
		if (strcmp(type, "object")) {
			return false;
		}
	}
	return false;
}

bool XMLInputArchive::isArray(const void* element) const {
	if (const char* type = nullptr; readAttribute(element, "type", type)) {
		if (strcmp(type, "object")) {
			return false;
		}
	}
	return false;
}

} // namespace Typhoon::Reflection

#endif
