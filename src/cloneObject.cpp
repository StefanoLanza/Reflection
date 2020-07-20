#include "cloneObject.h"
#include "bitMaskType.h"
#include "builtinType.h"
#include "config.h"
#include "containerType.h"
#include "enumType.h"
#include "field.h"
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

namespace Typhoon::Reflection {

namespace {
ErrorCode cloneObject(void* dstData, const void* srcData, const Type& type);
void      cloneBuiltin(void* data, const void* srcData, const BuiltinType& type);
void      cloneStruct(void* data, const void* srcData, const StructType& structType, const TypeDB& typeDB);
void      cloneEnum(void* data, const void* srcData, const EnumType& type);
void      cloneBitMask(void* data, const void* srcData, const BitMaskType& type);
void      cloneContainer(void* data, const void* srcData, const ContainerType& type);
void      clonePointer(void* data, const void* srcData, const PointerType& type);
void      cloneReference(void* data, const void* srcData, const ReferenceType& type);
void      cloneVariant(void* dstData, const void* srcData, const TypeDB& typeDB);
} // namespace

ErrorCode cloneObject(void* dstObject, const void* srcObject, TypeId typeId) {
	const TypeDB& typeDB = getTypeDB();
	const Type*   type = typeDB.tryGetType(typeId);
	ErrorCode     errorCode;
	if (type) {
		if (const CustomCloner customCloner = typeDB.getCustomCloner(type); customCloner) {
			customCloner(dstObject, srcObject);
			errorCode = ErrorCode::ok;
		}
		else {
			errorCode = cloneObject(dstObject, srcObject, *type);
		}
	}
	else {
		errorCode = ErrorCode::unknownType;
	}
	return errorCode;
}

namespace {
ErrorCode cloneObject(void* dstData, const void* srcData, const Type& type) {
	const TypeDB&        typeDB = getTypeDB();
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
		cloneContainer(dstData, srcData, static_cast<const ContainerType&>(type));
	}
	else if (subclass == Type::Subclass::Pointer) {
		clonePointer(dstData, srcData, static_cast<const PointerType&>(type));
	}
	else if (subclass == Type::Subclass::Reference) {
		cloneReference(dstData, srcData, static_cast<const ReferenceType&>(type));
	}
	else if (subclass == Type::Subclass::Variant) {
		cloneVariant(dstData, srcData, typeDB);
	}
	return ErrorCode::ok;
}

void cloneStruct(void* dstData, const void* srcData, const StructType& structType, const TypeDB& typeDB) {
	for (const auto& field : structType.getFields()) {
		if (field.flags & Flags::clonable) {
			void*       dstFieldData = advancePointer(dstData, field.offset);
			const void* srcFieldData = advancePointer(srcData, field.offset);
			cloneObject(dstFieldData, srcFieldData, *field.type);
		}
	}

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
	constexpr TypeId boolID = getTypeId<bool>();
	constexpr TypeId charID = getTypeId<char>();
	constexpr TypeId ucharID = getTypeId<unsigned char>();
	constexpr TypeId floatID = getTypeId<float>();
	constexpr TypeId shortID = getTypeId<short>();
	constexpr TypeId ushortID = getTypeId<unsigned short>();
	constexpr TypeId intID = getTypeId<int>();
	constexpr TypeId uintID = getTypeId<unsigned int>();
	constexpr TypeId longID = getTypeId<long>();
	constexpr TypeId ulongID = getTypeId<unsigned long>();
	constexpr TypeId longlongID = getTypeId<long long>();
	constexpr TypeId ulonglongID = getTypeId<unsigned long long>();
	constexpr TypeId doubleID = getTypeId<double>();
	constexpr TypeId constcharID = getTypeId<const char*>();
	constexpr TypeId stdstringID = getTypeId<std::string>();
	const TypeId     typeID = type.typeID;

	if (typeID == boolID) {
		*static_cast<bool*>(data) = *static_cast<const bool*>(srcData);
	}
	else if (typeID == charID) {
		*static_cast<char*>(data) = *static_cast<const char*>(srcData);
	}
	else if (typeID == ucharID) {
		*static_cast<unsigned char*>(data) = *static_cast<const unsigned char*>(srcData);
	}
	else if (typeID == shortID) {
		*static_cast<short*>(data) = *static_cast<const short*>(srcData);
	}
	else if (typeID == ushortID) {
		*static_cast<unsigned short*>(data) = *static_cast<const unsigned short*>(srcData);
	}
	else if (typeID == intID) {
		*static_cast<int*>(data) = *static_cast<const int*>(srcData);
	}
	else if (typeID == uintID) {
		*static_cast<unsigned int*>(data) = *static_cast<const unsigned int*>(srcData);
	}
	else if (typeID == longID) {
		*static_cast<long*>(data) = *static_cast<const long*>(srcData);
	}
	else if (typeID == ulongID) {
		*static_cast<unsigned long*>(data) = *static_cast<const unsigned long*>(srcData);
	}
	else if (typeID == longlongID) {
		*static_cast<long long*>(data) = *static_cast<const long long*>(srcData);
	}
	else if (typeID == ulonglongID) {
		*static_cast<unsigned long long*>(data) = *static_cast<const unsigned long long*>(srcData);
	}
	else if (typeID == floatID) {
		*static_cast<float*>(data) = *static_cast<const float*>(srcData);
	}
	else if (typeID == doubleID) {
		*static_cast<double*>(data) = *static_cast<const double*>(srcData);
	}
	else if (typeID == constcharID) {
	}
	else if (typeID == stdstringID) {
		*static_cast<std::string*>(data) = *static_cast<const std::string*>(srcData);
	}
	else {
		assert(0);
	}
}

void cloneEnum(void* data, const void* srcData, const EnumType& type) {
	std::memcpy(data, srcData, type.size);
}

void cloneBitMask(void* data, const void* srcData, const BitMaskType& type) {
	std::memcpy(data, srcData, type.size);
}

void cloneContainer(void* dstContainer, const void* srcContainer, const ContainerType& type) {
	const Type* key_type = type.getKeyType();
	const Type* value_type = type.getValueType();

	char tmp[64]; // FIXME alignment for iterators?
	char tmp2[64];

	WriteIterator* const writeIterator = type.newWriteIterator(tmp, sizeof tmp, dstContainer);
	ReadIterator* const  readIterator = type.newReadIterator(tmp2, sizeof tmp2, srcContainer);
	type.clear(dstContainer);

	while (readIterator->isValid()) {
		if (key_type) {
			const void* key = readIterator->getKey();
			const void* srcValue = readIterator->getValue();
			void*       dstValue = writeIterator->insert(key);
			// clone value
			cloneObject(dstValue, srcValue, *value_type);
		}
		else {
			const void* srcValue = readIterator->getValue();
			void*       dstValue = writeIterator->pushBack();
			// clone value
			cloneObject(dstValue, srcValue, *value_type);
		}
		readIterator->gotoNext();
	}

	type.deleteIterator(writeIterator);
	type.deleteIterator(readIterator);
}

void clonePointer(void* data, const void* srcData, const PointerType& type) {
	void*       dstPointer = type.resolvePointer(data);
	const void* srcPointer = type.resolvePointer(srcData);
	if (dstPointer && srcPointer) {
		cloneObject(dstPointer, srcPointer, type.getPointedType());
	}
}

void cloneReference(void* data, const void* srcData, const ReferenceType& type) {
	void*       dstPointer = *static_cast<void**>(data);
	const void* srcPointer = *static_cast<void* const*>(srcData);
	assert(dstPointer); // cannot be null
	assert(srcPointer);
	cloneObject(dstPointer, srcPointer, type.getReferencedType());
}

void cloneVariant(void* dstData, const void* srcData, const TypeDB& typeDB) {
	const Variant* srcVariant = static_cast<const Variant*>(srcData);
	Variant*       dstVariant = static_cast<Variant*>(dstData);
	if (srcVariant->getTypeId() == dstVariant->getTypeId()) {
		const Type& variantDataType = typeDB.getType(srcVariant->getTypeId());
		cloneObject(dstVariant->getStorage(), srcVariant->getStorage(), variantDataType);
	}
}

} // namespace

} // namespace Typhoon::Reflection
