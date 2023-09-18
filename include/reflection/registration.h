#pragma once

#include "config.h"

#include "arrayType.h"
#include "bitMaskType.h"
#include "commonAttributes.h"
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

Context& getContext();

} // namespace detail

// Macro-based reflection

#define BEGIN_REFLECTION()                                              \
	do {                                                                \
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
	while (false)

#define BEGIN_NAMESPACE(name)                                                   \
	do {                                                                        \
		const auto parentNamespace = currNamespace;                             \
		Namespace* newNamespace = parentNamespace->getNestedNamespace(#name);   \
		if (! newNamespace) {                                                   \
			newNamespace = scopedAllocator_.make<Namespace>(#name, allocator_); \
			parentNamespace->addNestedNamespace(newNamespace);                  \
		}                                                                       \
		currNamespace = newNamespace;

#define END_NAMESPACE()              \
	currNamespace = parentNamespace; \
	}                                \
	while (0)

#define BEGIN_BASE_CLASS(class)                                                                                                                   \
	do {                                                                                                                                          \
		using class_ = class;                                                                                                                     \
		const auto structType = scopedAllocator_.make<StructType>(#class, Typhoon::getTypeId<class_>(), sizeof(class_), alignof(class_), nullptr, \
		                                                          MethodTable {}, allocator_);

#define BEGIN_STRUCT(class)                                                                                                                       \
	do {                                                                                                                                          \
		using class_ = class;                                                                                                                     \
		const auto structType = scopedAllocator_.make<StructType>(#class, Typhoon::getTypeId<class_>(), sizeof(class_), alignof(class_), nullptr, \
		                                                          detail::buildMethodTable<class_>(), allocator_);                                \
		do {                                                                                                                                      \
	} while (0)

#define BEGIN_CLASS(class)                                                                                                                        \
	do {                                                                                                                                          \
		using class_ = class;                                                                                                                     \
		const auto structType = scopedAllocator_.make<StructType>(#class, Typhoon::getTypeId<class_>(), sizeof(class_), alignof(class_), nullptr, \
		                                                          detail::buildMethodTable<class_>(), allocator_);                                \
		do {                                                                                                                                      \
	} while (0)

#define BEGIN_SUB_CLASS(class, parentClass)                                                                                              \
	do {                                                                                                                                 \
		using class_ = class;                                                                                                            \
		static_assert(! std::is_same_v<parentClass, class>, #parentClass " and " #class " are the same class");                          \
		static_assert(std::is_base_of_v<parentClass, class>, #parentClass " is not a base class of " #class);                            \
		const StructType& parentType = static_cast<const StructType&>(typeDB_.getType<parentClass>());                                   \
		assert(parentType.getSubClass() == Type::Subclass::Struct);                                                                      \
		const auto structType = scopedAllocator_.make<StructType>(#class, Typhoon::getTypeId<class_>(), sizeof(class_), alignof(class_), \
		                                                          &parentType, detail::buildMethodTable<class_>(), allocator_);          \
		do {                                                                                                                             \
	} while (0)

#define FIELD_RENAMED(field, name) structType->addProperty(detail::ClassUtil<class_>::makeProperty(name, &class_::field, context))

#define FIELD(field) FIELD_RENAMED(field, #field)

#define PROPERTY(name, getter, setter) \
	structType->addProperty(detail::ClassUtil<class_>::makeProperty(name, &class_::setter, &class_::getter, context))

#define GETTER(name, getter)                                                                              \
	do {                                                                                                  \
		structType->addProperty(detail::ClassUtil<class_>::makeProperty(name, &class_::getter, context)); \
	} while (false)

#define SETTER(name, setter)                                                                              \
	do {                                                                                                  \
		structType->addProperty(detail::ClassUtil<class_>::makeProperty(name, &class_::setter, context)); \
	} while (false)

#define C_PROPERTY(name, getter, setter)                                                                 \
	do {                                                                                                 \
		structType->addProperty(detail::ClassUtil<class_>::makeProperty(name, setter, getter, context)); \
	} while (0)

#define C_SETTER(name, setter) structType->addProperty(detail::ClassUtil<class_>::makeProperty(name, setter, context))

#define C_GETTER(name, getter)                                                                   \
	do {                                                                                         \
		structType->addProperty(detail::ClassUtil<class_>::makeProperty(name, getter, context)); \
	} while (0)

#define FIELD_WITH_SETTER(field, setter)                                                                           \
	do {                                                                                                           \
		structType->addProperty(detail::ClassUtil<class_>::makeProperty(#field, setter, &class_::field, context)); \
	} while (false)

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
	typeDB_.registerType(structType);   \
	currNamespace->addType(structType); \
	}                                   \
	while (0)

#define END_CLASS()                     \
	typeDB_.registerType(structType);   \
	currNamespace->addType(structType); \
	}                                   \
	while (0)

#define BEGIN_ENUM(enumClass)                                    \
	do {                                                         \
		using enumClass_ = enumClass;                            \
		static_assert(std::is_enum_v<enumClass>, "Not an enum"); \
		constexpr const char* enumName = #enumClass;             \
	static const Enumerator enumerators[] = {

#define ENUMERATOR_NAMED(name, value) makeEnumerator(name, enumClass_::value),

#define ENUMERATOR(value) makeEnumerator(#value, enumClass_::value),

#define END_ENUM()                                                                                                                              \
	}                                                                                                                                           \
	;                                                                                                                                           \
	const Type& underlyingType = typeDB_.getType<std::underlying_type_t<enumClass_>>();                                                         \
	const auto  enumType = scopedAllocator_.make<EnumType>(enumName, Typhoon::getTypeId<enumClass_>(), sizeof(enumClass_), alignof(enumClass_), \
                                                          enumerators, std::size(enumerators), &underlyingType, allocator_);                   \
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
	const Type& underlyingType = typeDB_.getType<bitMaskStruct_::StorageType>();                                                                     \
	const auto  bitmaskType =                                                                                                                        \
	    scopedAllocator_.make<BitMaskType>(typeName, getTypeId<bitMaskStruct_>(), &underlyingType, enumerators, std::size(enumerators), allocator_); \
	typeDB_.registerType(bitmaskType);                                                                                                               \
	currNamespace->addType(bitmaskType);                                                                                                             \
	}                                                                                                                                                \
	while (false)

#define FLAGS(flags)         setFlags((flags))
#define SEMANTIC(semantic)   setSemantic((semantic))
#define ATTRIBUTE(type, ...) addAttribute(scopedAllocator_.make<type>(__VA_ARGS__))

} // namespace Typhoon::Reflection
