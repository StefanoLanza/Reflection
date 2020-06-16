#include "structType.h"
#include "field.h"
#include "property.h"

namespace Typhoon {

StructType::StructType(TypeId typeID, size_t size, size_t alignment, const StructType* parentType, const MethodTable& methods)
    : Type { typeID, Subclass::Struct, size, alignment, methods }
    , parentType(parentType) {
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

void StructType::addField(const Field& field) {
	fields.push_back(field);
}

void StructType::addProperty(std::unique_ptr<Property> property) {
	properties.push_back(std::move(property));
}

span<const Field> StructType::getFields() const {
	return { fields.data(), fields.size() };
}

span<const StructType::PropertyPtr> StructType::getProperties() const {
	return { properties.data(), properties.size() };
}

} // namespace Typhoon
