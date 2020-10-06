#include "namespace.h"

namespace Typhoon::Reflection {

Namespace::Namespace(const char* name, Allocator& allocator)
    : name(name)
    , types(stdAllocator<const Type*>(allocator)) {
}

Namespace::~Namespace() = default;

const char* Namespace::getName() const {
	return name;
}

span<const Type* const> Namespace::getTypes() const {
	return { types.data(), types.size() };
}

void Namespace::addType(const Type* type) {
	types.push_back(type);
}

} // namespace Typhoon::Reflection
