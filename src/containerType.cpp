#include "containerType.h"

namespace Typhoon::Reflection {

ContainerType::ContainerType(const char* typeName, TypeId typeID, size_t size, const Type* keyType, const Type* valueType, const MethodTable& methods, Allocator& allocator)
    : Type(typeName, typeID, Subclass::Container, size, 16, methods, allocator)
    , keyType(keyType)
    , valueType(valueType) {
}

} // namespace Typhoon::Reflection
