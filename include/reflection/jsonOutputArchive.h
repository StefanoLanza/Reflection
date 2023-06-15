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
	void writeAttributeKey(const char* key) const;

private:
	std::unique_ptr<rapidjson::StringBuffer>                          stream;
	std::unique_ptr<rapidjson::PrettyWriter<rapidjson::StringBuffer>> writer;
	bool                                                              endRoot;
};

} // namespace Typhoon::Reflection

#endif
