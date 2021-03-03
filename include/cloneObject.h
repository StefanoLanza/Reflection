#pragma once

#include "dataPtr.h"
#include "errorCodes.h"
#include <core/typeId.h>

namespace Typhoon::Reflection {

ErrorCode cloneObject(DataPtr dstObject, ConstDataPtr srcObject, TypeId typeId);

template <class T>
ErrorCode cloneObject(T* dstObject, const T& srcObject) {
	return cloneObject(dstObject, &srcObject, getTypeId<T>());
}

} // namespace Typhoon::Reflection
