#pragma once

#include "context.h"
#include "dataPtr.h"
#include "typeDB.h"

namespace Typhoon::Reflection {

class OutputArchive;

namespace detail {

Context& getContext();

bool writeData(ConstDataPtr data, const Type& type, OutputArchive& archive);
bool writeData(ConstDataPtr data, TypeId typeId, OutputArchive& archive);

template <typename T>
bool writeData(const T& object, OutputArchive& archive) {
	Context&    context = detail::getContext();
	const Type* type = context.typeDB->tryGetType<T>();
	if (! type) {
		// ? Allocate a temporary Type, without registering it ?
		type = autoRegisterHelper<T>::autoRegister(context);
	}
	if (type) {
		return writeData(&object, *type, archive);
	}
	else {
		return false;
	}
}

} // namespace detail

} // namespace Typhoon::Reflection
