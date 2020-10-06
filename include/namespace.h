#pragma once

#include "config.h"

#include <core/span.h>
#include <core/stdAllocator.h>
#include <vector>

namespace Typhoon::Reflection {

class Type;

class Namespace {
public:
	Namespace(const char* name, Allocator& allocator);
	~Namespace();

	const char*             getName() const;
	span<const Type* const> getTypes() const;
	void                    addType(const Type* type);

private:
	using TypeVector = std::vector<const Type*, stdAllocator<const Type*>>;
	const char* name;
	TypeVector  types;
};

} // namespace Typhoon::Reflection
