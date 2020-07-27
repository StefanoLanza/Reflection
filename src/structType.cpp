#include "structType.h"
#include "allocator.h"
#include "property.h"

namespace Typhoon::Reflection {

StructType::StructType(TypeId typeID, size_t size, size_t alignment, const StructType* parentType, const MethodTable& methods, Allocator& allocator)
    : Type { typeID, Subclass::Struct, size, alignment, methods }
    , parentType(parentType)
    , properties(stdAllocator<Property>(allocator)) {
}

StructType::~StructType() = default;

const StructType* StructType::getParentType() const {
	return parentType;
}

bool StructType::inheritsFrom(const StructType* type) const {
	const StructType* parent = this;
	bool              res = false;
	while (parent) {
		if (parent == type) {
			res = true;
			break;
		}
		parent = parent->parentType;
	}

	return res;
}

void StructType::addProperty(Property&& property) {
	properties.push_back(std::move(property));
}

span<const Property> StructType::getProperties() const {
	return { properties.data(), properties.size() };
}

} // namespace Typhoon::Reflection
