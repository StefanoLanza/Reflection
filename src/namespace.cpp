#include "namespace.h"

namespace Typhoon::Reflection {

Namespace::Namespace(const char* name, Allocator& allocator)
    : name(name)
    , nestedNamespaces(stdAllocator<const Namespace*>(allocator))
    , types(stdAllocator<const Type*>(allocator)) {
}

Namespace::~Namespace() = default;

const char* Namespace::getName() const {
	return name;
}

span<const Type* const> Namespace::getTypes() const {
	return { types.data(), types.size() };
}

span<const Namespace* const> Namespace::getNestedNamespaces() const {
	return { nestedNamespaces.data(), nestedNamespaces.size() };
}

void Namespace::addNestedNamespace(const Namespace* nestedNamespace) {
	nestedNamespaces.push_back(nestedNamespace);
}

void Namespace::addType(const Type* type) {
	types.push_back(type);
}

} // namespace Typhoon::Reflection
