#pragma once

#include <core/typeId.h>

namespace Typhoon::Reflection {

using DataPtr = void*;
using ConstDataPtr = const void*;

template <class T>
T* cast(DataPtr ptr) {
	return static_cast<T*>(ptr);
}

template <class T>
T& deref(DataPtr ptr) {
	return *static_cast<T*>(ptr);
}

template <class T>
const T* cast(ConstDataPtr ptr) {
	return static_cast<const T*>(ptr);
}

template <class T>
const T& deref(ConstDataPtr ptr) {
	return *static_cast<const T*>(ptr);
}

template <class T>
T* safeCast(DataPtr ptr, TypeId typeId) {
	if (getTypeId<T> == typeId) {
		return static_cast<T*>(ptr);
	}
	return nullptr;
}

template <class T>
const T* safeCast(ConstDataPtr ptr, TypeId typeId) {
	if (getTypeId<T> == typeId) {
		return static_cast<const T*>(ptr);
	}
	return nullptr;
}

} // namespace Typhoon::Reflection
