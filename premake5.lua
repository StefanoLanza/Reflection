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
local filter_vs = "action:vs*"
local filter_xcode = "action:xcode*"
local filter_gmake = "action:gmake*"
local filter_clang = "toolset:clang"
local filter_x86 = "platforms:x86"
local filter_x64 = "platforms:x86_64"
local filter_debug =  "configurations:Debug*"
local filter_release =  "configurations:Release*"

workspace ("Typhoon-Reflection")
configurations { "Debug", "Release" }
platforms { "x86", "x86_64" }
language "C++"
location (workspacePath)
characterset "MBCS"
flags   { "MultiProcessorCompile", } --"ConformanceMode", }
startproject "UnitTest"
exceptionhandling "Off"
cppdialect "c++17"
rtti "Off"

filter { filter_vs }
	buildoptions {
		"/permissive-",
	}
	system "Windows"
	defines { "_ENABLE_EXTENDED_ALIGNED_STORAGE", "_HAS_EXCEPTIONS=0", }
	-- systemversion "10.0.17134.0"

filter { filter_xcode }
	system "macosx"
	systemversion("10.12") -- MACOSX_DEPLOYMENT_TARGET

filter { filter_clang }
	buildoptions { "-stdlib=libc++ -Wno-unused-command-line-argument" }
	linkoptions { "-stdlib=libc++ -v -fuse-ld=gold" }

filter {  "toolset:gcc" }
    -- https://stackoverflow.com/questions/39236917/using-gccs-link-time-optimization-with-static-linked-libraries
    buildoptions { "-ffat-lto-objects" }

filter { filter_x86 }
	architecture "x86"
	  
filter { filter_x64 }
	architecture "x86_64"

filter { filter_vs, filter_x86, }
	defines { "WIN32", "_WIN32", }

filter { filter_vs, filter_x64, }
	defines { "WIN64", "_WIN64", }

filter { filter_vs, filter_debug, }
	defines { "_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1", "_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT=1",  }

filter { filter_vs, filter_release, }
	defines { "_ITERATOR_DEBUG_LEVEL=0", "_SECURE_SCL=0", "_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1", "_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT=1",  }

filter { filter_debug }
	defines { "_DEBUG", "DEBUG", }
	flags   { "NoManifest", }
	optimize("Off")
	inlining "Default"
	warnings "Extra"
	symbols "Full"
	runtime "Debug"

filter { filter_release }
	defines { "NDEBUG", }
	flags   { "NoManifest", "LinkTimeOptimization", "NoBufferSecurityCheck", "NoRuntimeChecks", }
	optimize("Full")
	inlining "Auto"
	warnings "Extra"
	symbols "Off"
	runtime "Release"

filter {}

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
		filter { filter_gmake }
			links({"Core", "TinyXML"})
		filter {}

	project("Example2")
		kind "ConsoleApp"
		files { "examples/example2.cpp", "examples/utils.*", }
		sysincludedirs { "include", "external", }
		links({"Reflection", })
		filter { filter_gmake }
			links({"Core", "TinyXML"})
		filter {}

	project("Example3")
		kind "ConsoleApp"
		files { "examples/example3.cpp", "examples/utils.*", }
		sysincludedirs { "include", "external", }
		links({"Reflection", })
		filter { filter_gmake }
			links({"Core", "TinyXML"})
		filter {}

	project("Example4")
		kind "ConsoleApp"
		files { "examples/example4.cpp", "examples/utils.*", }
		sysincludedirs { "include", "external",  }
		links({"Reflection", })
		filter { filter_gmake }
			links({"Core", "TinyXML"})
		filter {}

	project("Example5")
		kind "ConsoleApp"
		files { "examples/example5.cpp", "examples/utils.*", }
		sysincludedirs { "include", "external",  }
		links({"Reflection", })
		filter { filter_gmake }
			links({"Core", "TinyXML"})
		filter {}
end

if _OPTIONS["with-tests"] then
	project("UnitTest")
		kind "ConsoleApp"
		files "test/**.*"
		sysincludedirs { "include", "external", }
		links({"Reflection", })
		filter { filter_gmake }
			links({"Core", "TinyXML"})
		filter {}
end
