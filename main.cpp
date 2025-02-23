/*
Crafter.Build
Copyright (C) 2024 Catcrafts
Catcrafts.net

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
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
*/

#include <cstdint>
#include <print>
#include "json.hpp"
import Crafter.Build;
using namespace Crafter::Build;
namespace fs = std::filesystem;
int main(int argc, char* argv[]) {
    if(argc == 1) {
        std::println("No arguments provided, use --help for help");
    }
    fs::path filepath = "project.json";
    std::string configuration;
    std::string outputDir;
    bool run = false;
    for (std::uint_fast32_t i = 1; i < argc; i++) {
        std::string arg = std::string(argv[i]);
        if(arg == "--help"){
             std::println("--help\tDisplays this help message.\n-c The name of the configuration to build.\n-p The path to the project file. defualts to project.json\n-o Overrides the output folder.\n-r Runs the executable after building.");
             return 0;
        } else if(arg == "-c"){
            configuration = argv[++i];
        } else if(arg == "-r"){
            run = true;
        } else if(arg == "-o"){
            outputDir = argv[++i];
        } else if(arg == "-p"){
            filepath = fs::path(argv[++i]);
        } else{
            std::println("Unkown argument: {}", argv[i]);
            return 1;
        }
    }

    fs::path projectPath;
    if(filepath.is_relative()){
        projectPath = fs::current_path()/filepath;
    }else{
        projectPath = filepath;
    }

    Project project = Project::LoadFromJSON(projectPath);
    if(outputDir.empty()){
        project.Build(configuration);
    } else{
        project.Build(configuration, fs::path(outputDir));
    }

    if(run){
        for(const Configuration& config : project.configurations) {
            if(config.name == configuration){
                system(std::format("{}", (projectPath/fs::path(config.outputDir)/project.name).generic_string()).c_str());
                return 0;
            }
        }
    }
}
