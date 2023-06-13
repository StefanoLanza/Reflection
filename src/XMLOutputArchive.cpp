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
	beginElement("root");
	endRoot = true;
}

XMLOutputArchive::~XMLOutputArchive() {
	assert(typeStack.empty());
}

bool XMLOutputArchive::saveToFile(const char* fileName) {
	assert(fileName);

	if (endRoot) {
		endElement(); // end root
		endRoot = false;
	}
	const tinyxml2::XMLError error = document->SaveFile(fileName);
	return error == tinyxml2::XML_SUCCESS;
}

std::string XMLOutputArchive::saveToString() {
	std::string str;
	tinyxml2::XMLPrinter printer;
	// attach it to the document you want to convert in to a std::string
	if (document->Accept(&printer)) {
		// Create a std::string and copy your document data in to the string
		if (endRoot) {
			endElement(); // end root
			endRoot = false;
		}
		str = printer.CStr();
	}
	return str;
}

bool XMLOutputArchive::beginElement(const char* name) {
	assert(name);
	beginArrayElement();
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
	endArrayElement();
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

bool XMLOutputArchive::write(bool value) {
	beginArrayElement();
	currentNode->ToElement()->SetText(value);
	endArrayElement();
	return true;
}

bool XMLOutputArchive::write(int value) {
	beginArrayElement();
	currentNode->ToElement()->SetText(value);
	endArrayElement();
	return true;
}

bool XMLOutputArchive::write(unsigned int value) {
	beginArrayElement();
	currentNode->ToElement()->SetText(value);
	endArrayElement();
	return true;
}

bool XMLOutputArchive::write(int64_t value) {
	beginArrayElement();
	currentNode->ToElement()->SetText(value);
	endArrayElement();
	return true;
}

bool XMLOutputArchive::write(uint64_t value) {
	beginArrayElement();
	currentNode->ToElement()->SetText(value);
	endArrayElement();
	return true;
}

bool XMLOutputArchive::write(float value) {
	beginArrayElement();
	currentNode->ToElement()->SetText(value);
	endArrayElement();
	return true;
}

bool XMLOutputArchive::write(double value) {
	beginArrayElement();
	currentNode->ToElement()->SetText(value);
	endArrayElement();
	return true;
}

bool XMLOutputArchive::write(const char* str) {
	beginArrayElement();
	currentNode->ToElement()->SetText(str);
	endArrayElement();
	return true;
}

bool XMLOutputArchive::write(std::string_view sv) {
	return write(sv.data());//FIXME Not necessarily null terminated
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
