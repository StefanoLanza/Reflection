#pragma once

#include "type.h"
#include <core/span.h>
#include <core/stdAllocator.h>

#include <memory>
#include <vector>

namespace Typhoon::Reflection {

class Type;
class Property;

class StructType : public Type {
public:
	StructType(TypeId typeID, size_t size, size_t alignment, const StructType* parentType, const MethodTable& methods, Allocator& allocator);
	~StructType();

	const StructType*    getParentType() const;
	bool                 inheritsFrom(const StructType* type) const;
	void                 addProperty(Property&& property);
	span<const Property> getProperties() const;

private:
	using Vector = std::vector<Property, stdAllocator<Property>>;

	const StructType* parentType;
	Vector            properties;
};

} // namespace Typhoon::Reflection
