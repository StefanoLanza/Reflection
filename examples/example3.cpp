// Example showing reflection and serialization of a C++ object

#include <iostream>
#include <reflection/reflection.h>
#include <reflection/version.h>
#include <string>

#include <core/bitMask.h>

#define XML  0
#define JSON 1
#if TY_REFLECTION_JSON
#define ARCHIVE_TYPE JSON
#elif TY_REFLECTION_XML
#define ARCHIVE_TYPE XML
#else
#error "No supported archive types"
#endif

enum class ActionFlags : uint16_t {
	running = 1,
	shooting = 2,
	smiling = 4,
};

using ActionBitmask = Typhoon::Bitmask<ActionFlags>;

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
	int getLives() const {
		return lives;
	}
	void setName(const std::string& name_) {
		name = name_;
	}
	const std::string& getName() const {
		return name;
	}
	void setActionFlags(ActionBitmask flags) {
		actionFlags = flags;
	}
	ActionBitmask getActionFlags() const {
		return actionFlags;
	}
	void setPosition(const Coords& p) {
		position = p;
	}
	const Coords& getPosition() const {
		return position;
	}
	void setStamina(float value) {
		stamina = value;
	}
	float getStamina() const {
		return stamina;
	}

private:
	int           lives = 0;
	std::string   name;
	ActionBitmask actionFlags {};
	Coords        position { 0.f, 0.f, 0.f };
	float         stamina = 1.f;
};

void        registerUserTypes();
GameObject  makeGameObject();
std::string writeGameObject(const GameObject& obj, const char* XMLelement);
void        readGameObject(GameObject& obj, const std::string& xmlString, const char* XMLelement);

int main(int /*argc*/, char* /*argv*/[]) {
	std::cout << "Reflection version: " << refl::getVersionString() << std::endl;

	refl::initReflection();
	registerUserTypes();

	const char* element = "gameObject";
	GameObject  obj = makeGameObject();
	std::string archiveContent = writeGameObject(obj, element);
	GameObject  otherObj;
	readGameObject(otherObj, archiveContent, element);

	std::cout << "GameObject.stamina attributes:" << std::endl;
	const auto& type = static_cast<const refl::StructType&>(refl::getType<GameObject>());
	for (auto a : type.getProperty("stamina")->getAttributes()) {
		if (auto floatMin = a->tryCast<refl::FloatMin>(); floatMin) {
			std::cout << "FloatMin. minValue:" << floatMin->getMin() << std::endl;
		}
		else if (auto floatMax = a->tryCast<refl::FloatMax>(); floatMax) {
			std::cout << "FloatMax. maxValue:" << floatMax->getMax() << std::endl;
		}
	}

	refl::deinitReflection();
	return 0;
}

void registerUserTypes() {
	BEGIN_REFLECTION()

	BEGIN_BITMASK(ActionBitmask)
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
	PROPERTY("lives", getLives, setLives);
	PROPERTY("name", getName, setName);
	PROPERTY("position", getPosition, setPosition);
	PROPERTY("action", getActionFlags, setActionFlags);
	PROPERTY("stamina", getStamina, setStamina).ATTRIBUTE(FloatMin, 0.f).ATTRIBUTE(FloatMax, 10.f);
	END_CLASS();

	END_REFLECTION();
}

GameObject makeGameObject() {
	GameObject obj;
	obj.setLives(1000);
	obj.setName("Stefano");
	obj.setActionFlags(ActionBitmask { ActionFlags::running, ActionFlags::smiling });
	obj.setPosition({ 0.f, 1.f, 2.f });
	return obj;
}

std::string writeGameObject(const GameObject& obj, const char* element) {
#if ARCHIVE_TYPE == XML
	refl::XMLOutputArchive archive;
#elif ARCHIVE_TYPE == JSON
	refl::JSONOutputArchive archive;
#endif
	archive.write(element, obj);
	return archive.saveToString();
}

void readGameObject(GameObject& obj, const std::string& archiveContent, const char* element) {
#if ARCHIVE_TYPE == XML
	refl::XMLInputArchive archive;
#elif ARCHIVE_TYPE == JSON
	refl::JSONInputArchive archive;
#endif
	if (archive.initialize(archiveContent.data())) {
		archive.read(element, &obj);
	}
}
