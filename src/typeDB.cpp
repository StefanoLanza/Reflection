#include "typeDB.h"
#include "structType.h"
#include "namespace.h"
#include <core/scopedAllocator.h>

#include <cassert>

namespace Typhoon::Reflection {

TypeDB::TypeDB(Allocator& allocator, ScopedAllocator& scopedAllocator)
    : types(stdAllocator<const Type*>(allocator)) {
	types.reserve(64);
	globalNamespace = scopedAllocator.make<Namespace>("global", std::ref(allocator));
}

TypeDB::~TypeDB() = default;

void TypeDB::registerType(const Type* newType) {
	assert(newType);
	types.push_back(newType);
}

Namespace& TypeDB::getGlobalNamespace() const
{
	return *globalNamespace;
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

const Type* TypeDB::tryGetType(const char* typeName) const {
	for (const auto& type : types) {
		if (! strcmp(type->getName(), typeName)) {
			return type;
		}
	}
	return nullptr;
}

} // namespace Typhoon::Reflection
