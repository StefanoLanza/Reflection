#pragma once

#include "pointerType.h"

#include <memory>

namespace Typhoon::Reflection {

namespace detail {

template <typename T>
class StdSharedPointerType final : public PointerType {
public:
	StdSharedPointerType(TypeId typeID, size_t size, size_t alignment, const Type* pointedType);

	const void* resolvePointer(const void* ptr) const override;
	void*       resolvePointer(void* ptr) const override;
};

template <typename T>
inline StdSharedPointerType<T>::StdSharedPointerType(TypeId typeID, size_t size, size_t alignment, const Type* pointedType)
    : PointerType { typeID, size, alignment, pointedType } {
}

template <typename T>
inline const void* StdSharedPointerType<T>::resolvePointer(const void* data) const {
	const void* pointer = nullptr;
	const auto& sharedPtr = *reinterpret_cast<const std::shared_ptr<T>*>(data);
	const T*    srcPtr = sharedPtr.get();
	std::memcpy(&pointer, &srcPtr, sizeof pointer);
	return pointer;
}

template <typename T>
inline void* StdSharedPointerType<T>::resolvePointer(void* data) const {
	void*    pointer = nullptr;
	auto&    sharedPtr = *reinterpret_cast<std::shared_ptr<T>*>(data);
	const T* srcPtr = sharedPtr.get();
	std::memcpy(&pointer, &srcPtr, sizeof pointer);
	return pointer;
}

// Specialization for std::shared_ptr
template <class _Ty>
struct autoRegisterHelper<std::shared_ptr<_Ty>> {
	static const Type* autoRegister(TypeDB& typeDB) {
		using pointer_type = std::shared_ptr<_Ty>;
		const Type* valueType = autoRegisterType<_Ty>(typeDB);
		assert(valueType);
		static const StdSharedPointerType<_Ty> types { getTypeId<pointer_type>(), sizeof(pointer_type), std::alignment_of_v<pointer_type>,
			                                           valueType };
		typeDB.registerType(&types);
		return &types;
	}
};

} // namespace detail

} // namespace Typhoon
