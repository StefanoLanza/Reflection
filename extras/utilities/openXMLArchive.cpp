#include "openXMLArchive.h"
#include <cassert>
#include <file/fileServer.h>
#include <file/fileUtil.h>
#include <file/stream.h>
#include <include/XMLInputArchive.h>

namespace Typhoon {

bool openXMLArchive(XMLInputArchive& archive, const char* fileName, IO::FileServer& fileServer) {
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

} // namespace Typhoon
