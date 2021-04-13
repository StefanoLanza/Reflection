#pragma once

#include "dataPtr.h"

namespace Typhoon::Reflection {

class OutputArchive;
class Type;
struct Context;

namespace detail {

bool writeData(ConstDataPtr data, const Type& type, OutputArchive& archive, const Context& context);

} // namespace detail

} // namespace Typhoon::Reflection
