#pragma once

#include "context.h"
#include "dataPtr.h"
#include "errorCodes.h"
#include "typeDB.h"
#include <core/typeId.h>

namespace Typhoon::Reflection {

ErrorCode cloneObject(DataPtr dstObject, ConstDataPtr srcObject, const Type& type);

namespace detail {

Context& getContext();

} // namespace detail

template <class T>
ErrorCode cloneObject(T* dstObject, const T& srcObject) {
	Context&    context = detail::getContext();
	const Type* type = context.typeDB->tryGetType<T>();
	if (! type) {
		type = detail::autoRegisterHelper<T>::autoRegister(context);
	}
	if (type) {
		return cloneObject(dstObject, &srcObject, *type);
	}
	return ErrorCode::unknownType;
}

} // namespace Typhoon::Reflection
