#include "testClasses.h"
#include <reflection/archive.h>
#include <reflection/readObject.h>
#include <reflection/writeObject.h>

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
	return ! memcmp(&a, &b, sizeof a);
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

bool customSaveMaterial(const void* data, refl::OutputArchive& archive) {
	const Material* material = static_cast<const Material*>(data);
	archive.beginObject();
	archive.write("name", material->name);
	archive.write("color", material->color);
	archive.endObject();
	return true;
}

void customReadMaterial(void* data, refl::InputArchive& archive) {
	Material* material = static_cast<Material*>(data);
	archive.beginObject();
	archive.read("name", material->name);
	archive.read("color", material->color);
	archive.endObject();
}
