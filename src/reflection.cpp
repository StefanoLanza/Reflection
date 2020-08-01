#include "../include/reflection.h"
#include "allocator.h"
#include "archive.h"
#include "builtinType.h"
#include "context.h"
#include "scopedAllocator.h"
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
void createBuiltin(Context& context) {
	BuiltinType* type = context.scopedAllocator->make<BuiltinType>(getTypeId<T>(), sizeof(T), alignof(T), detail::buildMethodTable<T>());
	type->setCustomReader(&readBuiltin<T>);
	type->setCustomWriter(&writeBuiltin<T>);
	context.typeDB->registerType(type);
}

void registerBuiltinTypes(Context& context) {
	createBuiltin<bool>(context);
	createBuiltin<char>(context);
	createBuiltin<unsigned char>(context);
	createBuiltin<short>(context);
	createBuiltin<unsigned short>(context);
	createBuiltin<int>(context);
	createBuiltin<unsigned int>(context);
	createBuiltin<long>(context);
	createBuiltin<unsigned long>(context);
	createBuiltin<long long>(context);
	createBuiltin<unsigned long long>(context);
	createBuiltin<float>(context);
	createBuiltin<double>(context);
	createBuiltin<const char*>(context);
	createBuiltin<std::string>(context);

	context.typeDB->registerType(context.scopedAllocator->make<VariantType>());
}

HeapAllocator defaultAllocator;
Context       context {};

} // namespace

void initReflection() {
	initReflection(defaultAllocator);
}

void initReflection(Allocator& allocator) {
	assert(! context.typeDB);

	context.allocator = &allocator;
	context.scopedAllocator = new (allocator.alloc<ScopedAllocator>()) ScopedAllocator(allocator);
	context.typeDB = context.scopedAllocator->make<TypeDB>(std::ref(allocator));
	registerBuiltinTypes(context);
}

void deinitReflection() {
	assert(context.scopedAllocator);
	context.scopedAllocator->~ScopedAllocator();
	context.allocator->free(context.scopedAllocator, sizeof(ScopedAllocator));
	context.scopedAllocator = nullptr;
	context.typeDB = nullptr;
	context.allocator = nullptr;
}

const Type& getType(TypeId typeID) {
	return context.typeDB->getType(typeID);
}

const Type* tryGetType(TypeId typeID) {
	return context.typeDB->tryGetType(typeID);
}

namespace detail {

Context& getContext() {
	return context;
}

TypeDB& getTypeDB() {
	assert(context.typeDB);
	return *context.typeDB;
}

} // namespace detail

} // namespace Typhoon::Reflection
