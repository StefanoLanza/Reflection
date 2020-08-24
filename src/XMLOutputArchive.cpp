#include "XMLOutputArchive.h"

#if TY_REFLECTION_XML

#include <TinyXML/tinyxml2.h>
#include <cassert>

namespace Typhoon::Reflection {

XMLOutputArchive::XMLOutputArchive()
    : document(std::make_unique<tinyxml2::XMLDocument>()) {
	// Insert declaration "xml version=\"1.0\" encoding=\"UTF-8\""
	document->LinkEndChild(document->NewDeclaration());
	currentNode = document.get();
}

XMLOutputArchive::~XMLOutputArchive() {
	assert(typeStack.empty());
}

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

std::string_view XMLOutputArchive::getString() {
	return {};
}

void XMLOutputArchive::beginRoot() {
	beginElement("root");
}

void XMLOutputArchive::endRoot() {
	endElement();
}

bool XMLOutputArchive::beginElement(const char* name) {
	beginArrayElement();
	assert(name);
	auto element = document->NewElement(name);
	currentNode = currentNode->InsertEndChild(element);
	return true;
}

void XMLOutputArchive::endElement() {
	assert(currentNode);
	currentNode = currentNode->Parent();
	endArrayElement();
}

bool XMLOutputArchive::beginObject() {
	beginArrayElement();
	typeStack.push(Type::object);
	writeAttribute("type", "object");
	return true;
}

void XMLOutputArchive::endObject() {
	assert(typeStack.top() == Type::object);
	typeStack.pop();
	endArrayElement();
}

bool XMLOutputArchive::beginArray() {
	typeStack.push(Type::array);
	writeAttribute("type", "array");
	return true;
}

void XMLOutputArchive::endArray() {
	assert(typeStack.top() == Type::array);
	typeStack.pop();
}

void XMLOutputArchive::writeAttribute(const char* name, const char* str) {
	assert(str);
	currentNode->ToElement()->SetAttribute(name, str);
}

void XMLOutputArchive::writeAttribute(const char* name, bool value) {
	currentNode->ToElement()->SetAttribute(name, value);
}

void XMLOutputArchive::writeAttribute(const char* name, int value) {
	currentNode->ToElement()->SetAttribute(name, value);
}

void XMLOutputArchive::writeAttribute(const char* name, unsigned int value) {
	currentNode->ToElement()->SetAttribute(name, value);
}

void XMLOutputArchive::writeAttribute(const char* name, float value) {
	currentNode->ToElement()->SetAttribute(name, value);
}

void XMLOutputArchive::writeAttribute(const char* name, double value) {
	currentNode->ToElement()->SetAttribute(name, value);
}

bool XMLOutputArchive::write(const char* text) {
	beginArrayElement(); // insert "element" node for array
	bool res = currentNode->InsertEndChild(document->NewText(text)) != nullptr;
	endArrayElement();
	return res;
}

bool XMLOutputArchive::writeBool(const char* key, bool value) {
	auto element = document->NewElement(key);
	element->SetText(value);
	currentNode->InsertEndChild(element);
	return true;
}

bool XMLOutputArchive::writeInt(const char* key, int value) {
	auto element = document->NewElement(key);
	element->SetText(value);
	currentNode->InsertEndChild(element);
	return true;
}

bool XMLOutputArchive::writeUInt(const char* key, unsigned int value) {
	auto element = document->NewElement(key);
	element->SetText(value);
	currentNode->InsertEndChild(element);
	return true;
}

bool XMLOutputArchive::writeFloat(const char* key, float value) {
	auto element = document->NewElement(key);
	element->SetText(value);
	currentNode->InsertEndChild(element);
	return true;
}

bool XMLOutputArchive::writeDouble(const char* key, double value) {
	auto element = document->NewElement(key);
	element->SetText(value);
	currentNode->InsertEndChild(element);
	return true;
}

bool XMLOutputArchive::writeString(const char* key, const char* str) {
	auto element = document->NewElement(key);
	element->SetText(str);
	currentNode->InsertEndChild(element);
	return true;
}

void XMLOutputArchive::beginArrayElement() {
	if (! typeStack.empty() && typeStack.top() == Type::array) {
		auto element = document->NewElement("element");
		currentNode = currentNode->InsertEndChild(element);
	}
}

void XMLOutputArchive::endArrayElement() {
	if (! typeStack.empty() && typeStack.top() == Type::array) {
		assert(currentNode);
		currentNode = currentNode->Parent();
	}
}

} // namespace Typhoon::Reflection

#endif
