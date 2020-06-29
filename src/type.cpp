#include "type.h"

namespace Typhoon {

Type::Type(TypeId typeId, Subclass subClass, size_t size, size_t alignment, const MethodTable& methods)
    : typeID(typeId)
    , size(size)
    , alignment(alignment)
    , subClass(subClass)
    , methods(methods) {
}

TypeId Type::getTypeId() const {
	return typeID;
}

size_t Type::getSize() const {
	return size;
}

size_t Type::getAlignment() const {
	return alignment;
}

Type::Subclass Type::getSubClass() const {
	return subClass;
}

void Type::constructObject(void* object) const {
	if (methods.defaultConstructor) {
		methods.defaultConstructor(object);
	}
}

void Type::copyConstructObject(void* object, const void* src) const {
	if (methods.copyConstructor) {
		methods.copyConstructor(object, src);
	}
}

void Type::destructObject(void* object) const {
	if (methods.destructor) {
		methods.destructor(object);
	}
}

void Type::copyObject(void* a, const void* b) const {
	if (methods.copyAssignment) {
		methods.copyAssignment(a, b);
	}
}

void Type::moveConstructObject(void* object, void* src) const {
	if (methods.moveConstructor) {
		methods.moveConstructor(object, src);
	}
}

void Type::moveObject(void* a, void* b) const {
	if (methods.moveAssignment) {
		methods.moveAssignment(a, b);
	}
}

bool Type::compareObjects(const void* a, const void* b) const //
{
	return methods.equalityOperator(a, b);
}

} // namespace Typhoon