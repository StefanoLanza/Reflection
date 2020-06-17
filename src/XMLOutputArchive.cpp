#include "XMLOutputArchive.h"

#if TY_REFLECTION_XML

#include <TinyXML/tinyxml2.h>
#include <cassert>

namespace Typhoon {

XMLOutputArchive::XMLOutputArchive()
    : document(std::make_unique<tinyxml2::XMLDocument>()) {
	// Insert declaration "xml version=\"1.0\" encoding=\"UTF-8\""
	document->LinkEndChild(document->NewDeclaration());
	currentNode = document.get();
}

XMLOutputArchive::~XMLOutputArchive() = default;

bool XMLOutputArchive::saveToFile(const char* fileName) {
	assert(fileName);
	const tinyxml2::XMLError error = document->SaveFile(fileName);
	return error == tinyxml2::XML_SUCCESS;
}

bool XMLOutputArchive::saveToString(std::string& string) {
	tinyxml2::XMLPrinter printer;
	// attach it to the document you want to convert in to a std::string
	if (document->Accept(&printer)) {
		// Create a std::string and copy your document data in to the string
		string = printer.CStr();
		return true;
	}
	return false;
}

bool XMLOutputArchive::beginElement(const char* name) {
	auto element = document->NewElement(name);
	currentNode = currentNode->InsertEndChild(element);
	return true;
}

void XMLOutputArchive::endElement() {
	assert(currentNode);
	currentNode = currentNode->Parent();
}

bool XMLOutputArchive::writeAttribute(const char* name, const char* str) {
	assert(str);
	currentNode->ToElement()->SetAttribute(name, str);
	return true;
}

bool XMLOutputArchive::writeAttribute(const char* name, bool value) {
	currentNode->ToElement()->SetAttribute(name, value);
	return true;
}

bool XMLOutputArchive::writeAttribute(const char* name, char value) {
	int ivalue = static_cast<int>(value);
	return writeAttribute(name, ivalue);
}

bool XMLOutputArchive::writeAttribute(const char* name, unsigned char value) {
	int ivalue = static_cast<int>(value);
	return writeAttribute(name, ivalue);
}

bool XMLOutputArchive::writeAttribute(const char* name, int value) {
	currentNode->ToElement()->SetAttribute(name, value);
	return true;
}

bool XMLOutputArchive::writeAttribute(const char* name, unsigned int value) {
	currentNode->ToElement()->SetAttribute(name, value);
	return true;
}

bool XMLOutputArchive::writeAttribute(const char* name, short value) {
	int ivalue = static_cast<int>(value);
	return writeAttribute(name, ivalue);
}

bool XMLOutputArchive::writeAttribute(const char* name, unsigned short value) {
	int ivalue = static_cast<int>(value);
	return writeAttribute(name, ivalue);
}

bool XMLOutputArchive::writeAttribute(const char* name, float value) {
	currentNode->ToElement()->SetAttribute(name, value);
	return true;
}

bool XMLOutputArchive::writeAttribute(const char* name, double value) {
	currentNode->ToElement()->SetAttribute(name, value);
	return true;
}

bool XMLOutputArchive::write(const char* text) {
	return currentNode->InsertEndChild(document->NewText(text)) != nullptr;
}

} // namespace Typhoon

#endif
