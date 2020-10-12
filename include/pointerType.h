#pragma once

#include "context.h"
#include "typeDB.h"
#include "type.h"
#include <core/scopedAllocator.h>

namespace Typhoon::Reflection {

class PointerType : public Type {
public:
	PointerType(const char* typeName, TypeId typeID, size_t size, size_t alignment, const Type* pointedType);
	virtual ~PointerType() = default;

	const Type&         getPointedType() const;
	virtual const void* resolvePointer(const void* ptr) const = 0;
	virtual void*       resolvePointer(void* ptr) const = 0;

private:
	const Type* pointedType;
};

namespace detail {

class RawPointerType final : public PointerType {
public:
	RawPointerType(const char* typeName, TypeId typeID, size_t size, size_t alignment, const Type* pointedType);

	const void* resolvePointer(const void* ptr) const override;
	void*       resolvePointer(void* ptr) const override;
};

const char* decorateTypeName(const char* typeName, const char* prefix, const char* suffix, ScopedAllocator& alloc);

// Specialization for raw pointers
template <class T>
struct autoRegisterHelper<T*> {
	static const Type* autoRegister(Context& context) {
		using pointer_type = std::add_pointer_t<T*>;
		using non_const_type = std::remove_const_t<T>;
		const Type* valueType = autoRegisterType<non_const_type>(context);
		const char* typeName = decorateTypeName(valueType->getName(), "", "*", *context.scopedAllocator);
		auto type = context.scopedAllocator->make<RawPointerType>(typeName, getTypeId<pointer_type>(), sizeof(pointer_type), alignof(pointer_type),
		                                                          valueType);
		context.typeDB->registerType(type);
		return type;
	}
};

} // namespace detail

} // namespace Typhoon::Reflection
