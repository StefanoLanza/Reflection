#pragma once

#include "errorCodes.h"
#include "semantics.h"
#include "stdVectorType.h"
#include "typeDB.h"
#include <array>
#include <cassert>
#include <core/typeId.h>
#include <vector>

namespace Typhoon {

class InputArchive;

bool readObject(void* object, TypeId typeId, InputArchive& archive, ReflSemantic semantic = ReflSemantic::none);
bool readObject(void* object, TypeId typeId, const char* name, InputArchive& archive, ReflSemantic semantic = ReflSemantic::none);

template <typename T>
bool readObject(T* object, const char* name, InputArchive& archive, ReflSemantic semantic = ReflSemantic::none) {
	return readObject(object, getTypeId<T>(), name, archive, semantic);
}

template <typename T>
bool readObject(T* object, InputArchive& archive, ReflSemantic semantic = ReflSemantic::none) {
	return readObject(object, getTypeId<T>(), archive, semantic);
}

std::pair<bool, size_t> readArray(void* array, size_t arraySize, TypeId elementType, const char* arrayName, InputArchive& archive);

template <class T, size_t size>
bool readArray(std::array<T, size>& array, const char* elementName, InputArchive& archive) {
	return readArray(array.data(), array.size(), elementName, archive);
}

template <class T>
std::pair<bool, size_t> readArray(T array[], size_t arraySize, const char* arrayName, InputArchive& archive) {
	return readArray(array, arraySize, getTypeId<T>(), arrayName, archive);
}

class ContainerType;

bool readContainer(void* container, const char* containerName, const ContainerType& type, InputArchive& archive);

// Utility to read a std::vector from an archive
template <class T>
bool readVector(std::vector<T>& vector, const char* vectorName, InputArchive& archive) {
	using container_type = std::vector<T>;
	const Type&                                      valueType = getTypeDB().getType<T>();
	constexpr TypeId                                 typeID = getTypeId<container_type>();
	const detail::StdVectorContainer<container_type> type { typeID, &valueType };
	return readContainer(&vector, vectorName, type, archive);
}

} // namespace Typhoon