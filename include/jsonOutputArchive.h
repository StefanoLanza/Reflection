#pragma once

#include "config.h"

#if TY_REFLECTION_JSON

#include "archive.h"
#include <memory>
#include <rapidjson/include/rapidjson/fwd.h>
#include <rapidjson/include/rapidjson/prettywriter.h>

namespace Typhoon::Reflection {

class JSONOutputArchive final : public OutputArchive {
public:
	JSONOutputArchive();
	~JSONOutputArchive();

	bool             saveToFile(const char* filename) override;
	bool             saveToString(std::string& string) override;
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
	void             write(const char* text) override;
	void             writeBool(bool value) override;
	void             writeInt(int value) override;
	void             writeUInt(unsigned int value) override;
	void             writeInt64(int64_t value) override;
	void             writeUInt64(uint64_t value) override;
	void             writeFloat(float value) override;
	void             writeDouble(double value) override;
	void             writeString(const char* str) override;

private:
	void writeAttributeKey(const char* key) const;

private:
	std::unique_ptr<rapidjson::StringBuffer>                          stream;
	std::unique_ptr<rapidjson::PrettyWriter<rapidjson::StringBuffer>> writer;
	bool                                                              endRoot;
};

} // namespace Typhoon::Reflection

#endif
