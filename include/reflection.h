#pragma once

#include "config.h"

#include "bitMaskType.h"
#include "builtinType.h"
#include "cloneObject.h"
#include "containerType.h"
#include "pointerType.h"
#include "readObject.h"
#include "serializeBuiltIns.h"
#include "structType.h"
#include "variant.h"
#include "variantType.h"
#include "writeObject.h"
#include "namespace.h"

#include "registration.h"

#ifdef TY_REFLECTION_XML
#include "XMLInputArchive.h"
#include "XMLOutputArchive.h"
#endif

#ifdef TY_REFLECTION_JSON
#include "jsonInputArchive.h"
#include "jsonOutputArchive.h"
#endif

namespace Typhoon {

class Allocator;

}

namespace Typhoon::Reflection {

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

/**
 * @brief
 * @return the global namespace
 */
const Namespace& getGlobalNamespace();

} // namespace Typhoon::Reflection
