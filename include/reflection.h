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

void initReflection(Allocator& allocator);
void initReflection();
void deinitReflection();

TypeDB& getTypeDB();
const Type& getType(TypeId typeID);
const Type* tryGetType(TypeId typeID);

}

// Namespace alias
#if TY_REFLECTION_ALIAS_NAMESPACE
namespace refl = Typhoon::Reflection;
#endif
