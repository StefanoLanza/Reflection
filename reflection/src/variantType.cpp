#include "variantType.h"
#include "variant.h"

namespace Typhoon {

VariantType::VariantType()
    : Type(Typhoon::getTypeId<Variant>(), Subclass::Variant, 0, 0, {}) {
}

} // namespace Typhoon
