#include "namespace.h"
#include <cassert>
#include <cstring>

namespace Typhoon::Reflection {

Namespace::Namespace(const char* name, Allocator& allocator)
    : name(name)
    , nestedNamespaces(stdAllocator<Namespace*>(allocator))
    , types(stdAllocator<const Type*>(allocator)) {
}

Namespace::~Namespace() = default;

const char* Namespace::getName() const {
	return name;
}

std::span<const Type* const> Namespace::getTypes() const {
	return types;
}

std::span<Namespace* const> Namespace::getNestedNamespaces() const {
	return nestedNamespaces;
}

Namespace* Namespace::getNestedNamespace(const char* nestedName) const {
	assert(nestedName);
	for (auto n : nestedNamespaces) {
		if (! strcmp(n->getName(), nestedName)) {
			return n;
		}
	}
	return nullptr;
}

void Namespace::addNestedNamespace(Namespace* nestedNamespace) {
	nestedNamespaces.push_back(nestedNamespace);
}

void Namespace::addType(const Type* type) {
	types.push_back(type);
}

} // namespace Typhoon::Reflection
