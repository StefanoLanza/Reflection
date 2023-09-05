#pragma once

#include "type.h"
#include <core/span.h>
#include <core/stdAllocator.h>

#include <vector>

namespace Typhoon::Reflection {

class Type;
class Property;
class Attribute;

class StructType : public Type {
public:
	StructType(const char* typeName, TypeId typeID, size_t size, size_t alignment, const StructType* parentType, const MethodTable& methods,
	           Allocator& allocator);
	~StructType();

	const StructType*            getParentType() const;
	bool                         inheritsFrom(const StructType* type) const;
	Property&                    addProperty(Property&& property);
	void                         addAttribute(const Attribute* attribute);
	span<const Property>         getProperties() const;
	span<const Attribute* const> getAttributes() const;

private:
	using Vector = std::vector<Property, stdAllocator<Property>>;
	using AttributeVec = std::vector<const Attribute*, stdAllocator<const Attribute*>>;

	const StructType* parentType;
	Vector            properties;
	AttributeVec      attributes;
};

} // namespace Typhoon::Reflection
