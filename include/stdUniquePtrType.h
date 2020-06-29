#pragma once

#include "pointerType.h"
#include <memory>

namespace Typhoon {

namespace detail {

template <typename T>
class StdUniquePointerType final : public PointerType {
public:
	StdUniquePointerType(TypeId typeID, size_t size, size_t alignment, const Type* pointedType);

	const void* resolvePointer(const void* ptr) const override;
	void*       resolvePointer(void* ptr) const override;
};

template <typename T>
inline StdUniquePointerType<T>::StdUniquePointerType(TypeId typeID, size_t size, size_t alignment, const Type* pointedType)
    : PointerType { typeID, size, alignment, pointedType } {
}

template <typename T>
inline const void* StdUniquePointerType<T>::resolvePointer(const void* data) const {
	const void* pointer = nullptr;
	const auto& uniquePtr = *reinterpret_cast<const std::unique_ptr<T>*>(data);
	const T*    srcPtr = uniquePtr.get();
	std::memcpy(&pointer, &srcPtr, sizeof pointer);
	return pointer;
}

template <typename T>
inline void* StdUniquePointerType<T>::resolvePointer(void* data) const {
	void*    pointer = nullptr;
	auto&    uniquePtr = *reinterpret_cast<std::unique_ptr<T>*>(data);
	const T* srcPtr = uniquePtr.get();
	std::memcpy(&pointer, &srcPtr, sizeof pointer);
	return pointer;
}

// Specialization for std::unique_ptr
template <class _Ty>
struct autoRegisterHelper<std::unique_ptr<_Ty>> {
	static const Type* autoRegister(TypeDB& typeDB) {
		using pointer_type = std::unique_ptr<_Ty>;
		const Type* valueType = autoRegisterType<_Ty>(typeDB);
		assert(valueType);
		static const StdUniquePointerType<_Ty> types { getTypeId<pointer_type>(), sizeof(pointer_type), std::alignment_of_v<pointer_type>,
			                                           valueType };
		typeDB.registerType(&types);
		return &types;
	}
};

} // namespace detail

} // namespace Typhoon
