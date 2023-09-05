#pragma once

#include "property.h"
#include "typeDB.h"
#include <cassert>

namespace Typhoon::Reflection::detail {

template <typename C>
class ClassUtil {
private:
	template <typename T>
	static Getter wrapGetter(T C::*memberPtr) {
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

	template <typename T>
	static Setter wrapSetter(T C::*memberPtr) {
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
	template <class R>
	static Getter wrapGetter(R (C::*func)() const) {
		using T = std::decay_t<R>;
		return [func](ConstDataPtr self, DataPtr temporary) {
			deref<T>(temporary) = (cast<C>(self)->*func)();
			return temporary;
		};
	}

	template <typename A>
	static Setter wrapSetter(void (C::*func)(A)) {
		using T = std::decay_t<A>;
		return [func](DataPtr self, ConstDataPtr value) { //
			(cast<C>(self)->*func)(deref<T>(value));
		};
	}

	template <typename R>
	static Getter wrapGetter(R (*func)(const C&)) {
		using T = std::decay_t<R>;
		return [func](ConstDataPtr self, DataPtr temporary) {
			// TODO don't always build temporary
			deref<T>(temporary) = func(deref<C>(self));
			return temporary;
		};
	}

	template <typename A>
	static Setter wrapSetter(void (*func)(C&, A)) {
		using T = std::decay_t<A>;
		return [func](DataPtr self, ConstDataPtr value) { func(deref<C>(self), deref<T>(value)); };
	}

public:
	template <typename R, typename A>
	static Property makeProperty(const char* name, void (*setter)(C&, A), R (*getter)(const C&), Context& context) {
		static_assert(std::is_same_v<std::decay_t<R>, std::decay_t<A>>);
		using ValueType = std::decay_t<R>;
		const Type* valueType = autoRegisterType<ValueType>(context);
		return { wrapSetter(setter), wrapGetter(getter), name, valueType, *context.allocator };
	}

	template <typename A>
	static Property makeProperty(const char* name, void (*setter)(C&, A), Context& context) {
		using ValueType = std::decay_t<A>;
		const Type* type = autoRegisterType<ValueType>(context);
		return { wrapSetter(setter), nullptr, name, type, *context.allocator };
	}

	template <typename T, typename A>
	static Property makeProperty(const char* name, void (*setter)(C&, A), T C::*memberPtr, Context& context) {
		static_assert(std::is_same_v<T, std::decay_t<A>>);
		const Type* varType = autoRegisterType<T>(context);
		return { wrapSetter(setter), wrapGetter(memberPtr), name, varType, *context.allocator };
	}

	template <typename R, typename A>
	static Property makeProperty(const char* name, void (C::*setter)(A), R (C::*getter)() const, Context& context) {
		static_assert(std::is_same_v<std::decay_t<R>, std::decay_t<A>>);
		using ValueType = std::decay_t<R>;
		const Type* valueType = autoRegisterType<ValueType>(context);
		return { wrapSetter(setter), wrapGetter(getter), name, valueType, *context.allocator };
	}

	template <typename R>
	static Property makeProperty(const char* name, R (*getter)(const C&), Context& context) {
		using ValueType = std::decay_t<R>;
		const Type* A = autoRegisterType<ValueType>(context);
		assert(A);
		return { nullptr, wrapGetter(getter), name, A, *context.allocator };
	}

	template <typename R>
	static Property makeProperty(const char* name, R (C::*getter)() const, Context& context) {
		using ValueType = std::decay_t<R>;
		const Type* valueType = autoRegisterType<ValueType>(context);
		return { nullptr, wrapGetter(getter), name, valueType, *context.allocator };
	}

	template <typename A>
	static Property makeProperty(const char* name, void (C::*setter)(A), Context& context) {
		using ValueType = std::decay_t<A>;
		const Type* valueType = autoRegisterType<ValueType>(context);
		return { wrapSetter(setter), nullptr, name, valueType, *context.allocator };
	}

	template <typename T>
	static Property makeProperty(const char* name, T C::*memberPtr, Context& context) {
		const Type* varType = autoRegisterType<T>(context);
		return { wrapSetter(memberPtr), wrapGetter(memberPtr), name, varType, *context.allocator };
	}
};

} // namespace Typhoon::Reflection::detail
