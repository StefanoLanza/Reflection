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

const char* buildTemplateTypeName(const char* typeNames[], const char* prefix, const char* suffix, ScopedAllocator& alloc);

// std::pair specialization
template <class first_type, class second_type>
struct autoRegisterHelper<std::pair<first_type, second_type>> {
	static const Type* autoRegister(Context& context) {
		using PairType = std::pair<first_type, second_type>;
		constexpr TypeId typeId = getTypeId<PairType>();
		const Type*      firstType = autoRegisterType<first_type>(context);
		const Type*      secondType = autoRegisterType<second_type>(context);
		const char*      innerTypeNames[] = { firstType->getName(), secondType->getName(), nullptr };
		const char*      typeName = buildTemplateTypeName(innerTypeNames, "std::pair<", ">", *context.scopedAllocator);

		auto type = context.scopedAllocator->make<StructType>(typeName, typeId, sizeof(PairType), alignof(PairType), nullptr,
		                                                      buildMethodTable<PairType>(), *context.allocator);
		type->addProperty(ClassUtil<PairType>::makeProperty("first", &PairType::first, context));
		type->addProperty(ClassUtil<PairType>::makeProperty("second", &PairType::second, context));
		return type;
	}
};

} // namespace Typhoon::Reflection::detail
