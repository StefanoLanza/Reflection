#pragma once

#include "pointerType.h"

#include <memory>

namespace Typhoon::Reflection::detail {

template <typename T>
class StdSharedPointerType final : public PointerType {
public:
	StdSharedPointerType(const char* typeName, TypeId typeID, size_t size, size_t alignment, const Type* pointedType);

	const void* resolvePointer(const void* ptr) const override;
	void*       resolvePointer(void* ptr) const override;
};

template <typename T>
inline StdSharedPointerType<T>::StdSharedPointerType(const char* typeName, TypeId typeID, size_t size, size_t alignment, const Type* pointedType)
    : PointerType { typeName, typeID, size, alignment, pointedType } {
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

const char* decorateTypeName(const char* typeName, const char* prefix, const char* suffix, ScopedAllocator& alloc);

// Specialization for std::shared_ptr
template <class T>
struct autoRegisterHelper<std::shared_ptr<T>> {
	static const Type* autoRegister(Context& context) {
		using PointerType = std::shared_ptr<T>;
		const Type* valueType = autoRegisterType<T>(context);
		assert(valueType);
		const char* typeName = decorateTypeName(valueType->getName(), "std::shared_ptr<", ">", *context.scopedAllocator);
		auto        type =
		    context.scopedAllocator->make<StdSharedPointerType<T>>(typeName, getTypeId<PointerType>(), sizeof(PointerType), alignof(PointerType), valueType);
		context.typeDB->registerType(type);
		return type;
	}
};

} // namespace Typhoon::Reflection::detail
