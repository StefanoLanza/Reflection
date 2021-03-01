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

namespace detail {

Context& getContext();

} // namespace detail

namespace {

bool readObject(DataPtr data, const char* name, const Type& type, Semantic semantic, const TypeDB& typeDB, InputArchive& archive,
                LinearAllocator& tempAllocator);
bool readObject(DataPtr data, const Type& type, Semantic semantic, const TypeDB& typeDB, InputArchive& archive, LinearAllocator& tempAllocator);
// Readers
bool readStruct(DataPtr data, const Type& type, Semantic semantic, const TypeDB& typeDB, InputArchive& archive, LinearAllocator& tempAllocator);
bool readEnum(DataPtr data, const Type& type, Semantic semantic, const TypeDB& typeDB, InputArchive& archive, LinearAllocator& tempAllocator);
bool readBitMask(DataPtr data, const Type& type, Semantic semantic, const TypeDB& typeDB, InputArchive& archive, LinearAllocator& tempAllocator);
bool readContainer(DataPtr data, const Type& type, Semantic semantic, const TypeDB& typeDB, InputArchive& archive, LinearAllocator& tempAllocator);
bool readPointer(DataPtr data, const Type& type, Semantic semantic, const TypeDB& typeDB, InputArchive& archive, LinearAllocator& tempAllocator);
bool readReference(DataPtr data, const Type& type, Semantic semantic, const TypeDB& typeDB, InputArchive& archive, LinearAllocator& tempAllocator);
bool readVariant(DataPtr data, const Type& type, Semantic semantic, const TypeDB& typeDB, InputArchive& archive, LinearAllocator& tempAllocator);

using Reader = bool (*)(DataPtr data, const Type& type, Semantic semantic, const TypeDB& typeDB, InputArchive&, LinearAllocator&);
constexpr Reader perClassReaders[] = {
	nullptr, // builtins use per-type readers
	readStruct, readEnum, readBitMask, readContainer, readPointer, readReference, readVariant,
};

} // namespace

bool readObject(DataPtr object, TypeId typeId, const char* name, InputArchive& archive, Semantic semantic) {
	assert(object);
	assert(name);
	bool res = false;
	if (archive.beginElement(name)) {
		res = readObject(object, typeId, archive, semantic);
		archive.endElement();
	}
	return res;
}

bool readObject(DataPtr object, TypeId typeId, InputArchive& archive, Semantic semantic) {
	assert(object);
	bool        res = false;
	const Type* type = detail::getTypeDB().tryGetType(typeId);
	if (type) {
		res = readObject(object, *type, semantic, detail::getTypeDB(), archive, *detail::getContext().pagedAllocator);
	}
	return res;
}

