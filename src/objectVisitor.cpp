#pragma once

#include <include/objectVisitor.h>

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

namespace {}

void visitObject(void* object, const TypeId typeId, ObjectVisitor& visitors) {
	const TypeDB& typeDB = *detail::getContext().typeDB;
	//	visitObject(object, typeDB.getType(typeId), visitor, options, typeDB, context);
}

} // namespace Typhoon::Reflection
