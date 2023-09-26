#include "variantType.h"
#include "variant.h"

namespace Typhoon::Reflection {

VariantType::VariantType(Allocator& allocator)
    : Type("Variant", Typhoon::getTypeId<Variant>(), Subclass::Variant, sizeof(Variant), alignof(Variant), detail::buildMethodTable<Variant>(), allocator) {
}

} // namespace Typhoon::Reflection
