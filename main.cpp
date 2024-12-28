#include <cstdint>
#include <iostream>
#include <fstream>
#include <print>
#include "json.hpp"
import Crafter.Build;
using namespace Crafter::Build;
int main(int argc, char* argv[]) {
    if(argc == 1) {
        std::println("No arguments provided, use --help for help");
    }
    std::string filename = "project.json";
    std::string configuration;
    std::string outputDir;
    for (std::uint_fast32_t i = 1; i < argc; i++) {
        std::string arg = std::string(argv[i]);
        if(arg == "--help"){
             std::println("--help\tDisplays this help message.\n-c The name of the configuration to build.\n-p\nThe name of the project file.\n-o Overrides the output folder.\n");
             return 0;
        } else if(arg == "-c"){
            configuration = argv[++i];
        } else if(arg == "-o"){
            outputDir = argv[++i];
        } else if(arg == "-p"){
            filename = argv[++i];
        } else{
            std::println("Unkown argument: {}", argv[i]);
            return 1;
        }
    }
    Project project = Project::LoadFromJSON(filename);
    if(outputDir.empty()){
        project.Build(configuration);
    } else{
        project.Build(configuration, outputDir);
    }
}
