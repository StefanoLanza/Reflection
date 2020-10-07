#pragma once

#include "config.h"

#include <core/typeId.h>
#include <functional>

namespace Typhoon::Reflection {

class Type;
class Namespace;
class StructType;

struct VisitContext {
	const char*      objectName;
};

class Visitor {
public:

	~Visitor() = default;

	virtual void beginNamespace(const Namespace&) = 0;
	virtual void endNamespace() = 0;
	virtual void beginClass(const StructType& type) = 0;
	virtual void endClass() = 0;
	virtual void visitType(const Type& type) = 0;
	virtual void visitField(const char* fieldName, const Type& type) = 0;
};
//= std::function<void(const Type& type, const VisitContext& context)>;

struct VisitOptions {
	bool recursive = true;
};

void visitType(TypeId typeId, Visitor& visitor, const VisitOptions& options);

void visitType(const Type& type, Visitor& visitor, const VisitOptions& options);

void visitObject(void* object, const TypeId typeId, Visitor& visitor, const VisitOptions& options);

// Helper
template <class T>
inline void visitType(Visitor& visitor, const VisitOptions& options) {
	visitType(getTypeId<T>(), visitor, options);
}

template <class T>
inline void visitObject(T& object, Visitor& visitor, const VisitOptions& options) {
	visitObject(const_cast<void*>(&object), getTypeId<T>(), visitor, options);
}

void visitNamespace(const Namespace& nameSpace, Visitor& visitor, const VisitOptions& options);

} // namespace Typhoon::Reflection
