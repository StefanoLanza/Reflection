#pragma once

#include "errorCodes.h"
#include "typeDB.h"

namespace Typhoon {

class Type;
class TypeDB;
class OutputArchive;

template <typename T>
bool writeObject(const T& object, const char* name, OutputArchive& archive) {
	TypeDB&     typeDB = getTypeDB();
	const Type* types = typeDB.tryGetType<T>();
	if (! types) {
		types = detail::autoRegisterHelper<T>::autoRegister(typeDB);
	}
	if (types) {
		return writeObject(&object, name, *types, archive);
	}
	else {
		return false;
	}
}

bool writeObject(const void* data, const char* name, const Type& type, OutputArchive& archive);

} // namespace Typhoon
