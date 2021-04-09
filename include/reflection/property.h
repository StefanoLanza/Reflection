#pragma once

#include "config.h"
#include "dataPtr.h"
#include "semantics.h"
#include <core/span.h>
#include <core/stdAllocator.h>
#include <cstdint>
#include <functional>
#include <vector>

namespace Typhoon::Reflection {

class Type;
class Attribute;

using Getter = std::function<ConstDataPtr(ConstDataPtr self, DataPtr temporary)>;
using Setter = std::function<void(DataPtr self, ConstDataPtr value)>; // TODO DataPtr value to allow move?

class Property {
public:
	Property(Setter&& setter, Getter&& getter, const char* name, const Type* valueType, uint32_t flags, Semantic semantic, Allocator& allocator);

	const char* getName() const;
	const Type& getValueType() const;
	uint32_t    getFlags() const;
	Semantic    getSemantic() const;

	void                         setValue(DataPtr self, ConstDataPtr value) const;
	void                         getValue(ConstDataPtr self, DataPtr value) const;
	void                         copyValue(DataPtr dstSelf, ConstDataPtr srcSelf, LinearAllocator& alloc) const;
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
