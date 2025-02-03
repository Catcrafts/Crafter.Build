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
#include "json.hpp"
module Crafter.Build;
using namespace Crafter::Build;
namespace fs = std::filesystem;

Configuration::Configuration(std::string name, std::string standard, std::vector<fs::path> sourceFiles, std::vector<fs::path> moduleFiles, std::string optimizationLevel, std::string buildDir, std::string outputDir, std::string type, std::string target, std::string march, std::vector<Dependency> dependencies, std::vector<fs::path> additionalFiles, std::vector<std::string> flags): name(name), standard(standard), sourceFiles(sourceFiles), moduleFiles(moduleFiles), optimizationLevel(optimizationLevel), buildDir(buildDir), outputDir(outputDir), type(type), target(target), march(march), dependencies(dependencies), additionalFiles(additionalFiles), flags(flags) {

}

Configuration::Configuration(const nlohmann::json& configs, const nlohmann::json& config, fs::path workingDir) {
     name = config["name"].get<std::string>();
     for (auto& [key, val] : config.items())
     {
        if(key == "standard"){
              standard = val.get<std::string>();
        } else if(key == "target") {
             target = val.get<std::string>();
        } else if(key == "type") {
            type = val.get<std::string>();
        } else if(key == "march") {
            march = val.get<std::string>();
        } else if(key == "source_files") {
            const std::vector<std::string> tempSourceFiles = val.get<std::vector<std::string>>();
            sourceFiles = std::vector<fs::path>(tempSourceFiles.size());
            for(std::int_fast32_t i = 0; i < sourceFiles.size(); i++){
                const std::filesystem::path filePath (tempSourceFiles[i]);
                const std::filesystem::path fullFilePath = workingDir / filePath;
                sourceFiles[i] = fullFilePath.generic_string();
            }
        } else if(key == "flags") {
            flags = val.get<std::vector<std::string>>();
        } else if(key == "module_files") {
            const std::vector<std::string> tempModuleFiles = val.get<std::vector<std::string>>();
            moduleFiles = std::vector<fs::path>(tempModuleFiles.size());
            for(std::int_fast32_t i = 0; i < moduleFiles.size(); i++){
                const std::filesystem::path filePath (tempModuleFiles[i]);
                const std::filesystem::path fullFilePath = workingDir / filePath;
                moduleFiles[i] = fullFilePath.generic_string();
            }
        } else if(key == "additional_files") {
            const std::vector<std::string> tempAdditionalFiles = val.get<std::vector<std::string>>();
            additionalFiles = std::vector<fs::path>(tempAdditionalFiles.size());
            for(std::int_fast32_t i = 0; i < additionalFiles.size(); i++){
                const std::filesystem::path filePath (tempAdditionalFiles[i]);
                const std::filesystem::path fullFilePath = workingDir / filePath;
                additionalFiles[i] = fullFilePath.generic_string();
            }
        } else if(key == "optimization_level") {
            optimizationLevel = val.get<std::string>();
        } else if(key == "build_dir") {
            const std::string tempBuildDir = val.get<std::string>();
            const std::filesystem::path buildPath (tempBuildDir);
            const std::filesystem::path fullBuildPath = workingDir / buildPath;
            buildDir = fullBuildPath.generic_string();
        } else if(key == "output_dir") {
            const std::string tempOutputDir = val.get<std::string>();
            const std::filesystem::path outputPath (tempOutputDir);
            const std::filesystem::path fullOutputPath = workingDir / outputPath;
            outputDir = fullOutputPath.generic_string();
        } else if(key == "dependencies") {
            for (auto it : val) {
                dependencies.emplace_back(it["path"].get<std::string>(), it["configuration"].get<std::string>());
            }
        } else if(key != "extends") {
            additionalProperties.insert({key, val});
        }
     }
    if(config.contains("extends")) {
        const std::vector<std::string> extends = config["extends"].get<std::vector<std::string>>();
        for(const std::string& extendName : extends) {
            for (auto it : configs) {
                if(it["name"].get<std::string>() == extendName) {
                    Configuration extendData = Configuration(configs, it, workingDir);
                    if(!extendData.standard.empty() && standard.empty()){
                        standard = extendData.standard;
                    }
                    if(!extendData.sourceFiles.empty()){
                        sourceFiles.insert(sourceFiles.end(), extendData.sourceFiles.begin(), extendData.sourceFiles.end());
                    }
                    if(!extendData.moduleFiles.empty()){
                        moduleFiles.insert(moduleFiles.end(), extendData.moduleFiles.begin(), extendData.moduleFiles.end());
                    }
                    if(!extendData.additionalFiles.empty()){
                        additionalFiles.insert(additionalFiles.end(), extendData.additionalFiles.begin(), extendData.additionalFiles.end());
                    }
                    if(!extendData.optimizationLevel.empty() && optimizationLevel.empty()){
                        optimizationLevel = extendData.optimizationLevel;
                    }
                    if(!extendData.dependencies.empty()){
                        dependencies.insert(dependencies.end(), extendData.dependencies.begin(), extendData.dependencies.end());
                    }
                    if(!extendData.buildDir.empty() && buildDir.empty()) {
                        buildDir = extendData.buildDir;
                    }
                    if(!extendData.outputDir.empty() && outputDir.empty()) {
                        outputDir = extendData.outputDir;
                    }
                    if(!extendData.target.empty() && target.empty()) {
                        target = extendData.target;
                    }
                    if(!extendData.type.empty() && type.empty()) {
                        type = extendData.type;
                    }
                    if(!extendData.march.empty() && march.empty()) {
                        march = extendData.march;
                    }
                    if(!extendData.flags.empty()){
                        flags.insert(flags.end(), extendData.flags.begin(), extendData.flags.end());
                    }
                    break;
                }
            }
        }
    }
}
