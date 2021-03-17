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
void readBuiltin(DataPtr data, InputArchive& archive) {
	read(*cast<T>(data), archive);
}

template <class T>
bool writeBuiltin(ConstDataPtr data, OutputArchive& archive) {
	return write(*cast<T>(data), archive);
}

template <>
void readBuiltin<std::string>(DataPtr data, InputArchive& archive) {
	const char* cstr = nullptr;
	if (archive.readString(cstr)) {
		*cast<std::string>(data) = cstr;
	}
}

template <>
bool writeBuiltin<std::string>(ConstDataPtr data, OutputArchive& archive) {
	archive.write(cast<std::string>(data)->c_str());
	return true;
}

template <class T>
void createBuiltin(Context& context, const char* typeName) {
	BuiltinType* type = context.scopedAllocator->make<BuiltinType>(typeName, getTypeId<T>(), sizeof(T), alignof(T), detail::buildMethodTable<T>());
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

	auto variantType = context.scopedAllocator->make<VariantType>();
	context.typeDB->registerType(variantType);
	context.typeDB->getGlobalNamespace().addType(variantType);
}

HeapAllocator  defaultAllocator;
Context        context {};

} // namespace

void initReflection() {
	initReflection(defaultAllocator);
}

void initReflection(Allocator& allocator) {
	assert(! context.typeDB);

	context.allocator = &allocator;
	context.pagedAllocator = allocator.construct<PagedAllocator>(std::ref(allocator), PagedAllocator::defaultPageSize);
	context.scopedAllocator = allocator.construct<ScopedAllocator>(std::ref(*context.pagedAllocator));
	context.typeDB = context.scopedAllocator->make<TypeDB>(std::ref(allocator), std::ref(*context.scopedAllocator));
	registerBuiltinTypes(context);
}

void deinitReflection() {
	assert(context.scopedAllocator);
	context.allocator->destroy(context.scopedAllocator);
	context.allocator->destroy(context.pagedAllocator);
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

const Namespace& getGlobalNamespace() {
	return context.typeDB->getGlobalNamespace();
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
