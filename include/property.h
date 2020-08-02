#pragma once

#include "config.h"
#include "semantics.h"

#include <cstdint>
#include <functional>

namespace Typhoon::Reflection {

class Type;

using Getter = std::function<const void*(const void* self, void* temporary)>;
using Setter = std::function<void(void* self, const void* value)>; // FIXME void* value to allow move?

class Property {
public:
	Property(Setter&& setter, Getter&& getter, const char* name, const Type* valueType, uint32_t flags, Semantic semantic);

	const char* getName() const;
	const Type& getValueType() const;
	uint32_t    getFlags() const;
	Semantic    getSemantic() const;

	void setValue(void* self, const void* value) const;
	void getValue(const void* self, void* value) const;
	void copyValue(void* dstSelf, const void* srcSelf) const;

private:
	Setter      setter;
	Getter      getter;
	const char* name;
	const Type* valueType;
	uint32_t    flags;
	Semantic    semantic;
};

} // namespace Typhoon::Reflection
