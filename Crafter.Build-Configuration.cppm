/*
Crafter.Build
Copyright (C) 2024 Catcrafts

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 3 of the License, or (at your option) any later version.

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
#include <string>
#include <vector>
#include "json.hpp"
#include <unordered_map>
export module Crafter.Build:Configuration;
import :Dependency;

namespace fs = std::filesystem;

export namespace Crafter::Build {
    class Configuration {
    public:
        std::string name;
        std::string standard;
        std::vector<fs::path> sourceFiles;
        std::vector<fs::path> moduleFiles;
        std::string optimizationLevel;
        std::string buildDir;
        std::string outputDir;
        std::string type;
        std::string target;
        std::vector<Dependency> dependencies;
        std::unordered_map<std::string, nlohmann::json> additionalProperties;
        Configuration(std::string name, std::string standard, std::vector<fs::path> sourceFiles, std::vector<fs::path> moduleFiles, std::string optimizationLevel, std::string buildDir, std::string outputDir, std::string type, std::string target, std::vector<Dependency> dependencies);
        Configuration(const nlohmann::json& configs, const nlohmann::json& config, fs::path workingDir);
    };
}
