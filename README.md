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
* Support for custom serialization procedures
* No exceptions
* No RTTI

# Requirements
* C++ 17

# INSTALLATION
* Clone the repository to a local folder
* For testing
  * Run ```premake5.exe vs2019``` to generate a Visual Studio 2019 solution
  * Select a build configuration (Release, Debug, Win32, x64)
  * Build and run the UnitTest project
* For integration in your own project
  * Add the folders core, reflection and TinyXML to your project build pipeline.

# CONFIGURATION
Look at the file src/config.h Here you can find configuration settings for the library. You can change these settings by either editing this file or by defining them with the preprocessor in your build configuration.

# USAGE
Look inside the examples folder for sample code.

# TODO
- [ ] Custom memory allocator
- [ ] JSON and YAML support
- [ ] Documentation

# CONTRIBUTE
