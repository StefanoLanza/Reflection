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

	const void* getKey() override {
		return nullptr;
	};
	const void* getValue() override {
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

	void* insert(const void* key) override {
		assert(false);
		(void)key;
		return nullptr;
	}
	void* pushBack() override {
		assert(position < LENGTH);
		void* value = &container[position];
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
	ArrayContainer(TypeId typeID, const Type* valueType)
	    : ContainerType(typeID, sizeof(TYPE) * LENGTH, nullptr, valueType, {}) {
	}

	bool isEmpty(const void* /*container*/) const override {
		return LENGTH == 0;
	}

	ReadIterator* newReadIterator(const void* container, ScopedAllocator& allocator) const override {
		return allocator.make<ReadIteratorType>(reinterpret_cast<const TYPE*>(container));
	}

	WriteIterator* newWriteIterator(void* container, ScopedAllocator& allocator) const override {
		return allocator.make<WriteIteratorType>(reinterpret_cast<TYPE*>(container));
	}
	void clear([[maybe_unused]] void* container) const override {
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
		auto             type = context.scopedAllocator->make<ArrayContainer<ElementType, N>>(typeID, elementType);
		context.typeDB->registerType(type);
		return type;
	}
};

} // namespace Typhoon::Reflection::detail
