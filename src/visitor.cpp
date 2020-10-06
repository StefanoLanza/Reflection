#pragma once

#include <include/visitor.h>

#include "context.h"
#include "enumType.h"
#include "property.h"
#include "structType.h"
#include "typeDB.h"

namespace Typhoon::Reflection {

namespace detail {

Context& getContext();

} // namespace detail

namespace {

void visitType(const Type& type, const Visitor& visitor, const VisitOptions& options, const TypeDB& typeDB, VisitContext& context);

void visitFundamental(const Type& type, const Visitor& visitor, [[maybe_unused]] const VisitOptions& options, [[maube_unused]] const TypeDB& typeDB,
                      VisitContext& context) {
	visitor(type, context);
}

void visitStruct(const Type& type, const Visitor& visitor, const VisitOptions& options, const TypeDB& typeDB, VisitContext& context) {
	const StructType* structType = static_cast<const StructType*>(&type);
	visitor(*structType, context);
	const int level = context.level;
	do {
		for (const auto& property : structType->getProperties()) {
			const Type& valueType = property.getValueType();
			context.objectName = property.getName();
			context.level = level + 1;
			visitType(valueType, visitor, options, typeDB, context);
		}
		structType = structType->getParentType();
	} while (structType);
}

void visitEnum(const Type& type, const Visitor& visitor, const VisitOptions& options, const TypeDB& typeDB, VisitContext& context) {
	visitor(type, context);
}

void visitBitMask(const Type& type, const Visitor& visitor, const VisitOptions& options, const TypeDB& typeDB, VisitContext& context) {
}

void visitContainer(const Type& type, const Visitor& visitor, const VisitOptions& options, const TypeDB& typeDB, VisitContext& context) {
}

void visitPointer(const Type& type, const Visitor& visitor, const VisitOptions& options, const TypeDB& typeDB, VisitContext& context) {
}

void visitReference(const Type& type, const Visitor& visitor, const VisitOptions& options, const TypeDB& typeDB, VisitContext& context) {
}

void visitVariant(const Type& type, const Visitor& visitor, const VisitOptions& options, const TypeDB& typeDB, VisitContext& context) {
}

using VisitFunc = void (*)(const Type& type, const Visitor& visitor, const VisitOptions& options, const TypeDB& typeDB, VisitContext& context);
constexpr VisitFunc perClassVisitors[] = {
	visitFundamental, visitStruct, visitEnum, visitBitMask, visitContainer, visitPointer, visitReference, visitVariant,
};

void visitType(const Type& type, const Visitor& visitor, const VisitOptions& options, const TypeDB& typeDB, VisitContext& context) {
	perClassVisitors[(int)type.subClass](type, visitor, options, typeDB, context);
}

void visitObject(void* object, const Type& type, const Visitor& visitor, const VisitOptions& options, const TypeDB& typeDB,
                 const VisitContext& context) {
	// perClassVisitors[(int)type.subClass](type, visitor, options, typeDB, level);
}

} // namespace

void visitType(TypeId typeId, const Visitor& visitor, const VisitOptions& options) {
	const TypeDB& typeDB = *detail::getContext().typeDB;
	VisitContext  context;
	context.level = 0;
	context.objectName = nullptr;
	visitType(typeDB.getType(typeId), visitor, options, typeDB, context);
}

void visitObject(void* object, const TypeId typeId, const Visitor& visitor, const VisitOptions& options) {
	const TypeDB& typeDB = *detail::getContext().typeDB;
	VisitContext  context;
	context.level = 0;
	context.objectName = nullptr;
	visitObject(object, typeDB.getType(typeId), visitor, options, typeDB, context);
}

} // namespace Typhoon::Reflection
