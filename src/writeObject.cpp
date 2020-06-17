#include "writeObject.h"
#include "archive.h"
#include "bitMaskType.h"
#include "builtinType.h"
#include "containerType.h"
#include "enumType.h"
#include "field.h"
#include "flags.h"
#include "pointerType.h"
#include "property.h"
#include "referenceType.h"
#include "structType.h"
#include "type.h"
#include "typeDB.h"
#include "variant.h"
#include <cassert>
#include <core/ptrUtil.h>
#include <core/stackAlloc.h>

namespace Typhoon {

namespace {

bool writeObjectImpl(const void* data, const Type& type, const TypeDB& typeDB, OutputArchive& archive);
bool writeStruct(const void* data, const Type& type, const TypeDB& typeDB, OutputArchive& archive);
bool writeEnum(const void* data, const Type& type, const TypeDB& typeDB, OutputArchive& archive);
bool writeBitMask(const void* data, const Type& type, const TypeDB& typeDB, OutputArchive& archive);
bool writeContainer(const void* data, const Type& type, const TypeDB& typeDB, OutputArchive& archive);
bool writePointer(const void* data, const Type& type, const TypeDB& typeDB, OutputArchive& archive);
bool writeReference(const void* data, const Type& type, const TypeDB& typeDB, OutputArchive& archive);
bool writeVariant(const void* data, const Type& type, const TypeDB& typeDB, OutputArchive& archive);

using Writer = bool (*)(const void* data, const Type& type, const TypeDB& typeDB, OutputArchive&);
constexpr Writer perClasswriters[] = {
	nullptr, // built-ins use per-type savers
	writeStruct, writeEnum, writeBitMask, writeContainer, writePointer, writeReference, writeVariant,
};

} // namespace

bool writeObject(const void* data, const char* name, const Type& type, OutputArchive& archive) {
	assert(data);
	assert(name);
	TypeDB& typeDB = getTypeDB();

	bool res = false;
	if (archive.beginElement(name)) {
		res = writeObjectImpl(data, type, typeDB, archive);
		archive.endElement();
	}
	return res;
}

namespace {

bool writeObjectImpl(const void* data, const Type& type, const TypeDB& typeDB, OutputArchive& archive) {
	bool res = false;
	if (const CustomWriter customWriter = typeDB.getCustomWriter(&type); customWriter) {
		res = customWriter(data, archive);
	}
	else {
		res = perClasswriters[(int)type.subClass](data, type, typeDB, archive);
	}
	return res;
}

bool writeStruct(const void* data, const Type& type, const TypeDB& typeDB, OutputArchive& archive) {
	char        stackMemory[1024];
	StackBuffer stackBuffer { stackMemory };

	const StructType& structTypeInfo = static_cast<const StructType&>(type);
	for (const auto& field : structTypeInfo.getFields()) {
		if (field.flags & Flags::writeable) {
			// Each field knows its offset so add that to the base address of the
			// object being saved to get at the individual field data
			const void* const field_data = advancePointer(data, field.offset);
			writeObject(field_data, field.name, *field.type, archive);
		}
	}

	for (const auto& property : structTypeInfo.getProperties()) {
		if (property->getFlags() & Flags::writeable) {
			const Type& valueType = property->getValueType();

			// Allocate a temporary for the value
			void* const temporary = stackAlloc(stackBuffer, valueType.getSize(), valueType.getAlignment());
			if (temporary) {
				valueType.constructObject(temporary);
				const void* const self = data;
				property->getValue(self, temporary);
				writeObject(temporary, property->getName(), valueType, archive);
				valueType.destructObject(temporary);
			}
			stackFree(stackBuffer, temporary);
		}
	}

	const StructType* const parentType = structTypeInfo.getParentType();
	if (parentType) {
		writeStruct(data, *parentType, typeDB, archive);
	}
	return true;
}

bool writeEnum(const void* data, const Type& type, const TypeDB& /*typeDB*/, OutputArchive& archive) {
	const EnumType&   enumTypeInfo = static_cast<const EnumType&>(type);
	const Enumerator* enumerator = enumTypeInfo.findEnumeratorByValue(data, enumTypeInfo.size);
	bool              res = false;
	if (enumerator) {
		res = archive.write(enumerator->name);
	}
	return res;
}

bool writeBitMask(const void* data, const Type& type, const TypeDB& /*typeDB*/, OutputArchive& archive) {
	const BitMaskType& bitMaskTypeInfo = static_cast<const BitMaskType&>(type);
	// Cast the source type to an uint64_t
	BitMaskStorageType bitMask = 0;
	std::memcpy(&bitMask, data, type.size);

	char str[1024];
	str[0] = 0;
	int  length = 0;
	bool res = true;

	for (const BitMaskConstant& enumerator : bitMaskTypeInfo.getEnumerators()) {
		if ((bitMask & enumerator.mask) == enumerator.mask) {
			// Append enumerator name
			const int r = sprintf_s(str + length, std::size(str) - length, "%s|", enumerator.name);
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

bool writeContainer(const void* data, const Type& type, const TypeDB& typeDB, OutputArchive& archive) {
	char                 tmp[64];
	const ContainerType& containerTypeInfo = static_cast<const ContainerType&>(type);
	const Type*          keyType = containerTypeInfo.getKeyType();
	const Type*          valueType = containerTypeInfo.getValueType();
	constexpr TypeId     variant_typeID = getTypeId<Variant>();

	ReadIterator* iterator = containerTypeInfo.newReadIterator(tmp, sizeof(tmp), data);
	while (iterator->isValid()) {
		const char* elementName = "element";
		if (valueType->typeID == variant_typeID) {
			// a variant is already written inside an element that has the var name
			elementName = nullptr;
		}

		if (keyType) {
			// write key and value
			// TODO broken, not generic enough, doesn't work with vars etc.
			if (archive.beginElement(elementName)) {
				writeObject(iterator->getKey(), "key", *keyType, archive);
				writeObject(iterator->getValue(), "value", *valueType, archive);
				archive.endElement();
			}
		}
		else {
			// Write value
			if (elementName) {
				writeObject(iterator->getValue(), elementName, *valueType, archive);
			}
			else {
				writeObjectImpl(iterator->getValue(), *valueType, typeDB, archive);
			}
		}

		iterator->gotoNext();
	}

	containerTypeInfo.deleteIterator(iterator);
	return true;
}

bool writePointer(const void* data, const Type& type, const TypeDB& typeDB, OutputArchive& archive) {
	const PointerType& pointerTypeInfo = static_cast<const PointerType&>(type);
	const void*        pointer = pointerTypeInfo.resolvePointer(data);
	bool               res = true;
	if (pointer) {
		res = writeObjectImpl(pointer, pointerTypeInfo.getPointedType(), typeDB, archive);
	}
	return res;
}

bool writeReference(const void* data, const Type& type, const TypeDB& typeDB, OutputArchive& archive) {
	const ReferenceType& referenceType = static_cast<const ReferenceType&>(type);
	const void*          pointerToData = *static_cast<void* const*>(data);
	assert(pointerToData); // cannot have a null reference
	return writeObjectImpl(pointerToData, referenceType.getReferencedType(), typeDB, archive);
}

bool writeVariant(const void* data, const Type& /*type*/, const TypeDB& typeDB, OutputArchive& archive) {
	const Variant* variant = static_cast<const Variant*>(data);
	const Type&    type = typeDB.getTypeInfo(variant->getTypeId());
	return writeObject(variant->getStorage(), variant->getName(), type, archive);
}

} // namespace

} // namespace Typhoon
