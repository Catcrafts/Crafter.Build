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
#include <fstream>
#include <iostream>
#include <regex>
#include <print>
module Crafter.Build;
using namespace Crafter::Build;
namespace fs = std::filesystem;


ModuleFile* ModuleFile::CompileModuleFile(fs::path path, std::string clangDir, const Configuration& config, fs::path pcmDir, std::string target) {
    if(fs::exists(path.generic_string()+".cppm")) {
        allFilesMutex.lock();
        if(!allFiles.contains(path)) {
            return new ModuleFile(path, clangDir, config, pcmDir, target);
        } else{
            allFilesMutex.unlock();
            return allFiles[path];
        }
    } else{
        return nullptr;
    }
}

ModuleFile::ModuleFile(fs::path path, std::string clangDir, const Configuration& config, fs::path pcmDir, std::string target): path(path), recompiled(false) {
    fileMutex.lock();
    allFiles.insert({path,this});
    allFilesMutex.unlock();
    needsRecompiling = !fs::exists((pcmDir/path.filename()).generic_string()+".pcm") || fs::last_write_time(path.generic_string()+".cppm") > fs::last_write_time((pcmDir/path.filename()).generic_string()+".pcm");
    std::ifstream t(path.generic_string()+".cppm");
    std::stringstream buffer;
    buffer << t.rdbuf();
    std::string fileContent = buffer.str();

    std::regex rgx("import (.*);");
    std::smatch matches;
    std::regex_search(fileContent, matches, rgx);

    std::string::const_iterator searchStart( fileContent.cbegin() );
    while ( regex_search( searchStart, fileContent.cend(), matches, rgx ) )
    {
        std::string matchString = matches[1].str();
        if(matchString[0] == ':'){
            std::regex rgx2("export module ([a-zA-Z_\\-0-9\\.]*).*;");
            std::smatch matches2;
            std::regex_search(fileContent, matches2, rgx2);
            matchString.erase(0, 1);
            matchString = matches2[1].str()+"-"+matchString;
        }
        fs::path importPath = path.remove_filename()/fs::path(matchString);
        ModuleFile* file = CompileModuleFile(importPath,clangDir, config, pcmDir, target);
        if(file){
            file->fileMutex.lock();

            if(file->needsRecompiling){
                needsRecompiling = true;
            }

            file->fileMutex.unlock();
            dependencies.push_back(file);
        }
        searchStart = matches.suffix().first;
    }
    fileMutex.unlock();
    Compile(clangDir, config, pcmDir, target);
}

void ModuleFile::Compile(std::string clangDir, const Configuration& config, fs::path pcmDir, std::string target) {
    if(needsRecompiling) {
        fileMutex.lock();
        if(!recompiled){
            for(ModuleFile* dep : dependencies){
                dep->Compile(clangDir, config, pcmDir, target);
            }

            std::string flags;
            for(const std::string& flag : config.flags) {
                flags+=flag;
            }
            
            std::string march;
            if(config.target != "wasm32-unknown-wasi"){
                march = std::format("-march={}", config.march);
            }

            system(std::format("{} -std={} {}.cppm --precompile {} {} -fprebuilt-module-path={} -o {}.pcm {}", clangDir, config.standard, path.generic_string(), march, flags, pcmDir.generic_string(), (pcmDir/path.filename()).generic_string(), target).c_str());

            recompiled = true;
            fileMutex.unlock();

            system(std::format("{} -std={} {}.pcm -fprebuilt-module-path={} -c -O{} {} {} -o {}.o {}", clangDir, config.standard, (pcmDir/path.filename()).generic_string(), pcmDir.generic_string(), config.optimizationLevel, march, flags, (config.buildDir/path.filename()).generic_string(), target).c_str());
        } else {
            fileMutex.unlock();
        }
    }
}
