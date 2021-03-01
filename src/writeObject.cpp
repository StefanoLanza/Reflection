#include "writeObject.h"
#include "archive.h"
#include "bitMaskType.h"
#include "builtinType.h"
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

bool writeObjectImpl(ConstDataPtr data, const Type& type, const TypeDB& typeDB, OutputArchive& archive, LinearAllocator& tempAllocator);
bool writeStruct(ConstDataPtr data, const Type& type, const TypeDB& typeDB, OutputArchive& archive, LinearAllocator& tempAllocator);
bool writeEnum(ConstDataPtr data, const Type& type, const TypeDB& typeDB, OutputArchive& archive, LinearAllocator& tempAllocator);
bool writeBitMask(ConstDataPtr data, const Type& type, const TypeDB& typeDB, OutputArchive& archive, LinearAllocator& tempAllocator);
bool writeContainer(ConstDataPtr data, const Type& type, const TypeDB& typeDB, OutputArchive& archive, LinearAllocator& tempAllocator);
bool writePointer(ConstDataPtr data, const Type& type, const TypeDB& typeDB, OutputArchive& archive, LinearAllocator& tempAllocator);
bool writeReference(ConstDataPtr data, const Type& type, const TypeDB& typeDB, OutputArchive& archive, LinearAllocator& tempAllocator);
bool writeVariant(ConstDataPtr data, const Type& type, const TypeDB& typeDB, OutputArchive& archive, LinearAllocator& tempAllocator);

using Writer = bool (*)(ConstDataPtr data, const Type& type, const TypeDB& typeDB, OutputArchive&, LinearAllocator&);
constexpr Writer perClasswriters[] = {
	nullptr, // built-ins use per-type savers
	writeStruct, writeEnum, writeBitMask, writeContainer, writePointer, writeReference, writeVariant,
};

} // namespace

bool writeObject(ConstDataPtr data, const char* name, const Type& type, OutputArchive& archive) {
	assert(data);
	assert(name);
	TypeDB& typeDB = *detail::getContext().typeDB;

	bool res = false;
	if (archive.beginElement(name)) {
		res = writeObjectImpl(data, type, typeDB, archive, *detail::getContext().pagedAllocator);
		archive.endElement();
	}
	return res;
}

