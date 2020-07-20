// Example showing reflection and serialization of an object with custom read and write procedures
// In this case the object is a texture, which is uniquely identified by a file name and contains data loaded from an external file.
// Only the filename is serialized, the data is loaded on demand

#include "customAllocator.h"
#include <include/reflection.h>
#include <include/version.h>
#include <iostream>
#include <string>

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
void           readTexture(void* data, refl::InputArchive& archive);
void           registerUserTypes(refl::TypeDB& typeDB);
std::string    writeTextureToXML(const Texture& obj, const char* XMLelement);
void           readTextureFromXML(Texture& obj, const std::string& xmlString, const char* XMLelement);

int __cdecl main(int /*argc*/, char* /*argv*/[]) {
	std::cout << "Reflection version: " << refl::getVersionString() << std::endl;

	CustomAllocator customAllocator;
	refl::TypeDB&   typeDB = initReflection(customAllocator);
	registerUserTypes(typeDB);

	const char* xmlElement = "Texture";
	Texture     texture { "glossy.mat" };
	std::string xmlContent = writeTextureToXML(texture, xmlElement);
	Texture     otherObj;
	readTextureFromXML(otherObj, xmlContent, xmlElement);

	std::cout << "Total allocated memory " << customAllocator.getTotalAlloc() << " bytes" << std::endl;
	return 0;
}

void registerUserTypes(refl::TypeDB& typeDB) {
	BEGIN_REFLECTION(typeDB)

	BEGIN_STRUCT(Texture);
	READER(readTexture);
	END_STRUCT();
	// TODO macros
	typeDB.setCustomReader<Texture>(readTexture);
	typeDB.setCustomWriter<Texture>(writeTexture);

	END_REFLECTION();
}

std::string writeTextureToXML(const Texture& obj, const char* XMLelement) {
	std::string            xmlContent;
	refl::XMLOutputArchive archive;
	writeObject(obj, XMLelement, archive);
	archive.saveToString(xmlContent);
	return xmlContent;
}

void readTextureFromXML(Texture& obj, const std::string& xmlContent, const char* XMLelement) {
	refl::XMLInputArchive archive;
	if (archive.initialize(xmlContent.data())) {
		readObject(&obj, XMLelement, archive);
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
	writeObject(texture->fileName, "fileName", archive);
	return true;
}

void readTexture(void* data, refl::InputArchive& archive) {
	Texture* texture = static_cast<Texture*>(data);
	if (readObject(&texture->fileName, "fileName", archive)) {
		texture->data = loadTextureFromFile(texture->fileName);
	}
}
