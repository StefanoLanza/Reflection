#include "builtinType.h"

namespace Typhoon::Reflection {

BuiltinType::BuiltinType(const char* typeName, TypeId typeID, size_t size, size_t alignment, const Traits& traits, const MethodTable& methods,
                         ArenaAllocator& allocator)
    : Type { typeName, typeID, Subclass::Builtin, size, alignment, traits, methods, allocator } {
}

} // namespace Typhoon::Reflection
