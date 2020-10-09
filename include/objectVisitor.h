#pragma once

#include "config.h"

#include <core/typeId.h>

namespace Typhoon::Reflection {

class Type;

class ObjectVisitor {
public:
	~ObjectVisitor() = default;

	virtual void visitField(const char* fieldName, const Type& type) = 0;
};


void visitObject(void* object, const TypeId typeId, ObjectVisitor& visitor);

// Helper

template <class T>
inline void visitObject(T& object, ObjectVisitor& visitor) {
	visitObject(const_cast<void*>(&object), getTypeId<T>(), visitor);
}

} // namespace Typhoon::Reflection
