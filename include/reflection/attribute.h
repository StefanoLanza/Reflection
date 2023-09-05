#pragma once

#include <cassert>
#include <core/typeId.h>

namespace Typhoon::Reflection {

class Attribute {
public:
	Attribute(TypeId typeId)
	    : typeId { typeId } {
	}

	template <class T>
	const T* tryCast() const;

	template <class T>
	const T& cast() const;

private:
	TypeId typeId;
};

template <class T>
inline const T* Attribute::tryCast() const {
	if (getTypeId<T>() == typeId) {
		return static_cast<const T*>(this);
	}
	return nullptr;
}

template <class T>
inline const T& Attribute::cast() const {
	assert(getTypeId<T>() == typeId);
	return static_cast<const T&>(*this);
}

} // namespace Typhoon::Reflection
