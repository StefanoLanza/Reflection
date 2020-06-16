#include "XMLInputArchive.h"

#if TY_REFLECTION_XML

#include <TinyXML/tinyxml2.h>
#include <cassert>

using namespace Typhoon;

namespace Typhoon {

XMLInputArchive::XMLInputArchive()
    : document(std::make_unique<tinyxml2::XMLDocument>())
    , currentNode(nullptr) {
}

XMLInputArchive::~XMLInputArchive() = default;

bool XMLInputArchive::initialize(const char* buffer) {
	assert(buffer);
	currentNode = nullptr;
	const auto error = document->Parse(buffer);
	if (error == tinyxml2::XML_SUCCESS) {
		currentNode = document.get();
		return true;
	}
	else {
		return false;
	}
}

std::string XMLInputArchive::getErrorDesc() const {
	return document->ErrorName() + std::string { " ,line: " } + std::to_string(document->ErrorLineNum());
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

void XMLInputArchive::endElement() {
	if (currentNode) {
		currentNode = currentNode->Parent();
	}
}

bool XMLInputArchive::beginElement() {
	tinyxml2::XMLElement* const element = currentNode->FirstChildElement();
	if (! element) {
		return false;
	}
	else {
		currentNode = element;
		return true;
	}
}

bool XMLInputArchive::beginElement(const char* name) {
	assert(name);
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
	if (! it) {
		childIt = currentNode->FirstChildElement();
		if (childIt == nullptr) {
			it.reset();
			return false;
		}
	}
	else {
		childIt = reinterpret_cast<tinyxml2::XMLNode*>(it.getNode())->NextSibling();
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

bool XMLInputArchive::iterateChild(ArchiveIterator& it, const char* name) {
	tinyxml2::XMLNode* childIt = nullptr;
	if (! it) {
		childIt = currentNode->FirstChildElement(name);
		if (childIt == nullptr) {
			it.reset();
			return false;
		}
	}
	else {
		childIt = reinterpret_cast<tinyxml2::XMLNode*>(it.getNode())->NextSiblingElement(name);
		if (childIt == nullptr) {
			currentNode = reinterpret_cast<tinyxml2::XMLNode*>(it.getNode())->Parent();
			it.reset();
			return false;
		}
	}
	currentNode = childIt;

	it.reset(childIt);
	return true;
}

bool XMLInputArchive::readAttribute(const char* name, const char** str) {
	tinyxml2::XMLElement* elem = currentNode->ToElement();
	if (elem) {
		const char* attr = elem->Attribute(name);
		if (attr) {
			*str = attr;
			return true;
		}
	}
	return false;
}

bool XMLInputArchive::readAttribute(const char* name, bool& value) {
	const char* str = nullptr;
	if (readAttribute(name, &str)) {
		if (! _stricmp(str, "true")) {
			value = true;
		}
		else if (! _stricmp(str, "false")) {
			value = false;
		}
		return true;
	}
	return false;
}

bool XMLInputArchive::readAttribute(const char* name, char& value) {
	int ivalue;
	if (readAttribute(name, ivalue)) {
		value = static_cast<char>(ivalue);
		return true;
	}
	else {
		return false;
	}
}

bool XMLInputArchive::readAttribute(const char* name, unsigned char& value) {
	int ivalue = 0;
	if (readAttribute(name, ivalue)) {
		value = static_cast<unsigned char>(ivalue);
		return true;
	}
	else {
		return false;
	}
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

bool XMLInputArchive::readAttribute(const char* name, short& value) {
	int ivalue = 0;
	if (readAttribute(name, ivalue)) {
		value = static_cast<short>(ivalue);
		return true;
	}
	else {
		return false;
	}
}

bool XMLInputArchive::readAttribute(const char* name, unsigned short& value) {
	int ivalue = 0;
	if (readAttribute(name, ivalue)) {
		value = static_cast<unsigned short>(ivalue);
		return true;
	}
	else {
		return false;
	}
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

} // namespace Typhoon

#endif
