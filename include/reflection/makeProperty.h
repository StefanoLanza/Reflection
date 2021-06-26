#pragma once

#include "property.h"
#include <cassert>

namespace Typhoon::Reflection::detail {

template <class T>
const Type* autoRegisterType(TypeDB& typeDB);

template <class C, typename T>
Getter makeFieldGetter(T C::*memberPtr) {
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

template <class C, typename T>
Setter makeFieldSetter(T C::*memberPtr) {
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

template <class C>
class PropertyHelpers {
	template <class R>
	static Getter makeMemberGetter(R (C::*func)() const) {
		using T = std::decay_t<R>;
		return [func](ConstDataPtr self, DataPtr temporary) {
			deref<T>(temporary) = (cast<C>(self)->*func)();
			return temporary;
		};
	}

	template <typename A>
	static Setter makeMemberSetter(void (C::*func)(A)) {
		using T = std::decay_t<A>;
		return [func](DataPtr self, ConstDataPtr value) { //
			(cast<C>(self)->*func)(deref<T>(value));
		};
	}

	template <typename R>
	static Getter makeFreeGetter(R (*func)(const C&)) {
		using T = std::decay_t<R>;
		return [func](ConstDataPtr self, DataPtr temporary) {
			// TODO don't always build temporary
			deref<T>(temporary) = func(deref<C>(self));
			return temporary;
		};
	}

	template <typename A>
	static Setter makeFreeSetter(void (*func)(C&, A)) {
		using T = std::decay_t<A>;
		return [func](DataPtr self, ConstDataPtr value) { func(deref<C>(self), deref<T>(value)); };
	}

public:
	template <typename R, typename A>
	static Property createRWProperty(const char* name, uint32_t flags, Semantic semantic, void (*setter)(C&, A), R (*getter)(const C&),
	                                 Context& context) {
		static_assert(std::is_same_v<std::decay_t<R>, std::decay_t<A>>);
		using ValueType = std::decay_t<R>;
		const Type* valueType = autoRegisterType<ValueType>(context);
		return { makeFreeSetter(setter), makeFreeGetter(getter), name, valueType, flags, semantic, *context.allocator };
	}

	template <typename R, typename A>
	static Property createRWProperty(const char* name, uint32_t flags, Semantic semantic, void (C::*setter)(A), R (C::*getter)() const,
	                                 Context& context) {
		static_assert(std::is_same_v<std::decay_t<R>, std::decay_t<A>>);
		using ValueType = std::decay_t<R>;
		const Type* valueType = autoRegisterType<ValueType>(context);
		return { makeMemberSetter(setter), makeMemberGetter(getter), name, valueType, flags, semantic, *context.allocator };
	}

	template <typename R>
	static Property createROProperty(const char* name, uint32_t flags, Semantic semantic, R (*getter)(const C&), Context& context) {
		using ValueType = std::decay_t<R>;
		const Type* A = autoRegisterType<ValueType>(context);
		assert(A);
		return { nullptr, makeFreeGetter(getter), name, A, flags, semantic, *context.allocator };
	}

	template <typename A>
	static Property createProperty(const char* name, uint32_t flags, Semantic semantic, void (*setter)(C&, A), Context& context) {
		using ValueType = std::decay_t<A>;
		const Type* type = autoRegisterType<ValueType>(context);
		return { makeFreeSetter(setter), nullptr, name, type, flags, semantic, *context.allocator };
	}

	template <typename R>
	static Property createROProperty(const char* name, uint32_t flags, Semantic semantic, R (C::*getter)() const, Context& context) {
		using ValueType = std::decay_t<R>;
		const Type* valueType = autoRegisterType<ValueType>(context);
		return { nullptr, makeMemberGetter(getter), name, valueType, flags, semantic, *context.allocator };
	}

	template <typename T, typename A>
	static Property createProperty(const char* name, uint32_t flags, Semantic semantic, void (*setter)(C&, A), T C::*memberPtr, Context& context) {
		static_assert(std::is_same_v<T, std::decay_t<A>>);
		const Type* varType = autoRegisterType<T>(context);
		return { makeFreeSetter(setter), makeFieldGetter(memberPtr), name, varType, flags, semantic, *context.allocator };
	}
};

template <typename C, typename T>
Property createFieldProperty(const char* name, uint32_t flags, Semantic semantic, T C::*memberPtr, Context& context) {
	const Type* varType = autoRegisterType<T>(context);
	return { makeFieldSetter(memberPtr), makeFieldGetter(memberPtr), name, varType, flags, semantic, *context.allocator };
}

} // namespace Typhoon::Reflection::detail
