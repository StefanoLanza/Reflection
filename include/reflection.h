#pragma once

#include "config.h"

#include "XMLInputArchive.h"
#include "XMLOutputArchive.h"
#include "bitMaskType.h"
#include "builtinType.h"
#include "cloneObject.h"
#include "readObject.h"
#include "registration.h"
#include "serializeBuiltIns.h"
#include "variant.h"
#include "writeObject.h"

namespace Typhoon::Reflection {

class Allocator;

/**
 * @brief Initialize the reflection library with the default allocator
 * @return The context object
 */
void initReflection();

/**
 * @brief  Initialize the reflection library with a custom allocator
 * @param allocator custom allocator
 */
void initReflection(Allocator& allocator);

/**
 * @brief Terminate the reflection library
 */
void deinitReflection();

/**
 * @brief
 * @param typeID
 * @return
 */
const Type& getType(TypeId typeID);

/**
 * @brief
 * @param typeID
 * @return
 */
const Type* tryGetType(TypeId typeID);

} // namespace Typhoon::Reflection
