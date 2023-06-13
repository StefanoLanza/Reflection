#pragma once

// Set to 1/0 to enable/disable XML serialization support
#ifndef TY_REFLECTION_XML
#define TY_REFLECTION_XML 1
#endif

// Set to 1/0 to enable/disable JSON serialization support
#ifndef TY_REFLECTION_JSON
#define TY_REFLECTION_JSON 1
#endif

#ifndef TY_REFLECTION_STD
#define TY_REFLECTION_STD 1
#endif

// Set to 1 to enable the namespace alias "refl" for "Typhoon::Reflection"
#ifndef TY_REFLECTION_ALIAS_NAMESPACE
#define TY_REFLECTION_ALIAS_NAMESPACE 1
#endif

#if TY_REFLECTION_ALIAS_NAMESPACE
namespace Typhoon::Reflection {}

// Namespace alias
namespace refl = Typhoon::Reflection;
#endif
