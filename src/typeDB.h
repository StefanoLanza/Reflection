#pragma once

#include "config.h"

#include "type.h"
#include <core/stdAllocator.h>
#include <core/uncopyable.h>

#include <vector>

namespace Typhoon::Reflection {

class TypeDB : Uncopyable {
public:
	TypeDB(Allocator& allocator);
	~TypeDB();

	void registerType(const Type* type);

	const Type& getType(TypeId typeID) const;
	const Type* tryGetType(TypeId typeID) const;
	const Type* tryGetType(const char* typeName) const;

	template <class T>
	const Type& getType() const {
		return *tryGetType<T>();
	}

	template <class T>
	const Type* tryGetType() const {
		return tryGetType(getTypeId<T>());
	}

private:
	std::vector<const Type*, stdAllocator<const Type*>> types;
};

} // namespace Typhoon::Reflection
