#include <iostream>
#include <fstream>
#include <print>
#include "json.hpp"
import Crafter.Build;
using namespace Crafter::Build;
int main(int argc, char* argv[]) {
    if(argc == 1){
      std::println("No configuration provided, use --help for help");
    } else if(argc == 2) {
        if(std::string(argv[1]) == "--help"){
            std::println("<configuration> to build this configuration with the defualt project filename \"project.json\"\n<configuration> <project> to build this configuration with the specified project filename\nhelp displays this help message");
        } else {
            Project project = Project::LoadFromJSON("./project.json");
            project.Build(argv[1]);
        }
        return 0;
    } else if(argc == 3) {
        Project project = Project::LoadFromJSON(argv[2]);
        project.Build(argv[1]);
        return 0;
    } else {
        std::println("Too many arguments provided");
        return 1;
    }
}
