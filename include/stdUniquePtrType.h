#pragma once

#include "pointerType.h"
#include <memory>

namespace Typhoon::Reflection::detail {

template <typename T>
class StdUniquePointerType final : public PointerType {
public:
	StdUniquePointerType(const char* typeName, TypeId typeID, size_t size, size_t alignment, const Type* pointedType);

	ConstDataPtr resolvePointer(ConstDataPtr ptr) const override;
	DataPtr      resolvePointer(DataPtr ptr) const override;
};

template <typename T>
inline StdUniquePointerType<T>::StdUniquePointerType(const char* typeName, TypeId typeID, size_t size, size_t alignment, const Type* pointedType)
    : PointerType { typeName, typeID, size, alignment, pointedType } {
}

template <typename T>
inline ConstDataPtr StdUniquePointerType<T>::resolvePointer(ConstDataPtr data) const {
	ConstDataPtr pointer = nullptr;
	const auto&  uniquePtr = *castPointer<std::unique_ptr<T>>(data);
	const T*     srcPtr = uniquePtr.get();
	std::memcpy(&pointer, &srcPtr, sizeof pointer);
	return pointer;
}

template <typename T>
inline DataPtr StdUniquePointerType<T>::resolvePointer(DataPtr data) const {
	DataPtr  pointer = nullptr;
	auto&    uniquePtr = *castPointer<std::unique_ptr<T>>(data);
	const T* srcPtr = uniquePtr.get();
	std::memcpy(&pointer, &srcPtr, sizeof pointer);
	return pointer;
}

const char* decorateTypeName(const char* typeName, const char* prefix, const char* suffix, ScopedAllocator& alloc);

// Specialization for std::unique_ptr
template <class T>
struct autoRegisterHelper<std::unique_ptr<T>> {
	static const Type* autoRegister(Context& context) {
		using PointerType = std::unique_ptr<T>;
		const Type* valueType = autoRegisterType<T>(context);
		assert(valueType);
		const char* typeName = decorateTypeName(valueType->getName(), "std::unique_ptr<", ">", *context.scopedAllocator);
		auto        type = context.scopedAllocator->make<StdUniquePointerType<T>>(typeName, getTypeId<PointerType>(), sizeof(PointerType),
                                                                           alignof(PointerType), valueType);
		context.typeDB->registerType(type);
		return type;
	}
};

} // namespace Typhoon::Reflection::detail
