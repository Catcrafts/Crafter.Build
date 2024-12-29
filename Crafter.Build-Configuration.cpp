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
#include <filesystem>
module Crafter.Build:ConfigurationImpl;
import :Configuration;
import :Dependency;
using namespace Crafter::Build;
namespace fs = std::filesystem;

Configuration::Configuration(std::string name, std::string standard, std::vector<fs::path> sourceFiles, std::vector<fs::path> moduleFiles, std::string optimizationLevel, std::string buildDir, std::string outputDir, std::string type, std::string target, std::vector<Dependency> dependencies): name(name), standard(standard), sourceFiles(sourceFiles), moduleFiles(moduleFiles), optimizationLevel(optimizationLevel), buildDir(buildDir), outputDir(outputDir), type(type), target(target), dependencies(dependencies) {

}

Configuration::Configuration(nlohmann::json& configs, nlohmann::json& config, fs::path workingDir) {
    name = config["name"].get<std::string>();
    if(config.contains("standard")) {
        standard = config["standard"].get<std::string>();
    }
    if(config.contains("target")) {
        target = config["target"].get<std::string>();
    }
    if(config.contains("type")) {
        type = config["type"].get<std::string>();
    }
    if(config.contains("source_files")) {
        const std::vector<std::string> tempSourceFiles = config["source_files"].get<std::vector<std::string>>();
        sourceFiles = std::vector<fs::path>(tempSourceFiles.size());
        for(std::int_fast32_t i = 0; i < sourceFiles.size(); i++){
            std::filesystem::path filePath (tempSourceFiles[i]);
            std::filesystem::path fullFilePath = workingDir / filePath;
            sourceFiles[i] = fullFilePath.generic_string();
        }
    }
    if(config.contains("module_files")) {
        const std::vector<std::string> tempModuleFiles = config["module_files"].get<std::vector<std::string>>();
        moduleFiles = std::vector<fs::path>(tempModuleFiles.size());
        for(std::int_fast32_t i = 0; i < moduleFiles.size(); i++){
            std::filesystem::path filePath (tempModuleFiles[i]);
            std::filesystem::path fullFilePath = workingDir / filePath;
            moduleFiles[i] = fullFilePath.generic_string();
        }
    }
    if(config.contains("optimization_level")) {
        optimizationLevel = config["optimization_level"].get<std::string>();
    }
    if(config.contains("build_dir")) {
        const std::string tempBuildDir = config["build_dir"].get<std::string>();
        std::filesystem::path buildPath (tempBuildDir);
        std::filesystem::path fullBuildPath = workingDir / buildPath;
        buildDir = fullBuildPath.generic_string();
    }
    if(config.contains("output_dir")) {
        const std::string tempOutputDir = config["output_dir"].get<std::string>();
        std::filesystem::path outputPath (tempOutputDir);
        std::filesystem::path fullOutputPath = workingDir / outputPath;
        outputDir = fullOutputPath.generic_string();
    }
    if(config.contains("dependencies")) {
        nlohmann::json dependenciesJson = config["dependencies"];
        for (nlohmann::json::iterator it = dependenciesJson.begin(); it != dependenciesJson.end(); ++it) {
            dependencies.emplace_back((*it)["path"].get<std::string>(), (*it)["configuration"].get<std::string>(), (*it)["filename"].get<std::string>());
        }
    }
    if(config.contains("extends")){
        const std::vector<std::string> extends = config["extends"].get<std::vector<std::string>>();
        for(const std::string& extendName : extends) {
            for (nlohmann::json::iterator it = configs.begin(); it != configs.end(); ++it) {
                if((*it)["name"].get<std::string>() == extendName){
                    Configuration extendData = Configuration(configs, (*it), workingDir);
                    if(!extendData.standard.empty() && standard.empty()){
                        standard = extendData.standard;
                    }
                    if(!extendData.sourceFiles.empty()){
                        sourceFiles.insert(sourceFiles.end(), extendData.sourceFiles.begin(), extendData.sourceFiles.end());
                    }
                    if(!extendData.moduleFiles.empty()){
                        moduleFiles.insert(moduleFiles.end(), extendData.moduleFiles.begin(), extendData.moduleFiles.end());
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
                    break;
                }
            }
        }
    }
}
