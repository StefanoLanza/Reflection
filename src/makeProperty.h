#pragma once

#include "property.h"

namespace Typhoon::Reflection::detail {

template <class T>
const Type* autoRegisterType(TypeDB& typeDB);

template <class C, typename T>
Getter makeFieldGetter(T C::*memberPtr) {
	return [memberPtr](const void* self, void* temporary) {
		//(void)temporary;
		// return &(static_cast<const C*>(self)->*memberPtr); // return field address directly
		if constexpr (std::is_array_v<T>) {
			// Arrays are not assignable in C++
			auto& dst = *static_cast<T*>(temporary);
			auto& src = (static_cast<const C*>(self)->*memberPtr);
			std::copy(std::begin(src), std::end(src), std::begin(dst));
		}
		else {
			*static_cast<T*>(temporary) = (static_cast<const C*>(self)->*memberPtr);
		}
		return temporary;
	};
}

template <class C, typename T>
Setter makeFieldSetter(T C::*memberPtr) {
	return [memberPtr](void* self, const void* value) {
		if constexpr (std::is_array_v<T>) {
			// Arrays are not assignable in C++
			auto& src = *static_cast<const T*>(value);
			auto& dst = (static_cast<C*>(self)->*memberPtr);
			std::copy(std::begin(src), std::end(src), std::begin(dst));
		}
		else {
			(static_cast<C*>(self)->*memberPtr) = *static_cast<const T*>(value);
		}
	};
}

template <class C>
class PropertyHelpers {
	template <class R>
	static Getter makeMemberGetter(R (C::*func)() const) {
		using T = std::decay_t<R>;
		return [func](const void* self, void* temporary) {
			*static_cast<T*>(temporary) = (static_cast<const C*>(self)->*func)();
			return temporary;
		};
	}

	template <typename A>
	static Setter makeMemberSetter(void (C::*func)(A)) {
		using T = std::decay_t<A>;
		return [func](void* self, const void* value) { //
			(static_cast<C*>(self)->*func)(*static_cast<const T*>(value));
		};
	}

	template <typename R>
	static Getter makeFreeGetter(R (*func)(const C&)) {
		using T = std::decay_t<R>;
		return [func](const void* self, void* temporary) {
			// TODO don't always build temporary
			*static_cast<T*>(temporary) = func(*static_cast<const C*>(self));
			return temporary;
		};
	}

	template <typename A>
	static Setter makeFreeSetter(void (*func)(C&, A)) {
		using T = std::decay_t<A>;
		return [func](void* self, const void* value) { func(*static_cast<C*>(self), *static_cast<const T*>(value)); };
	}

public:
	template <typename R, typename A>
	static Property createRWProperty(const char* name, uint32_t flags, Semantic semantic, void (*setter)(C&, A), R (*getter)(const C&),
	                                 Context& context) {
		static_assert(std::is_same_v<std::decay_t<R>, std::decay_t<A>>);
		using value_type = std::decay_t<R>;
		const Type* valueType = autoRegisterType<value_type>(context);
		return { makeFreeSetter(setter), makeFreeGetter(getter), name, valueType, flags, semantic };
	}

	template <typename R, typename A>
	static Property createRWProperty(const char* name, uint32_t flags, Semantic semantic, void (C::*setter)(A), R (C::*getter)() const,
	                                 Context& context) {
		static_assert(std::is_same_v<std::decay_t<R>, std::decay_t<A>>);
		using value_type = std::decay_t<R>;
		const Type* valueType = autoRegisterType<value_type>(context);
		return { makeMemberSetter(setter), makeMemberGetter(getter), name, valueType, flags, semantic };
	}

	template <typename R>
	static Property createROProperty(const char* name, uint32_t flags, Semantic semantic, R (*getter)(const C&), Context& context) {
		using value_type = std::decay_t<R>;
		const Type* A = autoRegisterType<value_type>(context);
		assert(A);
		return { nullptr, makeFreeGetter(getter), name, A, flags, semantic };
	}

	template <typename A>
	static Property createProperty(const char* name, uint32_t flags, Semantic semantic, void (*setter)(C&, A), Context& context) {
		using value_type = std::decay_t<A>;
		const Type* type = autoRegisterType<value_type>(context);
		return { makeFreeSetter(setter), nullptr, name, type, flags, semantic };
	}

	template <typename R>
	static Property createROProperty(const char* name, uint32_t flags, Semantic semantic, R (C::*getter)() const, Context& context) {
		using value_type = std::decay_t<R>;
		const Type* valueType = autoRegisterType<value_type>(context);
		return { nullptr, makeMemberGetter(getter), name, valueType, flags, semantic };
	}

	template <typename T, typename A>
	static Property createProperty(const char* name, uint32_t flags, Semantic semantic, void (*setter)(C&, A), T C::*memberPtr, Context& context) {
		static_assert(std::is_same_v<T, std::decay_t<A>>);
		const Type* varType = autoRegisterType<T>(context);
		return { makeFreeSetter(setter), makeFieldGetter(memberPtr), name, varType, flags, semantic };
	}
};

template <typename C, typename T>
Property createFieldProperty(const char* name, uint32_t flags, Semantic semantic, T C::*memberPtr, Context& context) {
	const Type* varType = autoRegisterType<T>(context);
	return { makeFieldSetter(memberPtr), makeFieldGetter(memberPtr), name, varType, flags, semantic };
}

} // namespace Typhoon::Reflection::detail
