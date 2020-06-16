#pragma once

#include "type.h"
#include "typeDB.h"

namespace Typhoon {

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
template <class _Ty>
struct autoRegisterHelper<_Ty*> {
	static const Type* autoRegister(TypeDB& typeDB) {
		using pointer_type = std::add_pointer_t<_Ty*>;
		using non_const_type = std::remove_const_t<_Ty>;
		const Type* valueTypeInfo = autoRegisterType<non_const_type>(typeDB);
		assert(valueTypeInfo);
		static const RawPointerType typeInfo { getTypeId<pointer_type>(), sizeof(pointer_type), std::alignment_of_v<pointer_type>, valueTypeInfo };
		typeDB.registerType(&typeInfo);
		return &typeInfo;
	}
};

} // namespace detail

} // namespace Typhoon
