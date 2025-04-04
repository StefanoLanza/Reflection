#pragma once

#include <core/bitMask.h>
#include <reflection/fwdDecl.h>

#include <array>
#include <string>

enum class ActionFlags : uint16_t {
	running = 0x1,
	shooting = 0x2,
	smiling = 0x4,
};

using ActionBitmask = Typhoon::Bitmask<ActionFlags>;

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

void customSaveMaterial(const void* data, refl::OutputArchive& archive);
void customReadMaterial(void* data, const refl::InputArchive& archive);
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
	void setMaterial(const Material& res) {
		material = res;
	}
	const Material& getMaterial() const {
		return material;
	}
	int getReadOnly() const {
		return readOnly;
	}
	void setWriteOnly(float value) {
		writeOnly = value;
	}

private:
	int           lives = 0;
	std::string   name;
	ActionBitmask actionFlags {};
	Coords        position { 0.f, 0.f, 0.f };
	Material      material;
	int           readOnly = 0xFF;
	float         writeOnly = -1.f;
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
	using ElvProfile = std::array<float, 8>;
	Color      color = { 0.5f, 0.5f, 0.5f };
	float      density = 0.1f;
	ElvProfile elevationProfile {};
	char       tag[4] {};
};

void                   setDensity(Fog& fog, float value);
float                  getDensity(const Fog& fog);
void                   setColor(Fog& fog, Color c);
Color                  getColor(const Fog& fog);
void                   setElevationProfile(Fog& fog, Fog::ElvProfile profile);
const Fog::ElvProfile& getElevationProfile(const Fog& fog);

bool operator==(const Fog& a, const Fog& b);
