#pragma once

#include "config.h"

#if TY_REFLECTION_JSON

#include "../external/rapidjson/include/rapidjson/fwd.h"
#include "archive.h"
#include <memory>
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

	bool readAttribute(const char* name, bool& value) override;
	bool readAttribute(const char* name, int& value) override;
	bool readAttribute(const char* name, unsigned int& value) override;
	bool readAttribute(const char* name, float& value) override;
	bool readAttribute(const char* name, double& value) override;
	bool readAttribute(const char* name, const char*& str) override;
	bool readAttribute(const char* name, std::string_view& sv) override;

	using InputArchive::read;

private:
	bool                    beginAttribute(const char* name);
	const rapidjson::Value& getValue(const char* key) const;

private:
	struct StackItem {
		const char*             name;
		const rapidjson::Value* value;
	};

	std::unique_ptr<rapidjson::Document> document;
	std::stack<StackItem>                stack;
};

} // namespace Typhoon::Reflection

#endif
