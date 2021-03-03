#pragma once

#include "config.h"
#include "dataPtr.h"

#include <core/typeId.h>

namespace Typhoon {
class LinearAllocator;
}

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
	virtual bool visitObject(DataPtr data, const Type& type) = 0;
};

bool visitObject(DataPtr object, const TypeId typeId, ObjectVisitor& visitor, LinearAllocator& tempAllocator);

// Helper

template <class T>
inline void visitObject(T& object, ObjectVisitor& visitor) {
	visitObject(const_cast<DataPtr>(&object), getTypeId<T>(), visitor);
}

} // namespace Typhoon::Reflection
