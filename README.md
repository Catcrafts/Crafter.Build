# About

This is a simple and easy to use C++ build system for Linux, it uses JSON based project files.

# Install

## Prerequisites:
```
clang 18>
git
```
## Clone and build:
```bash
git clone https://github.com/The-Mighty-Cat/Crafter.Build.git
cd Crafter.Build
./build.sh
```
(optionally add to path)

# How to use

## Quickstart

create a ``project.json`` in an empty folder, open it in your preffered text editor.
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

``--help`` Displays this help message.

``-c`` The name of the configuration to build.

``-p`` The name of the project file.

``-o`` Overrides the output folder.

