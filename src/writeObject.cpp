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

bool writeObjectImpl(const void* data, const Type& type, const TypeDB& typeDB, OutputArchive& archive, LinearAllocator& stackAllocator);
bool writeStruct(const void* data, const Type& type, const TypeDB& typeDB, OutputArchive& archive, LinearAllocator& stackAllocator);
bool writeEnum(const void* data, const Type& type, const TypeDB& typeDB, OutputArchive& archive, LinearAllocator& stackAllocator);
bool writeBitMask(const void* data, const Type& type, const TypeDB& typeDB, OutputArchive& archive, LinearAllocator& stackAllocator);
bool writeContainer(const void* data, const Type& type, const TypeDB& typeDB, OutputArchive& archive, LinearAllocator& stackAllocator);
bool writePointer(const void* data, const Type& type, const TypeDB& typeDB, OutputArchive& archive, LinearAllocator& stackAllocator);
bool writeReference(const void* data, const Type& type, const TypeDB& typeDB, OutputArchive& archive, LinearAllocator& stackAllocator);
bool writeVariant(const void* data, const Type& type, const TypeDB& typeDB, OutputArchive& archive, LinearAllocator& stackAllocator);

using Writer = bool (*)(const void* data, const Type& type, const TypeDB& typeDB, OutputArchive&, LinearAllocator&);
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
		char            stack[stackSize];
		LinearAllocator stackAllocator(stack, std::size(stack), nullptr);
		res = writeObjectImpl(data, type, typeDB, archive, stackAllocator);
		archive.endElement();
	}
	return res;
}

namespace {

bool writeObjectImpl(const void* data, const Type& type, const TypeDB& typeDB, OutputArchive& archive, LinearAllocator& stackAllocator) {
	bool res = false;
	if (const CustomWriter& customWriter = type.getCustomWriter(); customWriter) {
		res = customWriter(data, archive);
	}
	else {
		res = perClasswriters[(int)type.subClass](data, type, typeDB, archive, stackAllocator);
	}
	return res;
}

void writeStructProperties(const void* data, const StructType& structType, OutputArchive& archive, LinearAllocator& stackAllocator) {
	for (const auto& property : structType.getProperties()) {
		if (property.getFlags() & Flags::writeable) {
			const Type& valueType = property.getValueType();

			// Allocate a temporary for the value
			ScopedAllocator scopedAllocator(stackAllocator);
			void* const     temporary = scopedAllocator.alloc(valueType.getSize(), valueType.getAlignment());
			if (temporary) {
				valueType.constructObject(temporary);
				const void* const self = data;
				property.getValue(self, temporary);
				writeObject(temporary, property.getName(), valueType, archive);
				valueType.destructObject(temporary);
			}
		}
	}
}

bool writeStruct(const void* data, const Type& type, [[maybe_unused]] const TypeDB& typeDB, OutputArchive& archive, LinearAllocator& stackAllocator) {
	if (archive.beginObject()) {
		const StructType* structType = static_cast<const StructType*>(&type);
		do {
			writeStructProperties(data, *structType, archive, stackAllocator);
			structType = structType->getParentType();
		} while (structType);
		archive.endObject();
		return true;
	}
	return false;
}

bool writeEnum(const void* data, const Type& type, const TypeDB& /*typeDB*/, OutputArchive& archive, LinearAllocator& /*stackAllocator*/) {
	const EnumType&   enumType = static_cast<const EnumType&>(type);
	const Enumerator* enumerator = enumType.findEnumeratorByValue(data, enumType.size);
	bool              res = false;
	if (enumerator) {
		res = true;
		archive.write(enumerator->name);
	}
	return res;
}

bool writeBitMask(const void* data, const Type& type, const TypeDB& /*typeDB*/, OutputArchive& archive, LinearAllocator& /*stackAllocator*/) {
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

bool writeContainer(const void* data, const Type& type, const TypeDB& typeDB, OutputArchive& archive, LinearAllocator& stackAllocator) {
	const ContainerType& containerType = static_cast<const ContainerType&>(type);
	const Type*          keyType = containerType.getKeyType();
	const Type*          valueType = containerType.getValueType();

	if (! archive.beginArray()) {
		return false;
	}

	ScopedAllocator scopedAllocator(stackAllocator);
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
			writeObjectImpl(iterator->getValue(), *valueType, typeDB, archive, stackAllocator);
		}

		iterator->gotoNext();
	}

	archive.endArray();
	return true;
}

bool writePointer(const void* data, const Type& type, const TypeDB& typeDB, OutputArchive& archive, LinearAllocator& stackAllocator) {
	const PointerType& pointerType = static_cast<const PointerType&>(type);
	const void*        pointer = pointerType.resolvePointer(data);
	bool               res = true;
	if (pointer) {
		res = writeObjectImpl(pointer, pointerType.getPointedType(), typeDB, archive, stackAllocator);
	}
	return res;
}

bool writeReference(const void* data, const Type& type, const TypeDB& typeDB, OutputArchive& archive, LinearAllocator& stackAllocator) {
	const ReferenceType& referenceType = static_cast<const ReferenceType&>(type);
	const void*          pointerToData = *static_cast<void* const*>(data);
	assert(pointerToData); // cannot have a null reference
	return writeObjectImpl(pointerToData, referenceType.getReferencedType(), typeDB, archive, stackAllocator);
}

bool writeVariant(const void* data, const Type& /*type*/, const TypeDB& typeDB, OutputArchive& archive, LinearAllocator& stackAllocator) {
	const Variant* variant = static_cast<const Variant*>(data);
	const Type&    type = typeDB.getType(variant->getTypeId());
	const char*    typeName = variant->getType().getName();
	if (! typeName) {
		return false;
	}
	archive.beginObject();
	archive.writeString("type", typeName);
	archive.writeString("name", variant->getName());
	archive.beginElement("value");
	writeObjectImpl(variant->getStorage(), type, typeDB, archive, stackAllocator);
	archive.endElement();
	archive.endObject();
	return true;
}

} // namespace

} // namespace Typhoon::Reflection
