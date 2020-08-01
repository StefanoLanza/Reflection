#pragma once

#include "errorCodes.h"
#include "src/context.h"
#include "src/typeDB.h"

namespace Typhoon::Reflection {

class Type;
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

bool writeObject(const void* data, const char* name, const Type& type, OutputArchive& archive);

} // namespace Typhoon::Reflection
