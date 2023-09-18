#include "XMLInputArchive.h"

#if TY_REFLECTION_XML

#include <TinyXML/tinyxml2.h>
#include <cassert>

namespace Typhoon::Reflection {

XMLInputArchive::XMLInputArchive()
    : document(std::make_unique<tinyxml2::XMLDocument>())
    , currentNode(nullptr) {
}

XMLInputArchive::~XMLInputArchive() {
	assert(typeStack.size() <= 1); // "root" might be open
}

ParseResult XMLInputArchive::initialize(const char* buffer) {
	assert(buffer);
	currentNode = nullptr;
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

bool XMLInputArchive::read(bool& value) const {
	bool res = false;
	if (auto element = currentNode->ToElement(); element) {
		if (auto error = element->QueryBoolText(&value); error == tinyxml2::XML_SUCCESS) {
			res = true;
		}
	}
	return res;
}

bool XMLInputArchive::read(int& value) const {
	bool res = false;
	if (auto element = currentNode->ToElement(); element) {
		if (auto error = element->QueryIntText(&value); error == tinyxml2::XML_SUCCESS) {
			res = true;
		}
	}
	return res;
}

bool XMLInputArchive::read(unsigned int& value) const {
	bool res = false;
	if (auto element = currentNode->ToElement(); element) {
		if (auto error = element->QueryUnsignedText(&value); error == tinyxml2::XML_SUCCESS) {
			res = true;
		}
	}
	return res;
}

bool XMLInputArchive::read(int64_t& value) const {
	bool res = false;
	if (auto element = currentNode->ToElement(); element) {
		if (auto error = element->QueryInt64Text(&value); error == tinyxml2::XML_SUCCESS) {
			res = true;
		}
	}
	return res;
}

bool XMLInputArchive::read(uint64_t& value) const {
	bool res = false;
	if (auto element = currentNode->ToElement(); element) {
		if (auto error = element->QueryUnsigned64Text(&value); error == tinyxml2::XML_SUCCESS) {
			res = true;
		}
	}
	return res;
}
bool XMLInputArchive::read(float& value) const {
	bool res = false;
	if (auto element = currentNode->ToElement(); element) {
		if (auto error = element->QueryFloatText(&value); error == tinyxml2::XML_SUCCESS) {
			res = true;
		}
	}
	return res;
}

bool XMLInputArchive::read(double& value) const {
	bool res = false;
	if (auto element = currentNode->ToElement(); element) {
		if (auto error = element->QueryDoubleText(&value); error == tinyxml2::XML_SUCCESS) {
			res = true;
		}
	}
	return res;
}

bool XMLInputArchive::read(const char*& str) const {
	bool res = false;
	if (auto element = currentNode->ToElement(); element) {
		if (const char* text = element->GetText(); text) {
			str = text;
			res = true;
		}
	}
	return res;
}

bool XMLInputArchive::read(std::string_view& sv) const {
	if (const char* str = nullptr; read(str)) {
		sv = str;
		return true;
	}
	return false;
}

void XMLInputArchive::endElement() const {
	assert(currentNode);
	currentNode = currentNode->Parent();
	typeStack.pop();
}

bool XMLInputArchive::isObject() const {
	return (typeStack.top() == ValueType::Object) || (typeStack.top() == ValueType::Undefined);
}

bool XMLInputArchive::isArray() const {
	return (typeStack.top() == ValueType::Array) || (typeStack.top() == ValueType::Undefined);
}

ValueType XMLInputArchive::getValueType() const {
	return typeStack.top();
}

bool XMLInputArchive::beginElement(const char* name) const {
	if (! currentNode) {
		return false;
	}
	if (auto element = currentNode->FirstChildElement(name); element) {
		currentNode = element;
		readElementType();
		return true;
	}
	else {
		return false;
	}
}

bool XMLInputArchive::iterateChild(ArchiveIterator& it) const {
	tinyxml2::XMLNode* childIt = nullptr;
	if (it.getNode()) {
		typeStack.pop();
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
	readElementType();
	it.setNode(childIt);
	return true;
}

bool XMLInputArchive::iterateChild(ArchiveIterator& it, const char* name) const {
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
bool XMLInputArchive::readAttribute(const char* name, bool& value) const {
	if (tinyxml2::XMLElement* elem = currentNode->ToElement()) {
		if (auto error = elem->QueryBoolAttribute(name, &value); error == tinyxml2::XML_SUCCESS) {
			return true;
		}
	}
	return false;
}

bool XMLInputArchive::readAttribute(const char* name, int& value) const {
	if (tinyxml2::XMLElement* elem = currentNode->ToElement()) {
		if (auto error = elem->QueryIntAttribute(name, &value); error == tinyxml2::XML_SUCCESS) {
			return true;
		}
	}
	return false;
}

bool XMLInputArchive::readAttribute(const char* name, unsigned int& value) const {
	if (tinyxml2::XMLElement* elem = currentNode->ToElement()) {
		if (auto error = elem->QueryUnsignedAttribute(name, &value); error == tinyxml2::XML_SUCCESS) {
			return true;
		}
	}
	return false;
}

bool XMLInputArchive::readAttribute(const char* name, float& value) const {
	if (tinyxml2::XMLElement* elem = currentNode->ToElement()) {
		if (auto error = elem->QueryFloatAttribute(name, &value); error == tinyxml2::XML_SUCCESS) {
			return true;
		}
	}
	return false;
}

bool XMLInputArchive::readAttribute(const char* name, double& value) const {
	if (tinyxml2::XMLElement* elem = currentNode->ToElement()) {
		if (auto error = elem->QueryDoubleAttribute(name, &value); error == tinyxml2::XML_SUCCESS) {
			return true;
		}
	}
	return false;
}

bool XMLInputArchive::readAttribute(const char* name, const char*& str) const {
	tinyxml2::XMLElement* elem = currentNode->ToElement();
	if (elem) {
		if (auto error = elem->QueryStringAttribute(name, &str); error == tinyxml2::XML_SUCCESS) {
			return true;
		}
	}
	return false;
}

bool XMLInputArchive::readAttribute(const char* name, std::string_view& sv) const {
	if (const char* str = nullptr; readAttribute(name, str)) {
		sv = str;
		return true;
	}
	return false;
}

void XMLInputArchive::readElementType() const {
	ValueType type = ValueType::Undefined;
	if (const char* typeAttrib = nullptr; readAttribute("type", typeAttrib)) {
		if (! strcmp(typeAttrib, "object")) {
			type = ValueType::Object;
		}
		else if (! strcmp(typeAttrib, "array")) {
			type = ValueType::Array;
		}
	}
	typeStack.push(type);
}

} // namespace Typhoon::Reflection

#endif
