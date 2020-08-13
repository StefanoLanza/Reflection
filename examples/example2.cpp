// Example showing reflection and serialization of a struct. Some fields are serialized directly. Some fields are set via a C-style API,
// which does parameter validation

#include <include/reflection.h>
#include <include/version.h>
#include <iostream>
#include <string>

#define XML          0
#define JSON         1
#define ARCHIVE_TYPE JSON

struct Coords {
	float x;
	float y;
	float z;
};

constexpr int maxLives = 10;

// C struct. Its fields are public and can be serialized either directly or with a C-style API
struct GameObject {
	std::string name;
	Coords      position { 0.f, 0.f, 0.f };
	Coords      velocity { 0.f, 0.f, 0.f };
	int         lives = 0;
};

void setLives(GameObject& obj, int value) {
	obj.lives = std::clamp(value, 0, maxLives); // validation
}

// Read only property
int getRemainingLives(const GameObject& obj) {
	return maxLives - obj.lives;
}

void        registerUserTypes();
GameObject  makeGameObject();
std::string writeGameObject(const GameObject& obj, const char* XMLelement);
void        readGameObject(GameObject& obj, const std::string& xmlString, const char* XMLelement);

int __cdecl main(int /*argc*/, char* /*argv*/[]) {
	std::cout << "Reflection version: " << refl::getVersionString() << std::endl;

	refl::initReflection();
	registerUserTypes();

	const char* element = "gameObject";
	GameObject  obj = makeGameObject();
	std::string archiveContent = writeGameObject(obj, element);
	GameObject  otherObj;
	readGameObject(otherObj, archiveContent, element);

	refl::deinitReflection();
	return 0;
}

void registerUserTypes() {
	BEGIN_REFLECTION()

	BEGIN_STRUCT(Coords);
	FIELD(x);
	FIELD(y);
	FIELD(z);
	END_STRUCT();

	BEGIN_STRUCT(GameObject);
	FIELD(name);
	C_SETTER(lives, setLives);
	C_GETTER("remainingLives", getRemainingLives);
	FIELD(position);
	FIELD(velocity);
	END_STRUCT();

	END_REFLECTION();
}

GameObject makeGameObject() {
	GameObject obj;
	setLives(obj, 1000);
	obj.name = "Stefano";
	obj.position = { 0.f, 1.f, 2.f };
	obj.velocity = { 3.f, 4.f, 5.f };
	return obj;
}

std::string writeGameObject(const GameObject& obj, const char* element) {
	std::string            content;
#if ARCHIVE_TYPE == XML
	refl::XMLOutputArchive archive;
#elif ARCHIVE_TYPE == JSON
	refl::JSONOutputArchive archive;
#endif
	writeObject(obj, element, archive);
	archive.saveToString(content);
	return content;
}

void readGameObject(GameObject& obj, const std::string& archiveContent, const char* element) {
#if ARCHIVE_TYPE == XML
	refl::XMLInputArchive archive;
#elif ARCHIVE_TYPE == JSON
	refl::JSONInputArchive archive;
#endif
	if (archive.initialize(archiveContent.data())) {
		readObject(&obj, element, archive);
	}
}
