#pragma once

#include "pointerType.h"

#include <cassert>
#include <cstring>
#include <memory>

namespace Typhoon::Reflection::detail {

template <typename T>
class StdSharedPointerType final : public PointerType {
public:
	StdSharedPointerType(const char* typeName, TypeId typeID, size_t size, size_t alignment, const Type* pointedType);

	ConstDataPtr resolvePointer(ConstDataPtr ptr) const override;
	DataPtr      resolvePointer(DataPtr ptr) const override;
};

template <typename T>
inline StdSharedPointerType<T>::StdSharedPointerType(const char* typeName, TypeId typeID, size_t size, size_t alignment, const Type* pointedType)
    : PointerType { typeName, typeID, size, alignment, pointedType } {
}

template <typename T>
inline ConstDataPtr StdSharedPointerType<T>::resolvePointer(ConstDataPtr data) const {
	ConstDataPtr pointer = nullptr;
	const auto&  sharedPtr = *cast<const std::shared_ptr<T>>(data);
	const T*     srcPtr = sharedPtr.get();
	std::memcpy(&pointer, &srcPtr, sizeof pointer);
	return pointer;
}

template <typename T>
inline DataPtr StdSharedPointerType<T>::resolvePointer(DataPtr data) const {
	DataPtr  pointer = nullptr;
	auto&    sharedPtr = *cast<std::shared_ptr<T>>(data);
	const T* srcPtr = sharedPtr.get();
	std::memcpy(&pointer, &srcPtr, sizeof pointer);
	return pointer;
}

// Specialization for std::shared_ptr
template <class T>
struct autoRegisterHelper<std::shared_ptr<T>> {
	static const Type* autoRegister(Context& context) {
		using PointerType = std::shared_ptr<T>;
		const Type* valueType = autoRegisterType<T>(context);
		assert(valueType);
		const char* typeName = decorateTypeName(valueType->getName(), "std::shared_ptr<", ">", *context.scopedAllocator);
		auto        type = context.scopedAllocator->make<StdSharedPointerType<T>>(typeName, getTypeId<PointerType>(), sizeof(PointerType),
                                                                           alignof(PointerType), valueType);
		context.typeDB->registerType(type);
		return type;
	}
};

} // namespace Typhoon::Reflection::detail
