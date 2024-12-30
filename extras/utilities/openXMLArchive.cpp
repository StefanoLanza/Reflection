#include "openXMLArchive.h"
#include <cassert>
#include <IO/fileServer.h>
#include <IO/fileUtil.h>
#include <IO/stream.h>
#include <reflection/XMLInputArchive.h>
#include <reflection/jsonInputArchive.h>

namespace Typhoon {

#if TY_REFLECTION_XML

refl::ParseResult openXMLArchive(Reflection::XMLInputArchive& archive, std::string_view fileName, IO::FileServer& fileServer) {
	// Open stream
	auto stream { fileServer.openFile(fileName) };
	if (! stream) {
		return refl::ParseResult{false, "cannot open file", 0};
	}

	const size_t streamSize = stream->getSize();
	if (! streamSize) {
		return refl::ParseResult{false, "empty file", 0};
	}

	std::vector<char> buffer(streamSize + 2);
	if (! stream->read(buffer.data(), streamSize)) {
		return refl::ParseResult{false, "cannot read file", 0};
	}

	// Add a newline and null terminator (bug in TinyXML)
	buffer[streamSize] = '\n';
	buffer[streamSize + 1] = '\0';

	// Initialize XML archive
	return archive.initialize(buffer.data());
}

#endif

#if TY_REFLECTION_JSON

refl::ParseResult openJSONArchive(Reflection::JSONInputArchive& archive, std::string_view fileName, IO::FileServer& fileServer) {
	// Open stream
	auto stream = fileServer.openFile(fileName);
	if (! stream) {
		return refl::ParseResult{false, "cannot open file", 0};
	}

	const size_t streamSize = stream->getSize();
	if (! streamSize) {
		return refl::ParseResult{false, "empty file", 0};
	}

	std::vector<char> buffer(streamSize + 2);
	if (! stream->read(buffer.data(), streamSize)) {
		return refl::ParseResult{false, "cannot read file", 0};
	}

	// Initialize XML archive
	return archive.initialize(buffer.data());
}

#endif

} // namespace Typhoon
