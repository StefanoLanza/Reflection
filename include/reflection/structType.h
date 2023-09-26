#pragma once

#include "type.h"
#include <core/span.h>
#include <core/stdAllocator.h>

#include <vector>

namespace Typhoon::Reflection {

class Property;

class StructType final : public Type {
public:
	StructType(const char* typeName, TypeId typeID, size_t size, size_t alignment, const StructType* parentType, const MethodTable& methods,
	           Allocator& allocator);
	~StructType();

	const StructType*    getParentType() const;
	bool                 inheritsFrom(const StructType* type) const;
	Property&            addProperty(Property&& property);
	span<const Property> getProperties() const;
	const Property*      getProperty(const char* propertyName) const;

private:
	using Vector = std::vector<Property, stdAllocator<Property>>;

	const StructType* parentType;
	Vector            properties;
};

} // namespace Typhoon::Reflection
