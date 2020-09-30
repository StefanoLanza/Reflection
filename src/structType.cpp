#include "structType.h"
#include "attribute.h"
#include "property.h"
#include <core/allocator.h>

namespace Typhoon::Reflection {

StructType::StructType(const char* typeName, TypeId typeID, size_t size, size_t alignment, const StructType* parentType, const MethodTable& methods, Allocator& allocator)
    : Type { typeName, typeID, Subclass::Struct, size, alignment, methods }
    , parentType(parentType)
    , properties(stdAllocator<Property>(allocator))
    , attributes(stdAllocator<const Attribute*>(allocator)) {
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

void StructType::addProperty(Property&& property) {
	properties.push_back(std::move(property));
}

void StructType::addAttribute(const Attribute* attribute) {
	attributes.push_back(attribute);
}

span<const Property> StructType::getProperties() const {
	return { properties.data(), properties.size() };
}

span<const Attribute* const> StructType::getAttributes() const {
	return { attributes.data(), attributes.size() };
}

} // namespace Typhoon::Reflection
