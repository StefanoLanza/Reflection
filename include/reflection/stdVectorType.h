#pragma once

#include "containerType.h"
#include "context.h"
#include "typeDB.h"
#include <cassert>
#include <core/scopedAllocator.h>
#include <vector>

namespace Typhoon::Reflection::detail {

template <typename VECTOR_TYPE>
class StdVectorReadIterator final : public ReadIterator {
public:
	StdVectorReadIterator(const VECTOR_TYPE* container)
	    : container(container)
	    , iter(container->begin()) {
	}

	ConstDataPtr getKey() override {
		return nullptr;
	}
	ConstDataPtr getValue() override {
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

	DataPtr insert(ConstDataPtr key) override {
		assert(false);
		(void)key;
		return nullptr;
	}

	DataPtr pushBack() override {
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
	StdVectorContainer(const char* typeName, TypeId typeID, const Type* valueType)
	    : ContainerType(typeName, typeID, sizeof(VECTOR_TYPE), nullptr, valueType, buildMethodTable<VECTOR_TYPE>()) {
	}

	ReadIterator* newReadIterator(ConstDataPtr container, ScopedAllocator& allocator) const override {
		return allocator.make<ReadIteratorType>(cast<VECTOR_TYPE>(container));
	}

	WriteIterator* newWriteIterator(DataPtr container, ScopedAllocator& allocator) const override {
		return allocator.make<WriteIteratorType>(cast<VECTOR_TYPE>(container));
	}

private:
	using ReadIteratorType = StdVectorReadIterator<VECTOR_TYPE>;
	using WriteIteratorType = StdVectorWriteIterator<VECTOR_TYPE>;
};

// std::vector specialization
template <class T>
struct autoRegisterHelper<std::vector<T>> {
	static const Type* autoRegister(Context& context) {
		using ValueType = T;
		using ContainerType = std::vector<T>;
		// AutoRegister valueType (it might be a pointer or container etc)
		const Type*      valueType = autoRegisterType<ValueType>(context);
		constexpr TypeId typeID = getTypeId<ContainerType>();
		const char*      typeName = decorateTypeName(valueType->getName(), "std::vector<", ">", *context.scopedAllocator);
		auto             type = context.scopedAllocator->make<StdVectorContainer<ContainerType>>(typeName, typeID, valueType);
		context.typeDB->registerType(type);
		return type;
	}
};

} // namespace Typhoon::Reflection::detail
