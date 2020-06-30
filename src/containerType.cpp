#include "containerType.h"

namespace Typhoon::Reflection {

ContainerType::ContainerType(TypeId typeID, size_t size, const Type* keyType, const Type* valueType, const MethodTable& methods)
    : Type(typeID, Subclass::Container, size, 16, methods)
    , keyType(keyType)
    , valueType(valueType) {
}

} // namespace Typhoon
