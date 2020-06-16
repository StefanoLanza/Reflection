#pragma once

#include "type.h"
#include "typeDB.h"

namespace Typhoon {

class ReferenceType : public Type {
public:
	ReferenceType(TypeId typeID, const Type* referencedType, bool bIsConst);

	const Type& getReferencedType() const {
		return *referencedType;
	}
	bool isConst() const {
		return bIsConst;
	}
	void* resolvePointer(void* data) const;

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
	static const Type* autoRegister(TypeDB& typeDB)
	{
		using reference_type = _Ty&;
		using non_const_type = std::remove_const_t<_Ty>;
		const Type* valueTypeInfo = AutoRegisterType<non_const_type>(typeDB);
		assert(valueTypeInfo);
		constexpr bool isConst = std::is_const_v<_Ty>;
		static const ReferenceType typeInfo{ getTypeId<reference_type>(), valueTypeInfo, isConst };
		RegisterTypeInfo(&typeInfo, typeDB);
		return &typeInfo;
	}
};

}

#endif

} // namespace Typhoon
