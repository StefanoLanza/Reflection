#pragma once

#include "config.h"

#include "arrayType.h"
#include "bitMaskType.h"
#include "containerType.h"
#include "enumType.h"
#include "flags.h"
#include "makeProperty.h"
#include "namespace.h"
#include "pointerType.h"
#include "property.h"
#include "referenceType.h"
#include "semantics.h"
#include "stdArrayType.h"
#include "stdMapType.h"
#include "stdPairType.h"
#include "stdSharedPtrType.h"
#include "stdTupleType.h"
#include "stdUniquePtrType.h"
#include "stdVectorType.h"
#include "structType.h"
#include "typeDB.h"
#include <core/scopedAllocator.h>

namespace Typhoon::Reflection {

namespace detail {

template <class T>
inline const Type* autoRegisterType(Context& context) {
	const Type* type = context.typeDB->tryGetType<T>();
	if (! type) {
		type = autoRegisterHelper<T>::autoRegister(context);
	}
	assert(type);
	return type;
}

Context& getContext();

} // namespace detail

// Macro-based reflection

#define BEGIN_REFLECTION()                                              \
	__pragma(warning(push)) __pragma(warning(disable : 4127)) do {      \
		using namespace refl;                                           \
		using namespace Typhoon;                                        \
		Context&         context = detail::getContext();                \
		TypeDB&          typeDB_ = *context.typeDB;                     \
		ScopedAllocator& scopedAllocator_ = *context.scopedAllocator;   \
		Allocator&       allocator_ = *context.allocator;               \
		Namespace*       currNamespace = &typeDB_.getGlobalNamespace(); \
		(void)allocator_;

#define END_REFLECTION() \
	}                    \
	while (false)        \
	__pragma(warning(pop))

#define BEGIN_BASE_CLASS(class)                                                                                                               \
	do {                                                                                                                                      \
		using class_ = class;                                                                                                                 \
		constexpr bool isClass = true;                                                                                                        \
		const auto     structType = scopedAllocator_.make<StructType>(Typhoon::getTypeId<class_>(), sizeof(class_), alignof(class_), nullptr, \
                                                                  MethodTable {}, std::ref(allocator_));

#define BEGIN_STRUCT(class)                                                                                                                           \
	do {                                                                                                                                              \
		using class_ = class;                                                                                                                         \
		constexpr bool isClass = false;                                                                                                               \
		const auto     structType = scopedAllocator_.make<StructType>(#class, Typhoon::getTypeId<class_>(), sizeof(class_), alignof(class_), nullptr, \
                                                                  detail::buildMethodTable<class_>(), std::ref(allocator_));                      \
		do {                                                                                                                                          \
	} while (0)

#define BEGIN_CLASS(class)                                                                                                                            \
	do {                                                                                                                                              \
		using class_ = class;                                                                                                                         \
		constexpr bool isClass = true;                                                                                                                \
		const auto     structType = scopedAllocator_.make<StructType>(#class, Typhoon::getTypeId<class_>(), sizeof(class_), alignof(class_), nullptr, \
                                                                  detail::buildMethodTable<class_>(), std::ref(allocator_));                      \
		do {                                                                                                                                          \
	} while (0)

#define BEGIN_SUB_CLASS(class, parentClass)                                                                                               \
	do {                                                                                                                                  \
		using class_ = class;                                                                                                             \
		constexpr bool isClass = true;                                                                                                    \
		static_assert(! std::is_same_v<parentClass, class>, #parentClass " and " #class " are the same class");                           \
		static_assert(std::is_base_of_v<parentClass, class>, #parentClass " is not a base class of " #class);                             \
		const StructType& parentType = static_cast<const StructType&>(typeDB_.getType<parentClass>());                                    \
		assert(parentType.subClass == Type::Subclass::Struct);                                                                            \
		const auto structType = scopedAllocator_.make<StructType>(#class, Typhoon::getTypeId<class_>(), sizeof(class_), alignof(class_),  \
		                                                          &parentType, detail::buildMethodTable<class_>(), std::ref(allocator_)); \
		do {                                                                                                                              \
	} while (0)

#define FIELD_RENAMED_EXT(field, name, flags, semantic)                                                       \
	do {                                                                                                      \
		structType->addProperty(detail::createFieldProperty(name, flags, semantic, &class_::field, context)); \
	} while (false)

#define FIELD_RENAMED(field, name) FIELD_RENAMED_EXT(field, name, Flags::all, Semantic::none)

#define FIELD(field) FIELD_RENAMED(field, #field)

#define FIELD_EXT(field, flags, semantic) FIELD_RENAMED_EXT(field, #field, flags, semantic)

#define PROPERTY(name, getter, setter)                                                                                                       \
	do {                                                                                                                                     \
		static_assert(isClass);                                                                                                              \
		structType->addProperty(                                                                                                             \
		    detail::PropertyHelpers<class_>::createRWProperty(name, Flags::all, Semantic::none, &class_::setter, &class_::getter, context)); \
	} while (false)

#define GETTER(name, getter)                                                                                                                    \
	do {                                                                                                                                        \
		structType->addProperty(detail::PropertyHelpers<class_>::createROProperty(name, Flags::all, Semantic::none, &class_::getter, context)); \
	} while (false)

#define SETTER(name, setter)                                                                                                         \
	do {                                                                                                                             \
		structType->addProperty(detail::PropertyHelpers<class_>::createProperty(name, Flags::all, Semantic::none, setter, context)); \
	} while (false)

#define PROPERTY_EX(name, getter, setter, flags, semantic)                                                                        \
	do {                                                                                                                          \
		structType->addProperty(                                                                                                  \
		    detail::PropertyHelpers<class_>::createRWProperty(name, flags, semantic, &class_::setter, &class_::getter, context)); \
	} while (false)

#define C_PROPERTY(name, setter, getter)                                                                                                       \
	do {                                                                                                                                       \
		structType->addProperty(detail::PropertyHelpers<class_>::createRWProperty(name, Flags::all, Semantic::none, setter, getter, context)); \
	} while (0)

#define C_PROPERTY_EXT(name, setter, getter, flags, semantic)                                                                       \
	do {                                                                                                                            \
		structType->addProperty(detail::PropertyHelpers<class_>::createRWProperty(name, flags, semantic, setter, getter, context)); \
	} while (0)

#define C_SETTER_EX(member, setter, flags, semantic)                                                                                          \
	do {                                                                                                                                      \
		structType->addProperty(detail::PropertyHelpers<class_>::createProperty(#member, flags, semantic, setter, &class_::member, context)); \
	} while (0)

#define C_SETTER(member, setter) C_SETTER_EX(member, setter, Flags::all, Semantic::none)

#define C_GETTER(name, getter)                                                                                                         \
	do {                                                                                                                               \
		structType->addProperty(                                                                                                       \
		    detail::PropertyHelpers<class_>::createROProperty(name, Flags::view | Flags::writeable, Semantic::none, getter, context)); \
	} while (0)

#define READER(reader)                       \
	do {                                     \
		structType->setCustomReader(reader); \
	} while (0)

#define WRITER(writer)                       \
	do {                                     \
		structType->setCustomWriter(writer); \
	} while (0)

#define CLONER(cloner)                       \
	do {                                     \
		structType->setCustomCloner(cloner); \
	} while (0)

#define END_STRUCT()                    \
	static_assert(! isClass);           \
	typeDB_.registerType(structType);   \
	currNamespace->addType(structType); \
	}                                   \
	while (0)

#define END_CLASS()                     \
	static_assert(isClass);             \
	typeDB_.registerType(structType);   \
	currNamespace->addType(structType); \
	}                                   \
	while (0)

#define BEGIN_ENUM(enumClass)                                    \
	do {                                                         \
		using enumClass_ = enumClass;                            \
		static const char* enumName = #enumClass;                \
		static_assert(std::is_enum_v<enumClass>, "Not an enum"); \
	static const Enumerator enumerators[] = {

#define ENUMERATOR_NAMED(name, value) makeEnumerator(name, enumClass_::value),

#define ENUMERATOR(value) makeEnumerator(#value, enumClass_::value),

#define END_ENUM()                                                                                                                              \
	}                                                                                                                                           \
	;                                                                                                                                           \
	const Type& underlyingType = typeDB_.getType<std::underlying_type_t<enumClass_>>();                                                       \
	const auto  enumType = scopedAllocator_.make<EnumType>(enumName, Typhoon::getTypeId<enumClass_>(), sizeof(enumClass_), alignof(enumClass_), \
                                                          enumerators, _countof(enumerators), &underlyingType);                                \
	typeDB_.registerType(enumType);                                                                                                             \
	currNamespace->addType(enumType);                                                                                                           \
	}                                                                                                                                           \
	while (false)

#define BEGIN_BITMASK(bitMaskStruct)                                                                                  \
	do {                                                                                                              \
		using bitMaskStruct_ = bitMaskStruct;                                                                         \
		static_assert(std::is_convertible_v<bitMaskStruct_::StorageType, BitMaskStorageType>, "Not of type BitMask"); \
		const char* typeName = #bitMaskStruct;                                                                        \
	static const BitMaskConstant enumerators[] = {

#define BITMASK_VALUE(name) { #name, static_cast<BitMaskStorageType>(bitMaskStruct_::name) },

#define END_BITMASK()                                                                                                                                \
	}                                                                                                                                                \
	;                                                                                                                                                \
	const auto bitmaskType = scopedAllocator_.make<BitMaskType>(typeName, Typhoon::getTypeId<bitMaskStruct_>(), sizeof(bitMaskStruct_::StorageType), \
	                                                            alignof(bitMaskStruct_::StorageType), enumerators, std::size(enumerators));          \
	typeDB_.registerType(bitmaskType);                                                                                                               \
	currNamespace->addType(bitmaskType);                                                                                                             \
	}                                                                                                                                                \
	while (false)

} // namespace Typhoon::Reflection
