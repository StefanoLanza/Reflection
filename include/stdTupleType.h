#pragma once

#include "flags.h"
#include "property.h"
#include "semantics.h"
#include "structType.h"
#include "typeDB.h"
#include <tuple>

namespace Typhoon::Reflection::detail {

template <class T>
const Type* autoRegisterType(TypeDB& typeDB);

template <class C>
class TupleHelper {
public:
	template <size_t index>
	static Property makeProperty(const char* name, TypeDB& typeDB, Allocator& allocator) {
		using T = std::tuple_element_t<index, C>;
		const Type* varType = autoRegisterType<T>(typeDB, allocator);
		return { setter<index>(), getter<index>(), name, varType, Flags::all, Semantic::none };
	}

private:
	template <size_t index>
	static Getter getter() {
		return [](const void* self, void* temporary) {
			const C* tuple = reinterpret_cast<const C*>(self);
			using T = std::tuple_element_t<index, C>;
			*static_cast<T*>(temporary) = std::get<index>(*tuple);
			return temporary;
		};
	}

	template <size_t index>
	static Setter setter() {
		return [](void* self, const void* value) {
			C* tuple = reinterpret_cast<C*>(self);
			using T = std::tuple_element_t<index, C>;
			std::get<index>(*tuple) = *static_cast<const T*>(value);
		};
	}
};

template <typename Tuple, size_t ElementIndex>
void registerTupleElement(StructType& tupleType, TypeDB& typeDB, Allocator& allocator) {
	static const char* elementName[] = { "element0", "element1", "element2", "element3", "element4", "element5", "element6", "element7" };
	static_assert(std::tuple_size_v<Tuple> <= std::size(elementName));

	if constexpr (ElementIndex < std::tuple_size_v<Tuple>) {
		tupleType.addProperty(TupleHelper<Tuple>::makeProperty<ElementIndex>(elementName[ElementIndex], typeDB, allocator));
		registerTupleElement<Tuple, ElementIndex + 1>(tupleType, typeDB, allocator);
	}
}

// std::tuple specialization
template <typename... Args>
struct autoRegisterHelper<std::tuple<Args...>> {
	static const Type* autoRegister(TypeDB& typeDB, Allocator& allocator) {
		using Tuple = std::tuple<Args...>;

		constexpr TypeId  typeId = getTypeId<Tuple>();
		StructType* const tupleType =
		    allocator.make<StructType>(typeId, sizeof(Tuple), alignof(Tuple), nullptr, buildMethodTable<Tuple>(), std::ref(allocator));
		registerTupleElement<Tuple, 0>(*tupleType, typeDB, allocator);

		typeDB.registerType(tupleType);
		return tupleType;
	}
};

} // namespace Typhoon::Reflection::detail
