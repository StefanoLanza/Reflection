#pragma once

#include "flags.h"
#include "structType.h"
#include "typeDB.h"
#include <tuple>

namespace Typhoon {

namespace detail {

template <typename Tuple, size_t ElementIndex>
constexpr size_t getTupleElementOffset() {
	// Similar to implementation of offsetof
	const Tuple* tuple = reinterpret_cast<const Tuple*>(0);
	return reinterpret_cast<uintptr_t>(&std::get<ElementIndex>(*tuple));
}

template <typename... Args, std::size_t... argIndices>
const Type* registerTuple(TypeDB& typeDB, std::integer_sequence<std::size_t, argIndices...> /*integerSequence*/) {
	using Tuple = std::tuple<Args...>;
	const Type* const argType[] = { autoRegisterType<Args>(typeDB)..., nullptr };
	const size_t      argOffset[] = { getTupleElementOffset<Tuple, argIndices>()..., 0 };

	static const char* elementName[] = { "element0", "element1", "element2", "element3", "element4", "element5", "element6", "element7" };
	static_assert(std::size(argType) - 1 <= std::size(elementName));

	constexpr TypeId  typeId = getTypeId<Tuple>();
	static StructType structType { typeId, sizeof(Tuple), std::alignment_of_v<Tuple> };
	for (size_t i = 0; i < std::size(argType) - 1; ++i) {
		structType.addField(Field { elementName[i], argType[i], argOffset[i], Flags::all, Semantic::none });
	}

	typeDB.registerType(&structType);
	return &structType;
}

// std::tuple specialization
template <typename... Args>
struct autoRegisterHelper<std::tuple<Args...>> {
	static const Type* autoRegister(TypeDB& typeDB) {
		return registerTuple<Args...>(typeDB, std::index_sequence_for<Args...> {});
	}
};

} // namespace detail

} // namespace Typhoon
