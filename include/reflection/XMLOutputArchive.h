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
	std::string      saveToString() override;
	std::string_view getString() override;
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
	bool             write(bool value) override;
	bool             write(int value) override;
	bool             write(unsigned int value) override;
	bool             write(int64_t value) override;
	bool             write(uint64_t value) override;
	bool             write(float value) override;
	bool             write(double value) override;
	bool             write(const char* str) override;
	bool             write(std::string_view str) override;

	using OutputArchive::write;

private:
	void beginArrayElement();
	void endArrayElement();

private:
	enum class Type {
		array,
		object
	};

	std::unique_ptr<tinyxml2::XMLDocument> document;    // The tinyxml document object
	tinyxml2::XMLNode*                     currentNode; // The node that is currently being processed
	std::stack<Type>                       typeStack;
	bool                                   endRoot;
};

} // namespace Typhoon::Reflection

#endif
