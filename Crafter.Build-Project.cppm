module;
#include <vector>
#include <string>
export module Crafter.Build:Project;
import :Configuration;

export namespace Crafter::Build {
    class Project {
    public:
        std::string name;
        std::vector<Configuration> configurations;
        Project(std::string name, std::vector<Configuration> configurations);
        void Build(std::string configuration);
        void SaveToJSON(std::string path);
        static Project LoadFromJSON(std::string file);
    };
}
