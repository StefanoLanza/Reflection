#pragma once

#include "allocator.h"
#include "config.h"
#include "semantics.h"

#include <cstdint>
#include <functional>
#include <memory>

namespace Typhoon::Reflection {

class Type;
class TypeDB;

using Getter = std::function<const void*(const void* self, void* temporary)>;
using Setter = std::function<void(void* self, const void* value)>; // FIXME void* value to allow move?

class Property {
public:
	Property(Setter&& setter, Getter&& getter, const char* name, const Type* valueType, uint32_t flags, Semantic semantic);

	const char* getName() const;
	const Type& getValueType() const;
	uint32_t    getFlags() const;
	Semantic    getSemantic() const;

	void setValue(void* self, const void* value) const;
	void getValue(const void* self, void* value) const;
	void copyValue(void* dstSelf, const void* srcSelf) const;

private:
	Setter      setter;
	Getter      getter;
	const char* name;
	const Type* valueType;
	uint32_t    flags;
	Semantic    semantic;
};

namespace detail {

template <class T>
const Type* autoRegisterType(TypeDB& typeDB);

template <class C>
class ClassHelpers {
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

	template <typename T>
	static Getter makeFieldGetter(T C::*memberPtr) {
		return [memberPtr](const void* self, void* temporary) {
			//(void)temporary;
			// return &(static_cast<const C*>(self)->*memberPtr); // return field address directly
			*static_cast<T*>(temporary) = (static_cast<const C*>(self)->*memberPtr);
			return temporary;
		};
	}

	template <typename T>
	static Setter makeFieldSetter(T C::*memberPtr) {
		return [memberPtr](void* self, const void* value) { (static_cast<C*>(self)->*memberPtr) = *static_cast<const T*>(value); };
	}

public:
	template <typename R, typename A>
	static Property createRWProperty(const char* name, uint32_t flags, Semantic semantic, void (*setter)(C&, A), R (*getter)(const C&),
	                                 TypeDB& typeDB, Allocator& allocator) {
		static_assert(std::is_same_v<std::decay_t<R>, std::decay_t<A>>);
		using value_type = std::decay_t<R>;
		const Type* valueType = autoRegisterType<value_type>(typeDB, allocator);
		return { makeFreeSetter(setter), makeFreeGetter(getter), name, valueType, flags, semantic };
	}

	template <typename R, typename A>
	static Property createRWProperty(const char* name, uint32_t flags, Semantic semantic, void (C::*setter)(A), R (C::*getter)() const,
	                                 TypeDB& typeDB, Allocator& allocator) {
		static_assert(std::is_same_v<std::decay_t<R>, std::decay_t<A>>);
		using value_type = std::decay_t<R>;
		const Type* valueType = autoRegisterType<value_type>(typeDB, allocator);
		return { makeMemberSetter(setter), makeMemberGetter(getter), name, valueType, flags, semantic };
	}

	template <typename R>
	static Property createROProperty(const char* name, uint32_t flags, Semantic semantic, R (*getter)(const C&), TypeDB& typeDB,
	                                 Allocator& allocator) {
		using value_type = std::decay_t<R>;
		const Type* A = autoRegisterType<value_type>(typeDB, allocator);
		assert(A);
		return { nullptr, makeFreeGetter(getter), name, A, flags, semantic };
	}

	template <typename A>
	static Property createProperty(const char* name, uint32_t flags, Semantic semantic, void (*setter)(C&, A), TypeDB& typeDB, Allocator& allocator) {
		using value_type = std::decay_t<A>;
		const Type* type = autoRegisterType<value_type>(typeDB, allocator);
		return { makeFreeSetter(setter), nullptr, name, type, flags, semantic };
	}

	template <typename R>
	static Property createROProperty(const char* name, uint32_t flags, Semantic semantic, R (C::*getter)() const, TypeDB& typeDB,
	                                 Allocator& allocator) {
		using value_type = std::decay_t<R>;
		const Type* valueType = autoRegisterType<value_type>(typeDB, allocator);
		return { nullptr, makeMemberGetter(getter), name, valueType, flags, semantic };
	}

	template <typename T, typename A>
	static Property createProperty(const char* name, uint32_t flags, Semantic semantic, void (*setter)(C&, A), T C::*memberPtr, TypeDB& typeDB,
	                               Allocator& allocator) {
		static_assert(std::is_same_v<T, std::decay_t<A>>);
		const Type* varType = autoRegisterType<T>(typeDB, allocator);
		return { makeFreeSetter(setter), makeFieldGetter(memberPtr), name, varType, flags, semantic };
	}

	template <typename T>
	static Property createFieldProperty(const char* name, uint32_t flags, Semantic semantic, T C::*memberPtr, TypeDB& typeDB, Allocator& allocator) {
		const Type* varType = autoRegisterType<T>(typeDB, allocator);
		return { makeFieldSetter(memberPtr), makeFieldGetter(memberPtr), name, varType, flags, semantic };
	}
};

} // namespace detail

} // namespace Typhoon::Reflection
