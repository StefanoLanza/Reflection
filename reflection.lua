require "external/core/core"

project("TinyXML")
	local path = "external/TinyXML"
	kind "StaticLib"
	files { path .. "/**.cpp" }
	files { path .. "/**.h" }
	includedirs { "TinyXML", }
	exceptionhandling "Off"
	rtti "Off"
	usage "INTERFACE"
		includedirs { "external", }
		links { "TinyXML" }

project("Reflection")
	kind "StaticLib"
	files "src/**.cpp"
	files "src/**.h"
	files "include/**.h"
	includedirs { "include/reflection", "external", }
	exceptionhandling "Off"
	rtti "Off"
	uses { "Core", "TinyXML", }
	usage "INTERFACE"
		uses { "Core" }
		includedirs { "include", }
		links { "Reflection" }
