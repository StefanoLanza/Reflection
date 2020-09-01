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
#include <core/stackAlloc.h>

namespace Typhoon::Reflection {

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
	TypeDB& typeDB = *detail::getContext().typeDB;

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
	if (const CustomWriter& customWriter = type.getCustomWriter(); customWriter) {
		res = customWriter(data, archive);
	}
	else {
		res = perClasswriters[(int)type.subClass](data, type, typeDB, archive);
	}
	return res;
}

void writeStructProperties(const void* data, const StructType& structType, OutputArchive& archive) {
	char        stackMemory[1024]; // FIXME linear allocator
	StackBuffer stackBuffer { stackMemory };
	for (const auto& property : structType.getProperties()) {
		if (property.getFlags() & Flags::writeable) {
			const Type& valueType = property.getValueType();

			// Allocate a temporary for the value
			void* const temporary = stackAlloc(stackBuffer, valueType.getSize(), valueType.getAlignment());
			if (temporary) {
				valueType.constructObject(temporary);
				const void* const self = data;
				property.getValue(self, temporary);
				writeObject(temporary, property.getName(), valueType, archive);
				valueType.destructObject(temporary);
			}
			stackFree(stackBuffer, temporary);
		}
	}
}

bool writeStruct(const void* data, const Type& type, [[maybe_unused]] const TypeDB& typeDB, OutputArchive& archive) {
	if (archive.beginObject()) {
		const StructType* structType = static_cast<const StructType*>(&type);
		do {
			writeStructProperties(data, *structType, archive);
			structType = structType->getParentType();
		} while (structType);
		archive.endObject();
		return true;
	}
	return false;
}

bool writeEnum(const void* data, const Type& type, const TypeDB& /*typeDB*/, OutputArchive& archive) {
	const EnumType&   enumType = static_cast<const EnumType&>(type);
	const Enumerator* enumerator = enumType.findEnumeratorByValue(data, enumType.size);
	bool              res = false;
	if (enumerator) {
		res = true;
		archive.write(enumerator->name);
	}
	return res;
}

bool writeBitMask(const void* data, const Type& type, const TypeDB& /*typeDB*/, OutputArchive& archive) {
	const BitMaskType& bitMaskType = static_cast<const BitMaskType&>(type);
	// Cast the source type to an uint64_t
	BitMaskStorageType bitMask = 0;
	std::memcpy(&bitMask, data, type.size);

	char str[1024];
	str[0] = 0;
	int  length = 0;
	bool res = true;

	for (const BitMaskConstant& enumerator : bitMaskType.getEnumerators()) {
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
	const ContainerType& containerType = static_cast<const ContainerType&>(type);
	const Type*          keyType = containerType.getKeyType();
	const Type*          valueType = containerType.getValueType();

	if (! archive.beginArray()) {
		return false;
	}

	ReadIterator* iterator = containerType.newReadIterator(tmp, sizeof(tmp), data); // TODO scoped allocator instead
	while (iterator->isValid()) {
#if 0
		const char* elementName = "element";
		if (valueType->typeID == variant_typeID) {
			// a variant is already written inside an element that has the var name
			elementName = nullptr;
		}
#endif

		if (keyType) {
			// write key and value
			// TODO broken, not generic enough, doesn't work with vars etc.			// if (archive.beginElement(elementName)) {
			if (archive.beginObject()) {
				writeObject(iterator->getKey(), "key", *keyType, archive);
				writeObject(iterator->getValue(), "value", *valueType, archive);
				archive.endObject();
				//	archive.endElement();
				//}
			}
		}
		else {
			// Write value
			/*if (elementName) {
			    writeObject(iterator->getValue(), elementName, *valueType, archive);
			}
			else*/
			{
				writeObjectImpl(iterator->getValue(), *valueType, typeDB, archive);
			}
		}

		iterator->gotoNext();
	}

	containerType.deleteIterator(iterator); // TODO scoped allocator
	archive.endArray();
	return true;
}

bool writePointer(const void* data, const Type& type, const TypeDB& typeDB, OutputArchive& archive) {
	const PointerType& pointerType = static_cast<const PointerType&>(type);
	const void*        pointer = pointerType.resolvePointer(data);
	bool               res = true;
	if (pointer) {
		res = writeObjectImpl(pointer, pointerType.getPointedType(), typeDB, archive);
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
	const Type&    type = typeDB.getType(variant->getTypeId());
	const char*    typeName = typeIdToName(variant->getTypeId());
	if (! typeName) {
		return false;
	}
	archive.beginObject();
	archive.writeString("type", typeName);
	archive.writeString("name", variant->getName());
	writeObject(variant->getStorage(), "value", type, archive);
	archive.endObject();
	return true;
}

} // namespace

} // namespace Typhoon::Reflection
