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
 * @brief  Initialize the reflection library with a custom allocator
 * @param allocator custom allocator
*/
void initReflection(Allocator& allocator);

/**
 * @brief Initialize the reflection library with the default allocator
 * @return The context object
*/
void initReflection();

/**
 * @brief
*/
void deinitReflection();

TypeDB& getTypeDB();
Allocator& getAllocator();

const Type& getType(TypeId typeID);
const Type* tryGetType(TypeId typeID);

}

// Namespace alias
#if TY_REFLECTION_ALIAS_NAMESPACE
namespace refl = Typhoon::Reflection;
#endif
