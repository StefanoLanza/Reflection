#include "builtinType.h"

namespace Typhoon::Reflection {

BuiltinType::BuiltinType(const char* typeName, TypeId typeID, size_t size, size_t alignment, const MethodTable& methods)
    : Type(typeName, typeID, Subclass::Builtin, size, alignment, methods) {
}

} // namespace Typhoon::Reflection
