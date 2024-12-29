/*
Crafter.Build
Copyright (C) 2024 Jorijn van der Graaf

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
USA
*/

module;
#include <vector>
#include <string>
#include <fstream>
#include "json.hpp"
module Crafter.Build:ConfigurationImpl;
import :Configuration;
using namespace Crafter::Build;

Configuration::Configuration(std::string name, std::string standard, std::vector<std::string> sourceFiles, std::vector<std::string> moduleFiles, std::string optimizationLevel, std::string buildDir, std::string outputDir): name(name), standard(standard), sourceFiles(sourceFiles), moduleFiles(moduleFiles), optimizationLevel(optimizationLevel), buildDir(buildDir), outputDir(outputDir){

}
