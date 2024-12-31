clear
mkdir build
mkdir bin

clang++ -std=c++26 Crafter.Build-Dependency.cppm --precompile -fprebuilt-module-path=./build -o ./build/Crafter.Build-Dependency.pcm
clang++ -std=c++26 Crafter.Build-Configuration.cppm --precompile -fprebuilt-module-path=./build -o ./build/Crafter.Build-Configuration.pcm
clang++ -std=c++26 Crafter.Build-ModuleFile.cppm --precompile -fprebuilt-module-path=./build -o ./build/Crafter.Build-ModuleFile.pcm
clang++ -std=c++26 Crafter.Build-Project.cppm --precompile -fprebuilt-module-path=./build -o ./build/Crafter.Build-Project.pcm
clang++ -std=c++26 Crafter.Build.cppm --precompile -fprebuilt-module-path=./build -o ./build/Crafter.Build.pcm

clang++ -std=c++26 Crafter.Build-Dependency.cpp -fprebuilt-module-path=./build -c -O3 -o ./build/Crafter.Build-Dependency_source.o
clang++ -std=c++26 Crafter.Build-Configuration.cpp -fprebuilt-module-path=./build -c -O3 -o ./build/Crafter.Build-Configuration_source.o
clang++ -std=c++26 Crafter.Build-ModuleFile.cpp -fprebuilt-module-path=./build -c -O3 -o ./build/Crafter.Build-ModuleFile_source.o
clang++ -std=c++26 Crafter.Build-Project.cpp -fprebuilt-module-path=./build -c -O3 -o ./build/Crafter.Build-Project_source.o
clang++ -std=c++26 main.cpp -fprebuilt-module-path=./build -c -o ./build/main.o

clang++ -std=c++26 ./build/Crafter.Build-Project.pcm -fprebuilt-module-path=./build -c -O3 -o ./build/Crafter.Build-Project.o
clang++ -std=c++26 ./build/Crafter.Build-Configuration.pcm -fprebuilt-module-path=./build -c -O3 -o ./build/Crafter.Build-Configuration.o
clang++ -std=c++26 ./build/Crafter.Build-ModuleFile.pcm -fprebuilt-module-path=./build -c -O3 -o ./build/Crafter.Build-ModuleFile.o
clang++ -std=c++26 ./build/Crafter.Build-Dependency.pcm -fprebuilt-module-path=./build -c -O3 -o ./build/Crafter.Build-Dependency.o
clang++ -std=c++26 ./build/Crafter.Build.pcm -fprebuilt-module-path=./build -c -O3 -o ./build/Crafter.Build.o
clang++ ./build/main.o ./build/Crafter.Build-ModuleFile_source.o ./build/Crafter.Build-ModuleFile.o ./build/Crafter.Build.o ./build/Crafter.Build-Configuration.o ./build/Crafter.Build-Configuration_source.o ./build/Crafter.Build-Project.o ./build/Crafter.Build-Project_source.o ./build/Crafter.Build-Dependency.o ./build/Crafter.Build-Dependency_source.o -O3 -o ./bin/crafter-build

rm -rf build;
# crafter-build -c debug
