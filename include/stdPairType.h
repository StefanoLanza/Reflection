#pragma once

#include "flags.h"
#include "semantics.h"
#include "structType.h"
#include "typeDB.h"
#include <utility>

namespace Typhoon::Reflection::detail {

// std::pair specialization
template <class first_type, class second_type>
struct autoRegisterHelper<std::pair<first_type, second_type>> {
	static const Type* autoRegister(TypeDB& typeDB, Allocator& allocator) {
		using PairType = std::pair<first_type, second_type>;
		const Type*      firstType = autoRegisterType<first_type>(typeDB, allocator);
		const Type*      secondType = autoRegisterType<second_type>(typeDB, allocator);
		constexpr TypeId typeId = getTypeId<PairType>();
		auto             type =
		    allocator.make<StructType>(typeId, sizeof(PairType), alignof(PairType), nullptr, buildMethodTable<PairType>(), std::ref(allocator));
		type->addField(Field { "first", firstType, offsetof(PairType, first), Flags::all, Semantic::none });
		type->addField(Field { "second", secondType, offsetof(PairType, second), Flags::all, Semantic::none });
		typeDB.registerType(type);
		return type;
	}
};

} // namespace Typhoon::Reflection::detail
