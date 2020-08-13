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

class XMLInputArchive final : public InputArchive {
public:
	XMLInputArchive();
	~XMLInputArchive();

	ParseResult initialize(const char* buffer);
	bool        beginElement(const char* name) override;
	void        endElement() override;
	bool        beginObject() override;
	void        endObject() override;
	bool        beginArray() override;
	void        endArray() override;
	bool        iterateChild(ArchiveIterator& it) override;
	bool        iterateChild(ArchiveIterator& it, const char* name) override;
	const char* currNodeText() override;
	bool        readAttribute(const char* name, bool& value) override;
	bool        readAttribute(const char* name, char& value) override;
	bool        readAttribute(const char* name, unsigned char& value) override;
	bool        readAttribute(const char* name, int& value) override;
	bool        readAttribute(const char* name, unsigned int& value) override;
	bool        readAttribute(const char* name, unsigned short& value) override;
	bool        readAttribute(const char* name, short& value) override;
	bool        readAttribute(const char* name, float& value) override;
	bool        readAttribute(const char* name, double& value) override;
	bool        readAttribute(const char* name, const char** str) override;

private:
	std::unique_ptr<tinyxml2::XMLDocument> document;    // The tinyxml document object
	tinyxml2::XMLNode*                     currentNode; // The node that is currently being processed

	enum class Type {
		array,
		object
	};
	std::stack<Type> typeStack;
};

} // namespace Typhoon::Reflection

#endif