namespace {

bool writeObjectImpl(ConstDataPtr data, const Type& type, const TypeDB& typeDB, OutputArchive& archive, LinearAllocator& tempAllocator) {
	bool res = false;
	if (const CustomWriter& customWriter = type.getCustomWriter(); customWriter) {
		res = customWriter(data, archive);
	}
	else {
		res = perClasswriters[(int)type.getSubClass()](data, type, typeDB, archive, tempAllocator);
	}
	return res;
}

void writeStructProperties(ConstDataPtr data, const StructType& structType, OutputArchive& archive, LinearAllocator& tempAllocator) {
	for (const auto& property : structType.getProperties()) {
		if (property.getFlags() & Flags::writeable) {
			const Type& valueType = property.getValueType();
			// Allocate a temporary for the value
			void*       allocOffs = tempAllocator.getOffset();
			void* const temporary = tempAllocator.alloc(valueType.getSize(), valueType.getAlignment());
			if (temporary) {
				valueType.constructObject(temporary);
				ConstDataPtr const self = data;
				property.getValue(self, temporary);
				writeObject(temporary, property.getName(), valueType, archive);
				valueType.destructObject(temporary);
				tempAllocator.rewind(allocOffs);
			}
		}
	}
}

bool writeStruct(ConstDataPtr data, const Type& type, [[maybe_unused]] const TypeDB& typeDB, OutputArchive& archive, LinearAllocator& tempAllocator) {
	if (archive.beginObject()) {
		const StructType* structType = static_cast<const StructType*>(&type);
		do {
			writeStructProperties(data, *structType, archive, tempAllocator);
			structType = structType->getParentType();
		} while (structType);
		archive.endObject();
		return true;
	}
	return false;
}

bool writeEnum(ConstDataPtr data, const Type& type, const TypeDB& /*typeDB*/, OutputArchive& archive, LinearAllocator& /*tempAllocator*/) {
	const EnumType& enumType = static_cast<const EnumType&>(type);
	if (const Enumerator* enumerator = enumType.findEnumeratorByValue(data, enumType.getSize()); enumerator) {
		archive.write(enumerator->name);
		return true;
	}
	return false;
}

bool writeBitMask(ConstDataPtr data, const Type& type, const TypeDB& /*typeDB*/, OutputArchive& archive, LinearAllocator& /*tempAllocator*/) {
	const BitMaskType& bitMaskType = static_cast<const BitMaskType&>(type);
	// Cast the source type to an uint64_t
	BitMaskStorageType bitMask = 0;
	std::memcpy(&bitMask, data, type.getSize());

	char str[1024];
	str[0] = 0;
	int  length = 0;
	bool res = true;

	for (const BitMaskConstant& enumerator : bitMaskType.getEnumerators()) {
		if ((bitMask & enumerator.mask) == enumerator.mask) {
			// Append enumerator name
			const int r = snprintf(str + length, std::size(str) - length, "%s|", enumerator.name);
			if (r < 0) {
				res = false;
				break;
			}
			length += r;
		}
	}
	if (res) {
		archive.write(str);
	}
	return res;
}

bool writeContainer(ConstDataPtr data, const Type& type, const TypeDB& typeDB, OutputArchive& archive, LinearAllocator& tempAllocator) {
	const ContainerType& containerType = static_cast<const ContainerType&>(type);
	const Type*          keyType = containerType.getKeyType();
	const Type*          valueType = containerType.getValueType();

	if (! archive.beginArray()) {
		return false;
	}

	ScopedAllocator scopedAllocator(tempAllocator);
	ReadIterator*   iterator = containerType.newReadIterator(data, scopedAllocator);
	while (iterator->isValid()) {
		if (keyType) {
			// write key and value
			if (archive.beginObject()) {
				writeObject(iterator->getKey(), "key", *keyType, archive);
				writeObject(iterator->getValue(), "value", *valueType, archive);
				archive.endObject();
			}
		}
		else {
			writeObjectImpl(iterator->getValue(), *valueType, typeDB, archive, tempAllocator);
		}

		iterator->gotoNext();
	}

	archive.endArray();
	return true;
}

bool writePointer(ConstDataPtr data, const Type& type, const TypeDB& typeDB, OutputArchive& archive, LinearAllocator& tempAllocator) {
	const PointerType& pointerType = static_cast<const PointerType&>(type);
	ConstDataPtr       pointer = pointerType.resolvePointer(data);
	bool               res = true;
	if (pointer) {
		res = writeObjectImpl(pointer, pointerType.getPointedType(), typeDB, archive, tempAllocator);
	}
	return res;
}

bool writeReference(ConstDataPtr data, const Type& type, const TypeDB& typeDB, OutputArchive& archive, LinearAllocator& tempAllocator) {
	const ReferenceType& referenceType = static_cast<const ReferenceType&>(type);
	ConstDataPtr         pointerToData = referenceType.resolvePointer(data);
	assert(pointerToData); // cannot have a null reference
	return writeObjectImpl(pointerToData, referenceType.getReferencedType(), typeDB, archive, tempAllocator);
}

bool writeVariant(ConstDataPtr data, const Type& /*type*/, const TypeDB& typeDB, OutputArchive& archive, LinearAllocator& tempAllocator) {
	const Variant* variant = castPointer<Variant>(data);
	const Type&    type = typeDB.getType(variant->getTypeId());
	const char*    typeName = variant->getType().getName();
	if (! typeName) {
		return false;
	}
	archive.beginObject();
	archive.writeString("type", typeName);
	archive.writeString("name", variant->getName());
	archive.beginElement("value");
	writeObjectImpl(variant->getStorage(), type, typeDB, archive, tempAllocator);
	archive.endElement();
	archive.endObject();
	return true;
}

} // namespace

} // namespace Typhoon::Reflection
