#pragma once

#include "../src/context.h"
#include "../src/scopedAllocator.h"
#include "../src/typeDB.h"
#include "type.h"

namespace Typhoon::Reflection {

class PointerType : public Type {
public:
	PointerType(TypeId typeID, size_t size, size_t alignment, const Type* pointedType);
	virtual ~PointerType() = default;

	const Type&         getPointedType() const;
	virtual const void* resolvePointer(const void* ptr) const = 0;
	virtual void*       resolvePointer(void* ptr) const = 0;

private:
	const Type* pointedType;
};

class RawPointerType final : public PointerType {
public:
	RawPointerType(TypeId typeID, size_t size, size_t alignment, const Type* pointedType);

	const void* resolvePointer(const void* ptr) const override;
	void*       resolvePointer(void* ptr) const override;
};

namespace detail {

// Specialization for raw pointers
template <class T>
struct autoRegisterHelper<T*> {
	static const Type* autoRegister(Context& context) {
		using pointer_type = std::add_pointer_t<T*>;
		using non_const_type = std::remove_const_t<T>;
		const Type* valueType = autoRegisterType<non_const_type>(context);
		assert(valueType);
		auto type = context.scopedAllocator->make<RawPointerType>(getTypeId<pointer_type>(), sizeof(pointer_type), alignof(pointer_type), valueType);
		context.typeDB->registerType(type);
		return type;
	}
};

} // namespace detail

} // namespace Typhoon::Reflection
