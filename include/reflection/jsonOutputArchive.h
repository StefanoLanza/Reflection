#pragma once

#include "config.h"

#if TY_REFLECTION_JSON

#include "../external/rapidjson/include/rapidjson/fwd.h"
#include "../external/rapidjson/include/rapidjson/prettywriter.h"
#include "archive.h"
#include <memory>

namespace Typhoon::Reflection {

class JSONOutputArchive final : public OutputArchive {
public:
	JSONOutputArchive();
	~JSONOutputArchive();

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
	void writeAttributeKey(const char* key) const;

private:
	std::unique_ptr<rapidjson::StringBuffer>                          stream;
	std::unique_ptr<rapidjson::PrettyWriter<rapidjson::StringBuffer>> writer;
	bool                                                              endRoot;
};

} // namespace Typhoon::Reflection

#endif
