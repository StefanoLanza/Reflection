#include "variantType.h"
#include "variant.h"

namespace Typhoon::Reflection {

VariantType::VariantType()
    : Type("Variant", Typhoon::getTypeId<Variant>(), Subclass::Variant, sizeof(Variant), alignof(Variant), detail::buildMethodTable<Variant>()) {
}

} // namespace Typhoon::Reflection
