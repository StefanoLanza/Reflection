#pragma once

#include "config.h"

#include <core/typeId.h>

#include <cassert>
#include <string>
#include <type_traits>

namespace Typhoon::Reflection {

class Type;

class Variant {
public:
	Variant();
	Variant(const Variant& other);
	Variant(Variant&& other) noexcept;
	Variant(const void* data, TypeId typeId, std::string_view name);
	Variant(TypeId typeId, std::string_view name);

	template <class T>
	explicit Variant(T&& value, std::string_view name);

	~Variant();

	Variant& operator=(const Variant& other);
	Variant& operator=(Variant&& other) noexcept;

	TypeId getTypeId() const {
		return typeId;
	}
	const std::string& getName() const;
	void*              getStorage() {
        return &storage;
	}
	const void* getStorage() const {
		return &storage;
	}

	template <class T>
	void set(T&& value);

	template <class T>
	const T& get() const;

	template <class T>
	bool tryGet(T* value) const;

	const Type& getType() const;

	bool operator==(const Variant& other) const;
	bool operator!=(const Variant& other) const;

private:
	void destruct();

private:
	TypeId                         typeId;
	std::string                    name;
	alignas(max_align_t) std::byte storage[64];	
};

template <class T>
inline Variant::Variant(T&& value, std::string_view name)
    : typeId { nullTypeId }
    , name { name } {
	set(std::forward<T>(value));
}

template <class T>
inline void Variant::set(T&& value) {
	using ValueType = std::decay_t<T>;
	static_assert(sizeof(ValueType) <= sizeof(storage));
	static_assert(std::is_copy_constructible_v<ValueType>);
	if (typeId == Typhoon::getTypeId<ValueType>()) {
		*reinterpret_cast<ValueType*>(&storage) = std::forward<T>(value);
	}
	else {
		destruct();
		new (&storage) ValueType { std::forward<T>(value) };
		typeId = Typhoon::getTypeId<ValueType>();
	}
}

template <class T>
inline const T& Variant::get() const {
	assert(typeId == Typhoon::getTypeId<T>());
	return *reinterpret_cast<const T*>(&storage);
}

template <class T>
inline bool Variant::tryGet(T* valuePtr) const {
	assert(valuePtr);
	if (typeId == Typhoon::getTypeId<T>()) {
		*valuePtr = *reinterpret_cast<const T*>(&storage);
		return true;
	}
	return false;
}

} // namespace Typhoon::Reflection
