#pragma once

#include "property.h"
#include "typeDB.h"
#include <cassert>

namespace Typhoon::Reflection::detail {

template <typename C, typename T>
Getter makeGetter(T C::*memberPtr) {
	return [memberPtr](ConstDataPtr self, DataPtr temporary) {
		if constexpr (std::is_array_v<T>) {
			// Arrays are not assignable in C++
			auto& dst = deref<T>(temporary);
			auto& src = (cast<C>(self)->*memberPtr);
			std::copy(std::begin(src), std::end(src), std::begin(dst));
		}
		else {
			deref<T>(temporary) = (cast<C>(self)->*memberPtr);
		}
		return temporary;
	};
}

template <typename C, typename T>
Setter makeSetter(T C::*memberPtr) {
	return [memberPtr](DataPtr self, ConstDataPtr value) {
		if constexpr (std::is_array_v<T>) {
			// Arrays are not assignable in C++
			auto& src = deref<T>(value);
			auto& dst = (cast<C>(self)->*memberPtr);
			std::copy(std::begin(src), std::end(src), std::begin(dst));
		}
		else {
			(cast<C>(self)->*memberPtr) = deref<T>(value);
		}
	};
}
template <typename C, class R>
static Getter makeGetter(R (C::*func)() const) {
	using T = std::decay_t<R>;
	return [func](ConstDataPtr self, DataPtr temporary) {
		deref<T>(temporary) = (cast<C>(self)->*func)();
		return temporary;
	};
}

template <typename C, typename A>
Setter makeSetter(void (C::*func)(A)) {
	using T = std::decay_t<A>;
	return [func](DataPtr self, ConstDataPtr value) { //
		(cast<C>(self)->*func)(deref<T>(value));
	};
}

template <typename C, typename R>
Getter makeGetter(R (*func)(const C&)) {
	using T = std::decay_t<R>;
	return [func](ConstDataPtr self, DataPtr temporary) {
		// TODO don't always build temporary
		deref<T>(temporary) = func(deref<C>(self));
		return temporary;
	};
}

template <typename C, typename A>
Setter makeSetter(void (*func)(C&, A)) {
	using T = std::decay_t<A>;
	return [func](DataPtr self, ConstDataPtr value) { func(deref<C>(self), deref<T>(value)); };
}

template <typename C, typename R, typename A>
Property createProperty(const char* name, uint32_t flags, Semantic semantic, void (*setter)(C&, A), R (*getter)(const C&), Context& context) {
	static_assert(std::is_same_v<std::decay_t<R>, std::decay_t<A>>);
	using ValueType = std::decay_t<R>;
	const Type* valueType = autoRegisterType<ValueType>(context);
	return { makeSetter(setter), makeGetter(getter), name, valueType, flags, semantic, *context.allocator };
}

template <typename C, typename A>
Property createProperty(const char* name, uint32_t flags, Semantic semantic, void (*setter)(C&, A), Context& context) {
	using ValueType = std::decay_t<A>;
	const Type* type = autoRegisterType<ValueType>(context);
	return { makeSetter(setter), nullptr, name, type, flags, semantic, *context.allocator };
}

template <typename C, typename T, typename A>
Property createProperty(const char* name, uint32_t flags, Semantic semantic, void (*setter)(C&, A), T C::*memberPtr, Context& context) {
	static_assert(std::is_same_v<T, std::decay_t<A>>);
	const Type* varType = autoRegisterType<T>(context);
	return { makeSetter(setter), makeGetter(memberPtr), name, varType, flags, semantic, *context.allocator };
}

template <typename C, typename R, typename A>
Property createProperty(const char* name, uint32_t flags, Semantic semantic, void (C::*setter)(A), R (C::*getter)() const, Context& context) {
	static_assert(std::is_same_v<std::decay_t<R>, std::decay_t<A>>);
	using ValueType = std::decay_t<R>;
	const Type* valueType = autoRegisterType<ValueType>(context);
	return { makeSetter(setter), makeGetter(getter), name, valueType, flags, semantic, *context.allocator };
}

template <typename C, typename R>
Property createProperty(const char* name, uint32_t flags, Semantic semantic, R (*getter)(const C&), Context& context) {
	using ValueType = std::decay_t<R>;
	const Type* A = autoRegisterType<ValueType>(context);
	assert(A);
	return { nullptr, makeGetter(getter), name, A, flags, semantic, *context.allocator };
}

template <typename C, typename R>
Property createProperty(const char* name, uint32_t flags, Semantic semantic, R (C::*getter)() const, Context& context) {
	using ValueType = std::decay_t<R>;
	const Type* valueType = autoRegisterType<ValueType>(context);
	return { nullptr, makeGetter(getter), name, valueType, flags, semantic, *context.allocator };
}

template <typename C, typename A>
Property createProperty(const char* name, uint32_t flags, Semantic semantic, void (C::*setter)(A), Context& context) {
	using ValueType = std::decay_t<A>;
	const Type* valueType = autoRegisterType<ValueType>(context);
	return { makeSetter(setter), nullptr, name, valueType, flags, semantic, *context.allocator };
}

template <typename C, typename T>
Property createProperty(const char* name, uint32_t flags, Semantic semantic, T C::*memberPtr, Context& context) {
	const Type* varType = autoRegisterType<T>(context);
	return { makeSetter(memberPtr), makeGetter(memberPtr), name, varType, flags, semantic, *context.allocator };
}

} // namespace Typhoon::Reflection::detail
