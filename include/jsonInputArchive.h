#pragma once

#include "config.h"

#if TY_REFLECTION_JSON

#include "archive.h"
#include <memory>
#include <rapidjson-master/include/rapidjson/fwd.h>
#include <stack>

namespace Typhoon::Reflection {

class JSONInputArchive final : public InputArchive {
public:
	JSONInputArchive();
	~JSONInputArchive();

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

	bool readAttribute(const char* name, bool& value) override;
	bool readAttribute(const char* name, char& value) override;
	bool readAttribute(const char* name, unsigned char& value) override;
	bool readAttribute(const char* name, int& value) override;
	bool readAttribute(const char* name, unsigned int& value) override;
	bool readAttribute(const char* name, unsigned short& value) override;
	bool readAttribute(const char* name, short& value) override;
	bool readAttribute(const char* name, float& value) override;
	bool readAttribute(const char* name, double& value) override;
	bool readAttribute(const char* name, const char** str) override;

private:
	enum class StackState {
		beforeStart, //!< An object/array is in the stack but it is not yet called by StartObject()/StartArray().
		started,     //!< An object/array is called by StartObject()/StartArray().
		closed       //!< An array is closed after read all element, but before EndArray().
	};

	struct StackItem {
		const rapidjson::Value* value;
		StackState              state;
		rapidjson::SizeType     index; // For array iteration
	};

	std::unique_ptr<rapidjson::Document> document;

	std::stack<StackItem> stack;
};

} // namespace Typhoon::Reflection

#endif
