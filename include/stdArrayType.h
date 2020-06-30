#pragma once

#include "containerType.h"
#include "typeDB.h"
#include <array>

namespace Typhoon::Reflection {

namespace detail {

template <typename TYPE, size_t L>
class StdArrayReadIterator final : public ReadIterator {
public:
	StdArrayReadIterator(const TYPE* container)
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

	void* insert(const void* key) override {
		assert(false);
		(void)key;
		return nullptr;
	}
	void* pushBack() override {
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

	StdArrayContainer(TypeId typeID, const Type* valueType)
	    : ContainerType(typeID, sizeof(array_type), nullptr, valueType, buildMethodTable<array_type>()) {
	}

	bool isEmpty(const void* container) const override {
		(void)container;
		return L == 0;
	}

	ReadIterator* newReadIterator(void* mem, size_t memSize, const void* container) const override {
		assert(sizeof(ReadIteratorType) <= memSize);
		(void)memSize;
		return new (mem) ReadIteratorType(reinterpret_cast<const T*>(container));
	}

	WriteIterator* newWriteIterator(void* mem, size_t memSize, void* container) const override {
		assert(sizeof(WriteIteratorType) <= memSize);
		(void)memSize;
		return new (mem) WriteIteratorType(reinterpret_cast<T*>(container));
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
	using ReadIteratorType = StdArrayReadIterator<T, L>;
	using WriteIteratorType = StdArrayWriteIterator<T, L>;
};

template <class _Ty, size_t N>
struct autoRegisterHelper<std::array<_Ty, N>> {
	static const Type* autoRegister(TypeDB& typeDB) {
		using element_type = _Ty;
		using container_type = std::array<_Ty, N>;
		const Type*                                     elementType = autoRegisterType<element_type>(typeDB);
		constexpr TypeId                                typeID = getTypeId<container_type>();
		static const StdArrayContainer<element_type, N> types { typeID, elementType };
		typeDB.registerType(&types);
		return &types;
	}
};

} // namespace detail

} // namespace Typhoon
