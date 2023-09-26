#pragma once

#include <cassert>
#include <core/typeId.h>

namespace Typhoon::Reflection {

struct AttributeTargets {
	enum : uint32_t {
		Class = 1 << 0,
		Enum = 1 << 1,
		Field = 1 << 2,
		Property = 1 << 3,
		Struct = 1 << 4,
	};
};

class Attribute {
public:
	Attribute(TypeId typeId, uint32_t targets)
	    : typeId { typeId }
	    , targets { targets } {
	}

	template <class T>
	const T* tryCast() const;

	template <class T>
	const T& cast() const;

	TypeId getTypeId() const {
		return typeId;
	}

	uint32_t getTargets() const {
		return targets;
	}

private:
	TypeId   typeId;
	uint32_t targets;
};

template <class T>
inline const T* Attribute::tryCast() const {
	if (Typhoon::getTypeId<T>() == typeId) {
		return static_cast<const T*>(this);
	}
	return nullptr;
}

template <class T>
inline const T& Attribute::cast() const {
	assert(Typhoon::getTypeId<T>() == typeId);
	return static_cast<const T&>(*this);
}

} // namespace Typhoon::Reflection
