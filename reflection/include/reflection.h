#pragma once

#include "config.h"

#include "XMLInputArchive.h"
#include "XMLOutputArchive.h"
#include "cloneObject.h"
#include "readObject.h"
#include "registration.h"
#include "variant.h"
#include "writeObject.h"

namespace Typhoon {

void initReflection(TypeDB& typeDB);

}