std::pair<bool, size_t> readArray(DataPtr array, size_t arraySize, TypeId elementTypeId, const char* arrayName, InputArchive& archive) {
	const auto& typeDB = detail::getTypeDB();
	const Type* elementType = typeDB.tryGetType(elementTypeId);
	if (! elementType) {
		return { false, 0 };
	}
	bool    res = false;
	size_t  count = 0;
	DataPtr destPtr = array;
	if (archive.beginElement(arrayName)) {
		res = true;
		ArchiveIterator iter;
		while (archive.iterateChild(iter)) {
			if (count < arraySize) {
				readObject(destPtr, *elementType, Semantic::none, typeDB, archive, *detail::getContext().pagedAllocator);
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

bool readContainer(DataPtr container, const char* containerName, const ContainerType& type, InputArchive& archive) {
	bool res = false;
	if (archive.beginElement(containerName)) {
		res = readContainer(container, type, Semantic::none, detail::getTypeDB(), archive, *detail::getContext().pagedAllocator);
		archive.endElement();
	}
	return res;
}

namespace {

bool readObject(DataPtr data, const char* name, const Type& type, Semantic semantic, const TypeDB& typeDB, InputArchive& archive,
                LinearAllocator& tempAllocator) {
	bool res = false;
	if (archive.beginElement(name)) {
		res = readObject(data, type, semantic, typeDB, archive, tempAllocator);
		archive.endElement();
	}
	return res;
}

bool readObject(DataPtr data, const Type& type, Semantic semantic, const TypeDB& typeDB, InputArchive& archive, LinearAllocator& tempAllocator) {
	bool res = false;
	if (const CustomReader& customReader = type.getCustomReader(); customReader) {
		customReader(data, archive);
		res = true; // TODO error code ?
	}
	else {
		res = perClassReaders[(int)type.getSubClass()](data, type, semantic, typeDB, archive, tempAllocator);
	}
	return res;
}

void readStructProperties(DataPtr data, const StructType& type, const TypeDB& typeDB, InputArchive& archive, LinearAllocator& tempAllocator) {
	for (const auto& property : type.getProperties()) {
		if (property.getFlags() & Flags::readable) {
			if (archive.beginElement(property.getName())) {
				const Type& valueType = property.getValueType();

				ScopedAllocator scopedAllocator(tempAllocator);
				// Allocate a temporary for the value
				void* const temporary = scopedAllocator.alloc(valueType.getSize(), valueType.getAlignment());
				if (temporary) {
					valueType.constructObject(temporary);
					const DataPtr self = data;
					// First set temporary value using getter as readObject might fail or partly fill the data
					property.getValue(self, temporary);
					readObject(temporary, valueType, property.getSemantic(), typeDB, archive, tempAllocator);
					property.setValue(self, temporary);
					valueType.destructObject(temporary);
				}
				archive.endElement();
			}
		}
	}
}

bool readStruct(DataPtr data, const Type& type, [[maybe_unused]] Semantic semantic, const TypeDB& typeDB, InputArchive& archive,
                LinearAllocator& tempAllocator) {
	const StructType* structType = static_cast<const StructType*>(&type);
	do {
		readStructProperties(data, *structType, typeDB, archive, tempAllocator);
		structType = structType->getParentType();
	} while (structType);
	return true;
}

bool readEnum(DataPtr dstData, const Type& type, Semantic /*semantic*/, const TypeDB& /*typeDB*/, InputArchive& archive,
              LinearAllocator& /*tempAllocator*/) {
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

bool readBitMask(DataPtr dstData, const Type& type, Semantic /*semantic*/, const TypeDB& /*typeDB*/, InputArchive& archive,
                 LinearAllocator& /*tempAllocator*/) {
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

bool readContainer(DataPtr data, const Type& type, Semantic semantic, const TypeDB& typeDB, InputArchive& archive, LinearAllocator& tempAllocator) {
	const ContainerType& containerType = static_cast<const ContainerType&>(type);
	const Type*          key_type = containerType.getKeyType();
	const Type*          value_type = containerType.getValueType();

	containerType.clear(data);

	ScopedAllocator      outerScopedAllocator(tempAllocator);
	WriteIterator* const containerIterator = containerType.newWriteIterator(data, outerScopedAllocator);
	ArchiveIterator      archiveIterator;
	while (archive.iterateChild(archiveIterator)) {
		if (containerIterator->isValid()) {
			if (key_type) {
				// Construct a temporary for the key
				void* const key = tempAllocator.alloc(key_type->getSize(), key_type->getAlignment());
				assert(key);
				key_type->constructObject(key);
				// read key
				if (readObject(key, "key", *key_type, Semantic::none, typeDB, archive, tempAllocator)) {
					// Create value using key
					void* value = containerIterator->insert(key);
					readObject(value, "value", *value_type, semantic, typeDB, archive, tempAllocator);
					// TODO insert on success only ?
				}
				// Destruct key
				key_type->destructObject(key);
				tempAllocator.rewind(key);
			}
			else {
				// TODO insert on success only with a move
				readObject(containerIterator->pushBack(), *value_type, semantic, typeDB, archive, tempAllocator);
			}
		}
		else {
			// Out of bounds error
		}
	}

	return true;
}

bool readPointer(DataPtr data, const Type& type, Semantic semantic, const TypeDB& typeDB, InputArchive& archive, LinearAllocator& tempAllocator) {
	const PointerType& pointerType = static_cast<const PointerType&>(type);
	if (const DataPtr pointer = pointerType.resolvePointer(data); pointer) {
		return readObject(pointer, pointerType.getPointedType(), semantic, typeDB, archive, tempAllocator);
	}
	return false;
}

bool readReference(DataPtr data, const Type& type, Semantic semantic, const TypeDB& typeDB, InputArchive& archive, LinearAllocator& tempAllocator) {
	const ReferenceType& referenceType = static_cast<const ReferenceType&>(type);
	return readObject(referenceType.resolvePointer(data), referenceType.getReferencedType(), semantic, typeDB, archive, tempAllocator);
}

bool readVariant(DataPtr data, const Type& /*type*/, Semantic semantic, const TypeDB& typeDB, InputArchive& archive, LinearAllocator& tempAllocator) {
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

		Variant* variant = castPointer<Variant>(data);
		*variant = Variant(type->getTypeId(), name);
		res = readObject(variant->getStorage(), "value", *type, semantic, typeDB, archive, tempAllocator);

		archive.endObject();
	}
	return res;
}

} // namespace

} // namespace Typhoon::Reflection
