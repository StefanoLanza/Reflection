#pragma once

#include "type.h"
#include <core/span.h>
#include <memory>
#include <vector>

namespace Typhoon {

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
	const StructType*        parentType;
	std::vector<PropertyPtr> properties;
	std::vector<Field>       fields;
};

} // namespace Typhoon
