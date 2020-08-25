#include "readObject.h"
#include "archive.h"
#include "bitMaskType.h"
#include "builtinType.h"
#include "config.h"
#include "containerType.h"
#include "enumType.h"
#include "flags.h"
#include "pointerType.h"
#include "property.h"
#include "referenceType.h"
#include "serializeBuiltIns.h"
#include "structType.h"
#include "type.h"
#include "typeDB.h"
#include "variant.h"
#include <cassert>
#include <core/ptrUtil.h>
#include <core/stackAlloc.h>

namespace Typhoon::Reflection {

namespace {

bool readObject(void* data, const char* name, const Type& type, Semantic semantic, const TypeDB& typeDB, InputArchive& archive);
bool readObject(void* data, const Type& type, Semantic semantic, const TypeDB& typeDB, InputArchive& archive);
bool readStruct(void* data, const Type& type, Semantic semantic, const TypeDB& typeDB, InputArchive& archive);
bool readEnum(void* data, const Type& type, Semantic semantic, const TypeDB& typeDB, InputArchive& archive);
bool readBitMask(void* data, const Type& type, Semantic semantic, const TypeDB& typeDB, InputArchive& archive);
bool readContainer(void* data, const Type& type, Semantic semantic, const TypeDB& typeDB, InputArchive& archive);
bool readPointer(void* data, const Type& type, Semantic semantic, const TypeDB& typeDB, InputArchive& archive);
bool readReference(void* data, const Type& type, Semantic semantic, const TypeDB& typeDB, InputArchive& archive);
bool readVariant(void* data, const Type& type, Semantic semantic, const TypeDB& typeDB, InputArchive& archive);

using Reader = bool (*)(void* data, const Type& type, Semantic semantic, const TypeDB& typeDB, InputArchive&);
constexpr Reader perClassReaders[] = {
	nullptr, // builtins use per-type readers
	readStruct, readEnum, readBitMask, readContainer, readPointer, readReference, readVariant,
};

} // namespace

bool readObject(void* object, TypeId typeId, const char* name, InputArchive& archive, Semantic semantic) {
	assert(object);
	assert(name);
	bool        res = false;
	const Type* type = detail::getTypeDB().tryGetType(typeId);
	if (type) {
		res = readObject(object, name, *type, semantic, detail::getTypeDB(), archive);
	}
	return res;
}

bool readObject(void* object, TypeId typeId, InputArchive& archive, Semantic semantic) {
	assert(object);
	bool        res = false;
	const Type* type = detail::getTypeDB().tryGetType(typeId);
	if (type) {
		res = readObject(object, *type, semantic, detail::getTypeDB(), archive);
	}
	return res;
}

std::pair<bool, size_t> readArray(void* array, size_t arraySize, TypeId elementTypeId, const char* arrayName, InputArchive& archive) {
	const auto& typeDB = detail::getTypeDB();
	const Type* elementType = typeDB.tryGetType(elementTypeId);
	if (! elementType) {
		return { false, 0 };
	}
	bool   res = false;
	size_t count = 0;
	void*  destPtr = array;
	if (archive.beginElement(arrayName)) {
		res = true;
		ArchiveIterator iter;
		while (archive.iterateChild(iter)) {
			if (count < arraySize) {
				readObject(destPtr, *elementType, Semantic::none, typeDB, archive);
			}
			else {
				res = false;
				break;
			}
			destPtr = advancePointer(destPtr, elementType->size);
			++count;
		}
		archive.endElement();
	}
	return { res, count };
}

bool readContainer(void* container, const char* containerName, const ContainerType& type, InputArchive& archive) {
	bool res = false;
	if (archive.beginElement(containerName)) {
		res = readContainer(container, type, Semantic::none, detail::getTypeDB(), archive);
		archive.endElement();
	}
	return res;
}

namespace {

bool readObject(void* data, const char* name, const Type& type, Semantic semantic, const TypeDB& typeDB, InputArchive& archive) {
	bool res = false;
	if (archive.beginElement(name)) {
		res = readObject(data, type, semantic, typeDB, archive);
		archive.endElement();
	}
	return res;
}

bool readObject(void* data, const Type& type, Semantic semantic, const TypeDB& typeDB, InputArchive& archive) {
	bool res = false;
	if (const CustomReader& customReader = type.getCustomReader(); customReader) {
		customReader(data, archive);
		res = true; // TODO error code ?
	}
	else {
		res = perClassReaders[(int)type.subClass](data, type, semantic, typeDB, archive);
	}
	return res;
}

void readStructProperties(void* data, const StructType& type, const TypeDB& typeDB, InputArchive& archive) {
	char        stackMemory[1024];
	StackBuffer stackBuffer { stackMemory };
	for (const auto& property : type.getProperties()) {
		if (property.getFlags() & Flags::readable) {
			if (archive.beginElement(property.getName())) {
				const Type& valueType = property.getValueType();

				// Allocate a temporary for the value
				void* const temporary = stackAlloc(stackBuffer, valueType.getSize(), valueType.getAlignment());
				if (temporary) {
					valueType.constructObject(temporary);
					void* const self = data;
					// First set temporary value using getter as readObject might fail or partly fill the data
					property.getValue(self, temporary);
					readObject(temporary, valueType, property.getSemantic(), typeDB, archive);
					property.setValue(self, temporary);
					valueType.destructObject(temporary);
				}
				stackFree(stackBuffer, temporary);
				archive.endElement();
			}
		}
	}
}

bool readStruct(void* data, const Type& type, [[maybe_unused]] Semantic semantic, const TypeDB& typeDB, InputArchive& archive) {
	const StructType* structType = static_cast<const StructType*>(&type);
	do {
		readStructProperties(data, *structType, typeDB, archive);
		structType = structType->getParentType();
	} while (structType);
	return true;
}

bool readEnum(void* dstData, const Type& type, Semantic /*semantic*/, const TypeDB& /*typeDB*/, InputArchive& archive) {
	const EnumType& enumType = static_cast<const EnumType&>(type);
	bool            res = false;
	if (const char* name = archive.currNodeText(); name) {
		const Enumerator* constant = enumType.findEnumeratorByName(name);
		if (constant) {
			// Cast the type to an enum and retrieve the value
			std::memcpy(dstData, &constant->value, enumType.size);
			res = true;
		}
	}
	else {
		// Error
	}
	return res;
}

bool readBitMask(void* dstData, const Type& type, Semantic /*semantic*/, const TypeDB& /*typeDB*/, InputArchive& archive) {
	const BitMaskType& bitMaskType = static_cast<const BitMaskType&>(type);
	assert(bitMaskType.size <= sizeof(BitMaskStorageType));
	bool res = false;
	if (const char* maskStr = archive.currNodeText(); maskStr) {
		BitMaskStorageType bitMask = 0;
		for (const BitMaskConstant& enumerator : bitMaskType.getEnumerators()) {
			if (strstr(maskStr, enumerator.name)) {
				bitMask |= enumerator.mask;
			}
		}
		// (Narrow) cast bitMask to the destination data
		std::memcpy(dstData, &bitMask, type.size);
		res = true;
	}
	return res;
}

bool readContainer(void* data, const Type& type, Semantic semantic, const TypeDB& typeDB, InputArchive& archive) {
	const ContainerType& containerType = static_cast<const ContainerType&>(type);
	const Type*          key_type = containerType.getKeyType();
	const Type*          value_type = containerType.getValueType();
	constexpr TypeId     variant_typeID = getTypeId<Variant>();

	char                 tmp[64];
	char                 stackMem[512];
	WriteIterator* const containerIterator = containerType.newWriteIterator(tmp, sizeof(tmp), data);
	ArchiveIterator      archiveIterator;
	while (archive.iterateChild(archiveIterator)) {
		if (containerIterator->isValid()) {
			if (key_type) {
				// Construct a temporary for the key TODO stack allocator
				size_t      space = sizeof(stackMem);
				void*       buffer = stackMem;
				void* const key = std::align(key_type->alignment, key_type->size, buffer, space);
				if (key) {
					key_type->constructObject(key);
					// read key
					readObject(key, "key", *key_type, Semantic::none, typeDB, archive);
					// Create value using key
					void* value = containerIterator->insert(key);
					// Destruct key
					key_type->destructObject(key);
					readObject(value, "value", *value_type, semantic, typeDB, archive);
				}
			}
			else {
				readObject(containerIterator->pushBack(), *value_type, semantic, typeDB, archive);
			}
		}
		else {
			// Out of bounds error
		}
	}

	containerType.deleteIterator(containerIterator);

	return true;
}

bool readPointer(void* data, const Type& type, Semantic semantic, const TypeDB& typeDB, InputArchive& archive) {
	const PointerType& pointerType = static_cast<const PointerType&>(type);
	void* const        pointer = pointerType.resolvePointer(data);
	if (pointer) {
		return readObject(pointer, pointerType.getPointedType(), semantic, typeDB, archive);
	}
	return false;
}

bool readReference(void* data, const Type& type, Semantic semantic, const TypeDB& typeDB, InputArchive& archive) {
	const ReferenceType& referenceType = static_cast<const ReferenceType&>(type);
	return readObject(referenceType.resolvePointer(data), referenceType.getReferencedType(), semantic, typeDB, archive);
}

bool readVariant(void* data, const Type& /*type*/, Semantic semantic, const TypeDB& typeDB, InputArchive& archive) {
	bool     res = false;
	if (archive.beginObject()) {
		const char* typeName = nullptr;
		if (! archive.readString("type", typeName)) {
			archive.endObject();
			return false;
		}
		
		TypeId typeId = typeNameToId(typeName);
		if (! typeId) {
			archive.endObject();
			return false;
		}

		const char* name = nullptr;
		if (! archive.readString("name", name)) {
			archive.endObject();
			return false;
		}

		const Type& type = typeDB.getType(typeId);
		Variant* variant = static_cast<Variant*>(data);
		*variant = Variant(typeId, name);
		res = readObject(variant->getStorage(), "value", type, semantic, typeDB, archive);
	
		archive.endObject();
	}
	return res;
}

} // namespace

} // namespace Typhoon::Reflection
