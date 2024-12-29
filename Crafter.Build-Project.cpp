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
#include <iostream>
#include "json.hpp"
#include <filesystem>
#include <thread>
module Crafter.Build:ProjectImpl;
import :Project;
import :Configuration;
using namespace Crafter::Build;

Project::Project(std::string name, std::vector<Configuration> configurations) : name(name), configurations(configurations) {

}

struct ConfigData {
    std::string standard;
    std::vector<std::string> sourceFiles;
    std::vector<std::string> moduleFiles;
    std::string optimizationLevel;
    std::string buildDir;
    std::string outputDir;
    std::string type;
    std::string target;
};

ConfigData CollapseConfig(nlohmann::json& configs,nlohmann::json& config) {
    ConfigData data;
    if(config.contains("standard")) {
        data.standard = config["standard"].get<std::string>();
    }
    if(config.contains("target")) {
        data.target = config["target"].get<std::string>();
    }
    if(config.contains("type")) {
        data.type = config["type"].get<std::string>();
    }
    if(config.contains("source_files")) {
        data.sourceFiles = config["source_files"].get<std::vector<std::string>>();
    }
    if(config.contains("module_files")) {
        data.moduleFiles = config["module_files"].get<std::vector<std::string>>();
    }
    if(config.contains("optimization_level")) {
        data.optimizationLevel = config["optimization_level"].get<std::string>();
    }
    if(config.contains("build_dir")) {
        data.buildDir = config["build_dir"].get<std::string>();
    }
    if(config.contains("output_dir")) {
        data.outputDir = config["output_dir"].get<std::string>();
    }
    if(config.contains("extends")){
       const std::vector<std::string> extends = config["extends"].get<std::vector<std::string>>();
       for(const std::string& extendName : extends) {
           for (nlohmann::json::iterator it = configs.begin(); it != configs.end(); ++it) {
               if((*it)["name"].get<std::string>() == extendName){
                   ConfigData extendData = CollapseConfig(configs, (*it));
                   if(!extendData.standard.empty() && data.standard.empty()){
                       data.standard = extendData.standard;
                   }
                   if(!extendData.sourceFiles.empty()){
                       data.sourceFiles.insert(data.sourceFiles.end(), extendData.sourceFiles.begin(), extendData.sourceFiles.end());
                   }
                   if(!extendData.moduleFiles.empty()){
                       data.moduleFiles.insert(data.moduleFiles.end(), extendData.moduleFiles.begin(), extendData.moduleFiles.end());
                   }
                   if(!extendData.optimizationLevel.empty() && data.standard.empty()){
                       data.optimizationLevel = extendData.optimizationLevel;
                   }
                   if(!extendData.buildDir.empty() && data.buildDir.empty()) {
                        data.buildDir = extendData.buildDir;
                   }
                   if(!extendData.outputDir.empty() && data.outputDir.empty()) {
                       data.outputDir = extendData.outputDir;
                   }
                   if(!extendData.target.empty() && data.target.empty()) {
                       data.target = extendData.target;
                   }
                   if(!extendData.type.empty() && data.type.empty()) {
                       data.type = extendData.type;
                   }
                   break;
               }
           }
       }
    }
    return data;
}

void Project::Build(std::string configuration) {
    for(const Configuration& config : configurations) {
        if(config.name == configuration){
            Build(config, config.outputDir);
            return;
        }
    }
    throw std::runtime_error("Configuration: " + configuration + " not found.");
}

void Project::Build(std::string configuration, std::string outputDir) {
    for(const Configuration& config : configurations) {
        if(config.name == configuration){
            Build(config, outputDir);
            return;
        }
    }
    throw std::runtime_error("Configuration: " + configuration + " not found.");
}

void Project::Build(Configuration configuration) {
    Build(configuration, configuration.outputDir);
}

void Project::Build(Configuration config, std::string outputDir) {
    if (!std::filesystem::exists(config.buildDir)) {
        std::filesystem::create_directory(config.buildDir);
    } else {
        for (const auto& entry : std::filesystem::directory_iterator(config.buildDir)){
            std::filesystem::remove_all(entry.path());
        }
    }
    if (!std::filesystem::exists(outputDir)) {
        std::filesystem::create_directory(outputDir);
    } else {
        for (const auto& entry : std::filesystem::directory_iterator(outputDir)){
            std::filesystem::remove_all(entry.path());
        }
    }

    std::string target;
    if(!config.target.empty()){
        target = std::format("-target {}", config.target);
    }
    std::string pcmDir;
    if(config.type == "library" || config.type == "shared-library"){
        pcmDir = outputDir;
    }else{
        pcmDir = config.buildDir;
    }

    for(const std::string& moduleFile : config.moduleFiles){
        system(std::format("clang++ -std={} {}.cppm --precompile -fprebuilt-module-path={} -o {}/{}.pcm", config.standard, moduleFile, pcmDir, pcmDir, moduleFile).c_str());
    }
    std::vector<std::thread> threads = std::vector<std::thread>(config.moduleFiles.size() + config.sourceFiles.size());
    std::string files;
    for(std::int_fast32_t i = 0; i < config.moduleFiles.size(); i++) {
        files+=std::format("{}/{}.o ",config.buildDir, config.moduleFiles[i]);
        threads[i] = std::thread([i, config, pcmDir, target](){
            system(std::format("clang++ -std={} {}/{}.pcm -fprebuilt-module-path={} -c -O{} -o {}/{}.o {}", config.standard, pcmDir, config.moduleFiles[i], pcmDir, config.optimizationLevel, config.buildDir, config.moduleFiles[i], target).c_str());
        });
    }
    for(std::int_fast32_t i = 0; i < config.sourceFiles.size(); i++) {
        files+=std::format("{}/{}_source.o ",config.buildDir, config.sourceFiles[i]);
        threads[config.moduleFiles.size()+i] = std::thread([i, config, pcmDir, target](){
        system(std::format("clang++ -std={} {}.cpp -fprebuilt-module-path={} -c -O{} -o {}/{}_source.o {}", config.standard, config.sourceFiles[i], pcmDir, config.optimizationLevel, config.buildDir, config.sourceFiles[i], target).c_str());
        });
    }
    for(std::thread& thread : threads){
        thread.join();
    }
    if(config.type == "executable"){
         system(std::format("clang++ {}-O{} -o {}/{} {}", files, config.optimizationLevel, outputDir, name, target).c_str());
    } else if(config.type == "library"){
         system(std::format("ar r {}/{}.a {}", outputDir, name, files, target).c_str());
    } else if(config.type == "shared-library"){
        system(std::format("ar r {}/{}.so {} -shared", outputDir, name, files, target).c_str());
    }

}

Project Project::LoadFromJSON(std::string file) {
     if (!std::filesystem::exists(file)) {
         throw std::runtime_error("Project file: " + file + " not found.");
     }
    std::ifstream f(file);
    nlohmann::json data = nlohmann::json::parse(f);
    const std::string name = data["name"].get<std::string>();
    std::vector<Configuration> configurations;
    nlohmann::json configs = data["configurations"];
    for (nlohmann::json::iterator it = configs.begin(); it != configs.end(); ++it) {
        ConfigData configData = CollapseConfig(configs, (*it));
        configurations.emplace_back((*it)["name"].get<std::string>(), configData.standard, configData.sourceFiles, configData.moduleFiles, configData.optimizationLevel, configData.buildDir, configData.outputDir,configData.type, configData.target);
    }
    return Project(name,configurations);
}
