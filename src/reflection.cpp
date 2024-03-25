#include "reflection.h"
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
bool readBuiltin(DataPtr data, const InputArchive& archive) {
	return read(*cast<T>(data), archive);
}

template <class T>
void writeBuiltin(ConstDataPtr data, OutputArchive& archive) {
	write(*cast<T>(data), archive);
}

template <>
bool readBuiltin<std::string>(DataPtr data, const InputArchive& archive) {
	const char* cstr = nullptr;
	if (archive.read(cstr)) {
		*cast<std::string>(data) = cstr;
		return true;
	}
	return false;
}

template <>
void writeBuiltin<std::string>(ConstDataPtr data, OutputArchive& archive) {
	archive.write(std::string_view { *cast<std::string>(data) });
}

template <>
bool readBuiltin<std::string_view>(DataPtr data, const InputArchive& archive) {
	return archive.read(*cast<std::string_view>(data));
}

template <>
void writeBuiltin<std::string_view>(ConstDataPtr data, OutputArchive& archive) {
	archive.write(*cast<std::string_view>(data));
}

template <class T>
void createBuiltin(Context& context, const char* typeName) {
	auto type = context.scopedAllocator->make<BuiltinType>(typeName, getTypeId<T>(), sizeof(T), alignof(T), detail::buildMethodTable<T>(),
	                                                       *context.allocator);
	type->setCustomReader(&readBuiltin<T>);
	type->setCustomWriter(&writeBuiltin<T>);
	context.typeDB->registerType(type);
	context.typeDB->getGlobalNamespace().addType(type);
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
	CREATE_BUILTIN(std::string_view, context);

	auto variantType = context.scopedAllocator->make<VariantType>(*context.allocator);
	context.typeDB->registerType(variantType);
	context.typeDB->getGlobalNamespace().addType(variantType);
}

HeapAllocator defaultAllocator;
Context       defaultContext {};

} // namespace

void initReflection() {
	initReflection(defaultAllocator);
}

void initReflection(Allocator& allocator) {
	auto& context = defaultContext;
	assert(! context.typeDB);

	context.allocator = &allocator;
	context.pagedAllocator = allocator.construct<PagedAllocator>(allocator, PagedAllocator::defaultPageSize);
	context.scopedAllocator = allocator.construct<ScopedAllocator>(*context.pagedAllocator);
	context.typeDB = context.scopedAllocator->make<TypeDB>(allocator, *context.scopedAllocator);
	registerBuiltinTypes(context);
}

void deinitReflection() {
	auto& context = defaultContext;
	assert(context.scopedAllocator);
	context.allocator->destroy(context.scopedAllocator);
	context.allocator->destroy(context.pagedAllocator);
	context.scopedAllocator = nullptr;
	context.typeDB = nullptr;
	context.allocator = nullptr;
}

bool isInitialized() {
	return defaultContext.allocator != nullptr;
}

const Type& getType(TypeId typeID) {
	return defaultContext.typeDB->getType(typeID);
}

const Type* tryGetType(TypeId typeID) {
	return defaultContext.typeDB->tryGetType(typeID);
}

const Namespace& getGlobalNamespace() {
	return defaultContext.typeDB->getGlobalNamespace();
}

namespace detail {

Context& getContext() {
	return defaultContext;
}

TypeDB& getTypeDB() {
	assert(defaultContext.typeDB);
	return *defaultContext.typeDB;
}

} // namespace detail

} // namespace Typhoon::Reflection
