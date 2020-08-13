#pragma once

#include "config.h"

#if TY_REFLECTION_JSON

#include "archive.h"
#include <memory>
#include <rapidjson-master/include/rapidjson/fwd.h>
#include <rapidjson-master/include/rapidjson/prettywriter.h>

namespace Typhoon::Reflection {

class JSONOutputArchive final : public OutputArchive {
public:
	JSONOutputArchive();
	~JSONOutputArchive();

	bool saveToFile(const char* filename) override;
	bool saveToString(std::string& string) override;
	std::string_view getString() override;
	bool beginElement(const char* name) override;
	void endElement() override;
	bool beginObject() override;
	void endObject() override;
	bool beginArray() override;
	void endArray() override;
	bool writeAttribute(const char* name, bool value) override;
	bool writeAttribute(const char* name, char value) override;
	bool writeAttribute(const char* name, unsigned char value) override;
	bool writeAttribute(const char* name, int value) override;
	bool writeAttribute(const char* name, unsigned int value) override;
	bool writeAttribute(const char* name, unsigned short value) override;
	bool writeAttribute(const char* name, short value) override;
	bool writeAttribute(const char* name, float value) override;
	bool writeAttribute(const char* name, double value) override;
	bool writeAttribute(const char* name, const char* str) override;
	bool write(const char* text) override;

private:
	std::unique_ptr<rapidjson::StringBuffer>                          stream;
	std::unique_ptr<rapidjson::PrettyWriter<rapidjson::StringBuffer>> writer;
	bool saved;
};

} // namespace Typhoon::Reflection

#endif
