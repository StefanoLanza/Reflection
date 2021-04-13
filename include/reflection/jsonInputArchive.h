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
#if TY_REFLECTION_DEPRECATED
	bool iterateChild(ArchiveIterator& it, const char* name) override;
#endif

	bool read(bool& value) const override;
	bool read(int& value) const override;
	bool read(unsigned int& value) const override;
	bool read(int64_t& value) const override;
	bool read(uint64_t& value) const override;
	bool read(float& value) const override;
	bool read(double& value) const override;
	bool read(const char*& str) const override;

	bool readAttribute(const char* name, bool& value) override;
	bool readAttribute(const char* name, int& value) override;
	bool readAttribute(const char* name, unsigned int& value) override;
	bool readAttribute(const char* name, float& value) override;
	bool readAttribute(const char* name, double& value) override;
	bool readAttribute(const char* name, const char*& str) override;

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
