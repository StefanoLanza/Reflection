#include "../include/reflection.h"
#include "allocator.h"
#include "archive.h"
#include "builtinType.h"
#include "serializeBuiltIns.h"
#include "variantType.h"
#include <cstdint>
#include <string>

namespace Typhoon::Reflection {

struct Context {
	TypeDB*    typeDB;
	Allocator* allocator;
};

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
void createBuiltin(TypeDB& typeDB, Allocator& allocator) {
	BuiltinType* type = allocator.make<BuiltinType>(getTypeId<T>(), sizeof(T), alignof(T), detail::buildMethodTable<T>());
	type->setCustomReader(&readBuiltin<T>);
	type->setCustomWriter(&writeBuiltin<T>);
	typeDB.registerType(type);
}

void registerBuiltinTypes(TypeDB& typeDB, Allocator& allocator) {
	createBuiltin<bool>(typeDB, allocator);
	createBuiltin<char>(typeDB, allocator);
	createBuiltin<unsigned char>(typeDB, allocator);
	createBuiltin<short>(typeDB, allocator);
	createBuiltin<unsigned short>(typeDB, allocator);
	createBuiltin<int>(typeDB, allocator);
	createBuiltin<unsigned int>(typeDB, allocator);
	createBuiltin<long>(typeDB, allocator);
	createBuiltin<unsigned long>(typeDB, allocator);
	createBuiltin<long long>(typeDB, allocator);
	createBuiltin<unsigned long long>(typeDB, allocator);
	createBuiltin<float>(typeDB, allocator);
	createBuiltin<double>(typeDB, allocator);
	createBuiltin<const char*>(typeDB, allocator);
	createBuiltin<std::string>(typeDB, allocator);

	typeDB.registerType(allocator.make<VariantType>());
}

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

void initReflection() {
	initReflection(defaultAllocator);
}

void initReflection(Allocator& allocator) {
	assert(! context.typeDB);

	context.allocator = &allocator;
	context.typeDB = allocator.make<TypeDB>(std::ref(allocator));
	registerBuiltinTypes(*context.typeDB, allocator);
}

void deinitReflection() {
	context.allocator->destroy(context.typeDB);
	context.typeDB = nullptr;
}

const Type& getType(TypeId typeID) {
	return context.typeDB->getType(typeID);
}

const Type* tryGetType(TypeId typeID) {
	return context.typeDB->tryGetType(typeID);
}

namespace detail {

TypeDB& getTypeDB() {
	assert(context.typeDB);
	return *context.typeDB;
}

Allocator& getAllocator() {
	assert(context.allocator);
	return *context.allocator;
}

} // namespace detail

} // namespace Typhoon::Reflection
