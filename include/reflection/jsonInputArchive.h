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

	bool readAttribute(const char* name, bool& value) const override;
	bool readAttribute(const char* name, int& value) const override;
	bool readAttribute(const char* name, unsigned int& value) const override;
	bool readAttribute(const char* name, float& value) const override;
	bool readAttribute(const char* name, double& value) const override;
	bool readAttribute(const char* name, const char*& str) const override;
	bool readAttribute(const char* name, std::string_view& sv) const override;

	using InputArchive::read;

private:
	bool                    beginAttribute(const char* name) const;
	const rapidjson::Value& getValue(const char* key) const;

private:
	using StackItem = const rapidjson::Value*;
	std::unique_ptr<rapidjson::Document> document;
	mutable std::stack<StackItem>        stack;
};

} // namespace Typhoon::Reflection

#endif
