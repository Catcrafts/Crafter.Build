#include <iostream>
#include <fstream>
#include "json.hpp"
import Crafter.Build;
using namespace Crafter::Build;
int main(int argc, char* argv[]) {
    Project project = Project::LoadFromJSON("/home/jorijn/repos/crafter-build/project.json");
    project.Build("debug");
    return 1;
}
