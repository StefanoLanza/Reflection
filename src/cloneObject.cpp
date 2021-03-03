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

ErrorCode cloneObjectImpl(DataPtr dstData, ConstDataPtr srcData, const Type& type, LinearAllocator& allocator);
void      cloneBuiltin(DataPtr data, ConstDataPtr srcData, const BuiltinType& type);
void      cloneStruct(DataPtr data, ConstDataPtr srcData, const StructType& structType, const TypeDB& typeDB, LinearAllocator& allocator);
void      cloneEnum(DataPtr data, ConstDataPtr srcData, const EnumType& type);
void      cloneBitMask(DataPtr data, ConstDataPtr srcData, const BitMaskType& type);
void      cloneContainer(DataPtr data, ConstDataPtr srcData, const ContainerType& type, LinearAllocator& allocator);
void      clonePointer(DataPtr data, ConstDataPtr srcData, const PointerType& type, LinearAllocator& allocator);
void      cloneReference(DataPtr data, ConstDataPtr srcData, const ReferenceType& type, LinearAllocator& allocator);
void      cloneVariant(DataPtr dstData, ConstDataPtr srcData, const TypeDB& typeDB);

} // namespace

ErrorCode cloneObject(DataPtr dstObject, ConstDataPtr srcObject, TypeId typeId) {
	const TypeDB& typeDB = detail::getTypeDB();
	const Type*   type = typeDB.tryGetType(typeId);
	ErrorCode     errorCode;
	if (type) {
		if (const CustomCloner& customCloner = type->getCustomCloner(); customCloner) {
			customCloner(dstObject, srcObject);
			errorCode = ErrorCode::ok;
		}
		else {
			errorCode = cloneObjectImpl(dstObject, srcObject, *type, *detail::getContext().pagedAllocator);
		}
	}
	else {
		errorCode = ErrorCode::unknownType;
	}
	return errorCode;
}

namespace {

ErrorCode cloneObjectImpl(DataPtr dstData, ConstDataPtr srcData, const Type& type, LinearAllocator& allocator) {
	const TypeDB&        typeDB = detail::getTypeDB();
	const Type::Subclass subclass = type.getSubClass();
	if (subclass == Type::Subclass::Builtin) {
		cloneBuiltin(dstData, srcData, static_cast<const BuiltinType&>(type));
	}
	else if (subclass == Type::Subclass::Struct) {
		cloneStruct(dstData, srcData, static_cast<const StructType&>(type), typeDB, allocator);
	}
	else if (subclass == Type::Subclass::Enum) {
		cloneEnum(dstData, srcData, static_cast<const EnumType&>(type));
	}
	else if (subclass == Type::Subclass::BitMask) {
		cloneBitMask(dstData, srcData, static_cast<const BitMaskType&>(type));
	}
	else if (subclass == Type::Subclass::Container) {
		cloneContainer(dstData, srcData, static_cast<const ContainerType&>(type), allocator);
	}
	else if (subclass == Type::Subclass::Pointer) {
		clonePointer(dstData, srcData, static_cast<const PointerType&>(type), allocator);
	}
	else if (subclass == Type::Subclass::Reference) {
		cloneReference(dstData, srcData, static_cast<const ReferenceType&>(type), allocator);
	}
	else if (subclass == Type::Subclass::Variant) {
		cloneVariant(dstData, srcData, typeDB);
	}
	return ErrorCode::ok;
}

void cloneStruct(DataPtr dstData, ConstDataPtr srcData, const StructType& structType, const TypeDB& typeDB, LinearAllocator& allocator) {
	for (const auto& property : structType.getProperties()) {
		if (property.getFlags() & Flags::clonable) {
			property.copyValue(dstData, srcData, allocator);
		}
	}

	if (const StructType* parentType = structType.getParentType(); parentType) {
		cloneStruct(dstData, srcData, *parentType, typeDB, allocator);
	}
}

void cloneBuiltin(DataPtr data, ConstDataPtr srcData, const BuiltinType& type) {
	type.copyObject(data, srcData);
}

void cloneEnum(DataPtr data, ConstDataPtr srcData, const EnumType& type) {
	std::memcpy(data, srcData, type.getSize());
}

void cloneBitMask(DataPtr data, ConstDataPtr srcData, const BitMaskType& type) {
	std::memcpy(data, srcData, type.getSize());
}

void cloneContainer(DataPtr dstContainer, ConstDataPtr srcContainer, const ContainerType& type, LinearAllocator& allocator) {
	const Type* key_type = type.getKeyType();
	const Type* value_type = type.getValueType();

	ScopedAllocator      scopedAllocator(allocator);
	WriteIterator* const writeIterator = type.newWriteIterator(dstContainer, scopedAllocator);
	ReadIterator* const  readIterator = type.newReadIterator(srcContainer, scopedAllocator);
	type.clear(dstContainer);

	while (readIterator->isValid()) {
		if (key_type) {
			ConstDataPtr key = readIterator->getKey();
			ConstDataPtr srcValue = readIterator->getValue();
			DataPtr      dstValue = writeIterator->insert(key);
			// Clone value
			cloneObjectImpl(dstValue, srcValue, *value_type, allocator);
		}
		else {
			ConstDataPtr srcValue = readIterator->getValue();
			DataPtr      dstValue = writeIterator->pushBack();
			// Clone value
			cloneObjectImpl(dstValue, srcValue, *value_type, allocator);
		}
		readIterator->gotoNext();
	}
}

void clonePointer(DataPtr data, ConstDataPtr srcData, const PointerType& type, LinearAllocator& allocator) {
	DataPtr      dstPointer = type.resolvePointer(data);
	ConstDataPtr srcPointer = type.resolvePointer(srcData);
	if (dstPointer && srcPointer) {
		cloneObjectImpl(dstPointer, srcPointer, type.getPointedType(), allocator);
	}
}

void cloneReference(DataPtr data, ConstDataPtr srcData, const ReferenceType& type, LinearAllocator& allocator) {
	DataPtr      dstPointer = *cast<DataPtr>(data);
	ConstDataPtr srcPointer = *cast<ConstDataPtr>(srcData);
	assert(dstPointer); // cannot be null
	assert(srcPointer);
	cloneObjectImpl(dstPointer, srcPointer, type.getReferencedType(), allocator);
}

void cloneVariant(DataPtr dstData, ConstDataPtr srcData, const TypeDB& /*typeDB*/) {
	const Variant* srcVariant = cast<Variant>(srcData);
	Variant*       dstVariant = cast<Variant>(dstData);
	*dstVariant = *srcVariant;
}

} // namespace

} // namespace Typhoon::Reflection
