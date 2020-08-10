#pragma once

#include "config.h"

#if TY_REFLECTION_JSON

#include "archive.h"
#include <memory>

namespace Typhoon::Reflection {

class JSONInputArchive final : public InputArchive {
public:
	JSONInputArchive();
	~JSONInputArchive();

	bool        initialize(const char* buffer);
	std::string getErrorDesc() const override;
	bool        beginElement() override;
	bool        beginElement(const char* name) override;
	void        endElement() override;
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
};

} // namespace Typhoon::Reflection

#endif
