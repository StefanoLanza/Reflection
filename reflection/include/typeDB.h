#pragma once

#include "config.h"
#include "type.h"

#include <cassert>
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

namespace Typhoon {

class InputArchive;
class OutputArchive;

// ? Error codes?
using CustomWriter = std::function<bool(const void*, OutputArchive&)>;
using CustomReader = std::function<void(void*, InputArchive&)>;
using CustomCloner = std::function<void(void*, const void*)>;

class TypeDB {
public:
	TypeDB();
	~TypeDB();

	void registerType(const Type* typeInfo, CustomReader reader = nullptr, CustomWriter writer = nullptr);

	const Type& getTypeInfo(TypeId typeID) const;
	const Type* tryGetTypeInfo(TypeId typeID) const;

	template <class T>
	const Type& getTypeInfo() const {
		return *tryGetTypeInfo<T>();
	}

	template <class T>
	const Type* tryGetTypeInfo() const {
		return tryGetTypeInfo(getTypeId<T>());
	}

	void         setCustomWriter(const Type* type, CustomWriter saver);
	CustomWriter getCustomWriter(const Type* type) const;

	void         setCustomReader(const Type* type, CustomReader loader);
	CustomReader getCustomReader(const Type* type) const;

	void         setCustomCloner(const Type* type, CustomCloner loader);
	CustomCloner getCustomCloner(const Type* type) const;

	// Helpers
	template <typename TYPE>
	void setCustomReader(CustomReader reader);

	template <typename TYPE>
	void setCustomWriter(CustomWriter writer);

	template <typename TYPE>
	void setCustomCloner(CustomCloner cloner);

private:
	std::vector<const Type*>                      typeInfo;
	std::unordered_map<const Type*, CustomWriter> customWriters;
	std::unordered_map<const Type*, CustomReader> customReaders;
	std::unordered_map<const Type*, CustomCloner> customCloners;
};

template <typename TYPE>
void TypeDB::setCustomReader(CustomReader reader) {
	const Type& type = getTypeInfo<TYPE>();
	setCustomReader(&type, std::move(reader));
}

template <typename TYPE>
void TypeDB::setCustomWriter(CustomWriter writer) {
	const Type& type = getTypeInfo<TYPE>();
	setCustomWriter(&type, std::move(writer));
}

template <typename TYPE>
void TypeDB::setCustomCloner(CustomCloner cloner) {
	const Type& type = getTypeInfo<TYPE>();
	setCustomCloner(&type, std::move(cloner));
}

TypeDB& getTypeDB();

} // namespace Typhoon
