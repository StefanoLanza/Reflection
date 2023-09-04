#include "testClasses.h"
#include <reflection/archive.h>
#include <reflection/readObject.h>
#include <reflection/writeObject.h>

#include <cstring>

bool operator==(const Coords& a, const Coords& b) {
	return a.x == b.x && a.y == b.y && a.z == b.z;
}

bool operator==(const Color& a, const Color& b) {
	return a.r == b.r && a.g == b.g && a.b == b.b;
}

bool operator==(const Material& a, const Material& b) {
	return a.name == b.name && a.color == b.color;
}

bool operator==(const Fog& a, const Fog& b) {
	static_assert(std::is_trivially_copy_assignable_v<Fog>);
	return ! std::memcmp(&a, &b, sizeof a);
}

void setDensity(Fog& fog, float value) {
	fog.density = value;
}

float getDensity(const Fog& fog) {
	return fog.density;
}

void setColor(Fog& fog, Color c) {
	fog.color = c;
}

Color getColor(const Fog& fog) {
	return fog.color;
}

void setElevationProfile(Fog& fog, Fog::ElvProfile profile) {
	fog.elevationProfile = profile;
}

const Fog::ElvProfile& getElevationProfile(const Fog& fog) {
	return fog.elevationProfile;
}

void customSaveMaterial(const void* data, refl::OutputArchive& archive) {
	const Material* material = static_cast<const Material*>(data);
	if (archive.beginObject()) {
		archive.write("name", material->name);
		archive.write("color", material->color);
		archive.endObject();
	}
}

void customReadMaterial(void* data, const refl::InputArchive& archive) {
	Material* material = static_cast<Material*>(data);
	if (archive.isObject()) {
		archive.read("name", material->name);
		archive.read("color", material->color);
	}
}
