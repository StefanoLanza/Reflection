#pragma once

#include "context.h"
#include "flags.h"
#include "makeProperty.h"
#include "property.h"
#include "semantics.h"
#include "structType.h"
#include "typeDB.h"
#include <core/scopedAllocator.h>
#include <utility>

namespace Typhoon::Reflection::detail {

// std::pair specialization
template <class first_type, class second_type>
struct autoRegisterHelper<std::pair<first_type, second_type>> {
	static const Type* autoRegister(Context& context) {
		using PairType = std::pair<first_type, second_type>;
		constexpr TypeId typeId = getTypeId<PairType>();
		auto             type = context.scopedAllocator->make<StructType>("std::pair", typeId, sizeof(PairType), alignof(PairType), nullptr,
                                                              buildMethodTable<PairType>(), std::ref(*context.allocator));
		type->addProperty(createFieldProperty("first", Flags::all, Semantic::none, &PairType::first, context));
		type->addProperty(createFieldProperty("second", Flags::all, Semantic::none, &PairType::second, context));
		context.typeDB->registerType(type);
		return type;
	}
};

} // namespace Typhoon::Reflection::detail
