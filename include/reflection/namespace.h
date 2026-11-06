#pragma once

#include "config.h"

#include <core/arenaVector.h>

#include <cstddef> // size_t
#include <span>
#include <vector>

namespace Typhoon::Reflection {

class Type;

class Namespace {
public:
	Namespace(const char* name, ArenaAllocator& allocator);
	~Namespace();

	static constexpr char* global = nullptr;
	static constexpr char  unnamed[] = "";

	const char*                  getName() const;
	std::span<const Type* const> getTypes() const;
	std::span<Namespace* const>  getNestedNamespaces() const;
	Namespace*                   getNestedNamespace(const char* name) const;
	void                         addNestedNamespace(Namespace* nestedNamespace);
	void                         addType(const Type* type);

private:
	const char*              name;
	ArenaVector<Namespace*>  nestedNamespaces;
	ArenaVector<const Type*> types;
};

} // namespace Typhoon::Reflection
