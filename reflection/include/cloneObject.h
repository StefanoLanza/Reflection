#pragma once

#include "errorCodes.h"
#include <core/typeId.h>

namespace Typhoon {

ErrorCode cloneObject(void* dstObject, const void* srcObject, TypeId typeId);

template <class T>
ErrorCode cloneObject(T* dstObject, const T& srcObject) {
	return cloneObject(dstObject, &srcObject, getTypeId<T>());
}

} // namespace Typhoon
