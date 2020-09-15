#pragma once

#include "config.h"
#include "semantics.h"
#include <core/span.h>
#include <core/stdAllocator.h>
#include <cstdint>
#include <functional>

namespace Typhoon::Reflection {

class Type;
class Attribute;

using Getter = std::function<const void*(const void* self, void* temporary)>;
using Setter = std::function<void(void* self, const void* value)>; // TODO void* value to allow move?

class Property {
public:
	Property(Setter&& setter, Getter&& getter, const char* name, const Type* valueType, uint32_t flags, Semantic semantic, Allocator& allocator);

	const char* getName() const;
	const Type& getValueType() const;
	uint32_t    getFlags() const;
	Semantic    getSemantic() const;

	void                         setValue(void* self, const void* value) const;
	void                         getValue(const void* self, void* value) const;
	void                         copyValue(void* dstSelf, const void* srcSelf) const;
	void                         addAttribute(const Attribute* attribute);
	span<const Attribute* const> getAttributes() const;

private:
	using AttributeVec = std::vector<const Attribute*, stdAllocator<const Attribute*>>;

	Setter       setter;
	Getter       getter;
	const char*  name;
	const Type*  valueType;
	uint32_t     flags;
	Semantic     semantic;
	AttributeVec attributes;
};

} // namespace Typhoon::Reflection
