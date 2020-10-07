#pragma once

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

void visitType(const Type& type, Visitor& visitor, const VisitOptions& options, const TypeDB& typeDB, VisitContext& context);

void visitFundamental(const Type& type, Visitor& visitor, [[maybe_unused]] const VisitOptions& options, [[maybe_unused]] const TypeDB& typeDB,
                      VisitContext& context) {
	visitor.visitType(type);//, context);
}

void visitStruct(const Type& type, Visitor& visitor, const VisitOptions& options, const TypeDB& typeDB, VisitContext& context) {
	const StructType* structType = static_cast<const StructType*>(&type);
	visitor.beginClass(*structType);
	do {
		for (const auto& property : structType->getProperties()) {
			const Type& valueType = property.getValueType();
			context.objectName = property.getName();
			visitor.visitField(property.getName(), valueType);
			//visitType(valueType, visitor, options, typeDB, context);
		}
		structType = structType->getParentType();
	} while (structType);
	visitor.endClass();
}

void visitEnum(const Type& type, Visitor& visitor, const VisitOptions& options, const TypeDB& typeDB, VisitContext& context) {
	visitor.visitType(type);//, context);
}

void visitBitMask(const Type& type, Visitor& visitor, const VisitOptions& options, const TypeDB& typeDB, VisitContext& context) {
	visitor.visitType(type);//, context);
}

void visitContainer(const Type& type, Visitor& visitor, const VisitOptions& options, const TypeDB& typeDB, VisitContext& context) {
	visitor.visitType(type);//, context);
}

void visitPointer(const Type& type, Visitor& visitor, const VisitOptions& options, const TypeDB& typeDB, VisitContext& context) {
}

void visitReference(const Type& type, Visitor& visitor, const VisitOptions& options, const TypeDB& typeDB, VisitContext& context) {
}

void visitVariant(const Type& type, Visitor& visitor, const VisitOptions& options, const TypeDB& typeDB, VisitContext& context) {
	visitor.visitType(type);//, context);
}

using VisitFunc = void (*)(const Type& type, Visitor& visitor, const VisitOptions& options, const TypeDB& typeDB, VisitContext& context);
constexpr VisitFunc perClassVisitors[] = {
	visitFundamental, visitStruct, visitEnum, visitBitMask, visitContainer, visitPointer, visitReference, visitVariant,
};

void visitType(const Type& type, Visitor& visitor, const VisitOptions& options, const TypeDB& typeDB, VisitContext& context) {
	perClassVisitors[(int)type.subClass](type, visitor, options, typeDB, context);
}

void visitObject(void* object, const Type& type, Visitor& visitor, const VisitOptions& options, const TypeDB& typeDB,
                 const VisitContext& context) {
	// perClassVisitors[(int)type.subClass](type, visitor, options, typeDB, level);
}

void visitNamespaceRecursive(const Namespace& nameSpace, Visitor& visitor, const VisitOptions& options, VisitContext& context) {
	visitor.beginNamespace(nameSpace);
	for (const auto& type : nameSpace.getTypes()) {
		visitType(*type, visitor, options);
	}
	for (const auto& nestedNamespace : nameSpace.getNestedNamespaces()) {
		visitNamespaceRecursive(*nestedNamespace, visitor, options, context);
	}
	visitor.endNamespace();
}

} // namespace

void visitType(TypeId typeId, Visitor& visitor, const VisitOptions& options) {
	const TypeDB& typeDB = *detail::getContext().typeDB;
	visitType(typeDB.getType(typeId), visitor, options);
}

void visitType(const Type& type, Visitor& visitor, const VisitOptions& options) {
	const TypeDB& typeDB = *detail::getContext().typeDB;
	VisitContext  context;
	context.objectName = nullptr;
	visitType(type, visitor, options, typeDB, context);
}

void visitObject(void* object, const TypeId typeId, Visitor& visitor, const VisitOptions& options) {
	const TypeDB& typeDB = *detail::getContext().typeDB;
	VisitContext  context;
	context.objectName = nullptr;
	visitObject(object, typeDB.getType(typeId), visitor, options, typeDB, context);
}

void visitNamespace(const Namespace& nameSpace, Visitor& visitor, const VisitOptions& options) {
	VisitContext context;
	context.objectName = nullptr;
	visitNamespaceRecursive(nameSpace, visitor, options, context);
}

} // namespace Typhoon::Reflection
