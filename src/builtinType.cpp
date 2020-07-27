#include "builtinType.h"

namespace Typhoon::Reflection {

BuiltinType::BuiltinType(TypeId typeID, size_t size, size_t alignment, const MethodTable& methods)
    : Type(typeID, Subclass::Builtin, size, alignment, methods) {
}

} // namespace Typhoon::Reflection
