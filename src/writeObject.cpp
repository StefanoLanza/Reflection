#include "writeObject.h"
#include "archive.h"
#include "bitMaskType.h"
#include "builtinType.h"
#include "containerType.h"
#include "context.h"
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

void writeObjectImpl(ConstDataPtr data, const Type& type, const TypeDB& typeDB, OutputArchive& archive, LinearAllocator& tempAllocator);
void writeStruct(ConstDataPtr data, const Type& type, const TypeDB& typeDB, OutputArchive& archive, LinearAllocator& tempAllocator);
void writeEnum(ConstDataPtr data, const Type& type, const TypeDB& typeDB, OutputArchive& archive, LinearAllocator& tempAllocator);
void writeBitMask(ConstDataPtr data, const Type& type, const TypeDB& typeDB, OutputArchive& archive, LinearAllocator& tempAllocator);
void writeContainer(ConstDataPtr data, const Type& type, const TypeDB& typeDB, OutputArchive& archive, LinearAllocator& tempAllocator);
void writePointer(ConstDataPtr data, const Type& type, const TypeDB& typeDB, OutputArchive& archive, LinearAllocator& tempAllocator);
void writeReference(ConstDataPtr data, const Type& type, const TypeDB& typeDB, OutputArchive& archive, LinearAllocator& tempAllocator);
void writeVariant(ConstDataPtr data, const Type& type, const TypeDB& typeDB, OutputArchive& archive, LinearAllocator& tempAllocator);

using Writer = void (*)(ConstDataPtr data, const Type& type, const TypeDB& typeDB, OutputArchive&, LinearAllocator&);
constexpr Writer perClassWriters[] = {
	nullptr, // built-ins use per-type savers
	writeStruct, writeEnum, writeBitMask, writeContainer, writePointer, writeReference, writeVariant,
};

} // namespace

namespace detail {

void writeData(ConstDataPtr data, const Type& type, OutputArchive& archive, const Context& context) {
	assert(data);
	writeObjectImpl(data, type, *context.typeDB, archive, *context.pagedAllocator);
}

} // namespace detail

