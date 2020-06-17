#pragma once

#include <core/bitMask.h>
#include <include/fwdDecl.h>
#include <string>

struct ActionFlags : Typhoon::BitMask<uint16_t> {
	enum : StorageType {
		running = 1,
		shooting = 2,
		smiling = 4,
	};
};

enum class SeasonType {
	spring,
	summer,
	autumn,
	winter,
};

struct Coords {
	float x;
	float y;
	float z;
};

bool operator==(const Coords& a, const Coords& b);

struct Color {
	float r, g, b;
};

bool operator==(const Color& a, const Color& b);

// Resource with custom loading and saving procedures
struct Material {
	std::string name;
	Color       color { 1.f, 1.f, 1.f };
};

bool customSaveMaterial(const void* data, Typhoon::OutputArchive& archive);
void customReadMaterial(void* data, Typhoon::InputArchive& archive);
bool operator==(const Material& a, const Material& b);

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
	void setNameMove(std::string&& name_) {
		name = std::move(name_);
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
	void setMaterial(const Material& res) {
		material = res;
	}
	const Material& getMaterial() const {
		return material;
	}

private:
	int         lives = 0;
	std::string name;
	ActionFlags actionFlags = {};
	Coords      position { 0.f, 0.f, 0.f };
	Material    material;
};

class DerivedGameObject : public GameObject {
public:
	void setEnergy(float value) {
		energy = value;
	}
	float getEnergy() const {
		return energy;
	}

private:
	float energy = 0.f;
};

// C style structure and API
struct Fog {
	Color color = { 0.5f, 0.5f, 0.5f };
	float density = 0.1f;
};

void  setDensity(Fog& fog, float value);
float getDensity(const Fog& fog);
void  setColor(Fog& fog, Color c);
bool  operator==(const Fog& a, const Fog& b);
