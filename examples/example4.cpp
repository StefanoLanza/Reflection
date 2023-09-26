// Example showing reflection and serialization of an object with custom read and write procedures
// In this case the object is a texture, which is uniquely identified by a file name and contains data loaded from an external file.
// Only the filename is serialized, the data is loaded on demand

#include "customAllocator.h"
#include <reflection/reflection.h>
#include <reflection/version.h>
#include <iostream>
#include <string>

#define XML          0
#define JSON         1
#if TY_REFLECTION_JSON
#define ARCHIVE_TYPE JSON
#elif TY_REFLECTION_XML
#define ARCHIVE_TYPE XML
#else
#error "No supported archive types"
#endif

struct TextureData {
	void*  data = nullptr;
	size_t size = 0;
	// dimension, format etc.
};

using TextureDataPtr = std::shared_ptr<TextureData>;

struct Texture {
	std::string    fileName;
	TextureDataPtr data;
};

TextureDataPtr loadTextureFromFile(const std::string& fileName);
bool           writeTexture(const void* data, refl::OutputArchive& archive);
void           readTexture(void* data, const refl::InputArchive& archive);
void           registerUserTypes();
std::string    writeTextureToArchive(const Texture& obj, const char* name);
void           readTextureFromArchive(Texture& obj, const std::string& xmlString, const char* name);

int main(int /*argc*/, char* /*argv*/[]) {
	std::cout << "Reflection version: " << refl::getVersionString() << std::endl;

	CustomAllocator customAllocator;
	refl::initReflection(customAllocator);
	registerUserTypes();

	const char* name = "Texture";
	Texture     texture { "glossy.mat", {} };
	std::string archiveContent = writeTextureToArchive(texture, name);
	Texture     otherObj;
	readTextureFromArchive(otherObj, archiveContent, name);

	std::cout << "Total allocated memory " << customAllocator.getTotalAlloc() << " bytes" << std::endl;
	refl::deinitReflection();
	return 0;
}

void registerUserTypes() {
	BEGIN_REFLECTION()

	BEGIN_STRUCT(Texture);
	READER(readTexture);
	WRITER(writeTexture);
	END_STRUCT();

	END_REFLECTION();
}

std::string writeTextureToArchive(const Texture& obj, const char* name) {
#if ARCHIVE_TYPE == XML
	refl::XMLOutputArchive archive;
#elif ARCHIVE_TYPE == JSON
	refl::JSONOutputArchive archive;
#endif
	archive.write(name, obj);
	return archive.saveToString();
}

void readTextureFromArchive(Texture& obj, const std::string& archiveContent, const char* name) {
#if ARCHIVE_TYPE == XML
	refl::XMLInputArchive archive;
#elif ARCHIVE_TYPE == JSON
	refl::JSONInputArchive  archive;
#endif
	if (archive.initialize(archiveContent.data())) {
		archive.read(name, obj);
	}
}

TextureDataPtr loadTextureFromFile(const std::string& fileName) {
	(void)fileName;
	// TODO look into map of loaded textures. load from file if not loaded yet
	return std::make_shared<TextureData>();
}

bool writeTexture(const void* data, refl::OutputArchive& archive) {
	const Texture* texture = static_cast<const Texture*>(data);
	// Only the texture fileName is serialized. Its data is stored externally
	if (archive.beginObject()) {
		archive.write("fileName", texture->fileName);
		archive.endObject();
		return true;
	}
	return false;
}

void readTexture(void* data, const refl::InputArchive& archive) {
	Texture* texture = static_cast<Texture*>(data);
	if (archive.read("fileName", texture->fileName)) {
		texture->data = loadTextureFromFile(texture->fileName);
	}
}
