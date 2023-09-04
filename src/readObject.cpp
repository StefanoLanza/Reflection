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

bool readObjectImpl(const char* key, DataPtr data, const Type& type, Semantic semantic, const TypeDB& typeDB, InputArchive& archive,
                LinearAllocator& tempAllocator);
bool readObjectImpl(DataPtr data, const Type& type, Semantic semantic, const TypeDB& typeDB, const InputArchive& archive, LinearAllocator& tempAllocator);
// Readers
bool readStruct(DataPtr data, const Type& type, Semantic semantic, const TypeDB& typeDB, const InputArchive& archive, LinearAllocator& tempAllocator);
bool readEnum(DataPtr data, const Type& type, Semantic semantic, const TypeDB& typeDB, const InputArchive& archive, LinearAllocator& tempAllocator);
bool readBitMask(DataPtr data, const Type& type, Semantic semantic, const TypeDB& typeDB, const InputArchive& archive, LinearAllocator& tempAllocator);
bool readContainer(DataPtr data, const Type& type, Semantic semantic, const TypeDB& typeDB, const InputArchive& archive, LinearAllocator& tempAllocator);
bool readPointer(DataPtr data, const Type& type, Semantic semantic, const TypeDB& typeDB, const InputArchive& archive, LinearAllocator& tempAllocator);
bool readReference(DataPtr data, const Type& type, Semantic semantic, const TypeDB& typeDB, const InputArchive& archive, LinearAllocator& tempAllocator);
bool readVariant(DataPtr data, const Type& type, Semantic semantic, const TypeDB& typeDB, const InputArchive& archive, LinearAllocator& tempAllocator);

using Reader = bool (*)(DataPtr data, const Type& type, Semantic semantic, const TypeDB& typeDB, const InputArchive&, LinearAllocator&);
constexpr Reader perClassReaders[] = {
	nullptr, // builtins use per-type readers
	readStruct, readEnum, readBitMask, readContainer, readPointer, readReference, readVariant,
};

} // namespace

namespace detail {

bool readData(DataPtr object, const Type& type, const InputArchive& archive, const Context& context, Semantic semantic) {
	assert(object);
	return readObjectImpl(object, type, semantic, *context.typeDB, archive, *context.pagedAllocator);
}

}

