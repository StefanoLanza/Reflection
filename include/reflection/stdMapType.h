#pragma once

#include "containerType.h"
#include "context.h"
#include "typeDB.h"
#include <core/scopedAllocator.h>

#include <cassert>
#include <map>

namespace Typhoon::Reflection::detail {

template <typename MAP_TYPE>
class StdMapReadIterator final : public ReadIterator {
public:
	StdMapReadIterator(const MAP_TYPE* container)
	    : container(container)
	    , iter(container->begin()) {
	}

	ConstDataPtr getKey() override {
		return &iter->first;
	}
	ConstDataPtr getValue() override {
		return &iter->second;
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
	const MAP_TYPE*                   container;
	typename MAP_TYPE::const_iterator iter;
};

template <typename MAP_TYPE>
class StdMapWriteIterator final : public WriteIterator {
public:
	StdMapWriteIterator(MAP_TYPE* container)
	    : container(container)
	    , iter(container->begin()) {
	}

	DataPtr pushBack() override {
		assert(0);
		return nullptr;
	}
	bool isValid() const override {
		return true;
	}
	DataPtr insert(ConstDataPtr key) override {
		// Insert a temporary value
		mapped_type               tmp {};
		std::pair<iterator, bool> res = container->insert(std::make_pair(*cast<key_type>(key), tmp));
		// Return pointer to mapped value
		return &res.first->second;
	}

private:
	using iterator = typename MAP_TYPE::iterator;
	using key_type = typename MAP_TYPE::key_type;
	using mapped_type = typename MAP_TYPE::mapped_type;

	MAP_TYPE* container;
	iterator  iter;
};

template <typename MAP_TYPE>
class StdMapContainer final : public ContainerType {
public:
	StdMapContainer(const char* typeName, TypeId typeID, const Type* keyType, const Type* valueType)
	    : ContainerType(typeName, typeID, sizeof(MAP_TYPE), keyType, valueType, buildMethodTable<MAP_TYPE>()) {
	}

	bool isEmpty(ConstDataPtr container) const override {
		return cast<MAP_TYPE>(container)->empty();
	}

	ReadIterator* newReadIterator(ConstDataPtr container, ScopedAllocator& allocator) const override {
		return allocator.make<ReadIteratorType>(cast<MAP_TYPE>(container));
	}

	WriteIterator* newWriteIterator(DataPtr container, ScopedAllocator& allocator) const override {
		return allocator.make<WriteIteratorType>(cast<MAP_TYPE>(container));
	}
	void clear(DataPtr container) const override {
		cast<MAP_TYPE>(container)->clear();
	}

private:
	using ReadIteratorType = StdMapReadIterator<MAP_TYPE>;
	using WriteIteratorType = StdMapWriteIterator<MAP_TYPE>;
};

const char* buildTemplateTypeName(const char* typeNames[], const char* prefix, const char* suffix, ScopedAllocator& alloc);

// std::map specialization
template <class _Kty, class T>
struct autoRegisterHelper<std::map<_Kty, T>> {
	static const Type* autoRegister(Context& context) {
		using KeyType = _Kty;
		using MappedType = T;
		using ContainerType = std::map<_Kty, T>;
		const Type*      keyType = autoRegisterType<KeyType>(context);
		const Type*      mappedType = autoRegisterType<MappedType>(context);
		constexpr TypeId typeID = getTypeId<ContainerType>();
		const char*      innerTypeNames[] = { keyType->getName(), mappedType->getName(), nullptr };
		const char*      typeName = buildTemplateTypeName(innerTypeNames, "std::map<", ">", *context.scopedAllocator);
		auto             type = context.scopedAllocator->make<StdMapContainer<ContainerType>>(typeName, typeID, keyType, mappedType);
		context.typeDB->registerType(type);
		return type;
	}
};

} // namespace Typhoon::Reflection::detail
