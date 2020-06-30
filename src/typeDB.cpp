#include "typeDB.h"
#include "structType.h"

#include <cassert>

namespace Typhoon::Reflection {

TypeDB* gTypeDB = nullptr;

TypeDB& getTypeDB() {
	return *gTypeDB;
}

TypeDB::TypeDB() {
	gTypeDB = this;
}

TypeDB::~TypeDB() = default;

void TypeDB::registerType(const Type* newType, CustomReader reader, CustomWriter writer) {
	types.push_back(newType);
	if (reader) {
		customReaders.emplace(newType, reader);
	}
	if (writer) {
		customWriters.emplace(newType, writer);
	}
}

const Type& TypeDB::getType(TypeId typeID) const {
	auto type = tryGetType(typeID);
	assert(type);
	return *type;
}

const Type* TypeDB::tryGetType(TypeId typeID) const {
	// TODO O(n), optimize
	for (const auto& type : types) {
		if (type->typeID == typeID) {
			return type;
		}
	}
	return nullptr;
}

void TypeDB::setCustomWriter(const Type* type, CustomWriter writer) {
	customWriters.emplace(type, std::move(writer));
}

CustomWriter TypeDB::getCustomWriter(const Type* type) const {
	auto it = customWriters.find(type);
	return it == customWriters.end() ? nullptr : it->second;
}

void TypeDB::setCustomReader(const Type* type, CustomReader reader) {
	customReaders.emplace(type, std::move(reader));
}

CustomReader TypeDB::getCustomReader(const Type* type) const {
	auto it = customReaders.find(type);
	return it == customReaders.end() ? nullptr : it->second;
}

void TypeDB::setCustomCloner(const Type* type, CustomCloner cloner) {
	customCloners.emplace(type, std::move(cloner));
}

CustomCloner TypeDB::getCustomCloner(const Type* type) const {
	auto it = customCloners.find(type);
	return it == customCloners.end() ? nullptr : it->second;
}

} // namespace Typhoon
