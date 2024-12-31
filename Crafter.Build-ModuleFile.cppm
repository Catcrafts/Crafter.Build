/*
Crafter.Build
Copyright (C) 2024 Catcrafts
Catcrafts.net

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 3.0 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
*/

module;
#include <string>
#include <filesystem>
#include <mutex>
#include <unordered_map>
export module Crafter.Build:ModuleFile;
import :Configuration;
namespace fs = std::filesystem;

export namespace Crafter::Build {
    class ModuleFile {
    public:
        inline static std::mutex allFilesMutex;
        std::mutex fileMutex;
        inline static std::unordered_map<fs::path, ModuleFile*> allFiles;
        bool needsRecompiling;
        bool recompiled = false;
        fs::path path;
        std::vector<ModuleFile*> dependencies;
        static ModuleFile* CompileModuleFile(fs::path path, std::string clangDir, const Configuration& config, fs::path pcmDir, std::string target);
    private:
        ModuleFile(fs::path path, std::string clangDir, const Configuration& config, fs::path pcmDir, std::string target);
        void Compile(std::string clangDir, const Configuration& config, fs::path pcmDir, std::string target);
    };
}
