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

	ParseResult         initialize(const char* buffer);
	InputArchiveElement beginElement(const void* element, const char* name) override;
	void                endElement() override;
	bool                beginObject() override;
	void                endObject() override;
	bool                beginArray() override;
	void                endArray() override;
	bool                beginObject(const char* key) override;
	bool                beginArray(const char* key) override;
	bool                iterateChild(ArchiveIterator& it) override;
	bool                iterateChild(ArchiveIterator& it, const char* name) override;
	bool                read(const void* element, bool& value) const override;
	bool                read(const void* element, int& value) const override;
	bool                read(const void* element, unsigned int& value) const override;
	bool                read(const void* element, int64_t& value) const override;
	bool                read(const void* element, uint64_t& value) const override;
	bool                read(const void* element, float& value) const override;
	bool                read(const void* element, double& value) const override;
	bool                read(const void* element, const char*& str) const override;
	bool                read(const void* element, std::string_view& sv) const override;

	bool readAttribute(const void* element, const char* name, bool& value) const override;
	bool readAttribute(const void* element, const char* name, int& value) const override;
	bool readAttribute(const void* element, const char* name, unsigned int& value) const override;
	bool readAttribute(const void* element, const char* name, float& value) const override;
	bool readAttribute(const void* element, const char* name, double& value) const override;
	bool readAttribute(const void* element, const char* name, const char*& str) const override;
	bool readAttribute(const void* element, const char* name, std::string_view& sv) const override;

	bool readAttribute(const void* element, const char* attributeName, const char*& str) const;

	using InputArchive::read;

private:
	bool isObject(const void* element) const override;
	bool isArray(const void* element) const override;

private:
	std::unique_ptr<tinyxml2::XMLDocument> document;    // The tinyxml document object
	
	enum class Type {
		array,
		object
	};
	std::stack<Type> typeStack;
};

} // namespace Typhoon::Reflection

#endif
