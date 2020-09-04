#include "cloneObject.h"
#include "bitMaskType.h"
#include "builtinType.h"
#include "config.h"
#include "containerType.h"
#include "enumType.h"
#include "flags.h"
#include "pointerType.h"
#include "property.h"
#include "referenceType.h"
#include "reflection.h"
#include "structType.h"
#include "type.h"
#include "typeDB.h"
#include "variant.h"
#include <cassert>
#include <core/ptrUtil.h>
#include <core/scopedAllocator.h>

namespace Typhoon::Reflection {

namespace {

ErrorCode cloneObjectImpl(void* dstData, const void* srcData, const Type& type, LinearAllocator& stackAllocator);
void      cloneBuiltin(void* data, const void* srcData, const BuiltinType& type);
void      cloneStruct(void* data, const void* srcData, const StructType& structType, const TypeDB& typeDB);
void      cloneEnum(void* data, const void* srcData, const EnumType& type);
void      cloneBitMask(void* data, const void* srcData, const BitMaskType& type);
void      cloneContainer(void* data, const void* srcData, const ContainerType& type, LinearAllocator& stackAllocator);
void      clonePointer(void* data, const void* srcData, const PointerType& type, LinearAllocator& stackAllocator);
void      cloneReference(void* data, const void* srcData, const ReferenceType& type, LinearAllocator& stackAllocator);
void      cloneVariant(void* dstData, const void* srcData, const TypeDB& typeDB);

} // namespace

ErrorCode cloneObject(void* dstObject, const void* srcObject, TypeId typeId) {
	const TypeDB& typeDB = detail::getTypeDB();
	const Type*   type = typeDB.tryGetType(typeId);
	ErrorCode     errorCode;
	if (type) {
		if (const CustomCloner& customCloner = type->getCustomCloner(); customCloner) {
			customCloner(dstObject, srcObject);
			errorCode = ErrorCode::ok;
		}
		else {
			char            stack[stackSize];
			LinearAllocator stackAllocator(stack, std::size(stack), nullptr);
			errorCode = cloneObjectImpl(dstObject, srcObject, *type, stackAllocator);
		}
	}
	else {
		errorCode = ErrorCode::unknownType;
	}
	return errorCode;
}

namespace {

ErrorCode cloneObjectImpl(void* dstData, const void* srcData, const Type& type, LinearAllocator& stackAllocator) {
	const TypeDB&        typeDB = detail::getTypeDB();
	const Type::Subclass subclass = type.subClass;
	if (subclass == Type::Subclass::Builtin) {
		cloneBuiltin(dstData, srcData, static_cast<const BuiltinType&>(type));
	}
	else if (subclass == Type::Subclass::Struct) {
		cloneStruct(dstData, srcData, static_cast<const StructType&>(type), typeDB);
	}
	else if (subclass == Type::Subclass::Enum) {
		cloneEnum(dstData, srcData, static_cast<const EnumType&>(type));
	}
	else if (subclass == Type::Subclass::BitMask) {
		cloneBitMask(dstData, srcData, static_cast<const BitMaskType&>(type));
	}
	else if (subclass == Type::Subclass::Container) {
		cloneContainer(dstData, srcData, static_cast<const ContainerType&>(type), stackAllocator);
	}
	else if (subclass == Type::Subclass::Pointer) {
		clonePointer(dstData, srcData, static_cast<const PointerType&>(type), stackAllocator);
	}
	else if (subclass == Type::Subclass::Reference) {
		cloneReference(dstData, srcData, static_cast<const ReferenceType&>(type), stackAllocator);
	}
	else if (subclass == Type::Subclass::Variant) {
		cloneVariant(dstData, srcData, typeDB);
	}
	return ErrorCode::ok;
}

void cloneStruct(void* dstData, const void* srcData, const StructType& structType, const TypeDB& typeDB) {
	for (const auto& property : structType.getProperties()) {
		if (property.getFlags() & Flags::clonable) {
			property.copyValue(dstData, srcData);
		}
	}

	const StructType* parentType = structType.getParentType();
	if (parentType) {
		cloneStruct(dstData, srcData, *parentType, typeDB);
	}
}

void cloneBuiltin(void* data, const void* srcData, const BuiltinType& type) {
	type.copyObject(data, srcData);
}

void cloneEnum(void* data, const void* srcData, const EnumType& type) {
	std::memcpy(data, srcData, type.size);
}

void cloneBitMask(void* data, const void* srcData, const BitMaskType& type) {
	std::memcpy(data, srcData, type.size);
}

void cloneContainer(void* dstContainer, const void* srcContainer, const ContainerType& type, LinearAllocator& stackAllocator) {
	const Type* key_type = type.getKeyType();
	const Type* value_type = type.getValueType();

	ScopedAllocator      scopedAllocator(stackAllocator);
	WriteIterator* const writeIterator = type.newWriteIterator(dstContainer, scopedAllocator);
	ReadIterator* const  readIterator = type.newReadIterator(srcContainer, scopedAllocator);
	type.clear(dstContainer);

	while (readIterator->isValid()) {
		if (key_type) {
			const void* key = readIterator->getKey();
			const void* srcValue = readIterator->getValue();
			void*       dstValue = writeIterator->insert(key);
			// Clone value
			cloneObjectImpl(dstValue, srcValue, *value_type, stackAllocator);
		}
		else {
			const void* srcValue = readIterator->getValue();
			void*       dstValue = writeIterator->pushBack();
			// Clone value
			cloneObjectImpl(dstValue, srcValue, *value_type, stackAllocator);
		}
		readIterator->gotoNext();
	}
}

void clonePointer(void* data, const void* srcData, const PointerType& type, LinearAllocator& stackAllocator) {
	void*       dstPointer = type.resolvePointer(data);
	const void* srcPointer = type.resolvePointer(srcData);
	if (dstPointer && srcPointer) {
		cloneObjectImpl(dstPointer, srcPointer, type.getPointedType(), stackAllocator);
	}
}

void cloneReference(void* data, const void* srcData, const ReferenceType& type, LinearAllocator& stackAllocator) {
	void*       dstPointer = *static_cast<void**>(data);
	const void* srcPointer = *static_cast<void* const*>(srcData);
	assert(dstPointer); // cannot be null
	assert(srcPointer);
	cloneObjectImpl(dstPointer, srcPointer, type.getReferencedType(), stackAllocator);
}

void cloneVariant(void* dstData, const void* srcData, const TypeDB& /*typeDB*/) {
	const Variant* srcVariant = static_cast<const Variant*>(srcData);
	Variant*       dstVariant = static_cast<Variant*>(dstData);
	*dstVariant = *srcVariant;
}

} // namespace

} // namespace Typhoon::Reflection
