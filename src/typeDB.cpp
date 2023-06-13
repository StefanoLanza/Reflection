#include "typeDB.h"
#include "namespace.h"
#include "structType.h"
#include <core/scopedAllocator.h>

#include <cassert>
#include <cstring>

namespace Typhoon::Reflection {

namespace detail {

const char* decorateTypeName(std::string_view typeName, std::string_view prefix, std::string_view suffix, ScopedAllocator& allocator) {
	size_t tl = typeName.size();
	size_t pl = prefix.size();
	size_t sl = suffix.size();
	char*  str = allocator.allocArray<char>(pl + sl + tl + 1);
	size_t offs = 0;
	memcpy(str + offs, prefix.data(), pl);
	offs += pl;
	memcpy(str + offs, typeName.data(), tl);
	offs += tl;
	memcpy(str + offs, suffix.data(), sl);
	offs += sl;
	str[offs] = 0; // null terminate
	return str;
}

} // namespace detail

TypeDB::TypeDB(Allocator& allocator, ScopedAllocator& scopedAllocator)
    : types { stdAllocator<const Type*>(allocator) }
    , globalNamespace { scopedAllocator.make<Namespace>(nullptr, allocator) } {
	types.reserve(64);
}

void TypeDB::registerType(const Type* newType) {
	assert(newType);
	types.push_back(newType);
}

Namespace& TypeDB::getGlobalNamespace() const {
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
		if (type->getTypeId() == typeID) {
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
