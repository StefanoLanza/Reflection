#pragma once

#include "type.h"

#include <memory>

namespace Typhoon {

class ScopedAllocator;

}

namespace Typhoon::Reflection {

class ReadIterator {
public:
	virtual ~ReadIterator() = default;

	virtual ConstDataPtr getKey() = 0;
	virtual ConstDataPtr getValue() = 0;
	virtual size_t       getCount() const = 0;
	virtual void         gotoNext() = 0;
	virtual bool         isValid() const = 0;
};

class WriteIterator {
public:
	virtual ~WriteIterator() = default;

	virtual DataPtr pushBack() = 0;
	virtual DataPtr insert(ConstDataPtr key) = 0;
	virtual bool    isValid() const = 0;
};

class ContainerType : public Type {
public:
	ContainerType(const char* typeName, TypeId typeID, size_t size, const Type* keyType, const Type* valueType, const MethodTable& methods);
	virtual ~ContainerType() = default;

	const Type* getKeyType() const {
		return keyType;
	}
	const Type* getValueType() const {
		return valueType;
	}

	virtual bool           isEmpty(ConstDataPtr container) const = 0;
	virtual ReadIterator*  newReadIterator(ConstDataPtr container, ScopedAllocator& allocator) const = 0;
	virtual WriteIterator* newWriteIterator(DataPtr container, ScopedAllocator& allocator) const = 0;
	virtual void           clear(DataPtr container) const = 0;

private:
	const Type* keyType;
	const Type* valueType;
};

} // namespace Typhoon::Reflection
