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
	void             beginRoot() override;
	void             endRoot() override;
	bool             beginElement(const char* name) override;
	void             endElement() override;
	bool             beginObject() override;
	void             endObject() override;
	bool             beginArray() override;
	void             endArray() override;
	void             writeAttribute(const char* name, bool value) override;
	void             writeAttribute(const char* name, int value) override;
	void             writeAttribute(const char* name, unsigned int value) override;
	void             writeAttribute(const char* name, float value) override;
	void             writeAttribute(const char* name, double value) override;
	void             writeAttribute(const char* name, const char* str) override;
	void             write(const char* text) override;
	void             writeBool(const char* key, bool value) override;
	void             writeInt(const char* key, int value) override;
	void             writeUInt(const char* key, unsigned int value) override;
	void             writeInt64(const char* key, int64_t value) override;
	void             writeUInt64(const char* key, uint64_t value) override;
	void             writeFloat(const char* key, float value) override;
	void             writeDouble(const char* key, double value) override;
	void             writeString(const char* key, const char* str) override;

private:
	void beginArrayElement();
	void endArrayElement();

private:
	enum class Type { array, object };

	std::unique_ptr<tinyxml2::XMLDocument> document;    // The tinyxml document object
	tinyxml2::XMLNode*                     currentNode; // The node that is currently being processed
	std::stack<Type>                       typeStack;
};

} // namespace Typhoon::Reflection

#endif
