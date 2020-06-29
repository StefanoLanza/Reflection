#pragma once

#include "containerType.h"
#include "typeDB.h"
#include <cassert>
#include <map>

namespace Typhoon {

namespace detail {

template <typename MAP_TYPE>
class StdMapReadIterator final : public ReadIterator {
public:
	StdMapReadIterator(const MAP_TYPE* container)
	    : container(container)
	    , iter(container->begin()) {
	}

	const void* getKey() override {
		return &iter->first;
	}
	const void* getValue() override {
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

	void SetKey(const char*) {
	}
	void SetValue(const char* data) {
		// container[position] = *reinterpret_cast<const TYPE*>(data);
	}
	void* pushBack() override {
		assert(0);
		return nullptr;
	}
	bool isValid() const override {
		return true;
	}
	void* insert(const void* key) override {
		// Insert a temporary value
		mapped_type               tmp;
		std::pair<iterator, bool> res = container->insert(std::make_pair(*reinterpret_cast<const key_type*>(key), tmp));
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
	StdMapContainer(TypeId typeID, const Type* keyType, const Type* valueType)
	    : ContainerType(typeID, sizeof(MAP_TYPE), keyType, valueType, buildMethodTable<MAP_TYPE>()) {
	}

	bool isEmpty(const void* container) const override {
		return reinterpret_cast<const MAP_TYPE*>(container)->empty();
	}

	ReadIterator* newReadIterator(void* mem, size_t memSize, const void* container) const override {
		assert(sizeof(ReadIteratorType) <= memSize);
		(void)memSize;
		return new (mem) ReadIteratorType(reinterpret_cast<const MAP_TYPE*>(container));
	}

	WriteIterator* newWriteIterator(void* mem, size_t memSize, void* container) const override {
		assert(sizeof(WriteIteratorType) <= memSize);
		(void)memSize;
		return new (mem) WriteIteratorType(reinterpret_cast<MAP_TYPE*>(container));
	}
	void deleteIterator(ReadIterator* iterator) const override {
		iterator->~ReadIterator();
	}
	void deleteIterator(WriteIterator* iterator) const override {
		iterator->~WriteIterator();
	}
	void clear(void* container) const override {
		reinterpret_cast<MAP_TYPE*>(container)->clear();
	}

private:
	using ReadIteratorType = StdMapReadIterator<MAP_TYPE>;
	using WriteIteratorType = StdMapWriteIterator<MAP_TYPE>;
};

// std::map specialization
template <class _Kty, class _Ty>
struct autoRegisterHelper<std::map<_Kty, _Ty>> {
	static const Type* autoRegister(TypeDB& typeDB) {
		using key_type = _Kty;
		using mapped_type = _Ty;
		using container_type = std::map<_Kty, _Ty>;
		const Type*                                  keyType = autoRegisterType<key_type>(typeDB);
		const Type*                                  mappedType = autoRegisterType<mapped_type>(typeDB);
		constexpr TypeId                             typeID = getTypeId<container_type>();
		static const StdMapContainer<container_type> types { typeID, keyType, mappedType };
		typeDB.registerType(&types);
		return &types;
	}
};

} // namespace detail

} // namespace Typhoon