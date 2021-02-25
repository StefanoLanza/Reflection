#include "variant.h"
#include "reflection.h"
#include "typeDB.h"

namespace Typhoon::Reflection {

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

Variant::Variant(const void* data, TypeId typeId, std::string_view name)
    : typeId { typeId }
    , name { name } {
	const auto& type = getType();
	assert(type.getSize() <= sizeof storage);
	type.copyConstructObject(&storage, data);
}

Variant::Variant(TypeId typeId, std::string_view name)
    : typeId { typeId }
    , name { name } {
	const auto& type = getType();
	assert(type.getSize() <= sizeof storage);
	type.constructObject(&storage);
}

Variant::~Variant() {
	destruct();
}

Variant& Variant::operator=(const Variant& other) {
	if (other.typeId == typeId) {
		if (typeId) {
			const Type& type = getType();
			type.copyObject(&storage, &other.storage);
		}
	}
	else {
		destruct();
		if (other.typeId) {
			const Type& type = other.getType();
			type.copyConstructObject(&storage, &other.storage);
		}
		typeId = other.typeId;
	}
	name = other.name;
	return *this;
}

Variant& Variant::operator=(Variant&& other) noexcept {
	if (other.typeId == typeId) {
		if (typeId) {
			const Type& type = getType();
			type.moveObject(&storage, &other.storage);
		}
	}
	else {
		destruct();
		if (other.typeId) {
			const Type& type = other.getType();
			type.moveConstructObject(&storage, &other.storage);
		}
		typeId = other.typeId;
	}
	name = std::move(other.name);
	return *this;
}

const Type& Variant::getType() const {
	return detail::getTypeDB().getType(typeId);
}

const char* Variant::getName() const {
	return name.c_str();
}

void Variant::destruct() {
	if (typeId != nullTypeId) {
		getType().destructObject(&storage);
		typeId = nullTypeId;
	}
}

bool Variant::operator==(const Variant& other) const {
	if (typeId != other.typeId) {
		return false;
	}
	return getType().compareObjects(getStorage(), other.getStorage());
}

bool Variant::operator!=(const Variant& other) const {
	return ! (*this == other);
}

} // namespace Typhoon::Reflection
