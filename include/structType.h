#pragma once

#include "../src/stdAllocator.h"
#include "type.h"
#include <core/span.h>
#include <memory>
#include <vector>

namespace Typhoon::Reflection {

class Type;
struct Field;
class Property;
class Allocator;

class StructType : public Type {
public:
	StructType(TypeId typeID, size_t size, size_t alignment, const StructType* parentType, const MethodTable& methods, Allocator& allocator);
	~StructType();

	const StructType*    getParentType() const;
	bool                 inheritsFrom(const StructType* type) const;
	void                 addField(const Field& field);
	void                 addProperty(Property&& property);
	span<const Field>    getFields() const;
	span<const Property> getProperties() const;

private:
	using Vector = std::vector<Property, stdAllocator<Property>>;
	using FieldVector = std::vector<Field, stdAllocator<Field>>;

	const StructType* parentType;
	Vector            properties;
	FieldVector       fields;
};

} // namespace Typhoon::Reflection
