#pragma once

#include <IO/fwdDecl.h>
#include <reflection/config.h>

#include <string_view>

namespace Typhoon {

namespace Reflection {
class XMLInputArchive;
class JSONInputArchive;
struct ParseResult;

}

#if TY_REFLECTION_XML

//! Open and parse an XML file
/*! \param file source file
 * \return true on success
 */
refl::ParseResult openXMLArchive(Reflection::XMLInputArchive& archive, std::string_view fileName, IO::FileServer& fileServer);

#endif

#if TY_REFLECTION_JSON

//! Open and parse an JSON file
/*! \param file source file
 * \return true on success
 */
refl::ParseResult openJSONArchive(Reflection::JSONInputArchive& archive, std::string_view fileName, IO::FileServer& fileServer);

#endif

} // namespace Typhoon
