#include "type.h"

namespace Typhoon::Reflection {

Type::Type(const char* typeName, TypeId typeId, Subclass subClass, size_t size, size_t alignment, const MethodTable& methods, Allocator& allocator)
    : typeID(typeId)
    , size(size)
    , alignment(alignment)
    , subClass(subClass)
    , typeName(typeName)
    , methods(methods)
    , attributes(stdAllocator<const Attribute*>(allocator)) {
}

const char* Type::getName() const {
	return typeName;
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

void Type::constructObject(DataPtr object) const {
	if (methods.defaultConstructor) {
		methods.defaultConstructor(object);
	}
}

void Type::copyConstructObject(DataPtr object, ConstDataPtr src) const {
	if (methods.copyConstructor) {
		methods.copyConstructor(object, src);
	}
}

void Type::destructObject(DataPtr object) const {
	if (methods.destructor) {
		methods.destructor(object);
	}
}

void Type::copyObject(DataPtr a, ConstDataPtr b) const {
	if (methods.copyAssignment) {
		methods.copyAssignment(a, b);
	}
}

void Type::moveConstructObject(DataPtr object, DataPtr src) const {
	if (methods.moveConstructor) {
		methods.moveConstructor(object, src);
	}
}

void Type::moveObject(DataPtr a, DataPtr b) const {
	if (methods.moveAssignment) {
		methods.moveAssignment(a, b);
	}
}

bool Type::compareObjects(ConstDataPtr a, ConstDataPtr b) const {
	return methods.equalityOperator(a, b);
}

void Type::setCustomWriter(CustomWriter writer) {
	customWriter = std::move(writer);
}

const CustomWriter& Type::getCustomWriter() const {
	return customWriter;
}

void Type::setCustomReader(CustomReader reader) {
	customReader = std::move(reader);
}

const CustomReader& Type::getCustomReader() const {
	return customReader;
}

void Type::setCustomCloner(CustomCloner cloner) {
	customCloner = std::move(cloner);
}

const CustomCloner& Type::getCustomCloner() const {
	return customCloner;
}

void Type::addAttribute(const Attribute* attribute) {
	attributes.push_back(attribute);
}

std::span<const Attribute* const> Type::getAttributes() const {
	return attributes;
}

} // namespace Typhoon::Reflection
