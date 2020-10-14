-- Global settings
local workspacePath = path.join("build", _ACTION)  -- e.g. build/vs2019 or build/xcode4

workspace ("Typhoon-Reflection")
configurations { "Debug", "Release" }
platforms { "x86", "x86_64" }
language "C++"
location (workspacePath)
characterset "MBCS"
flags   { "MultiProcessorCompile", } --"ConformanceMode", }
startproject "UnitTest"
exceptionhandling "Off"
defines { "_HAS_EXCEPTIONS=0" }
cppdialect "c++17"
rtti "Off"

filter "action:vs*"
	buildoptions
	{
		"/permissive-",
	}
	system "Windows"
	-- systemversion "10.0.17134.0"


filter "action:xcode*"
	system "macosx"

filter "platforms:x86"
	architecture "x86"
	defines { "WIN32", "_WIN32", }

filter "platforms:x86_64"
	architecture "x86_64"
	defines { "WIN64", "_WIN64", }

filter "configurations:Debug*"
	defines { "_DEBUG", "DEBUG", "_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1", "_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT=1", "_ENABLE_EXTENDED_ALIGNED_STORAGE", }
	flags   { "NoManifest", }
	optimize("Off")
	inlining "Default"
	warnings "Extra"
	symbols "Full"
	runtime "Debug"

filter "configurations:Release*"
	defines { "NDEBUG", "_ITERATOR_DEBUG_LEVEL=0", "_SECURE_SCL=0", "_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1", "_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT=1", "_ENABLE_EXTENDED_ALIGNED_STORAGE", }
	flags   { "NoManifest", "LinkTimeOptimization", "NoBufferSecurityCheck", "NoRuntimeChecks", }
	optimize("Full")
	inlining "Auto"
	warnings "Extra"
	symbols "Off"
	runtime "Release"

project("TinyXML")
	kind "StaticLib"
	files "external/TinyXML/**.cpp"
	files "external/TinyXML/**.h"
	includedirs { "TinyXML", }

project("Core")
	kind "StaticLib"
	files "external/core/**.cpp"
	files "external/core/**.h"
	includedirs { "./", }

project("Reflection")
	kind "StaticLib"
	files "src/**.cpp"
	files "src/**.h"
	files "include/**.h"
	sysincludedirs { "./", "include", "external", }
	links({"Core", "TinyXML"})

project("Example1")
	kind "ConsoleApp"
	files { "examples/example1.cpp", "examples/utils.*", }
	sysincludedirs { "./", "include", "external", }
	links({"Reflection", })

project("Example2")
	kind "ConsoleApp"
	files { "examples/example2.cpp", "examples/utils.*", }
	sysincludedirs { "./", "include", "external", }
	links({"Reflection", })

project("Example3")
	kind "ConsoleApp"
	files { "examples/example3.cpp", "examples/utils.*", }
	sysincludedirs { "./", "include", "external", }
	links({"Reflection", })

project("Example4")
	kind "ConsoleApp"
	files { "examples/example4.cpp", "examples/utils.*", }
	sysincludedirs { "./", "include", "external",  }
	links({"Reflection", })

project("Example5")
	kind "ConsoleApp"
	files { "examples/example5.cpp", "examples/utils.*", }
	sysincludedirs { "./", "include", "external",  }
	links({"Reflection", })

project("UnitTest")
	kind "ConsoleApp"
	links({"Reflection", })
	files "test/**.cpp"
	sysincludedirs { "./", "include", "external", }
