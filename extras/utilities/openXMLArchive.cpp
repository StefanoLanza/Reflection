#include "openXMLArchive.h"
#include <cassert>
#include <file/fileServer.h>
#include <file/fileUtil.h>
#include <file/stream.h>
#include <reflection/XMLInputArchive.h>
#include <reflection/jsonInputArchive.h>

namespace Typhoon {

#if TY_REFLECTION_XML

bool openXMLArchive(Reflection::XMLInputArchive& archive, const char* fileName, IO::FileServer& fileServer) {
	assert(fileName);
	// Open stream
	auto stream { fileServer.OpenFile(fileName) };
	if (! stream) {
		return false;
	}

	const size_t streamSize = stream->GetSize();
	if (! streamSize) {
		return false;
	}

	std::vector<char> buffer(streamSize + 2);
	if (! stream->Read(buffer.data(), streamSize)) {
		return false;
	}

	// Add a newline and null terminator (bug in TinyXML)
	buffer[streamSize] = '\n';
	buffer[streamSize + 1] = '\0';

	// Initialize XML archive
	if (! archive.initialize(buffer.data())) {
		return false;
	}

	return true;
}

#endif

#if TY_REFLECTION_JSON

bool openJSONArchive(Reflection::JSONInputArchive& archive, const char* fileName, IO::FileServer& fileServer) {
	assert(fileName);
	// Open stream
	auto stream = fileServer.OpenFile(fileName);
	if (! stream) {
		return false;
	}

	const size_t streamSize = stream->GetSize();
	if (! streamSize) {
		return false;
	}

	std::vector<char> buffer(streamSize + 2);
	if (! stream->Read(buffer.data(), streamSize)) {
		return false;
	}

	// Initialize XML archive
	if (! archive.initialize(buffer.data())) {
		return false;
	}

	return true;
}

#endif

} // namespace Typhoon
