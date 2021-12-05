#pragma once

#include "config.h"

#include <core/span.h>
#include <core/stdAllocator.h>
#include <cstddef> // size_t
#include <vector>

namespace Typhoon::Reflection {

class Type;

class Namespace {
public:
	Namespace(const char* name, Allocator& allocator);
	~Namespace();

	static constexpr char* global = nullptr;
	static constexpr char  unnamed[] = "";

	const char*             getName() const;
	span<const Type* const> getTypes() const;
	span<Namespace* const>  getNestedNamespaces() const;
	Namespace*              getNestedNamespace(const char* name) const;
	void                    addNestedNamespace(Namespace* nestedNamespace);
	void                    addType(const Type* type);

private:
	using TypeVector = std::vector<const Type*, stdAllocator<const Type*>>;
	using NamespaceVector = std::vector<Namespace*, stdAllocator<Namespace*>>;
	const char*     name;
	NamespaceVector nestedNamespaces;
	TypeVector      types;
};

} // namespace Typhoon::Reflection
