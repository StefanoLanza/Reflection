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
	bool        beginElement(const char* name) const override;
	void        endElement() const override;
	bool        isObject() const override;
	bool        isArray() const override;
	ValueType   getValueType() const override;
	bool        iterateChild(ArchiveIterator& it) const override;
	bool        iterateChild(ArchiveIterator& it, const char* name) const override;
	bool        read(bool& value) const override;
	bool        read(int& value) const override;
	bool        read(unsigned int& value) const override;
	bool        read(int64_t& value) const override;
	bool        read(uint64_t& value) const override;
	bool        read(float& value) const override;
	bool        read(double& value) const override;
	bool        read(const char*& str) const override;
	bool        read(std::string_view& sv) const override;
	bool        readAttribute(const char* name, bool& value) const override;
	bool        readAttribute(const char* name, int& value) const override;
	bool        readAttribute(const char* name, unsigned int& value) const override;
	bool        readAttribute(const char* name, float& value) const override;
	bool        readAttribute(const char* name, double& value) const override;
	bool        readAttribute(const char* name, const char*& str) const override;
	bool        readAttribute(const char* name, std::string_view& sv) const override;

	using InputArchive::read;

private:
	void readElementType() const;

private:
	std::unique_ptr<tinyxml2::XMLDocument> document;    // The tinyxml document object
	mutable tinyxml2::XMLNode*             currentNode; // The node that is currently being processed
	mutable std::stack<ValueType>          typeStack;
};

} // namespace Typhoon::Reflection

#endif
