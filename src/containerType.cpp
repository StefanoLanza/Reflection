#include "containerType.h"

namespace Typhoon::Reflection {

ContainerType::ContainerType(const char* typeName, TypeId typeID, size_t size, const Type* keyType, const Type* valueType, const Traits& traits,
                             const MethodTable& methods, ArenaAllocator& allocator)
    : Type { typeName, typeID, Subclass::Container, size, 16, traits, methods, allocator }
    , keyType(keyType)
    , valueType(valueType) {
}

} // namespace Typhoon::Reflection
