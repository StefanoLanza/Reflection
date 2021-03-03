#pragma once

#include "config.h"

#include <core/typeId.h>
#include <functional>

namespace Typhoon::Reflection {

class Type;
class Namespace;
class StructType;

class TypeVisitor {
public:
	~TypeVisitor() = default;

	virtual void beginNamespace(const Namespace&) = 0;
	virtual void endNamespace(const refl::Namespace& nameSpace) = 0;
	virtual void beginClass(const StructType& type) = 0;
	virtual void endClass() = 0;
	virtual void visitType(const Type& type) = 0;
	virtual void visitField(const char* fieldName, const Type& type) = 0;
};
//= std::function<void(const Type& type, const VisitContext& context)>;

struct VisitOptions {
	bool recursive = true;
};

void visitType(TypeId typeId, TypeVisitor& visitor, const VisitOptions& options);

void visitType(const Type& type, TypeVisitor& visitor, const VisitOptions& options);

// Helper
template <class T>
inline void visitType(TypeVisitor& visitor, const VisitOptions& options) {
	visitType(getTypeId<T>(), visitor, options);
}

void visitNamespace(const Namespace& nameSpace, TypeVisitor& visitor, const VisitOptions& options);

} // namespace Typhoon::Reflection