namespace {

void writeObjectImpl(ConstDataPtr data, const Type& type, const TypeDB& typeDB, OutputArchive& archive, LinearAllocator& tempAllocator) {
	if (const CustomWriter& customWriter = type.getCustomWriter(); customWriter) {
		customWriter(data, archive);
	}
	else {
		perClassWriters[(int)type.getSubClass()](data, type, typeDB, archive, tempAllocator);
	}
}

void writeStructProperties(ConstDataPtr data, const StructType& structType, const TypeDB& typeDB, OutputArchive& archive,
                           LinearAllocator& tempAllocator) {
	ConstDataPtr const self = data;
	for (const auto& property : structType.getProperties()) {
		if (property.getFlags() & Flags::writeable) {
			const Type& valueType = property.getValueType();
			void*       allocOffs = tempAllocator.getOffset();
			// Allocate a temporary for the value
			if (void* temporary = tempAllocator.alloc(valueType.getSize(), valueType.getAlignment()); temporary) {
				valueType.constructObject(temporary);
				property.getValue(self, temporary);
				archive.setKey(property.getName());
				writeObjectImpl(temporary, valueType, typeDB, archive, tempAllocator);
				valueType.destructObject(temporary);
			}
			tempAllocator.rewind(allocOffs);
		}
	}
}

void writeStruct(ConstDataPtr data, const Type& type, const TypeDB& typeDB, OutputArchive& archive, LinearAllocator& tempAllocator) {
	archive.beginObject();
	const StructType* structType = static_cast<const StructType*>(&type);
	do {
		writeStructProperties(data, *structType, typeDB, archive, tempAllocator);
		structType = structType->getParentType();
	} while (structType);
	archive.endObject();
}

void writeEnum(ConstDataPtr data, const Type& type, const TypeDB& /*typeDB*/, OutputArchive& archive, LinearAllocator& /*tempAllocator*/) {
	const EnumType& enumType = static_cast<const EnumType&>(type);
	if (const Enumerator* enumerator = enumType.findEnumeratorByValue(data, enumType.getSize()); enumerator) {
		archive.write(enumerator->name);
	}
}

void writeBitMask(ConstDataPtr data, const Type& type, const TypeDB& /*typeDB*/, OutputArchive& archive, LinearAllocator& /*tempAllocator*/) {
	const BitMaskType& bitMaskType = static_cast<const BitMaskType&>(type);
	// Cast the source type to an uint64_t
	BitMaskStorageType bitMask = 0;
	std::memcpy(&bitMask, data, type.getSize());

	char str[1024];
	str[0] = 0;
	int  length = 0;
	bool res = true;

	bool first = true;
	for (const BitMaskConstant& enumerator : bitMaskType.getEnumerators()) {
		if ((bitMask & enumerator.mask) == enumerator.mask) {
			// Append enumerator name
			const int r = snprintf(str + length, std::size(str) - length, first ? "%s" : "|%s", enumerator.name);
			if (r < 0) {
				res = false;
				break;
			}
			length += r;
		}
		first = false;
	}
	if (res) {
		archive.write(static_cast<const char*>(str));
	}
}

void writeContainer(ConstDataPtr data, const Type& type, const TypeDB& typeDB, OutputArchive& archive, LinearAllocator& tempAllocator) {
	const ContainerType& containerType = static_cast<const ContainerType&>(type);
	const Type*          keyType = containerType.getKeyType();
	const Type*          valueType = containerType.getValueType();

	archive.beginArray();

	ScopedAllocator scopedAllocator { tempAllocator };
	for (ReadIterator* iterator = containerType.newReadIterator(data, scopedAllocator); iterator->isValid();) {
		if (keyType) {
			// write key and value
			archive.beginObject();
			archive.setKey("key");
			writeObjectImpl(iterator->getKey(), *keyType, typeDB, archive, tempAllocator);
			archive.setKey("value");
			writeObjectImpl(iterator->getValue(), *valueType, typeDB, archive, tempAllocator);
			archive.endObject();
		}
		else {
			writeObjectImpl(iterator->getValue(), *valueType, typeDB, archive, tempAllocator);
		}
		iterator->gotoNext();
	}

	archive.endArray();
}

void writePointer(ConstDataPtr data, const Type& type, const TypeDB& typeDB, OutputArchive& archive, LinearAllocator& tempAllocator) {
	const PointerType& pointerType = static_cast<const PointerType&>(type);
	if (ConstDataPtr pointer = pointerType.resolvePointer(data); pointer) {
		writeObjectImpl(pointer, pointerType.getPointedType(), typeDB, archive, tempAllocator);
	}
}

void writeReference(ConstDataPtr data, const Type& type, const TypeDB& typeDB, OutputArchive& archive, LinearAllocator& tempAllocator) {
	const ReferenceType& referenceType = static_cast<const ReferenceType&>(type);
	ConstDataPtr         pointerToData = referenceType.resolvePointer(data);
	assert(pointerToData); // cannot have a null reference
	writeObjectImpl(pointerToData, referenceType.getReferencedType(), typeDB, archive, tempAllocator);
}

void writeVariant(ConstDataPtr data, const Type& /*type*/, const TypeDB& typeDB, OutputArchive& archive, LinearAllocator& tempAllocator) {
	const Variant* variant = cast<Variant>(data);
	const Type&    realType = typeDB.getType(variant->getTypeId());
	const char*    typeName = variant->getType().getName();
	if (! typeName) {
		return;
	}
	archive.beginObject();
	archive.write("type", typeName);
	archive.write("name", variant->getName());
	archive.setKey("value");
	writeObjectImpl(variant->getStorage(), realType, typeDB, archive, tempAllocator);
	archive.endObject();
}

} // namespace

} // namespace Typhoon::Reflection
