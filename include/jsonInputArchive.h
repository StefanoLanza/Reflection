#pragma once

#include "config.h"

#if TY_REFLECTION_JSON

#include "archive.h"
#include <memory>
#include "../external/rapidjson-master/include/rapidjson/fwd.h"
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
	bool        beginObject(const char* key) override;
	bool        beginArray(const char* key) override;
	bool        iterateChild(ArchiveIterator& it) override;
#if TY_REFLECTION_DEPRECATED
	bool iterateChild(ArchiveIterator& it, const char* name) override;
#endif

	const char* currNodeText() override;

	bool readBool(bool& value) override;
	bool readInt(const char* key, int& value) override;
	bool readUInt(const char* key, unsigned int& value) override;
	bool readInt64(const char* key, int64_t& value) override;
	bool readUInt64(const char* key, uint64_t& value) override;
	bool readFloat(const char* key, float& value) override;
	bool readDouble(const char* key, double& value) override;
	bool readString(const char* key, const char*& str) override;

	bool readAttribute(const char* name, bool& value) override;
	bool readAttribute(const char* name, int& value) override;
	bool readAttribute(const char* name, unsigned int& value) override;
	bool readAttribute(const char* name, float& value) override;
	bool readAttribute(const char* name, double& value) override;
	bool readAttribute(const char* name, const char*& str) override;

private:
	bool                    beginAttribute(const char* name);
	const rapidjson::Value& getValue(const char* key) const;

private:
	struct StackItem {
		const rapidjson::Value* value;
	};

	std::unique_ptr<rapidjson::Document> document;
	std::stack<StackItem>                stack;
};

} // namespace Typhoon::Reflection

#endif
