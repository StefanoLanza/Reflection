#include "../include/reflection.h"
#include "allocUtils.h"
#include "allocator.h"
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
	BuiltinType* type = detail::make<BuiltinType>(getTypeId<T>(), sizeof(T), alignof(T), detail::buildMethodTable<T>());
	type->setCustomReader(&readBuiltin<T>);
	type->setCustomWriter(&writeBuiltin<T>);
	typeDB.registerType(type);
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

	typeDB.registerType(detail::make<VariantType>());
}

struct Context {
	TypeDB*    typeDB;
	Allocator* allocator;
};

class DefaultAllocator : public Allocator {
public:
	void* alloc(size_t size, size_t /*alignment*/) override {
		return ::malloc(size);
	}

	void free(void* ptr, size_t /*size*/) override {
		::free(ptr);
	}
};

DefaultAllocator defaultAllocator;

Context context { nullptr, &defaultAllocator };

} // namespace

namespace detail {

void* alloc(size_t size, size_t alignment) {
	return context.allocator->alloc(size, alignment);
}

void free(void* ptr, size_t size) {
	context.allocator->free(ptr, size);
}

Allocator& getAllocator() {
	return *context.allocator;
}

} // namespace detail

void initReflection() {
	initReflection(defaultAllocator);
}

void initReflection(Allocator& allocator) {
	context.allocator = &allocator;
	context.typeDB = detail::make<TypeDB>();
	registerBuiltinTypes(*context.typeDB);
}

void deinitReflection() {
	context.typeDB->~TypeDB();
	context.allocator->free(context.typeDB, sizeof TypeDB);
	context.typeDB = nullptr;
}

TypeDB& getTypeDB() {
	assert(context.typeDB);
	return *context.typeDB;
}

const Type& getType(TypeId typeID) {
	return context.typeDB->getType(typeID);
}

const Type* tryGetType(TypeId typeID) {
	return context.typeDB->tryGetType(typeID);
}

} // namespace Typhoon::Reflection
