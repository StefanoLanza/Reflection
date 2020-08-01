#include "typeDB.h"
#include "structType.h"

#include <cassert>

namespace Typhoon::Reflection {

TypeDB::TypeDB(Allocator& allocator)
    : types(stdAllocator<const Type*>(allocator)) {
	types.reserve(64);
}

TypeDB::~TypeDB() = default;

void TypeDB::registerType(const Type* newType) {
	assert(newType);
	types.push_back(newType);
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

} // namespace Typhoon::Reflection
