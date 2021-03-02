#include <include/objectVisitor.h>

#include "context.h"
#include "enumType.h"
#include "namespace.h"
#include "property.h"
#include "structType.h"
#include "typeDB.h"
#include <core/scopedAllocator.h>

namespace Typhoon::Reflection {

namespace detail {

Context& getContext();

} // namespace detail

namespace {

bool visitObjectImpl(DataPtr data, const Type& type, const TypeDB& typeDB, ObjectVisitor& visitor, LinearAllocator& stackAllocator);

bool visitFundamental(DataPtr data, const Type& type, const TypeDB& typeDB, ObjectVisitor& visitor, LinearAllocator& stackAllocator) {
	return visitor.visitObject(data, type);
}

bool visitStruct(DataPtr data, const Type& type, const TypeDB& typeDB, ObjectVisitor& visitor, LinearAllocator& tempAllocator) {
	const StructType* structType = static_cast<const StructType*>(&type);
	do {
		for (const auto& property : structType->getProperties()) {
			const Type& valueType = property.getValueType();

			// Allocate a temporary for the value
			void* allocOffs = tempAllocator.getOffset();
			if (void* temporary = tempAllocator.alloc(valueType.getSize(), valueType.getAlignment()); temporary) {
				valueType.constructObject(temporary);
				const DataPtr self = data;
				property.getValue(self, temporary);
				if (visitor.beginField(property.getName())) {
					visitObjectImpl(temporary, valueType, typeDB, visitor, tempAllocator);
					visitor.endField();
				}
				valueType.destructObject(temporary);
			}
			tempAllocator.rewind(allocOffs);
		}
		structType = structType->getParentType();
	} while (structType);
	return true;
}

bool visitEnum(DataPtr data, const Type& type, const TypeDB& typeDB, ObjectVisitor& visitor, LinearAllocator& stackAllocator);
bool visitBitMask(DataPtr data, const Type& type, const TypeDB& typeDB, ObjectVisitor& visitor, LinearAllocator& stackAllocator);
bool visitContainer(DataPtr data, const Type& type, const TypeDB& typeDB, ObjectVisitor& visitor, LinearAllocator& stackAllocator);
bool visitPointer(DataPtr data, const Type& type, const TypeDB& typeDB, ObjectVisitor& visitor, LinearAllocator& stackAllocator);
bool visitReference(DataPtr data, const Type& type, const TypeDB& typeDB, ObjectVisitor& visitor, LinearAllocator& stackAllocator);
bool visitVariant(DataPtr data, const Type& type, const TypeDB& typeDB, ObjectVisitor& visitor, LinearAllocator& stackAllocator);

using VisitorFunc = bool (*)(DataPtr data, const Type&, const TypeDB&, ObjectVisitor&, LinearAllocator&);
constexpr VisitorFunc perClassVisitors[] = {
	visitFundamental, visitStruct, visitEnum, visitBitMask, visitContainer, visitPointer, visitReference, visitVariant,
};

bool visitObjectImpl(DataPtr data, const Type& type, const TypeDB& typeDB, ObjectVisitor& visitor, LinearAllocator& stackAllocator) {
	return perClassVisitors[(int)type.getSubClass()](data, type, typeDB, visitor, stackAllocator);
}

} // namespace

bool visitObject(DataPtr object, const TypeId typeId, ObjectVisitor& visitor, LinearAllocator& tempAllocator) {
	const TypeDB& typeDB = *detail::getContext().typeDB;
	const Type&   type = typeDB.getType(typeId);
	return visitObjectImpl(object, type, typeDB, visitor, tempAllocator);
}

} // namespace Typhoon::Reflection
