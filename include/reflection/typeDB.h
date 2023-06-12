#pragma once

#include "config.h"

#include "context.h"
#include "type.h"
#include <core/stdAllocator.h>
#include <core/uncopyable.h>

#include <cassert>
#include <string_view>
#include <vector>

namespace Typhoon {
class ScopedAllocator;
}

namespace Typhoon::Reflection {

class Namespace;

class TypeDB : Uncopyable {
public:
	TypeDB(Allocator& allocator, ScopedAllocator& scopedAllocator);
	~TypeDB();

	void       registerType(const Type* type);
	Namespace& getGlobalNamespace() const;

	const Type& getType(TypeId typeID) const;
	const Type* tryGetType(TypeId typeID) const;
	const Type* tryGetType(const char* typeName) const;

	template <class T>
	const Type& getType() const {
		return *tryGetType<T>();
	}

	template <class T>
	const Type* tryGetType() const {
		return tryGetType(getTypeId<T>());
	}

private:
	std::vector<const Type*, stdAllocator<const Type*>> types;
	Namespace*                                          globalNamespace;
};

struct Context;

namespace detail {

template <class T>
struct autoRegisterHelper {
	static const Type* autoRegister([[maybe_unused]] Context& context) {
		return nullptr; // not supported
	}
};

template <class T>
inline const Type* autoRegisterType(Context& context) {
	const Type* type = context.typeDB->tryGetType<T>();
	if (! type) {
		type = autoRegisterHelper<T>::autoRegister(context);
	}
	assert(type);
	return type;
}

const char* decorateTypeName(std::string_view typeName, std::string_view prefix, std::string_view suffix, ScopedAllocator& allocator);

} // namespace detail

} // namespace Typhoon::Reflection
