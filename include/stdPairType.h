#pragma once

#include "context.h"
#include "flags.h"
#include "property.h"
#include "scopedAllocator.h"
#include "semantics.h"
#include "structType.h"
#include "typeDB.h"
#include <utility>

namespace Typhoon::Reflection::detail {

// std::pair specialization
template <class first_type, class second_type>
struct autoRegisterHelper<std::pair<first_type, second_type>> {
	static const Type* autoRegister(Context& context) {
		using PairType = std::pair<first_type, second_type>;
		//		const Type*      firstType = autoRegisterType<first_type>(typeDB, allocator);
		//		const Type*      secondType = autoRegisterType<second_type>(typeDB, allocator);
		constexpr TypeId typeId = getTypeId<PairType>();
		auto type = context.scopedAllocator->make<StructType>(typeId, sizeof(PairType), alignof(PairType), nullptr, buildMethodTable<PairType>(),
		                                                      std::ref(*context.allocator));
		type->addProperty(ClassHelpers<PairType>::createFieldProperty("first", Flags::all, Semantic::none, &PairType::first, context));
		type->addProperty(ClassHelpers<PairType>::createFieldProperty("second", Flags::all, Semantic::none, &PairType::second, context));
		context.typeDB->registerType(type);
		return type;
	}
};

} // namespace Typhoon::Reflection::detail
