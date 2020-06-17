// Example showing reflection and serialization of a struct. Some fields are serialized directly. Some fields are set via a C-style API,
// which does parameter validation

#include <include/reflection.h>
#include <string>

using namespace Typhoon;

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

void        registerUserTypes(TypeDB& typeDB);
GameObject  makeGameObject();
std::string writeGameObjectToXML(const GameObject& obj, const char* XMLelement);
void        readGameObjectFromXML(GameObject& obj, const std::string& xmlString, const char* XMLelement);

int __cdecl main(int /*argc*/, char* /*argv*/[]) {
	TypeDB typeDB;
	initReflection(typeDB);
	registerUserTypes(typeDB);

	const char* xmlElement = "gameObject";
	GameObject  obj = makeGameObject();
	std::string xmlContent = writeGameObjectToXML(obj, xmlElement);
	GameObject  otherObj;
	readGameObjectFromXML(otherObj, xmlContent, xmlElement);
	return 0;
}

void registerUserTypes(TypeDB& typeDB) {
	BEGIN_REFLECTION(typeDB)

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

std::string writeGameObjectToXML(const GameObject& obj, const char* XMLelement) {
	std::string      xmlContent;
	XMLOutputArchive archive;
	writeObject(obj, XMLelement, archive);
	archive.saveToString(xmlContent);
	return xmlContent;
}

void readGameObjectFromXML(GameObject& obj, const std::string& xmlContent, const char* XMLelement) {
	XMLInputArchive archive;
	if (archive.initialize(xmlContent.data())) {
		readObject(&obj, XMLelement, archive);
	}
}
