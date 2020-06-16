#pragma once

#include "flags.h"
#include "structType.h"
#include "typeDB.h"
#include <utility>

namespace Typhoon {

namespace detail {

// std::pair specialization
template <class first_type, class second_type>
struct autoRegisterHelper<std::pair<first_type, second_type>> {
	static const Type* autoRegister(TypeDB& typeDB) {
		using pair_type = std::pair<first_type, second_type>;
		const Type*       firstType = autoRegisterType<first_type>(typeDB);
		const Type*       secondType = autoRegisterType<second_type>(typeDB);
		constexpr TypeId  typeId = getTypeId<pair_type>();
		static StructType typeInfo { typeId, sizeof(pair_type), std::alignment_of_v<pair_type> };
		typeInfo.addField(Field { "first", firstType, offsetof(pair_type, first), Flags::all, Semantic::none });
		typeInfo.addField(Field { "second", secondType, offsetof(pair_type, second), Flags::all, Semantic::none });
		typeDB.registerType(&typeInfo);
		return &typeInfo;
	}
};

} // namespace detail

} // namespace Typhoon
