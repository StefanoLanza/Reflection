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
	const Type* typeInfo = typeDB.tryGetTypeInfo<T>();
	if (! typeInfo) {
		typeInfo = detail::autoRegisterHelper<T>::autoRegister(typeDB);
	}
	if (typeInfo) {
		return writeObject(&object, name, *typeInfo, archive);
	}
	else {
		return false;
	}
}

bool writeObject(const void* data, const char* name, const Type& type, OutputArchive& archive);

} // namespace Typhoon
