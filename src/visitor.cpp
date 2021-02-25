#include <include/visitor.h>

#include "context.h"
#include "enumType.h"
#include "namespace.h"
#include "property.h"
#include "structType.h"
#include "typeDB.h"

namespace Typhoon::Reflection {

namespace detail {

Context& getContext();

} // namespace detail

namespace {

void visitType(const Type& type, TypeVisitor& visitor, const VisitOptions& options, const TypeDB& typeDB);

void visitFundamental(const Type& type, TypeVisitor& visitor, [[maybe_unused]] const VisitOptions& options, [[maybe_unused]] const TypeDB& typeDB) {
	visitor.visitType(type);
}

void visitStruct(const Type& type, TypeVisitor& visitor, const VisitOptions& options, const TypeDB& typeDB) {
	const StructType* structType = static_cast<const StructType*>(&type);
	visitor.beginClass(*structType);
	do {
		for (const auto& property : structType->getProperties()) {
			const Type& valueType = property.getValueType();
			visitor.visitField(property.getName(), valueType);
			// visitType(valueType, visitor, options, typeDB, context);
		}
		structType = structType->getParentType();
	} while (structType);
	visitor.endClass();
}

void visitEnum(const Type& type, TypeVisitor& visitor, const VisitOptions& options, const TypeDB& typeDB) {
	visitor.visitType(type);
}

void visitBitMask(const Type& type, TypeVisitor& visitor, const VisitOptions& options, const TypeDB& typeDB) {
	visitor.visitType(type);
}

void visitContainer(const Type& type, TypeVisitor& visitor, const VisitOptions& options, const TypeDB& typeDB) {
	visitor.visitType(type);
}

void visitPointer(const Type& type, TypeVisitor& visitor, const VisitOptions& options, const TypeDB& typeDB) {
}

void visitReference(const Type& type, TypeVisitor& visitor, const VisitOptions& options, const TypeDB& typeDB) {
}

void visitVariant(const Type& type, TypeVisitor& visitor, const VisitOptions& options, const TypeDB& typeDB) {
	visitor.visitType(type);
}

using VisitFunc = void (*)(const Type& type, TypeVisitor& visitor, const VisitOptions& options, const TypeDB& typeDB);
constexpr VisitFunc perClassVisitors[] = {
	visitFundamental, visitStruct, visitEnum, visitBitMask, visitContainer, visitPointer, visitReference, visitVariant,
};

void visitType(const Type& type, TypeVisitor& visitor, const VisitOptions& options, const TypeDB& typeDB) {
	perClassVisitors[(int)type.getSubClass()](type, visitor, options, typeDB);
}

void visitNamespaceRecursive(const Namespace& nameSpace, TypeVisitor& visitor, const VisitOptions& options) {
	visitor.beginNamespace(nameSpace);
	for (const auto& type : nameSpace.getTypes()) {
		visitType(*type, visitor, options);
	}
	for (const auto& nestedNamespace : nameSpace.getNestedNamespaces()) {
		visitNamespaceRecursive(*nestedNamespace, visitor, options);
	}
	visitor.endNamespace(nameSpace);
}

} // namespace

void visitType(TypeId typeId, TypeVisitor& visitor, const VisitOptions& options) {
	const TypeDB& typeDB = *detail::getContext().typeDB;
	visitType(typeDB.getType(typeId), visitor, options);
}

void visitType(const Type& type, TypeVisitor& visitor, const VisitOptions& options) {
	const TypeDB& typeDB = *detail::getContext().typeDB;
	visitType(type, visitor, options, typeDB);
}

void visitNamespace(const Namespace& nameSpace, TypeVisitor& visitor, const VisitOptions& options) {
	visitNamespaceRecursive(nameSpace, visitor, options);
}

} // namespace Typhoon::Reflection
