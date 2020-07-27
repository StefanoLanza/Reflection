#include "variantType.h"
#include "variant.h"

namespace Typhoon::Reflection {

VariantType::VariantType()
    : Type(Typhoon::getTypeId<Variant>(), Subclass::Variant, 0, 0, {}) {
}

} // namespace Typhoon::Reflection
