#include "../include/reflection.h"
#include "archive.h"
#include "builtinType.h"
#include "context.h"
#include "serializeBuiltIns.h"
#include "variantType.h"
#include <core/scopedAllocator.h>
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
	archive.write(static_cast<const std::string*>(data)->c_str());
	return true;
}

template <class T>
void createBuiltin(Context& context, const char* typeName) {
	BuiltinType* type = context.scopedAllocator->make<BuiltinType>(typeName, getTypeId<T>(), sizeof(T), alignof(T), detail::buildMethodTable<T>());
	type->setCustomReader(&readBuiltin<T>);
	type->setCustomWriter(&writeBuiltin<T>);
	context.typeDB->registerType(type);
}

#define CREATE_BUILTIN(type, context) createBuiltin<type>(context, #type)

void registerBuiltinTypes(Context& context) {
	CREATE_BUILTIN(bool, context);
	CREATE_BUILTIN(char, context);
	CREATE_BUILTIN(unsigned char, context);
	CREATE_BUILTIN(short, context);
	CREATE_BUILTIN(unsigned short, context);
	CREATE_BUILTIN(int, context);
	CREATE_BUILTIN(unsigned int, context);
	CREATE_BUILTIN(long, context);
	CREATE_BUILTIN(unsigned long, context);
	CREATE_BUILTIN(long long, context);
	CREATE_BUILTIN(unsigned long long, context);
	CREATE_BUILTIN(float, context);
	CREATE_BUILTIN(double, context);
	CREATE_BUILTIN(const char*, context);
	CREATE_BUILTIN(std::string, context);

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
	context.typeDB = context.scopedAllocator->make<TypeDB>(std::ref(allocator), std::ref(*context.scopedAllocator));
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
