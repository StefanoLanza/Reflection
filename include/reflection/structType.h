#pragma once

#include "property.h"
#include "type.h"
#include <core/arenaVector.h>

#include <span>
#include <vector>

namespace Typhoon::Reflection {

class Property;

class StructType final : public Type {
public:
	StructType(const char* typeName, TypeId typeID, size_t size, size_t alignment, const StructType* parentType, const MethodTable& methods,
	           ArenaAllocator& allocator);
	~StructType();

	const StructType*         getParentType() const;
	bool                      inheritsFrom(const StructType* type) const;
	Property&                 addProperty(Property&& property);
	std::span<const Property> getProperties() const;
	const Property*           getPropertyByName(const char* propertyName) const;

private:
	const StructType*     parentType;
	ArenaVector<Property> properties;
};

} // namespace Typhoon::Reflection
