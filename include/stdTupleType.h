#pragma once

#include "context.h"
#include "flags.h"
#include "property.h"
#include "scopedAllocator.h"
#include "semantics.h"
#include "structType.h"
#include "typeDB.h"
#include <tuple>

namespace Typhoon::Reflection::detail {

template <class C, size_t index>
Property tupleElementProperty(const char* name, Context& context) {
	using E = std::tuple_element_t<index, C>;
	const Type* varType = autoRegisterType<E>(context);

	auto getter = [](const void* self, void* temporary) {
		const C* tuple = reinterpret_cast<const C*>(self);
		*static_cast<E*>(temporary) = std::get<index>(*tuple);
		return temporary;
	};

	auto setter = [](void* self, const void* value) {
		C* tuple = reinterpret_cast<C*>(self);
		std::get<index>(*tuple) = *static_cast<const E*>(value);
	};

	return { setter, getter, name, varType, Flags::all, Semantic::none };
}

template <typename Tuple, size_t ElementIndex>
void registerTupleElement(StructType& tupleType, Context& context) {
	static const char* elementName[] = { "element0", "element1", "element2", "element3", "element4", "element5", "element6", "element7" };
	static_assert(std::tuple_size_v<Tuple> <= std::size(elementName));

	if constexpr (ElementIndex < std::tuple_size_v<Tuple>) {
		tupleType.addProperty(tupleElementProperty<Tuple, ElementIndex>(elementName[ElementIndex], context));
		registerTupleElement<Tuple, ElementIndex + 1>(tupleType, context); // recurse
	}
}

// std::tuple specialization
template <typename... Args>
struct autoRegisterHelper<std::tuple<Args...>> {
	static const Type* autoRegister(Context& context) {
		using Tuple = std::tuple<Args...>;

		constexpr TypeId  typeId = getTypeId<Tuple>();
		StructType* const tupleType = context.scopedAllocator->make<StructType>(typeId, sizeof(Tuple), alignof(Tuple), nullptr,
		                                                                        buildMethodTable<Tuple>(), std::ref(*context.allocator));
		// Register all elements
		registerTupleElement<Tuple, 0>(*tupleType, context);

		context.typeDB->registerType(tupleType);
		return tupleType;
	}
};

} // namespace Typhoon::Reflection::detail
