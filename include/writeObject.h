#pragma once

#include "errorCodes.h"
#include "typeDB.h"

namespace Typhoon::Reflection {

class Type;
class TypeDB;
class OutputArchive;

TypeDB& getTypeDB();
Allocator& getAllocator();

template <typename T>
bool writeObject(const T& object, const char* name, OutputArchive& archive) {
	TypeDB&     typeDB = getTypeDB();
	const Type* type = typeDB.tryGetType<T>();
	if (! type) {
		// ? Allocate a temporary Type, without registering it ?
		type = detail::autoRegisterHelper<T>::autoRegister(typeDB, getAllocator());
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
