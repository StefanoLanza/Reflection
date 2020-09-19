#pragma once

#include <file/fwdDecl.h>

namespace Typhoon {

namespace Reflection {
class XMLInputArchive;

}

//! Open and parse an XML file
/*! \param file source file
 * \return true on success
 */
bool openXMLArchive(Reflection::XMLInputArchive& archive, const char* fileName, IO::FileServer& fileServer);

} // namespace Typhoon
