#pragma once

#include "context.h"
#include "dataPtr.h"
#include "errorCodes.h"
#include "typeDB.h"

namespace Typhoon::Reflection {

class OutputArchive;

namespace detail {

Context& getContext();

} // namespace detail

template <typename T>
bool writeObject(const T& object, const char* name, OutputArchive& archive) {
	Context&    context = detail::getContext();
	const Type* type = context.typeDB->tryGetType<T>();
	if (! type) {
		// ? Allocate a temporary Type, without registering it ?
		type = detail::autoRegisterHelper<T>::autoRegister(context);
	}
	if (type) {
		return writeObject(&object, name, *type, archive);
	}
	else {
		return false;
	}
}

bool writeObject(ConstDataPtr data, const char* name, const Type& type, OutputArchive& archive);

} // namespace Typhoon::Reflection