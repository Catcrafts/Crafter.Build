module;
#include <vector>
#include <string>
#include <fstream>
#include "json.hpp"
module Crafter.Build:ConfigurationImpl;
import :Configuration;
using namespace Crafter::Build;

Configuration::Configuration(std::string name, std::string standard, std::vector<std::string> sourceFiles, std::vector<std::string> moduleFiles, std::string optimizationLevel, std::string buildDir, std::string outputDir): name(name), standard(standard), sourceFiles(sourceFiles), moduleFiles(moduleFiles), optimizationLevel(optimizationLevel), buildDir(buildDir), outputDir(outputDir){

}
