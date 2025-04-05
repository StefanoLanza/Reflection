#pragma once

#include "attribute.h"
#include "config.h"
#include "dataPtr.h"
#include "semantics.h"
#include <core/stdAllocator.h>

#include <cstdint>
#include <functional>
#include <span>
#include <vector>

namespace Typhoon::Reflection {

class Type;

using Getter = std::function<ConstDataPtr(ConstDataPtr self, DataPtr temporary)>;
using Setter = std::function<void(DataPtr self, ConstDataPtr value)>; // TODO DataPtr value to allow move?

class Property {
public:
	Property(Setter&& setter, Getter&& getter, const char* name, const Type* valueType, Allocator& allocator);
	const char*                       getName() const;
	const char*                       getPrettyName() const;
	const Type&                       getValueType() const;
	uint32_t                          getFlags() const;
	Semantic                          getSemantic() const;
	Property&                         setPrettyName(const char* str);
	Property&                         setFlags(uint32_t flags);
	Property&                         setSemantic(Semantic semantic);
	void                              setValue(DataPtr self, ConstDataPtr value) const;
	void                              getValue(ConstDataPtr self, DataPtr value) const;
	void                              copyValue(DataPtr dstSelf, ConstDataPtr srcSelf, LinearAllocator& alloc) const;
	Property&                         addAttribute(const Attribute* attribute);
	std::span<const Attribute* const> getAttributes() const;

	template <class T>
	const Attribute* queryAttribute() const;

private:
	using AttributeVec = std::vector<const Attribute*, stdAllocator<const Attribute*>>;

	Setter       setter;
	Getter       getter;
	const char*  name;
	const char*  prettyName;
	const Type*  valueType;
	uint32_t     flags;
	Semantic     semantic;
	AttributeVec attributes;
};

template <class T>
const Attribute* Property::queryAttribute() const {
	for (auto a : attributes) {
		if (a->tryCast<T>()) {
			return a;
		}
	}
	return nullptr;
}

} // namespace Typhoon::Reflection
