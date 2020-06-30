#pragma once

#include <core/typeId.h>
#include <type_traits>

namespace Typhoon::Reflection {

using Constructor = void (*)(void*);
using Destructor = void (*)(void*);
using CopyConstructor = void (*)(void*, const void* b);
using CopyAssignment = void (*)(void*, const void* b);
using MoveConstructor = void (*)(void*, void* b);
using MoveAssignment = void (*)(void*, void* b);
using EqualityOperator = bool (*)(const void*, const void* b);

struct MethodTable {
	Constructor      defaultConstructor;
	Destructor       destructor;
	CopyConstructor  copyConstructor;
	CopyAssignment   copyAssignment;
	MoveConstructor  moveConstructor;
	MoveAssignment   moveAssignment;
	EqualityOperator equalityOperator;
};

namespace detail {

template <typename Type>
inline void defaultConstruct(void* object) {
	// Use placement new to call the constructor
	if constexpr (std::is_default_constructible_v<Type>) {
		new (object) Type;
	}
	(void)object;
}

template <typename Type>
inline void destruct(void* object) {
	if constexpr (std::is_destructible_v<Type>) {
		// Explicit call of the destructor
		static_cast<Type*>(object)->Type::~Type();
	}
	(void)object;
}

template <typename Type>
inline void copyConstruct(void* object, const void* rhs) {
	// Warning: is_copy_constructible_v returns true in some cases like vector<unique_ptr>
	if constexpr (std::is_copy_constructible_v<Type>) {
		// Use placement new to call the copy constructor
		new (object) Type { *static_cast<const Type*>(rhs) };
	}
	(void)object;
	(void)rhs;
}

template <typename Type>
inline void copyAssign(void* a, const void* b) {
	// Warning: is_copy_assignable_v returns true in some cases like vector<unique_ptr>
	if constexpr (std::is_copy_assignable_v<Type>) {
		*static_cast<Type*>(a) = *static_cast<const Type*>(b);
	}
	(void)a;
	(void)b;
}

template <typename Type>
inline void moveConstruct(void* object, void* rhs) {
	if constexpr (std::is_move_constructible_v<Type>) {
		// Use placement new to call the move constructor
		new (object) Type { std::move(*static_cast<Type*>(rhs)) };
	}
	(void)object;
	(void)rhs;
}

template <typename Type>
inline void moveAssign(void* a, void* b) {
	if constexpr (std::is_move_assignable_v<Type>) {
		*static_cast<Type*>(a) = std::move(*static_cast<Type*>(b));
	}
	(void)a;
	(void)b;
}

// ref: https://stackoverflow.com/questions/6534041/how-to-check-whether-operator-exists
template <typename T, typename = bool>
struct has_equal_to : std::false_type {};

template <typename T>
struct has_equal_to<T, decltype(std::declval<T>() == std::declval<T>())> : std::true_type {};

template <typename Type>
inline bool equalityOperator(const void* a, const void* b) {
	if constexpr (has_equal_to<Type>::value) {
		return *static_cast<const Type*>(a) == *static_cast<const Type*>(b);
	}
	else {
		(void)a;
		(void)b;
		return false;
	}
}

template <typename Type>
inline MethodTable buildMethodTable() {
	return {
		defaultConstruct<Type>, destruct<Type>, copyConstruct<Type>, copyAssign<Type>, moveConstruct<Type>, moveAssign<Type>, equalityOperator<Type>,
	};
}
} // namespace detail

class Type {
public:
	enum class Subclass { Builtin, Struct, Enum, BitMask, Container, Pointer, Reference, Variant };

	Type(TypeId typeId, Subclass subClass, size_t size, size_t alignment, const MethodTable& methods);

	TypeId   getTypeId() const;
	size_t   getSize() const;
	size_t   getAlignment() const;
	Subclass getSubClass() const;
	void     constructObject(void* object) const;
	void     destructObject(void* object) const;
	void     copyConstructObject(void* object, const void* src) const;
	void     copyObject(void* a, const void* b) const;
	void     moveConstructObject(void* object, void* src) const;
	void     moveObject(void* a, void* b) const;
	bool     compareObjects(const void* a, const void* b) const;

	TypeId   typeID;
	size_t   size;
	size_t   alignment;
	Subclass subClass;

private:
	MethodTable methods;
};

class TypeDB;

namespace detail {

template <class _Ty>
struct autoRegisterHelper {
	static const Type* autoRegister(TypeDB&) {
		return nullptr; // not supported
	}
};

template <class _Ty>
const Type* autoRegisterType(TypeDB& typeDB);

} // namespace detail

} // namespace Typhoon
