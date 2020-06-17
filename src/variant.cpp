#include "variant.h"
#include "typeDB.h"

namespace Typhoon {

Variant::Variant()
    : typeId {
	nullTypeId
}
#ifdef _DEBUG
, storage {
}
#endif
{}

Variant::Variant(const Variant& other)
    : typeId { nullTypeId } {
	*this = other;
}

Variant::Variant(Variant&& other) noexcept
    : typeId { nullTypeId } {
	*this = std::move(other);
}

Variant::Variant(const void* data, TypeId typeId, const char* name)
    : typeId { typeId }
    , name { name } {
	const auto& typeInfo = getTypeInfo();
	assert(typeInfo.size <= sizeof storage);
	typeInfo.copyConstructObject(&storage, data);
}

Variant::~Variant() //
{
	destruct();
}

Variant& Variant::operator=(const Variant& other) {
	destruct();
	if (other.typeId != nullTypeId) {
		const Type& typeInfo = other.getTypeInfo();
		typeInfo.copyConstructObject(&storage, &other.storage);
	}
	typeId = other.typeId;
	name = other.name;
	return *this;
}

Variant& Variant::operator=(Variant&& other) noexcept {
	destruct();
	if (other.typeId != nullTypeId) {
		const Type& typeInfo = other.getTypeInfo();
		typeInfo.moveConstructObject(&storage, &other.storage);
	}
	typeId = other.typeId;
	name = std::move(other.name);
	return *this;
}

const Type& Variant::getTypeInfo() const {
	return getTypeDB().getTypeInfo(typeId);
}

const char* Variant::getName() const {
	return name.c_str();
}

void Variant::destruct() {
	if (typeId != nullTypeId) {
		getTypeInfo().destructObject(&storage);
		typeId = nullTypeId;
	}
}

bool Variant::operator==(const Variant& other) const {
	if (typeId != other.typeId) {
		return false;
	}
	return getTypeInfo().compareObjects(getStorage(), other.getStorage());
}

bool Variant::operator!=(const Variant& other) const {
	return ! (*this == other);
}

} // namespace Typhoon
