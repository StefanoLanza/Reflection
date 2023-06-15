#pragma once

#include "dataPtr.h"
#include <core/typeId.h>
#include <core/uncopyable.h>
#include <functional>
#include <type_traits>

namespace Typhoon::Reflection {

using Constructor = void (*)(DataPtr);
using Destructor = void (*)(DataPtr);
using CopyConstructor = void (*)(DataPtr, ConstDataPtr b);
using CopyAssignment = void (*)(DataPtr, ConstDataPtr b);
using MoveConstructor = void (*)(DataPtr, DataPtr b);
using MoveAssignment = void (*)(DataPtr, DataPtr b);
using EqualityOperator = bool (*)(ConstDataPtr, ConstDataPtr b);

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
inline void defaultConstruct([[maybe_unused]] DataPtr object) {
	// Use placement new to call the constructor
	if constexpr (std::is_default_constructible_v<Type>) {
		new (object) Type;
	}
}

template <typename Type>
inline void destruct([[maybe_unused]] DataPtr object) {
	if constexpr (std::is_destructible_v<Type>) {
		// Explicit call of the destructor
		static_cast<Type*>(object)->Type::~Type();
	}
}

template <typename Type>
inline void copyConstruct([[maybe_unused]] DataPtr object, [[maybe_unused]] ConstDataPtr rhs) {
	// Warning: is_copy_constructible_v returns true in some cases like vector<unique_ptr>
	if constexpr (std::is_copy_constructible_v<Type>) {
		// Use placement new to call the copy constructor
		new (object) Type { *static_cast<const Type*>(rhs) };
	}
}

template <typename Type>
inline void copyAssign([[maybe_unused]] DataPtr a, [[maybe_unused]] ConstDataPtr b) {
	// Warning: is_copy_assignable_v returns true in some cases like vector<unique_ptr>
	if constexpr (std::is_copy_assignable_v<Type>) {
		*static_cast<Type*>(a) = *static_cast<const Type*>(b);
	}
}

template <typename Type>
inline void moveConstruct([[maybe_unused]] DataPtr object, [[maybe_unused]] DataPtr rhs) {
	if constexpr (std::is_move_constructible_v<Type>) {
		// Use placement new to call the move constructor
		new (object) Type { std::move(*static_cast<Type*>(rhs)) };
	}
}

template <typename Type>
inline void moveAssign([[maybe_unused]] DataPtr a, [[maybe_unused]] DataPtr b) {
	if constexpr (std::is_move_assignable_v<Type>) {
		*static_cast<Type*>(a) = std::move(*static_cast<Type*>(b));
	}
}

// ref: https://stackoverflow.com/questions/6534041/how-to-check-whether-operator-exists
template <typename T, typename = bool>
struct has_equal_to : std::false_type {};

template <typename T>
struct has_equal_to<T, decltype(std::declval<T>() == std::declval<T>())> : std::true_type {};

template <typename Type>
inline bool equalityOperator([[maybe_unused]] ConstDataPtr a, [[maybe_unused]] ConstDataPtr b) {
	if constexpr (has_equal_to<Type>::value) {
		return *static_cast<const Type*>(a) == *static_cast<const Type*>(b);
	}
	else {
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

class InputArchive;
class OutputArchive;

// ? Error codes?
using CustomWriter = std::function<void(ConstDataPtr, OutputArchive&)>;
using CustomReader = std::function<void(DataPtr, InputArchive&)>;
using CustomCloner = std::function<void(DataPtr, ConstDataPtr)>;

class Type : Uncopyable {
public:
	enum class Subclass
	{
		Builtin,
		Struct,
		Enum,
		BitMask,
		Container,
		Pointer,
		Reference,
		Variant
	};

	Type(const char* typeName, TypeId typeId, Subclass subClass, size_t size, size_t alignment, const MethodTable& methods);

	const char*         getName() const;
	TypeId              getTypeId() const;
	size_t              getSize() const;
	size_t              getAlignment() const;
	Subclass            getSubClass() const;
	void                constructObject(DataPtr object) const;
	void                destructObject(DataPtr object) const;
	void                copyConstructObject(DataPtr object, ConstDataPtr src) const;
	void                copyObject(DataPtr a, ConstDataPtr b) const;
	void                moveConstructObject(DataPtr object, DataPtr src) const;
	void                moveObject(DataPtr a, DataPtr b) const;
	bool                compareObjects(ConstDataPtr a, ConstDataPtr b) const;
	void                setCustomWriter(CustomWriter saver);
	const CustomWriter& getCustomWriter() const;
	void                setCustomReader(CustomReader loader);
	const CustomReader& getCustomReader() const;
	void                setCustomCloner(CustomCloner loader);
	const CustomCloner& getCustomCloner() const;

private:
	TypeId       typeID;
	size_t       size;
	size_t       alignment;
	Subclass     subClass;
	const char*  typeName;
	MethodTable  methods;
	CustomWriter customWriter;
	CustomReader customReader;
	CustomCloner customCloner;
};

} // namespace Typhoon::Reflection
