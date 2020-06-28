#pragma once

#include "config.h"

#if TY_REFLECTION_YAML

#include "archive.h"
#include <memory>

namespace Typhoon {

class YAMLOutputArchive final : public OutputArchive {
public:
	YAMLOutputArchive();
	~YAMLOutputArchive();

	bool saveToFile(const char* filename) override;
	bool saveToString(std::string& string) override;
	bool beginElement(const char* name) override;
	void endElement() override;
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
};

} // namespace Typhoon

#endif
