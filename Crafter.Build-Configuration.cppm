module;
#include <string>
#include <cstdint>
#include <vector>
export module Crafter.Build:Configuration;

export namespace Crafter::Build {
    class Configuration {
    public:
        std::string name;
        std::string standard;
        std::vector<std::string> sourceFiles;
        std::vector<std::string> moduleFiles;
        std::string optimizationLevel;
        std::string buildDir;
        std::string outputDir;
        Configuration(std::string name, std::string standard, std::vector<std::string> sourceFiles, std::vector<std::string> moduleFiles, std::string optimizationLevel, std::string buildDir, std::string outputDir);
    };
}
