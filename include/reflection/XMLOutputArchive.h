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
	XMLOutputArchive(bool createRoot = true);
	~XMLOutputArchive();

	bool        saveToFile(const char* filename) override;
	std::string saveToString() override;
	void        setKey(const char* name) override;
	bool        beginObject() override;
	void        endObject() override;
	bool        beginArray() override;
	void        endArray() override;
	void        writeAttribute(const char* name, bool value) override;
	void        writeAttribute(const char* name, int value) override;
	void        writeAttribute(const char* name, unsigned int value) override;
	void        writeAttribute(const char* name, float value) override;
	void        writeAttribute(const char* name, double value) override;
	void        writeAttribute(const char* name, const char* str) override;
	void        write(bool value) override;
	void        write(int value) override;
	void        write(unsigned int value) override;
	void        write(int64_t value) override;
	void        write(uint64_t value) override;
	void        write(float value) override;
	void        write(double value) override;
	void        write(const char* str) override;
	void        write(std::string_view str) override;

	using OutputArchive::write;

private:
	void beginElement(const char* key);
	void endElement();
	void beginArrayElement();

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