std::pair<bool, size_t> readArray(DataPtr array, size_t arraySize, TypeId elementTypeId, const char* arrayName, const InputArchive& archive) {
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
				readObjectImpl(destPtr, *elementType, Semantic::none, typeDB, archive, *detail::getContext().pagedAllocator);
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

bool readObjectImpl(const char* key, DataPtr data, const Type& type, Semantic semantic, const TypeDB& typeDB, const InputArchive& archive,
                LinearAllocator& tempAllocator) {
	bool res = false;
	if (archive.beginElement(key)) {
		res = readObjectImpl(data, type, semantic, typeDB, archive, tempAllocator);
		archive.endElement();
	}
	return res;
}

bool readObjectImpl(DataPtr data, const Type& type, Semantic semantic, const TypeDB& typeDB, const InputArchive& archive, LinearAllocator& tempAllocator) {
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

void readStructProperties(DataPtr data, const StructType& type, const TypeDB& typeDB, const InputArchive& archive, LinearAllocator& tempAllocator) {
	for (const auto& property : type.getProperties()) {
		if (property.getFlags() & Flags::readable) {
			if (archive.beginElement(property.getName())) {
				const Type& valueType = property.getValueType();
				void*       allocOffs = tempAllocator.getOffset();
				// Allocate a temporary for the value
				if (void* temporary = tempAllocator.alloc(valueType.getSize(), valueType.getAlignment()); temporary) {
					valueType.constructObject(temporary);
					const DataPtr self = data;
					// First set temporary value using getter as readObject might fail or partly fill the data
					property.getValue(self, temporary);
					readObjectImpl(temporary, valueType, property.getSemantic(), typeDB, archive, tempAllocator);
					property.setValue(self, temporary);
					valueType.destructObject(temporary);
				}
				tempAllocator.rewind(allocOffs);
				archive.endElement();
			}
		}
	}
}

bool readStruct(DataPtr data, const Type& type, [[maybe_unused]] Semantic semantic, const TypeDB& typeDB, const InputArchive& archive,
                LinearAllocator& tempAllocator) {
	const StructType* structType = static_cast<const StructType*>(&type);
	do {
		readStructProperties(data, *structType, typeDB, archive, tempAllocator);
		structType = structType->getParentType();
	} while (structType);
	return true;
}

bool readEnum(DataPtr dstData, const Type& type, Semantic /*semantic*/, const TypeDB& /*typeDB*/, const InputArchive& archive,
              LinearAllocator& /*tempAllocator*/) {
	const EnumType& enumType = static_cast<const EnumType&>(type);
	bool            res = false;
	if (const char* name = nullptr; archive.read(name)) {
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

bool readBitMask(DataPtr dstData, const Type& type, Semantic /*semantic*/, const TypeDB& /*typeDB*/, const InputArchive& archive,
                 LinearAllocator& /*tempAllocator*/) {
	const BitMaskType& bitMaskType = static_cast<const BitMaskType&>(type);
	assert(bitMaskType.getSize() <= sizeof(BitMaskStorageType));
	bool res = false;
	if (const char* maskStr = nullptr; archive.read(maskStr)) {
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

bool readContainer(DataPtr data, const Type& type, Semantic semantic, const TypeDB& typeDB, const InputArchive& archive, LinearAllocator& tempAllocator) {
	const ContainerType& containerType = static_cast<const ContainerType&>(type);
	const Type*          key_type = containerType.getKeyType();
	const Type*          value_type = containerType.getValueType();

	ScopedAllocator      outerScopedAllocator(tempAllocator);
	WriteIterator* const containerIterator = containerType.newWriteIterator(data, outerScopedAllocator);
	ArchiveIterator      archiveIterator;
	while (archive.iterateChild(archiveIterator)) {
		if (containerIterator->isValid()) {
			if (key_type) {
				// Construct a temporary for the key
				void*       allocOffs = tempAllocator.getOffset();
				void* const key = tempAllocator.alloc(key_type->getSize(), key_type->getAlignment());
				assert(key);
				key_type->constructObject(key);
				// read key
				if (readObjectImpl("key", key, *key_type, Semantic::none, typeDB, archive, tempAllocator)) {
					// Create value using key
					DataPtr value = containerIterator->insert(key);
					readObjectImpl("value", value, *value_type, semantic, typeDB, archive, tempAllocator);
					// TODO insert on success only ?
				}
				// Destruct key
				key_type->destructObject(key);
				tempAllocator.rewind(allocOffs);
			}
			else {
				// TODO insert on success only with a move
				readObjectImpl(containerIterator->pushBack(), *value_type, semantic, typeDB, archive, tempAllocator);
			}
		}
		else {
			// Out of bounds error
		}
	}

	return true;
}

bool readPointer(DataPtr data, const Type& type, Semantic semantic, const TypeDB& typeDB, const InputArchive& archive, LinearAllocator& tempAllocator) {
	const PointerType& pointerType = static_cast<const PointerType&>(type);
	if (const DataPtr pointer = pointerType.resolvePointer(data); pointer) {
		return readObjectImpl(pointer, pointerType.getPointedType(), semantic, typeDB, archive, tempAllocator);
	}
	return false;
}

bool readReference(DataPtr data, const Type& type, Semantic semantic, const TypeDB& typeDB, const InputArchive& archive, LinearAllocator& tempAllocator) {
	const ReferenceType& referenceType = static_cast<const ReferenceType&>(type);
	return readObjectImpl(referenceType.resolvePointer(data), referenceType.getReferencedType(), semantic, typeDB, archive, tempAllocator);
}

bool readVariant(DataPtr data, const Type& /*type*/, Semantic semantic, const TypeDB& typeDB, const InputArchive& archive, LinearAllocator& tempAllocator) {
	bool res = false;
	if (archive.isObject()) {
		const char* typeName = nullptr;
		if (! archive.read("type", typeName)) {
			return false;
		}

		const Type* type = typeDB.tryGetType(typeName);
		if (! type) {
			return false;
		}

		const char* name = nullptr;
		if (! archive.read("name", name)) {
			return false;
		}

		Variant* variant = cast<Variant>(data);
		*variant = Variant(type->getTypeId(), name);
		res = readObjectImpl("value", variant->getStorage(), *type, semantic, typeDB, archive, tempAllocator);
	}
	return res;
}

} // namespace

} // namespace Typhoon::Reflection
