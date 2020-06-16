#pragma once

#include "type.h"
#include <memory>

namespace Typhoon {

class ReadIterator {
public:
	virtual ~ReadIterator() = default;

	virtual const void* getKey() = 0;
	virtual const void* getValue() = 0;
	virtual size_t      getCount() const = 0;
	virtual void        gotoNext() = 0;
	virtual bool        isValid() const = 0;
};

class WriteIterator {
public:
	virtual ~WriteIterator() = default;

	virtual void* pushBack() = 0;
	virtual void* insert(const void* key) = 0;
	virtual bool  isValid() const = 0;
};

class ContainerType : public Type {
public:
	ContainerType(TypeId typeID, size_t size, const Type* keyType, const Type* valueType, const MethodTable& methods);
	virtual ~ContainerType() = default;

	const Type* getKeyType() const {
		return keyType;
	}
	const Type* getValueType() const {
		return valueType;
	}

	virtual bool           isEmpty(const void* container) const = 0;
	virtual ReadIterator*  newReadIterator(void* mem, size_t memSize, const void* data) const = 0;
	virtual WriteIterator* newWriteIterator(void* mem, size_t memSize, void* data) const = 0;
	virtual void           deleteIterator(ReadIterator* iterator) const = 0;
	virtual void           deleteIterator(WriteIterator* iterator) const = 0;
	virtual void           clear(void* container) const = 0;

private:
	const Type* keyType;
	const Type* valueType;
};

} // namespace Typhoon
