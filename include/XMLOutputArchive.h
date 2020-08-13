#pragma once

#include "config.h"

#if TY_REFLECTION_XML

#include "archive.h"
#include <memory>
#include <stack>

// External classes
namespace tinyxml2 {

class XMLDocument;
class XMLElement;
class XMLNode;

} // namespace tinyxml2

namespace Typhoon::Reflection {

class XMLOutputArchive final : public OutputArchive {
public:
	XMLOutputArchive();
	~XMLOutputArchive();

	bool             saveToFile(const char* filename) override;
	bool             saveToString(std::string& string) override;
	std::string_view getString() override;
	bool             beginElement(const char* name) override;
	void             endElement() override;
	bool             beginObject() override;
	void             endObject() override;
	bool             beginArray() override;
	void             endArray() override;
	bool             writeAttribute(const char* name, bool value) override;
	bool             writeAttribute(const char* name, char value) override;
	bool             writeAttribute(const char* name, unsigned char value) override;
	bool             writeAttribute(const char* name, int value) override;
	bool             writeAttribute(const char* name, unsigned int value) override;
	bool             writeAttribute(const char* name, unsigned short value) override;
	bool             writeAttribute(const char* name, short value) override;
	bool             writeAttribute(const char* name, float value) override;
	bool             writeAttribute(const char* name, double value) override;
	bool             writeAttribute(const char* name, const char* str) override;
	bool             write(const char* text) override;

private:
	enum class Type { array, object };

	std::unique_ptr<tinyxml2::XMLDocument> document;    // The tinyxml document object
	tinyxml2::XMLNode*                     currentNode; // The node that is currently being processed
	std::stack<Type>                       typeStack;
};

} // namespace Typhoon::Reflection

#endif
