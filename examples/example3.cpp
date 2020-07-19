// Example showing reflection and serialization of a C++ object

#include <include/reflection.h>
#include <include/version.h>
#include <iostream>
#include <string>

struct ActionFlags : Typhoon::BitMask<uint16_t> {
	enum : StorageType {
		running = 1,
		shooting = 2,
		smiling = 4,
	};
};

struct Coords {
	float x;
	float y;
	float z;
};

class GameObject {
public:
	void setLives(int value) {
		lives = value;
	}
	int getLife() const {
		return lives;
	}
	void setName(const std::string& name_) {
		name = name_;
	}
	const std::string& getName() const {
		return name;
	}
	void setActionFlags(ActionFlags flags) {
		actionFlags = flags;
	}
	ActionFlags getActionFlags() const {
		return actionFlags;
	}
	void setPosition(const Coords& p) {
		position = p;
	}
	const Coords& getPosition() const {
		return position;
	}

private:
	int         lives = 0;
	std::string name;
	ActionFlags actionFlags = {};
	Coords      position { 0.f, 0.f, 0.f };
};

void        registerUserTypes(refl::TypeDB& typeDB);
GameObject  makeGameObject();
std::string writeGameObject(const GameObject& obj, const char* XMLelement);
void        readGameObject(GameObject& obj, const std::string& xmlString, const char* XMLelement);

int __cdecl main(int /*argc*/, char* /*argv*/[]) {
	std::cout << "Reflection version: " << refl::getVersionString() << std::endl;

	refl::TypeDB& typeDB = refl::initReflection();
	registerUserTypes(typeDB);

	const char* xmlElement = "gameObject";
	GameObject  obj = makeGameObject();
	std::string xmlContent = writeGameObject(obj, xmlElement);
	GameObject  otherObj;
	readGameObject(otherObj, xmlContent, xmlElement);
	return 0;
}

void registerUserTypes(refl::TypeDB& typeDB) {
	BEGIN_REFLECTION(typeDB)

	BEGIN_BITMASK(ActionFlags)
	BITMASK_VALUE(running)
	BITMASK_VALUE(shooting)
	BITMASK_VALUE(smiling)
	END_BITMASK();

	BEGIN_STRUCT(Coords);
	FIELD(x);
	FIELD(y);
	FIELD(z);
	END_STRUCT();

	BEGIN_CLASS(GameObject);
	PROPERTY("life", getLife, setLives);
	PROPERTY("name", getName, setName);
	PROPERTY("position", getPosition, setPosition);
	PROPERTY_EX("action", getActionFlags, setActionFlags, Flags::all, Semantic::none);
	END_CLASS();

	END_REFLECTION();
}

GameObject makeGameObject() {
	ActionFlags actionFlags;
	actionFlags.value = ActionFlags::running | ActionFlags::smiling;

	GameObject obj;
	obj.setLives(1000);
	obj.setName("Stefano");
	obj.setActionFlags(actionFlags);
	obj.setPosition({ 0.f, 1.f, 2.f });
	return obj;
}

std::string writeGameObject(const GameObject& obj, const char* XMLelement) {
	std::string            xmlContent;
	refl::XMLOutputArchive archive;
	writeObject(obj, XMLelement, archive);
	archive.saveToString(xmlContent);
	return xmlContent;
}

void readGameObject(GameObject& obj, const std::string& xmlContent, const char* XMLelement) {
	refl::XMLInputArchive archive;
	if (archive.initialize(xmlContent.data())) {
		readObject(&obj, XMLelement, archive);
	}
}
