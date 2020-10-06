#pragma once

#include "config.h"

#include <core/typeId.h>
#include <functional>

namespace Typhoon::Reflection {

class Type;

struct VisitContext {
	const char* objectName;
	int         level;
};

using Visitor = std::function<void(const Type& type, const VisitContext& context)>;

struct VisitOptions {
	bool recursive = true;
};

void visitType(TypeId typeId, const Visitor& visitor, const VisitOptions& options);

void visitObject(void* object, const TypeId typeId, const Visitor& visitor, const VisitOptions& options);

// Helper
template <class T>
inline void visitType(const Visitor& visitor, const VisitOptions& options) {
	visitType(getTypeId<T>(), visitor, options);
}

template <class T>
inline void visitObject(T& object, const Visitor& visitor, const VisitOptions& options) {
	visitObject(const_cast<void*>(&object), getTypeId<T>(), visitor, options);
}

} // namespace Typhoon::Reflection
