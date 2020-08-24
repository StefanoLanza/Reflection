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
	assert(typeStack.empty());
}

ParseResult XMLInputArchive::initialize(const char* buffer) {
	assert(buffer);
	currentNode = nullptr;
	const auto error = document->Parse(buffer);
	if (error == tinyxml2::XML_SUCCESS) {
		currentNode = document.get();
		beginElement("root"); // might fail
		return { true };
	}
	else {
		return { false, document->ErrorName(), document->ErrorLineNum() };
	}
}

const char* XMLInputArchive::currNodeText() {
	const tinyxml2::XMLNode* const textNode = currentNode->FirstChild();
	const char*                    res = nullptr;
	if (textNode) {
		const tinyxml2::XMLText* const text = textNode->ToText();
		if (text) {
			res = text->Value();
		}
	}
	return res;
}

bool XMLInputArchive::readBool(const char* key, bool& value)
{
	return false;
}

bool XMLInputArchive::readInt(const char* key, int& value)
{
	return false;
}

bool XMLInputArchive::readUInt(const char* key, unsigned int& value)
{
	return false;
}

bool XMLInputArchive::readFloat(const char* key, float& value)
{
	return false;
}

bool XMLInputArchive::readDouble(const char* key, double& value)
{
	return false;
}

bool XMLInputArchive::readString(const char* key, const char*& str) {
	bool res = false;
	if (auto element = currentNode->FirstChildElement(key); element) {
		if (const char* text = element->GetText(); text) {
			str = text;
			res = true;
		}
	}
	return res;
}

void XMLInputArchive::endElement() {
	if (currentNode) {
		currentNode = currentNode->Parent();
	}
}

bool XMLInputArchive::beginObject() {
	const char* type = nullptr;
	if (readAttribute("type", type)) {
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
	const char* type = nullptr;
	if (readAttribute("type", type)) {
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

bool XMLInputArchive::beginElement(const char* name) {
	if (! currentNode) {
		return false;
	}
	tinyxml2::XMLElement* const element = currentNode->FirstChildElement(name);
	if (! element) {
		return false;
	}
	else {
		currentNode = element;
		return true;
	}
}

bool XMLInputArchive::iterateChild(ArchiveIterator& it) {
	tinyxml2::XMLNode* childIt = nullptr;
	if (it.getNode()) {
		childIt = reinterpret_cast<tinyxml2::XMLNode*>(it.getNode())->NextSibling();
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
		currentNode = reinterpret_cast<tinyxml2::XMLNode*>(it.getNode())->Parent();
		it.reset();
		return false;
	}

	currentNode = childIt;
	it.reset(childIt);
	return true;
}

#if TY_REFLECTION_DEPRECATED

bool XMLInputArchive::iterateChild(ArchiveIterator& it, const char* name) {
	tinyxml2::XMLNode* childIt = nullptr;
	if (it.getNode()) {
		childIt = reinterpret_cast<tinyxml2::XMLNode*>(it.getNode())->NextSiblingElement(name);
		if (childIt == nullptr) {
			currentNode = reinterpret_cast<tinyxml2::XMLNode*>(it.getNode())->Parent();
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
	it.reset(childIt);
	return true;
}

#endif

bool XMLInputArchive::readAttribute(const char* name, bool& value) {
	if (tinyxml2::XMLElement* elem = currentNode->ToElement()) {
		if (auto error = elem->QueryBoolAttribute(name, &value); error == tinyxml2::XML_SUCCESS) {
			return true;
		}
	}
	return false;
}

bool XMLInputArchive::readAttribute(const char* name, int& value) {
	if (tinyxml2::XMLElement* elem = currentNode->ToElement()) {
		if (auto error = elem->QueryIntAttribute(name, &value); error == tinyxml2::XML_SUCCESS) {
			return true;
		}
	}
	return false;
}

bool XMLInputArchive::readAttribute(const char* name, unsigned int& value) {
	if (tinyxml2::XMLElement* elem = currentNode->ToElement()) {
		if (auto error = elem->QueryUnsignedAttribute(name, &value); error == tinyxml2::XML_SUCCESS) {
			return true;
		}
	}
	return false;
}

bool XMLInputArchive::readAttribute(const char* name, float& value) {
	if (tinyxml2::XMLElement* elem = currentNode->ToElement()) {
		if (auto error = elem->QueryFloatAttribute(name, &value); error == tinyxml2::XML_SUCCESS) {
			return true;
		}
	}
	return false;
}

bool XMLInputArchive::readAttribute(const char* name, double& value) {
	if (tinyxml2::XMLElement* elem = currentNode->ToElement()) {
		if (auto error = elem->QueryDoubleAttribute(name, &value); error == tinyxml2::XML_SUCCESS) {
			return true;
		}
	}
	return false;
}

bool XMLInputArchive::readAttribute(const char* name, const char*& str) {
	tinyxml2::XMLElement* elem = currentNode->ToElement();
	if (elem) {
		if (auto error = elem->QueryStringAttribute(name, &str); error == tinyxml2::XML_SUCCESS) {
			return true;
		}
	}
	return false;
}

} // namespace Typhoon::Reflection

#endif
