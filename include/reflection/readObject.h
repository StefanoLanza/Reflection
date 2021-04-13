#pragma once

#include "dataPtr.h"
#include "errorCodes.h"
#include "semantics.h"
#include "stdVectorType.h"
#include "typeDB.h"
#include <core/typeId.h>

#include <array>
#include <cassert>
#include <vector>

namespace Typhoon::Reflection {

class InputArchive;

namespace detail {
TypeDB& getTypeDB();

bool readData(DataPtr object, TypeId typeId, InputArchive& archive, Semantic semantic = Semantic::none);

}

std::pair<bool, size_t> readArray(DataPtr array, size_t arraySize, TypeId elementType, const char* arrayName, InputArchive& archive);

template <class T, size_t size>
bool readArray(std::array<T, size>& array, const char* elementName, InputArchive& archive) {
	return readArray(array.data(), array.size(), elementName, archive).first;
}

template <class T>
std::pair<bool, size_t> readArray(T array[], size_t arraySize, const char* arrayName, InputArchive& archive) {
	return readArray(array, arraySize, getTypeId<T>(), arrayName, archive);
}

class ContainerType;

bool readContainer(DataPtr container, const char* containerName, const ContainerType& type, InputArchive& archive);

// Utility to read a std::vector from an archive
template <class T>
bool readVector(std::vector<T>& vector, const char* vectorName, InputArchive& archive) {
	using container_type = std::vector<T>;
	const Type&                                      valueType = detail::getTypeDB().getType<T>();
	constexpr TypeId                                 typeID = getTypeId<container_type>();
	const detail::StdVectorContainer<container_type> type { nullptr, typeID, &valueType };
	return readContainer(&vector, vectorName, type, archive);
}

} // namespace Typhoon::Reflection
