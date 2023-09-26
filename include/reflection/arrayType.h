#pragma once

#include "containerType.h"
#include "context.h"
#include "typeDB.h"
#include <cassert>
#include <core/scopedAllocator.h>

namespace Typhoon::Reflection::detail {

template <typename TYPE, size_t LENGTH>
class ArrayReadIterator : public ReadIterator {
public:
	ArrayReadIterator(const TYPE* container)
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
		return LENGTH;
	}
	void gotoNext() override {
		++position;
	}
	bool isValid() const override {
		return position < LENGTH;
	}

private:
	const TYPE* container;
	size_t      position;
};

template <typename TYPE, size_t LENGTH>
class ArrayWriteIterator : public WriteIterator {
public:
	ArrayWriteIterator(TYPE* container)
	    : container(container)
	    , position(0) {
	}

	DataPtr insert([[maybe_unused]] ConstDataPtr key) override {
		assert(false);
		return nullptr;
	}
	DataPtr pushBack() override {
		assert(position < LENGTH);
		DataPtr value = &container[position];
		++position;
		return value;
	}
	bool isValid() const override {
		return position < LENGTH;
	}

private:
	TYPE*  container;
	size_t position;
};

template <typename TYPE, size_t LENGTH>
class ArrayContainer : public ContainerType {
public:
	ArrayContainer(const char* typeName, TypeId typeID, const Type* valueType, Allocator& allocator)
	    : ContainerType(typeName, typeID, sizeof(TYPE) * LENGTH, nullptr, valueType, {}, allocator) {
	}

	ReadIterator* newReadIterator(ConstDataPtr container, ScopedAllocator& allocator) const override {
		return allocator.make<ReadIteratorType>(cast<TYPE>(container));
	}

	WriteIterator* newWriteIterator(DataPtr container, ScopedAllocator& allocator) const override {
		return allocator.make<WriteIteratorType>(cast<TYPE>(container));
	}

private:
	using ReadIteratorType = ArrayReadIterator<TYPE, LENGTH>;
	using WriteIteratorType = ArrayWriteIterator<TYPE, LENGTH>;
};

// C-style array
template <class T, size_t N>
struct autoRegisterHelper<T[N]> {
	static const Type* autoRegister(Context& context) {
		using ElementType = T;
		using ContainerType = T[N];
		const Type*      elementType = autoRegisterType<ElementType>(context);
		constexpr TypeId typeID = getTypeId<ContainerType>();
		const char*      typeName = decorateTypeName(elementType->getName(), "", "[]", *context.scopedAllocator);
		return context.scopedAllocator->make<ArrayContainer<ElementType, N>>(typeName, typeID, elementType, *context.allocator);
	}
};

} // namespace Typhoon::Reflection::detail
