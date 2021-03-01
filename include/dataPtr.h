#pragma once

#include <core/typeId.h>

namespace Typhoon::Reflection {

using DataPtr = void*;
using ConstDataPtr = const void*;

template <class T>
T* castPointer(DataPtr ptr) {
	return static_cast<T*>(ptr);
}

template <class T>
const T* castPointer(ConstDataPtr ptr) {
	return static_cast<const T*>(ptr);
}

template <class T>
T* safeCastPointer(DataPtr ptr, TypeId typeId) {
	if (getTypeId<T> == typeId) {
		return static_cast<T*>(ptr);
	}
	return nullptr;
}

template <class T>
const T* safeCastPointer(ConstDataPtr ptr, TypeId typeId) {
	if (getTypeId<T> == typeId) {
		return static_cast<const T*>(ptr);
	}
	return nullptr;
}

} // namespace Typhoon::Reflection
