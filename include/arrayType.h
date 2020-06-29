#pragma once

#include "containerType.h"
#include "typeDB.h"
#include <cassert>

namespace Typhoon {

namespace detail {

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

	ReadIterator* newReadIterator(void* mem, size_t memSize, const void* container) const override {
		assert(sizeof(ReadIteratorType) <= memSize);
		(void)memSize;
		return new (mem) ReadIteratorType(reinterpret_cast<const TYPE*>(container));
	}

	WriteIterator* newWriteIterator(void* mem, size_t memSize, void* container) const override {
		assert(sizeof(WriteIteratorType) <= memSize);
		(void)memSize;
		return new (mem) WriteIteratorType(reinterpret_cast<TYPE*>(container));
	}
	void deleteIterator(ReadIterator* iterator) const override {
		iterator->~ReadIterator();
	}
	void deleteIterator(WriteIterator* iterator) const override {
		iterator->~WriteIterator();
	}
	void clear(void* container) const override {
		(void)container;
	}

private:
	using ReadIteratorType = ArrayReadIterator<TYPE, LENGTH>;
	using WriteIteratorType = ArrayWriteIterator<TYPE, LENGTH>;
};

// C-style array
template <class _Ty, size_t N>
struct autoRegisterHelper<_Ty[N]> {
	static const Type* autoRegister(TypeDB& typeDB) {
		using element_type = _Ty;
		using container_type = _Ty[N];
		const Type*                                  elementType = autoRegisterType<element_type>(typeDB);
		constexpr TypeId                             typeID = getTypeId<container_type>();
		static const ArrayContainer<element_type, N> typeInfo { typeID, elementType };
		typeDB.registerType(&typeInfo);
		return &typeInfo;
	}
};

} // namespace detail

} // namespace Typhoon
