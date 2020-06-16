#pragma once

#include "containerType.h"
#include "typeDB.h"
#include <cassert>
#include <vector>

namespace Typhoon {

namespace detail {

template <typename VECTOR_TYPE>
class StdVectorReadIterator final : public ReadIterator {
public:
	StdVectorReadIterator(const VECTOR_TYPE* container)
	    : container(container)
	    , iter(container->begin()) {
	}

	const void* getKey() override {
		return nullptr;
	}
	const void* getValue() override {
		return &*iter;
	}
	size_t getCount() const override {
		return container->size();
	}
	void gotoNext() override {
		++iter;
	}
	bool isValid() const override {
		return iter != container->end();
	}

private:
	const VECTOR_TYPE*                   container;
	typename VECTOR_TYPE::const_iterator iter;
};

template <typename VECTOR_TYPE>
class StdVectorWriteIterator final : public WriteIterator {
public:
	StdVectorWriteIterator(VECTOR_TYPE* container)
	    : container(container)
	    , iter(container->begin()) {
	}

	void* insert(const void* key) override {
		assert(false);
		(void)key;
		return nullptr;
	}

	void* pushBack() override {
		container->resize(container->size() + 1);
		return &container->back();
	}

	bool isValid() const override {
		return true;
	}

private:
	VECTOR_TYPE*                         container;
	typename VECTOR_TYPE::const_iterator iter;
};

template <typename VECTOR_TYPE>
class StdVectorContainer final : public ContainerType {
public:
	StdVectorContainer(TypeId typeID, const Type* valueType)
	    : ContainerType(typeID, sizeof(VECTOR_TYPE), nullptr, valueType, buildMethodTable<VECTOR_TYPE>()) {
	}

	bool isEmpty(const void* container) const override {
		return reinterpret_cast<const VECTOR_TYPE*>(container)->empty();
	}

	ReadIterator* newReadIterator(void* mem, size_t memSize, const void* container) const override {
		assert(sizeof(ReadIteratorType) <= memSize);
		(void)memSize;
		return new (mem) ReadIteratorType(reinterpret_cast<const VECTOR_TYPE*>(container));
	}

	WriteIterator* newWriteIterator(void* mem, size_t memSize, void* container) const override {
		assert(sizeof(WriteIteratorType) <= memSize);
		(void)memSize;
		return new (mem) WriteIteratorType(reinterpret_cast<VECTOR_TYPE*>(container));
	}

	void deleteIterator(ReadIterator* iterator) const override {
		iterator->~ReadIterator();
	}

	void deleteIterator(WriteIterator* iterator) const override {
		iterator->~WriteIterator();
	}

	void clear(void* container) const override {
		reinterpret_cast<VECTOR_TYPE*>(container)->clear();
	}

private:
	using ReadIteratorType = StdVectorReadIterator<VECTOR_TYPE>;
	using WriteIteratorType = StdVectorWriteIterator<VECTOR_TYPE>;
};

// std::vector specialization
template <class _Ty>
struct autoRegisterHelper<std::vector<_Ty>> {
	static const Type* autoRegister(TypeDB& typeDB) {
		using value_type = _Ty;
		using container_type = std::vector<_Ty>;
		// AutoRegister valueType (it might be a pointer or container etc)
		const Type*                                     valueType = autoRegisterType<value_type>(typeDB);
		constexpr TypeId                                typeID = getTypeId<container_type>();
		static const StdVectorContainer<container_type> typeInfo { typeID, valueType };
		typeDB.registerType(&typeInfo);
		return &typeInfo;
	}
};

} // namespace detail

} // namespace Typhoon
