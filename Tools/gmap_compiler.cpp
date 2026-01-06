/**
 * @file gmap_compiler.cpp
 * @brief Tool to compile .gmap JSON files to optimized binary format
 *
 * Usage: gmap-compiler input.gmap -o output.gmapb
 */

#include <iostream>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
namespace fs = std::filesystem;

void printUsage() {
    std::cout << "Usage: gmap-compiler <input.gmap> -o <output.gmapb>\n";
    std::cout << "\nOptions:\n";
    std::cout << "  -o, --output <file>    Output binary file\n";
    std::cout << "  -h, --help             Show this help\n";
    std::cout << "  -v, --verbose          Verbose output\n";
    std::cout << "\nExamples:\n";
    std::cout << "  gmap-compiler level1.gmap -o level1.gmapb\n";
    std::cout << "  gmap-compiler Maps/*.gmap -o build/maps/\n";
}

struct CompilerOptions {
    std::string inputFile;
    std::string outputFile;
    bool verbose = false;
};

bool parseArgs(int argc, char** argv, CompilerOptions& options) {
    if (argc < 2) {
        return false;
    }

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help") {
            return false;
        } else if (arg == "-v" || arg == "--verbose") {
            options.verbose = true;
        } else if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) {
                options.outputFile = argv[++i];
            } else {
                std::cerr << "Error: -o requires an argument\n";
                return false;
            }
        } else if (options.inputFile.empty()) {
            options.inputFile = arg;
        } else {
            std::cerr << "Error: Unknown argument: " << arg << "\n";
            return false;
        }
    }

    if (options.inputFile.empty()) {
        std::cerr << "Error: No input file specified\n";
        return false;
    }

    if (options.outputFile.empty()) {
        // Default output: replace .gmap with .gmapb
        options.outputFile = options.inputFile;
        size_t pos = options.outputFile.rfind(".gmap");
        if (pos != std::string::npos) {
            options.outputFile.replace(pos, 5, ".gmapb");
        } else {
            options.outputFile += ".gmapb";
        }
    }

    return true;
}

/**
 * @brief Compiles a .gmap JSON file to binary format
 *
 * TODO: Implement binary serialization (FlatBuffers, MessagePack, or custom)
 */
bool compileGMap(const CompilerOptions& options) {
    if (options.verbose) {
        std::cout << "[Compiler] Reading: " << options.inputFile << "\n";
    }

    // Read JSON
    std::ifstream inFile(options.inputFile);
    if (!inFile.is_open()) {
        std::cerr << "Error: Cannot open input file: " << options.inputFile << "\n";
        return false;
    }

    json data;
    try {
        data = json::parse(inFile);
    } catch (const std::exception& e) {
        std::cerr << "Error: Failed to parse JSON: " << e.what() << "\n";
        return false;
    }
    inFile.close();

    if (options.verbose) {
        std::cout << "[Compiler] Parsed JSON (version: " << data.value("version", "unknown") << ")\n";
    }

    // TODO: Serialize to binary format
    // For now, just write JSON as placeholder
    std::ofstream outFile(options.outputFile, std::ios::binary);
    if (!outFile.is_open()) {
        std::cerr << "Error: Cannot create output file: " << options.outputFile << "\n";
        return false;
    }

    // Placeholder: Write compact JSON (no whitespace)
    std::string compactJson = data.dump();
    outFile.write(compactJson.c_str(), compactJson.size());
    outFile.close();

    if (options.verbose) {
        std::cout << "[Compiler] Written: " << options.outputFile
                  << " (" << compactJson.size() << " bytes)\n";
    }

    // Calculate compression ratio
    size_t originalSize = fs::file_size(options.inputFile);
    size_t compressedSize = fs::file_size(options.outputFile);
    float ratio = 100.0f * (1.0f - (float)compressedSize / (float)originalSize);

    std::cout << "✓ Compiled: " << options.inputFile << " → " << options.outputFile << "\n";
    std::cout << "  Original: " << originalSize << " bytes\n";
    std::cout << "  Compiled: " << compressedSize << " bytes ("
              << (ratio > 0 ? "+" : "") << (int)ratio << "%)\n";

    return true;
}

int main(int argc, char** argv) {
    std::cout << "╔════════════════════════════════════════╗\n";
    std::cout << "║     .gmap Binary Compiler v1.0         ║\n";
    std::cout << "╚════════════════════════════════════════╝\n\n";

    CompilerOptions options;
    if (!parseArgs(argc, argv, options)) {
        printUsage();
        return 1;
    }

    if (!compileGMap(options)) {
        return 1;
    }

    std::cout << "\n✓ Compilation successful!\n";

    // TODO: Implement binary loader
    std::cout << "\n⚠️  Note: Binary loader not yet implemented.\n";
    std::cout << "   Currently using JSON format for development.\n";

    return 0;
}

