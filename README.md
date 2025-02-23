# About

This is a simple and easy to use C++ build system for Linux designed for use with C++20 modules in mind, it uses JSON based project files.

# Install

## Prerequisites:
```
lld
clang 18>
git
```
## Download Release

Please view the latest x86 version in the [releases](https://github.com/Catcrafts/Crafter.Build/releases)

## Build from source
```bash
git clone https://github.com/Catcrafts/Crafter.Build.git
cd Crafter.Build
./build.sh
```
(optionally add to path)

# How to use

## Quickstart

create a ``project.json`` in an empty folder, open it in your preferred text editor.
Create a basic project file, with a base configuration and debug and release configuration
```JSON
{
    "name": "hello-world",
    "configurations": [
        {
            "name": "base",
            "standard": "c++26",
            "source_files": ["main"],
            "module_files": [],
            "build_dir": "./build",
            "output_dir": "./bin"
        },
        {
            "name": "debug",
            "extends": ["base"],
            "optimization_level": "0"
        }
        {
            "name": "release",
            "extends": ["base"],
            "optimization_level": "3"
        }
    ]
}
```
Save and close the file, create a ``main.cpp``
```cpp
#include <print>
int main() {
  std::println("Hello World!");
}
```
Save and close, then run ``crafter-build -c debug``. Now you can run the ``hello-world`` executable that has appeared in the ``bin`` folder

## CLI arguments

``--help`` Displays a help message.

``-c`` The name of the configuration to build.

``-p`` The path to the project file, defaults to ``project.json``.

``-o`` Overrides the output folder.

``-r`` Runs the executable after building.

## configuration properties
``name`` Name of the configuration.

``standard`` C++ standard that this configuration uses, please refer to the [relevant clang documentation](https://clang.llvm.org/cxx_status.html), defaults to ``c++26``.

``source_files`` All source files of the project ``.cpp`` extension is assumed.

``module_files`` All C++ 20 module files of the project ``.cppm`` extension is assumed.

``additional_files`` Files that will be copied to the output_dir.

``build_dir`` The directory where intermediate files are stored.

``output_dir`` The directory where the output files will be placed.

``type`` The type of the project: ``executable``, ``library``, ``shared-library``, defaults to ``executable``.

``extends`` An array of configuration names that this configuration extends, later elements in the array take priority over previous ones.

``optimization_level`` Please refer to the [relevant clang documentation](https://clang.llvm.org/docs/CommandGuide/clang.html#code-generation-options), defaults to ``0``.

``dependencies`` An object array of the dependencies of this project, example:
```json
"dependencies": [
    {
        "path":"/home/Crafter.Build/project.json",
        "configuration":"debug-lib"
    }
    {
        "path":"https://github.com/Catcrafts/Crafter.Build.git",
        "configuration":"debug-lib"
    }
]
```
This will now link the library of Crafter.Build in the configuration you use this in.

``target`` Clang triplet this configuration uses, please refer to the [relevant clang documentation](https://clang.llvm.org/docs/CrossCompilation.html#target-triple).

Note: the WASI SDK needs to be installed to compile to webassmbly, and ``$WASI_SDK_PATH`` needs to be set, please refer to the [wasi-sdk installation guide](https://github.com/WebAssembly/wasi-sdk?tab=readme-ov-file#install).



