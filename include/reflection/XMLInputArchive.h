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
	bool        beginObject(const char* key) override;
	bool        beginArray(const char* key) override;
	bool        iterateChild(ArchiveIterator& it) override;
	bool        iterateChild(ArchiveIterator& it, const char* name) override;
	bool        read(bool& value) override;
	bool        read(int& value) override;
	bool        read(unsigned int& value) override;
	bool        read(int64_t& value) override;
	bool        read(uint64_t& value) override;
	bool        read(float& value) override;
	bool        read(double& value) override;
	bool        read(const char*& str) override;
	bool        read(std::string_view& sv) override;
	bool        readAttribute(const char* name, bool& value) override;
	bool        readAttribute(const char* name, int& value) override;
	bool        readAttribute(const char* name, unsigned int& value) override;
	bool        readAttribute(const char* name, float& value) override;
	bool        readAttribute(const char* name, double& value) override;
	bool        readAttribute(const char* name, const char*& str) override;
	bool        readAttribute(const char* name, std::string_view& sv) override;

	using InputArchive::read;

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
