#pragma once

#include <include/visitor.h>

#include "context.h"
#include "property.h"
#include "structType.h"
#include "typeDB.h"

namespace Typhoon::Reflection {

namespace detail {

Context& getContext();

} // namespace detail

namespace {

void visitType(const Type& type, const Visitor& visitor, const VisitOptions& options, const TypeDB& typeDB, int level);

void visitFundamental(const Type& type, const Visitor& visitor, [[maube_unused]] const VisitOptions& options, [[maube_unused]] const TypeDB& typeDB, int level) {
	VisitContext context;
	context.objectName = nullptr;
	context.level = level;
	visitor(type, context);
}

void visitStruct(const Type& type, const Visitor& visitor, const VisitOptions& options, const TypeDB& typeDB, int level) {
	const StructType* structType = static_cast<const StructType*>(&type);
	VisitContext context;
	context.objectName = nullptr;
	context.level = level;
	visitor(*structType, context);
	do {
		for (const auto& property : structType->getProperties()) {
			const Type& valueType = property.getValueType();
			visitType(valueType, visitor, options, typeDB, level + 1);
		}
		structType = structType->getParentType();
	} while (structType);
}

void visitEnum(const Type& type, const Visitor& visitor, const VisitOptions& options, const TypeDB& typeDB, int level) {
}

void visitBitMask(const Type& type, const Visitor& visitor, const VisitOptions& options, const TypeDB& typeDB, int level) {
}

void visitContainer(const Type& type, const Visitor& visitor, const VisitOptions& options, const TypeDB& typeDB, int level) {
}

void visitPointer(const Type& type, const Visitor& visitor, const VisitOptions& options, const TypeDB& typeDB, int level) {
}

void visitReference(const Type& type, const Visitor& visitor, const VisitOptions& options, const TypeDB& typeDB, int level) {
}

void visitVariant(const Type& type, const Visitor& visitor, const VisitOptions& options, const TypeDB& typeDB, int level) {
}

using VisitFunc = void (*)(const Type& type, const Visitor& visitor, const VisitOptions& options, const TypeDB& typeDB, int level);
constexpr VisitFunc perClassVisitors[] = {
	visitFundamental, visitStruct, visitEnum, visitBitMask, visitContainer, visitPointer, visitReference, visitVariant,
};

void visitType(const Type& type, const Visitor& visitor, const VisitOptions& options, const TypeDB& typeDB, int level) {
	perClassVisitors[(int)type.subClass](type, visitor, options, typeDB, level);
}

} // namespace

void visitType(TypeId typeId, const Visitor& visitor, const VisitOptions& options) {
	const TypeDB& typeDB = *detail::getContext().typeDB;
	visitType(typeDB.getType(typeId), visitor, options, typeDB, 0);
}

} // namespace Typhoon::Reflection
