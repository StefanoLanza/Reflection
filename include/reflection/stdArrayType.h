#pragma once

#include "containerType.h"
#include "context.h"
#include "typeDB.h"
#include <core/scopedAllocator.h>

#include <array>
#include <cassert>

namespace Typhoon::Reflection::detail {

template <typename TYPE, size_t L>
class StdArrayReadIterator final : public ReadIterator {
public:
	StdArrayReadIterator(const TYPE* container)
	    : container(container)
	    , position(0) {
	}

	ConstDataPtr getKey() override {
		return nullptr;
	};
	ConstDataPtr getValue() override {
		return &container[position];
	}
	size_t getCount() const override {
		return L;
	}
	void gotoNext() override {
		++position;
	}
	bool isValid() const override {
		return position < L;
	}

private:
	const TYPE* container;
	size_t      position;
};

template <typename TYPE, size_t L>
class StdArrayWriteIterator final : public WriteIterator {
public:
	StdArrayWriteIterator(TYPE* container)
	    : container(container)
	    , position(0) {
	}

	DataPtr insert(ConstDataPtr key) override {
		assert(false);
		(void)key;
		return nullptr;
	}
	DataPtr pushBack() override {
		assert(position < L);
		++position;
		return &container[position - 1];
	}
	bool isValid() const override {
		return position < L;
	}

private:
	TYPE*  container;
	size_t position;
};

template <typename T, size_t L>
class StdArrayContainer final : public ContainerType {
public:
	using array_type = std::array<T, L>;

	StdArrayContainer(const char* typeName, TypeId typeID, const Type* valueType, Allocator& allocator)
	    : ContainerType(typeName, typeID, sizeof(array_type), nullptr, valueType, buildMethodTable<array_type>(), allocator) {
	}

	ReadIterator* newReadIterator(ConstDataPtr container, ScopedAllocator& allocator) const override {
		return allocator.make<ReadIteratorType>(cast<T>(container));
	}

	WriteIterator* newWriteIterator(DataPtr container, ScopedAllocator& allocator) const override {
		return allocator.make<WriteIteratorType>(cast<T>(container));
	}

private:
	using ReadIteratorType = StdArrayReadIterator<T, L>;
	using WriteIteratorType = StdArrayWriteIterator<T, L>;
};

template <class T, size_t N>
struct autoRegisterHelper<std::array<T, N>> {
	static const Type* autoRegister(Context& context) {
		using element_type = T;
		using container_type = std::array<T, N>;
		const Type*      elementType = autoRegisterType<element_type>(context);
		constexpr TypeId typeID = getTypeId<container_type>();
		const char*      typeName = decorateTypeName(elementType->getName(), "std::array<", ">", *context.scopedAllocator);
		return context.scopedAllocator->make<StdArrayContainer<element_type, N>>(typeName, typeID, elementType, *context.allocator);
	}
};

} // namespace Typhoon::Reflection::detail
