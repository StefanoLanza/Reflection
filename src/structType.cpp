#include "structType.h"
#include "attribute.h"
#include "property.h"
#include <core/allocator.h>

namespace Typhoon::Reflection {

StructType::StructType(const char* typeName, TypeId typeID, size_t size, size_t alignment, const StructType* parentType, const MethodTable& methods,
                       Allocator& allocator)
    : Type { typeName, typeID, Subclass::Struct, size, alignment, methods, allocator }
    , parentType(parentType)
    , properties(stdAllocator<Property>(allocator)) {
}

StructType::~StructType() = default;

const StructType* StructType::getParentType() const {
	return parentType;
}

bool StructType::inheritsFrom(const StructType* type) const {
	const StructType* parent = parentType;
	while (parent) {
		if (parent == type) {
			return true;
		}
		parent = parent->parentType;
	}
	return false;
}

Property& StructType::addProperty(Property&& property) {
	properties.push_back(std::move(property));
	return properties.back();
}

std::span<const Property> StructType::getProperties() const {
	return properties;
}

const Property* StructType::getProperty(const char* propertyName) const {
	assert(propertyName);
	for (const auto& p : properties) {
		if (! strcmp(p.getName(), propertyName)) {
			return &p;
		}
	}
	return nullptr;
}

} // namespace Typhoon::Reflection
