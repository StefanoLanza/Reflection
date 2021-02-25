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
#include <core/scopedAllocator.h>

namespace Typhoon::Reflection {

namespace {

bool readObject(void* data, const char* name, const Type& type, Semantic semantic, const TypeDB& typeDB, InputArchive& archive,
                LinearAllocator& stackAllocator);
bool readObject(void* data, const Type& type, Semantic semantic, const TypeDB& typeDB, InputArchive& archive, LinearAllocator& stackAllocator);
// Readers
bool readStruct(void* data, const Type& type, Semantic semantic, const TypeDB& typeDB, InputArchive& archive, LinearAllocator& stackAllocator);
bool readEnum(void* data, const Type& type, Semantic semantic, const TypeDB& typeDB, InputArchive& archive, LinearAllocator& stackAllocator);
bool readBitMask(void* data, const Type& type, Semantic semantic, const TypeDB& typeDB, InputArchive& archive, LinearAllocator& stackAllocator);
bool readContainer(void* data, const Type& type, Semantic semantic, const TypeDB& typeDB, InputArchive& archive, LinearAllocator& stackAllocator);
bool readPointer(void* data, const Type& type, Semantic semantic, const TypeDB& typeDB, InputArchive& archive, LinearAllocator& stackAllocator);
bool readReference(void* data, const Type& type, Semantic semantic, const TypeDB& typeDB, InputArchive& archive, LinearAllocator& stackAllocator);
bool readVariant(void* data, const Type& type, Semantic semantic, const TypeDB& typeDB, InputArchive& archive, LinearAllocator& stackAllocator);

using Reader = bool (*)(void* data, const Type& type, Semantic semantic, const TypeDB& typeDB, InputArchive&, LinearAllocator&);
constexpr Reader perClassReaders[] = {
	nullptr, // builtins use per-type readers
	readStruct, readEnum, readBitMask, readContainer, readPointer, readReference, readVariant,
};

} // namespace

bool readObject(void* object, TypeId typeId, const char* name, InputArchive& archive, Semantic semantic) {
	assert(object);
	assert(name);
	bool res = false;
	if (archive.beginElement(name)) {
		res = readObject(object, typeId, archive, semantic);
		archive.endElement();
	}
	return res;
}

bool readObject(void* object, TypeId typeId, InputArchive& archive, Semantic semantic) {
	assert(object);
	bool        res = false;
	const Type* type = detail::getTypeDB().tryGetType(typeId);
	if (type) {
		char            stack[Defaults::stackSize];
		LinearAllocator stackAllocator(stack, std::size(stack), nullptr);
		res = readObject(object, *type, semantic, detail::getTypeDB(), archive, stackAllocator);
	}
	return res;
}

std::pair<bool, size_t> readArray(void* array, size_t arraySize, TypeId elementTypeId, const char* arrayName, InputArchive& archive) {
	const auto& typeDB = detail::getTypeDB();
	const Type* elementType = typeDB.tryGetType(elementTypeId);
	if (! elementType) {
		return { false, 0 };
	}
	bool            res = false;
	size_t          count = 0;
	void*           destPtr = array;
	char            stack[Defaults::stackSize];
	LinearAllocator stackAllocator(stack, std::size(stack), nullptr);
	if (archive.beginElement(arrayName)) {
		res = true;
		ArchiveIterator iter;
		while (archive.iterateChild(iter)) {
			if (count < arraySize) {
				readObject(destPtr, *elementType, Semantic::none, typeDB, archive, stackAllocator);
			}
			else {
				res = false;
				break;
			}
			destPtr = advancePointer(destPtr, elementType->getSize());
			++count;
		}
		archive.endElement();
	}
	return { res, count };
}

bool readContainer(void* container, const char* containerName, const ContainerType& type, InputArchive& archive) {
	bool            res = false;
	char            stack[Defaults::stackSize];
	LinearAllocator stackAllocator(stack, std::size(stack), nullptr);
	if (archive.beginElement(containerName)) {
		res = readContainer(container, type, Semantic::none, detail::getTypeDB(), archive, stackAllocator);
		archive.endElement();
	}
	return res;
}

namespace {

bool readObject(void* data, const char* name, const Type& type, Semantic semantic, const TypeDB& typeDB, InputArchive& archive,
                LinearAllocator& stackAllocator) {
	bool res = false;
	if (archive.beginElement(name)) {
		res = readObject(data, type, semantic, typeDB, archive, stackAllocator);
		archive.endElement();
	}
	return res;
}

bool readObject(void* data, const Type& type, Semantic semantic, const TypeDB& typeDB, InputArchive& archive, LinearAllocator& stackAllocator) {
	bool res = false;
	if (const CustomReader& customReader = type.getCustomReader(); customReader) {
		customReader(data, archive);
		res = true; // TODO error code ?
	}
	else {
		res = perClassReaders[(int)type.getSubClass()](data, type, semantic, typeDB, archive, stackAllocator);
	}
	return res;
}

void readStructProperties(void* data, const StructType& type, const TypeDB& typeDB, InputArchive& archive, LinearAllocator& stackAllocator) {
	for (const auto& property : type.getProperties()) {
		if (property.getFlags() & Flags::readable) {
			if (archive.beginElement(property.getName())) {
				const Type& valueType = property.getValueType();

				ScopedAllocator scopedAllocator(stackAllocator);
				// Allocate a temporary for the value
				void* const temporary = scopedAllocator.alloc(valueType.getSize(), valueType.getAlignment());
				if (temporary) {
					valueType.constructObject(temporary);
					void* const self = data;
					// First set temporary value using getter as readObject might fail or partly fill the data
					property.getValue(self, temporary);
					readObject(temporary, valueType, property.getSemantic(), typeDB, archive, stackAllocator);
					property.setValue(self, temporary);
					valueType.destructObject(temporary);
				}
				archive.endElement();
			}
		}
	}
}

bool readStruct(void* data, const Type& type, [[maybe_unused]] Semantic semantic, const TypeDB& typeDB, InputArchive& archive,
                LinearAllocator& stackAllocator) {
	const StructType* structType = static_cast<const StructType*>(&type);
	do {
		readStructProperties(data, *structType, typeDB, archive, stackAllocator);
		structType = structType->getParentType();
	} while (structType);
	return true;
}

bool readEnum(void* dstData, const Type& type, Semantic /*semantic*/, const TypeDB& /*typeDB*/, InputArchive& archive,
              LinearAllocator& /*stackAllocator*/) {
	const EnumType& enumType = static_cast<const EnumType&>(type);
	bool            res = false;
	if (const char* name = nullptr; archive.readString(name)) {
		const Enumerator* constant = enumType.findEnumeratorByName(name);
		if (constant) {
			// Cast the type to an enum and retrieve the value
			std::memcpy(dstData, &constant->value, enumType.getSize());
			res = true;
		}
	}
	else {
		// Error
	}
	return res;
}

bool readBitMask(void* dstData, const Type& type, Semantic /*semantic*/, const TypeDB& /*typeDB*/, InputArchive& archive,
                 LinearAllocator& /*stackAllocator*/) {
	const BitMaskType& bitMaskType = static_cast<const BitMaskType&>(type);
	assert(bitMaskType.getSize() <= sizeof(BitMaskStorageType));
	bool res = false;
	if (const char* maskStr = nullptr; archive.readString(maskStr)) {
		BitMaskStorageType bitMask = 0;
		for (const BitMaskConstant& enumerator : bitMaskType.getEnumerators()) {
			if (strstr(maskStr, enumerator.name)) {
				bitMask |= enumerator.mask;
			}
		}
		// (Narrow) cast bitMask to the destination data
		std::memcpy(dstData, &bitMask, type.getSize());
		res = true;
	}
	return res;
}

bool readContainer(void* data, const Type& type, Semantic semantic, const TypeDB& typeDB, InputArchive& archive, LinearAllocator& stackAllocator) {
	const ContainerType& containerType = static_cast<const ContainerType&>(type);
	const Type*          key_type = containerType.getKeyType();
	const Type*          value_type = containerType.getValueType();

	containerType.clear(data);

	ScopedAllocator      outerScopedAllocator(stackAllocator);
	WriteIterator* const containerIterator = containerType.newWriteIterator(data, outerScopedAllocator);
	ArchiveIterator      archiveIterator;
	while (archive.iterateChild(archiveIterator)) {
		if (containerIterator->isValid()) {
			if (key_type) {
				// Construct a temporary for the key
				ScopedAllocator scopedAllocator(stackAllocator);
				void* const     key = scopedAllocator.alloc(key_type->getSize(), key_type->getAlignment());
				assert(key);
				key_type->constructObject(key);
				// read key
				if (readObject(key, "key", *key_type, Semantic::none, typeDB, archive, stackAllocator)) {
					// Create value using key
					void* value = containerIterator->insert(key);
					readObject(value, "value", *value_type, semantic, typeDB, archive, stackAllocator);
					// TODO insert on success only ?
				}
				// Destruct key
				key_type->destructObject(key);
			}
			else {
				// TODO insert on success only with a move
				readObject(containerIterator->pushBack(), *value_type, semantic, typeDB, archive, stackAllocator);
			}
		}
		else {
			// Out of bounds error
		}
	}

	return true;
}

bool readPointer(void* data, const Type& type, Semantic semantic, const TypeDB& typeDB, InputArchive& archive, LinearAllocator& stackAllocator) {
	const PointerType& pointerType = static_cast<const PointerType&>(type);
	void* const        pointer = pointerType.resolvePointer(data);
	if (pointer) {
		return readObject(pointer, pointerType.getPointedType(), semantic, typeDB, archive, stackAllocator);
	}
	return false;
}

bool readReference(void* data, const Type& type, Semantic semantic, const TypeDB& typeDB, InputArchive& archive, LinearAllocator& stackAllocator) {
	const ReferenceType& referenceType = static_cast<const ReferenceType&>(type);
	return readObject(referenceType.resolvePointer(data), referenceType.getReferencedType(), semantic, typeDB, archive, stackAllocator);
}

bool readVariant(void* data, const Type& /*type*/, Semantic semantic, const TypeDB& typeDB, InputArchive& archive, LinearAllocator& stackAllocator) {
	bool res = false;
	if (archive.beginObject()) {
		const char* typeName = nullptr;
		if (! archive.readString("type", typeName)) {
			archive.endObject();
			return false;
		}

		const Type* type = typeDB.tryGetType(typeName);	
		if (! type) {
			archive.endObject();
			return false;
		}

		const char* name = nullptr;
		if (! archive.readString("name", name)) {
			archive.endObject();
			return false;
		}

		Variant*    variant = static_cast<Variant*>(data);
		*variant = Variant(type->getTypeId(), name);
		res = readObject(variant->getStorage(), "value", *type, semantic, typeDB, archive, stackAllocator);

		archive.endObject();
	}
	return res;
}

} // namespace

} // namespace Typhoon::Reflection
