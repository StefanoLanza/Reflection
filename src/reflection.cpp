#include "../include/reflection.h"
#include "archive.h"
#include "builtinType.h"
#include "serializeBuiltIns.h"
#include "variantType.h"
#include <cstdint>
#include <string>

namespace Typhoon::Reflection {

namespace {

template <class T>
void readBuiltin(void* data, InputArchive& archive) {
	read(*static_cast<T*>(data), archive);
}

template <class T>
bool writeBuiltin(const void* data, OutputArchive& archive) {
	return write(*static_cast<const T*>(data), archive);
}

template <>
void readBuiltin<std::string>(void* data, InputArchive& archive) {
	if (const char* str = archive.currNodeText(); str) {
		*static_cast<std::string*>(data) = str;
	}
}

template <>
bool writeBuiltin<std::string>(const void* data, OutputArchive& archive) {
	return archive.write(static_cast<const std::string*>(data)->c_str());
}

template <class T>
void createBuiltin(TypeDB& typeDB) {
	static const BuiltinType types { getTypeId<T>(), sizeof(T), std::alignment_of_v<T>, detail::buildMethodTable<T>() };
	typeDB.registerType(&types, &readBuiltin<T>, &writeBuiltin<T>);
}

void registerBuiltinTypes(TypeDB& typeDB) {
	createBuiltin<bool>(typeDB);
	createBuiltin<char>(typeDB);
	createBuiltin<unsigned char>(typeDB);
	createBuiltin<short>(typeDB);
	createBuiltin<unsigned short>(typeDB);
	createBuiltin<int>(typeDB);
	createBuiltin<unsigned int>(typeDB);
	createBuiltin<long>(typeDB);
	createBuiltin<unsigned long>(typeDB);
	createBuiltin<long long>(typeDB);
	createBuiltin<unsigned long long>(typeDB);
	createBuiltin<float>(typeDB);
	createBuiltin<double>(typeDB);
	createBuiltin<const char*>(typeDB);
	createBuiltin<std::string>(typeDB);

	static const VariantType variantType;
	typeDB.registerType(&variantType);
}

} // namespace

void initReflection(TypeDB& typeDB) {
	registerBuiltinTypes(typeDB);
}

} // namespace Typhoon::Reflection
