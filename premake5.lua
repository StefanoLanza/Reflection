-- Options
newoption {
	trigger     = "with-tests",
	description = "Build the unit test application",
}

newoption {
	trigger     = "with-examples",
	description = "Build the examples",
}

-- Global settings
local workspacePath = path.join("build", _ACTION)  -- e.g. build/vs2019 or build/xcode4

-- Filters
local vs = "action:vs*"
local xcode = "action:xcode*"
local x86 = "platforms:x86"
local x64 = "platforms:x86_64"
local debug =  "configurations:Debug*"
local release =  "configurations:Release*"

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

filter { vs }
	buildoptions {
		"/permissive-",
	}
	system "Windows"
	defines { "_ENABLE_EXTENDED_ALIGNED_STORAGE", }
	-- systemversion "10.0.17134.0"

filter { xcode }
	system "macosx"
	systemversion("10.12") -- MACOSX_DEPLOYMENT_TARGET

filter {  "toolset:clang" }
    buildoptions { "-stdlib=libc++" }
    linkoptions { "-stdlib=libc++ -v" }

filter { x86 }
	architecture "x86"
	  
filter { x64 }
	architecture "x86_64"

filter { vs, x86, }
	defines { "WIN32", "_WIN32", }

filter { vs, x64, }
	defines { "WIN64", "_WIN64", }

filter { vs, debug, }
	defines { "_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1", "_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT=1",  }

filter { vs, release, }
	defines { "_ITERATOR_DEBUG_LEVEL=0", "_SECURE_SCL=0", "_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1", "_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT=1",  }

filter { debug }
	defines { "_DEBUG", "DEBUG", }
	flags   { "NoManifest", }
	optimize("Off")
	inlining "Default"
	warnings "Extra"
	symbols "Full"
	runtime "Debug"

filter { release }
	defines { "NDEBUG", }
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
	sysincludedirs { "./", "include/reflection", "external", }
	links({"Core", "TinyXML"})

if _OPTIONS["with-examples"] then
	project("Example1")
		kind "ConsoleApp"
		files { "examples/example1.cpp", "examples/utils.*", }
		sysincludedirs { "include", "external", }
		links({"Reflection", })

	project("Example2")
		kind "ConsoleApp"
		files { "examples/example2.cpp", "examples/utils.*", }
		sysincludedirs { "include", "external", }
		links({"Reflection", })

	project("Example3")
		kind "ConsoleApp"
		files { "examples/example3.cpp", "examples/utils.*", }
		sysincludedirs { "include", "external", }
		links({"Reflection", })

	project("Example4")
		kind "ConsoleApp"
		files { "examples/example4.cpp", "examples/utils.*", }
		sysincludedirs { "include", "external",  }
		links({"Reflection", })

	project("Example5")
		kind "ConsoleApp"
		files { "examples/example5.cpp", "examples/utils.*", }
		sysincludedirs { "include", "external",  }
		links({"Reflection", })
end

if _OPTIONS["with-tests"] then
	project("UnitTest")
		kind "ConsoleApp"
		links({"Reflection", })
		files "test/**.*"
		sysincludedirs { "include", "external", }
end
