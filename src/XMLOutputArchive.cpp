#include "XMLOutputArchive.h"

#if TY_REFLECTION_XML

#include <TinyXML/tinyxml2.h>
#include <cassert>

namespace Typhoon::Reflection {

XMLOutputArchive::XMLOutputArchive(bool createRoot)
    : document(std::make_unique<tinyxml2::XMLDocument>()) {
	// Insert declaration "xml version=\"1.0\" encoding=\"UTF-8\""
	document->LinkEndChild(document->NewDeclaration());
	currentNode = document.get();

	if (createRoot) {
		setKey("root");
		beginObject();
		endRoot = true;
	}
	else {
		endRoot = false;
	}
}

XMLOutputArchive::~XMLOutputArchive() {
	assert(typeStack.empty());
}

bool XMLOutputArchive::saveToFile(const char* fileName) {
	assert(fileName);
	if (endRoot) {
		endObject(); // end root
		endRoot = false;
	}
	const tinyxml2::XMLError error = document->SaveFile(fileName);
	return error == tinyxml2::XML_SUCCESS;
}

std::string XMLOutputArchive::saveToString() {
	std::string          str;
	tinyxml2::XMLPrinter printer;
	// attach it to the document you want to convert in to a std::string
	if (document->Accept(&printer)) {
		// Create a std::string and copy your document data in to the string
		if (endRoot) {
			endObject(); // end root
			endRoot = false;
		}
		str = printer.CStr();
	}
	return str;
}

void XMLOutputArchive::setKey(const char* name) {
	beginElement(name);
}

void XMLOutputArchive::beginElement(const char* name) {
	assert(name);
	if (! typeStack.empty()) {
		assert(typeStack.top() != Type::array);
	}
	auto element = document->NewElement(name);
	currentNode = currentNode->InsertEndChild(element);
}

void XMLOutputArchive::endElement() {
	assert(currentNode);
	currentNode = currentNode->Parent();
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
	endElement();
}

bool XMLOutputArchive::beginArray() {
	beginArrayElement();
	typeStack.push(Type::array);
	writeAttribute("type", "array");
	return true;
}

void XMLOutputArchive::endArray() {
	assert(typeStack.top() == Type::array);
	typeStack.pop();
	endElement();
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

void XMLOutputArchive::write(bool value) {
	beginArrayElement();
	currentNode->ToElement()->SetText(value);
	endElement();
}

void XMLOutputArchive::write(int value) {
	beginArrayElement();
	currentNode->ToElement()->SetText(value);
	endElement();
}

void XMLOutputArchive::write(unsigned int value) {
	beginArrayElement();
	currentNode->ToElement()->SetText(value);
	endElement();
}

void XMLOutputArchive::write(int64_t value) {
	beginArrayElement();
	currentNode->ToElement()->SetText(value);
	endElement();
}

void XMLOutputArchive::write(uint64_t value) {
	beginArrayElement();
	currentNode->ToElement()->SetText(value);
	endElement();
}

void XMLOutputArchive::write(float value) {
	beginArrayElement();
	currentNode->ToElement()->SetText(value);
	endElement();
}

void XMLOutputArchive::write(double value) {
	beginArrayElement();
	currentNode->ToElement()->SetText(value);
	endElement();
}

void XMLOutputArchive::write(const char* str) {
	beginArrayElement();
	currentNode->ToElement()->SetText(str);
	endElement();
}

void XMLOutputArchive::write(std::string_view sv) {
	write(sv.data()); // FIXME Not necessarily null terminated
}

void XMLOutputArchive::beginArrayElement() {
	if (! typeStack.empty() && typeStack.top() == Type::array) {
		auto element = document->NewElement("element");
		currentNode = currentNode->InsertEndChild(element);
	}
}

} // namespace Typhoon::Reflection

#endif
