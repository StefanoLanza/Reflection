#pragma once

#include <file/fwdDecl.h>
#include <reflection/config.h>

namespace Typhoon {

namespace Reflection {
class XMLInputArchive;
class JSONInputArchive;

}

#if TY_REFLECTION_XML

//! Open and parse an XML file
/*! \param file source file
 * \return true on success
 */
bool openXMLArchive(Reflection::XMLInputArchive& archive, const char* fileName, IO::FileServer& fileServer);

#endif

#if TY_REFLECTION_JSON

//! Open and parse an JSON file
/*! \param file source file
 * \return true on success
 */
bool openJSONArchive(Reflection::JSONInputArchive& archive, const char* fileName, IO::FileServer& fileServer);

#endif

} // namespace Typhoon
