#pragma once

#include "flags.h"
#include "semantics.h"
#include "structType.h"
#include "typeDB.h"
#include <tuple>

namespace Typhoon::Reflection::detail {

template <typename Tuple, size_t ElementIndex>
constexpr size_t getTupleElementOffset() {
	// Similar to implementation of offsetof
	const Tuple* tuple = reinterpret_cast<const Tuple*>(0);
	return reinterpret_cast<uintptr_t>(&std::get<ElementIndex>(*tuple));
}

template <typename... Args, std::size_t... argIndices>
const Type* registerTuple(TypeDB& typeDB, Allocator& allocator, std::integer_sequence<std::size_t, argIndices...> /*integerSequence*/) {
	using Tuple = std::tuple<Args...>;
	const Type* const argType[] = { autoRegisterType<Args>(typeDB, allocator)..., nullptr };
	const size_t      argOffset[] = { getTupleElementOffset<Tuple, argIndices>()..., 0 };

	static const char* elementName[] = { "element0", "element1", "element2", "element3", "element4", "element5", "element6", "element7" };
	static_assert(std::size(argType) - 1 <= std::size(elementName));

	constexpr TypeId typeId = getTypeId<Tuple>();
	auto tupleType = allocator.make<StructType>(typeId, sizeof(Tuple), alignof(Tuple), nullptr, buildMethodTable<Tuple>(), std::ref(allocator));
	for (size_t i = 0; i < std::size(argType) - 1; ++i) {
		tupleType->addField(Field { elementName[i], argType[i], argOffset[i], Flags::all, Semantic::none });
	}

	typeDB.registerType(tupleType);
	return tupleType;
}

// std::tuple specialization
template <typename... Args>
struct autoRegisterHelper<std::tuple<Args...>> {
	static const Type* autoRegister(TypeDB& typeDB, Allocator& allocator) {
		return registerTuple<Args...>(typeDB, allocator, std::index_sequence_for<Args...> {});
	}
};

} // namespace Typhoon::Reflection::detail
