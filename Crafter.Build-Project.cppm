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
#include <filesystem>
export module Crafter.Build:Project;
import :Configuration;
namespace fs = std::filesystem;

export namespace Crafter::Build {
    class Project {
    public:
        std::string name;
        fs::path path;
        std::vector<Configuration> configurations;
        Project(std::string name, fs::path path, std::vector<Configuration> configurations);
        void Build(std::string configuration);
        void Build(std::string configuration, fs::path outputDir);
        void Build(Configuration configuration);
        void Build(Configuration configuration, fs::path outputDir);
        void SaveToJSON(fs::path path);
        static Project LoadFromJSON(fs::path path);
    };
}
