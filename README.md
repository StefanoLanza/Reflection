**Typhoon Reflection** is a C++ library for reflection and serialization of builtin types, structures, objects and std containers.

# Features
* Reflection and serialization of
  * builtin types (char, short, int, float, etc.)
  * enums and bitmasks
  * structures with a C-style API
  * classes
  * std containers
  * std::pair, std::tuple
  * std smart pointers
* Support for XML and JSON formats
* Configurable memory allocation
* Support for custom serialization procedures
* No exceptions
* No RTTI

# Requirements
* C++ 17 comformant compiler

# INSTALLATION
* Clone the repository to a local folder
  git clone https://github.com/StefanoLanza/Reflection.git
* Reflection uses [Premake](https://premake.github.io/) to generate projects for Windows, Linux and MacOS. 
* On Windows
  * Run ```premake5.exe vsXXXX --with-tests --with-examples``` to generate a Visual Studio XXXX (either vs2017 or 2019) solution
  * Open build/vs2019/Typhoon-Reflection.sln
  * Select a build configuration (Release, Debug, Win32, x64)
  * Build and run the examples and the UnitTest project
* On MacOS
  * Run ```premake xcode4 --with-tests --with-examples``` to generate an XCode workspace
  * Open build/xcode4/Typhoon-Reflection.xcworkspace
  * Select a scheme under Product/Scheme/
  * Build and run the examples and the UnitTest scheme
* On Linux
  * Run ```premake gmake -cc=gcc --with-tests --with-examples``` to generate GNU makefiles
  * cd build/gmake
  * Build and run the examples and the UnitTest application
     * make --config=configuration (e.g. release_x86)
* For integration in your own application
  * Add the folders include, src, external/core, external/TinyXML, external/rapidjson to your build configuration. Please see premake5.lua as a reference.

# BUILD CONFIGURATION
Look at the file include/reflection/config.h Here you can find configuration settings for the library. You can change these settings by either editing this file or by defining them with the preprocessor in your build configuration.

# USAGE
Look inside the examples folder for sample code.

# TODO
- [ ] YAML support
- [ ] Support for attributes
- [ ] Documentation
- [ ] Conan package
