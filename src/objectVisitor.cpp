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

bool visitObjectImpl(void* data, const Type& type, const TypeDB& typeDB, ObjectVisitor& visitor, LinearAllocator& stackAllocator);

bool visitFundamental(void* data, const Type& type, const TypeDB& typeDB, ObjectVisitor& visitor, LinearAllocator& stackAllocator) {
	return visitor.visitObject(data, type);
}

bool visitStruct(void* data, const Type& type, const TypeDB& typeDB, ObjectVisitor& visitor, LinearAllocator& stackAllocator) {
	const StructType* structType = static_cast<const StructType*>(&type);
	do {
		for (const auto& property : structType->getProperties()) {
			const Type& valueType = property.getValueType();

			// Allocate a temporary for the value
			ScopedAllocator scopedAllocator(stackAllocator);
			void* const     temporary = scopedAllocator.alloc(valueType.getSize(), valueType.getAlignment());
			if (temporary) {
				valueType.constructObject(temporary);
				const void* const self = data;
				property.getValue(self, temporary);
				if (visitor.beginField(property.getName())) {
					visitObjectImpl(temporary, valueType, typeDB, visitor, stackAllocator);
					visitor.endField();
				}
				valueType.destructObject(temporary);
			}
		}
		structType = structType->getParentType();
	} while (structType);
	return true;
}

bool visitEnum(void* data, const Type& type, const TypeDB& typeDB, ObjectVisitor& visitor, LinearAllocator& stackAllocator);
bool visitBitMask(void* data, const Type& type, const TypeDB& typeDB, ObjectVisitor& visitor, LinearAllocator& stackAllocator);
bool visitContainer(void* data, const Type& type, const TypeDB& typeDB, ObjectVisitor& visitor, LinearAllocator& stackAllocator);
bool visitPointer(void* data, const Type& type, const TypeDB& typeDB, ObjectVisitor& visitor, LinearAllocator& stackAllocator);
bool visitReference(void* data, const Type& type, const TypeDB& typeDB, ObjectVisitor& visitor, LinearAllocator& stackAllocator);
bool visitVariant(void* data, const Type& type, const TypeDB& typeDB, ObjectVisitor& visitor, LinearAllocator& stackAllocator);

using VisitorFunc = bool (*)(void* data, const Type&, const TypeDB&, ObjectVisitor&, LinearAllocator&);
constexpr VisitorFunc perClassVisitors[] = {
	visitFundamental, visitStruct, visitEnum, visitBitMask, visitContainer, visitPointer, visitReference, visitVariant,
};

bool visitObjectImpl(void* data, const Type& type, const TypeDB& typeDB, ObjectVisitor& visitor, LinearAllocator& stackAllocator) {
	return perClassVisitors[(int)type.subClass](data, type, typeDB, visitor, stackAllocator);
}


} // namespace

bool visitObject(void* object, const TypeId typeId, ObjectVisitor& visitor) {
	const TypeDB&   typeDB = *detail::getContext().typeDB;
	const Type&     type = typeDB.getType(typeId);
	char            stack[Defaults::stackSize];
	LinearAllocator stackAllocator(stack, std::size(stack), nullptr);
	return visitObjectImpl(object, type, typeDB, visitor, stackAllocator);
}

} // namespace Typhoon::Reflection
