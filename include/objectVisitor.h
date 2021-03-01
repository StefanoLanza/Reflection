#pragma once

#include "config.h"

#include <core/typeId.h>

namespace Typhoon::Reflection {

class Type;

enum class ObjectVisitorResult
{
	success = 0,
	error = 1,
	terminate = 2,
	outOfMemory = 3,
};

class ObjectVisitor {
public:
	~ObjectVisitor() = default;

	virtual bool beginField(const char* fieldName) = 0;
	virtual void endField() = 0;
	virtual bool visitObject(void* data, const Type& type) = 0;
};

// TODO pass allocator
bool visitObject(void* object, const TypeId typeId, ObjectVisitor& visitor);

// Helper

template <class T>
inline void visitObject(T& object, ObjectVisitor& visitor) {
	visitObject(const_cast<void*>(&object), getTypeId<T>(), visitor);
}

} // namespace Typhoon::Reflection
