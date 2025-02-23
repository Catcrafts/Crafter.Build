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
#include <vector>
#include <string>
#include <print>
#include <fstream>
#include <iostream>
#include "json.hpp"
#include <filesystem>
#include <thread>
module Crafter.Build;
using namespace Crafter::Build;
namespace fs = std::filesystem;

Project::Project(std::string name, fs::path path, std::vector<Configuration> configurations) : name(name), path(path), configurations(configurations) {

}

void Project::Build(std::string configuration) const {
    for(const Configuration& config : configurations) {
        if(config.name == configuration){
            Build(config);
            return;
        }
    }
    throw std::runtime_error("Configuration: " + configuration + " not found.");
}

void Project::Build(std::string configuration, fs::path outputDir) const {
    for(const Configuration& config : configurations) {
        if(config.name == configuration){
            Build(config, outputDir);
            return;
        }
    }
    throw std::runtime_error("Configuration: " + configuration + " not found.");
}

void Project::Build(std::string configuration, fs::path outputDir, fs::path binDir) const {
    for(const Configuration& config : configurations) {
        if(config.name == configuration){
            Build(config, outputDir, binDir);
            return;
        }
    }
    throw std::runtime_error("Configuration: " + configuration + " not found.");
}

void Project::Build(Configuration configuration) const {
    Build(configuration, configuration.outputDir);
}

void Project::Build(Configuration config, fs::path outputDir) const {
    Build(config, outputDir, outputDir);
}

