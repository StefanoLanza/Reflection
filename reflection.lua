require "external/core/core"

project("TinyXML")
	kind "StaticLib"
	files "external/TinyXML/**.cpp"
	files "external/TinyXML/**.h"
	includedirs { "TinyXML", }
	usage "INTERFACE"
		includedirs { "./" }

project("Reflection")
	kind "StaticLib"
	files "src/**.cpp"
	files "src/**.h"
	files "include/**.h"
	externalincludedirs { "./", "include/reflection", "external", }
	uses {"Core", "TinyXML", }
	usage "INTERFACE"
		includedirs { "include/reflection" }
