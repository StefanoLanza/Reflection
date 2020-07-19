#pragma once

#include "type.h"
#include <core/span.h>
#include <memory>
#include <src/stdAllocator.h>
#include <vector>

namespace Typhoon::Reflection {

class Type;
struct Field;
class Property;

class StructType : public Type {
public:
	using PropertyPtr = std::unique_ptr<Property>;

	StructType(TypeId typeID, size_t size, size_t alignment, const StructType* parentType = nullptr, const MethodTable& methods = {});
	~StructType();

	const StructType*       getParentType() const;
	bool                    inheritsFrom(const StructType* type) const;
	void                    addField(const Field& field);
	void                    addProperty(std::unique_ptr<Property> property);
	span<const Field>       getFields() const;
	span<const PropertyPtr> getProperties() const;

private:
	using Vector = std::vector<PropertyPtr, stdAllocator<PropertyPtr>>;
	using FieldVector = std::vector<Field, stdAllocator<Field>>;

	const StructType* parentType;
	Vector            properties;
	FieldVector       fields;
};

} // namespace Typhoon::Reflection