void Project::Build(Configuration config, fs::path outputDir, fs::path binDir) const {
    if(config.standard.empty()) {
        config.standard = "c++26";
    }
    if(config.march.empty()) {
        config.march = "native";
    }
    if(config.type.empty()) {
        config.type = "executable";
    }

    if (!fs::exists(config.buildDir)) {
        fs::create_directory(config.buildDir);
    }
    if (!fs::exists(outputDir)) {
        fs::create_directory(outputDir);
    }

    std::string target;
    if(!config.target.empty()){
        target = std::format("-target {}", config.target);
    }
    fs::path pcmDir;
    if(config.type == "library" || config.type == "shared-library"){
        pcmDir = outputDir;
    }else{
        pcmDir = config.buildDir;
    }

    std::string libs;

    std::vector<std::thread> depThreads = std::vector<std::thread>(config.dependencies.size());

    if(config.dependencies.size() > 0){
        libs += std::format(" -L{}", pcmDir.generic_string());
    }

    for(std::int_fast32_t i = 0; i < depThreads.size(); i++) {
        if(config.dependencies[i].path.ends_with(".git")) {
            system(std::format("cd {} && git clone {}", config.buildDir, config.dependencies[i].path).c_str());
            config.dependencies[i].path = fs::path(config.dependencies[i].path).filename().replace_extension();
            Project project = Project::LoadFromJSON(fs::path(config.buildDir)/config.dependencies[i].path/"project.json");
            libs+=std::format(" -l{}", project.name);
            depThreads[i] = std::thread([i, pcmDir, config, project, binDir]() {
                project.Build(config.dependencies[i].configuration, pcmDir, binDir);
            });
        } else{
            Project project = Project::LoadFromJSON(config.dependencies[i].path);
            libs+=std::format(" -l{}", project.name);
            depThreads[i] = std::thread([i, pcmDir, config, project, binDir]() {
                project.Build(config.dependencies[i].configuration, pcmDir, binDir);
            });
        }
       
    }

    std::string name = this->name;

    std::string clangDir;
    if(config.target == "wasm32-unknown-wasi" || config.target == "wasm64-unknown-wasi"){
        clangDir = "${WASI_SDK_PATH}/bin/clang++ --sysroot=${WASI_SDK_PATH}/share/wasi-sysroot -Wno-unused-command-line-argument -Wl,--export-all -fno-exceptions";
        if(config.type != "library") {
            name+=".wasm";
        }
    } else if(config.target == "wasm32" || config.target == "wasm64") {
        clangDir = "${WASI_SDK_PATH}/bin/clang++ --no-standard-libraries -Wl,--no-entry -Wl,--export-all -Wno-unused-command-line-argument -fno-exceptions";
        if(config.type != "library") {
            name+=".wasm";
        }
    } else {
        clangDir = "clang++ -Wno-unused-command-line-argument";
    }

    std::string flags;
    for(const std::string& flag : config.flags) {
        flags+=flag;
    }

    for(std::thread& thread : depThreads){
        thread.join();
    }

    std::string files;
    //std::vector<std::thread> moduleThreads(config.moduleFiles.size());
    for(std::uint_fast32_t i = 0; i < config.moduleFiles.size(); i++) {
        //moduleThreads[i] = std::thread([i, &config, pcmDir, target, clangDir](){
            ModuleFile::CompileModuleFile(config.moduleFiles[i], clangDir, config, pcmDir, target);
        //});
        files+=std::format("{}.o ",(config.buildDir/config.moduleFiles[i].filename()).generic_string());
    }
    // for(std::thread& thread : moduleThreads){
    //     thread.join();
    // }

    std::string march;
    if(config.target != "wasm32-unknown-wasi"){
        march = std::format("-march={}", config.march);
    }

    std::vector<std::thread> threads;
    for(std::uint_fast32_t i = 0; i < config.sourceFiles.size(); i++) {
        files+=std::format("{}_source.o ",(config.buildDir/config.sourceFiles[i].filename()).generic_string());
        //if(!fs::exists((config.buildDir/config.sourceFiles[i].filename()).generic_string()+"_source.o") || fs::last_write_time(config.sourceFiles[i].generic_string()+".cpp") > fs::last_write_time((config.buildDir/config.sourceFiles[i].filename()).generic_string()+"_source.o")) {
            threads.emplace_back([i, &config, pcmDir, target, clangDir, flags, march](){
                system(std::format("{} -std={} {}.cpp -fprebuilt-module-path={} -c -O{} {} {} -o {}_source.o {}", clangDir, config.standard, config.sourceFiles[i].generic_string(), pcmDir.generic_string(), config.optimizationLevel, march, flags, (config.buildDir/config.sourceFiles[i].filename()).generic_string(), target).c_str());
            });
        //}
    }

    for(std::thread& thread : threads){
        thread.join();
    }

    if(config.type == "executable"){
         system(std::format("{} {}-O{} -o {} {} {} -fuse-ld=lld", clangDir, files, config.optimizationLevel, (outputDir/name).generic_string(), target, libs).c_str());
    } else if(config.type == "library"){
         system(std::format("ar r {}.a {}", (outputDir/fs::path("lib"+name)).generic_string(), files).c_str());
    } else if(config.type == "shared-library"){
        system(std::format("ar r {}.so {}", (outputDir/fs::path("lib"+name)).generic_string(), files).c_str());
    }

    for(const fs::path& additionalFile : config.additionalFiles){
        if(!fs::exists(binDir/additionalFile.filename())) {
            fs::copy(additionalFile, binDir);
        } else if (fs::last_write_time(additionalFile) > fs::last_write_time(binDir/additionalFile.filename())){
            fs::remove(binDir/additionalFile.filename());
            fs::copy(additionalFile, binDir);
        }
    }
}

Project Project::LoadFromJSON(fs::path path) {
     if (!fs::exists(path)) {
         throw std::runtime_error(std::format("Project file: {} not found.", path.generic_string()));
     }

    std::ifstream f(path);
    nlohmann::json data = nlohmann::json::parse(f);
    const std::string name = data["name"].get<std::string>();
    std::vector<Configuration> configurations;
    nlohmann::json configs = data["configurations"];

    const fs::path workingDir = path.remove_filename();
    for (nlohmann::json::iterator it = configs.begin(); it != configs.end(); ++it) {
        configurations.emplace_back(configs, (*it), workingDir);
    }
    return Project(name, workingDir, configurations);
}
