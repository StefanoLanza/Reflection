#pragma once

#include "dataPtr.h"
#include "type.h"
#include "typeDB.h"

namespace Typhoon::Reflection {

class ReferenceType : public Type {
public:
	ReferenceType(TypeId typeID, const Type* referencedType, bool bIsConst, Allocator& allocator);

	const Type& getReferencedType() const {
		return *referencedType;
	}
	bool isConst() const {
		return bIsConst;
	}
	DataPtr      resolvePointer(DataPtr data) const;
	ConstDataPtr resolvePointer(ConstDataPtr data) const;

private:
	const Type* referencedType;
	bool        bIsConst;
};

#if 0
namespace detail {

// Specialization for references
template <class _Ty>
struct AutoRegisterHelper<_Ty&>
{
	static const Type* autoRegister(TypeDB& typeDB, Allocator& allocator)
	{
		using reference_type = _Ty&;
		using non_const_type = std::remove_const_t<_Ty>;
		const Type* valueType = AutoRegisterType<non_const_type>(typeDB, allocator);
		assert(valueType);
		constexpr bool isConst = std::is_const_v<_Ty>;
		static const ReferenceType type{ getTypeId<reference_type>(), valueType, isConst };
		RegisterTypeInfo(&type, typeDB);
		return &typeInfo;
	}
};

}

#endif

} // namespace Typhoon::Reflection
