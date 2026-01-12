/**
 * @file gmap_compiler_main.cpp
 * @brief Command-line entry point for GMAP binary compiler
 *
 * Usage: gmap-compiler <input.gmap> [-o output.gmapb] [-v]
 */

#include "Graphitech/ECS/Loaders/GMapCompiler.hpp"
#include <iostream>
#include <string>

void printUsage(const char* programName) {
    std::cout << "GMAP Binary Compiler v1.0" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: " << programName << " <input.gmap> [-o output.gmapb] [-v]" << std::endl;
    std::cout << std::endl;
    std::cout << "  <input.gmap>      Path to input GMAP JSON file" << std::endl;
    std::cout << "  -o <output>       Output binary file (default: input.gmapb)" << std::endl;
    std::cout << "  -v                Enable verbose output" << std::endl;
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    std::string inputFile = argv[1];
    std::string outputFile = inputFile;
    
    // Replace .gmap with .gmapb
    size_t pos = outputFile.rfind(".gmap");
    if (pos != std::string::npos) {
        outputFile.replace(pos, 5, ".gmapb");
    } else {
        outputFile += ".gmapb";
    }

    bool verbose = false;

    // Parse arguments
    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-o" && i + 1 < argc) {
            outputFile = argv[++i];
        } else if (arg == "-v") {
            verbose = true;
        }
    }

    bool success = ECS::GMapCompiler::compile(inputFile, outputFile, verbose);
    return success ? 0 : 1;
}
