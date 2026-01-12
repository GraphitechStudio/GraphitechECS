/**
 * @file GMapCompiler.cpp
 * @brief Implementation of GMAP binary compiler
 */

#include "GMapCompiler.hpp"
#include <fstream>
#include <iostream>
#include <cstring>
#include <sys/stat.h>
#include <iomanip>

namespace ECS {

bool GMapCompiler::fileExists(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

uint32_t GMapCompiler::fnv1a_hash(const std::string& str) {
    uint32_t hash_val = 2166136261u;
    for (unsigned char c : str) {
        hash_val ^= c;
        hash_val = (hash_val * 16777619u);
    }
    return hash_val;
}

void GMapCompiler::writeString(std::vector<uint8_t>& data, const std::string& str) {
    uint32_t len = str.length();
    data.push_back((len >> 0) & 0xFF);
    data.push_back((len >> 8) & 0xFF);
    data.push_back((len >> 16) & 0xFF);
    data.push_back((len >> 24) & 0xFF);
    
    for (char c : str) {
        data.push_back(static_cast<uint8_t>(c));
    }
}

template<typename T>
void GMapCompiler::writeValue(std::vector<uint8_t>& data, T value) {
    for (size_t i = 0; i < sizeof(T); ++i) {
        data.push_back(static_cast<uint8_t>((value >> (i * 8)) & 0xFF));
    }
}

template void GMapCompiler::writeValue<uint16_t>(std::vector<uint8_t>& data, uint16_t value);
template void GMapCompiler::writeValue<uint32_t>(std::vector<uint8_t>& data, uint32_t value);

uint32_t GMapCompiler::compilePrefabs(std::vector<uint8_t>& data, const json& prefabs_obj, bool verbose) {
    uint32_t start = data.size();
    
    uint32_t prefab_count = prefabs_obj.size();
    writeValue<uint32_t>(data, prefab_count);
    
    if (verbose) {
        std::cout << "Prefabs: " << prefab_count << std::endl;
    }

    for (auto it = prefabs_obj.begin(); it != prefabs_obj.end(); ++it) {
        const std::string& prefab_name = it.key();
        const json& prefab_data = it.value();

        uint32_t name_hash = fnv1a_hash(prefab_name);
        writeValue<uint32_t>(data, name_hash);
        writeString(data, prefab_name); // keep plain name for loader mapping

        const json& components = prefab_data.value("components", json::array());
        uint32_t comp_count = components.size();
        writeValue<uint32_t>(data, comp_count);

        if (verbose) {
            std::cout << "  - " << prefab_name << ": " << comp_count << " components" << std::endl;
        }

        // Write component metadata with data
        for (const auto& comp : components) {
            std::string comp_type = comp.value("type", "");
            uint32_t type_hash = fnv1a_hash(comp_type);
            writeValue<uint32_t>(data, type_hash);
            
            // Write component type name for parser lookup
            writeString(data, comp_type);

            // Serialize component data as JSON string
            json comp_data = comp.value("data", json::object());
            std::string data_str = comp_data.dump();
            writeString(data, data_str);
        }
    }

    return start;
}

uint32_t GMapCompiler::compileEntities(std::vector<uint8_t>& data, const json& entities_arr, bool verbose) {
    uint32_t start = data.size();

    uint32_t entity_count = entities_arr.size();
    writeValue<uint32_t>(data, entity_count);

    if (verbose) {
        std::cout << "Entities: " << entity_count << std::endl;
    }

    for (const auto& entity_data : entities_arr) {
        std::string name = entity_data.value("name", "");
        std::string prefab_name = entity_data.value("prefab", "");

        uint32_t name_hash = fnv1a_hash(name);
        uint32_t prefab_hash = fnv1a_hash(prefab_name);

        writeValue<uint32_t>(data, name_hash);
        writeValue<uint32_t>(data, prefab_hash);
        writeString(data, name);        // keep entity name (optional, but useful)
        writeString(data, prefab_name); // keep prefab name for lookup

        const json& components = entity_data.value("components", json::array());
        uint32_t comp_count = components.size();
        writeValue<uint32_t>(data, comp_count);

        if (verbose) {
            std::cout << "  - " << name << ": " << comp_count << " components" << std::endl;
        }

        // Write component metadata with data
        for (const auto& comp : components) {
            std::string comp_type = comp.value("type", "");
            uint32_t type_hash = fnv1a_hash(comp_type);
            writeValue<uint32_t>(data, type_hash);
            
            // Write component type name for parser lookup
            writeString(data, comp_type);

            // Serialize component data as JSON string
            json comp_data = comp.value("data", json::object());
            std::string data_str = comp_data.dump();
            writeString(data, data_str);
        }
    }

    return start;
}

void GMapCompiler::printHeader() {
    std::cout << "==============================" << std::endl;
    std::cout << " GMAP Binary Compiler v1.0" << std::endl;
    std::cout << "==============================" << std::endl << std::endl;
}

void GMapCompiler::printStatistics(const std::string& inputFile, const std::string& outputFile,
                                   uint32_t totalSize, const json& data) {
    std::cout << std::endl << "Compilation Statistics:" << std::endl;

    if (fileExists(inputFile)) {
        struct stat stat_buf;
        stat(inputFile.c_str(), &stat_buf);
        long input_size = stat_buf.st_size;

        struct stat out_stat;
        if (stat(outputFile.c_str(), &out_stat) == 0) {
            long output_size = out_stat.st_size;
            double ratio = 100.0 * (1.0 - static_cast<double>(output_size) / input_size);
            
            std::cout << "  Original JSON: " << std::setw(12) << input_size << " bytes" << std::endl;
            std::cout << "  Binary GMAP:   " << std::setw(12) << output_size << " bytes" << std::endl;
            std::cout << "  Compression:   " << std::setw(12) << std::fixed << std::setprecision(1) << ratio << "%" << std::endl;
        }
    }

    std::cout << std::endl << "Binary Format Details:" << std::endl;
    std::cout << "  Magic:         0x" << std::hex << std::setfill('0') << std::setw(8) << MAGIC << std::dec << " (GMAP)" << std::endl;
    std::cout << "  Version:       " << static_cast<int>(VERSION_MAJOR) << "." << static_cast<int>(VERSION_MINOR) << std::endl;
    std::cout << "  Total Size:    " << totalSize << " bytes" << std::endl;
    std::cout << "  Prefabs:       " << data.value("prefabs", json::object()).size() << std::endl;
    std::cout << "  Entities:      " << data.value("entities", json::array()).size() << std::endl;
    std::cout << std::endl;
}

bool GMapCompiler::compile(const std::string& inputFile, const std::string& outputFile, bool verbose) {
    printHeader();

    if (!fileExists(inputFile)) {
        std::cerr << "Error: File not found: " << inputFile << std::endl;
        return false;
    }

    std::cout << "Input:  " << inputFile << std::endl;
    std::cout << "Output: " << outputFile << std::endl << std::endl;

    // Read JSON
    json data;
    try {
        std::ifstream file(inputFile);
        if (!file.is_open()) {
            std::cerr << "❌ Error: Cannot open file: " << inputFile << std::endl;
            return false;
        }
        file >> data;
        file.close();
        std::cout << "JSON parsed successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
        return false;
    }

    // Create binary data
    std::vector<uint8_t> binary_data;

    // ============ HEADER (24 bytes) ============
    writeValue<uint32_t>(binary_data, MAGIC);
    writeValue<uint16_t>(binary_data, VERSION_MAJOR);
    writeValue<uint16_t>(binary_data, VERSION_MINOR);
    uint32_t total_size_offset = binary_data.size();
    writeValue<uint32_t>(binary_data, 0);  // Placeholder for total size
    uint32_t prefabs_offset_offset = binary_data.size();
    writeValue<uint32_t>(binary_data, 0);  // Placeholder for prefabs offset
    uint32_t entities_offset_offset = binary_data.size();
    writeValue<uint32_t>(binary_data, 0);  // Placeholder for entities offset

    // ============ PREFABS SECTION ============
    uint32_t prefabs_offset = binary_data.size();
    json prefabs_obj = data.value("prefabs", json::object());
    compilePrefabs(binary_data, prefabs_obj, verbose);

    // ============ ENTITIES SECTION ============
    uint32_t entities_offset = binary_data.size();
    json entities_arr = data.value("entities", json::array());
    compileEntities(binary_data, entities_arr, verbose);

    // ============ UPDATE HEADER WITH OFFSETS ============
    uint32_t total_size = binary_data.size();

    // Update total size
    binary_data[total_size_offset + 0] = (total_size >> 0) & 0xFF;
    binary_data[total_size_offset + 1] = (total_size >> 8) & 0xFF;
    binary_data[total_size_offset + 2] = (total_size >> 16) & 0xFF;
    binary_data[total_size_offset + 3] = (total_size >> 24) & 0xFF;

    // Update prefabs offset
    binary_data[prefabs_offset_offset + 0] = (prefabs_offset >> 0) & 0xFF;
    binary_data[prefabs_offset_offset + 1] = (prefabs_offset >> 8) & 0xFF;
    binary_data[prefabs_offset_offset + 2] = (prefabs_offset >> 16) & 0xFF;
    binary_data[prefabs_offset_offset + 3] = (prefabs_offset >> 24) & 0xFF;

    // Update entities offset
    binary_data[entities_offset_offset + 0] = (entities_offset >> 0) & 0xFF;
    binary_data[entities_offset_offset + 1] = (entities_offset >> 8) & 0xFF;
    binary_data[entities_offset_offset + 2] = (entities_offset >> 16) & 0xFF;
    binary_data[entities_offset_offset + 3] = (entities_offset >> 24) & 0xFF;

    // ============ WRITE FILE ============
    try {
        std::ofstream file(outputFile, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot write file: " << outputFile << std::endl;
            return false;
        }
        file.write(reinterpret_cast<const char*>(binary_data.data()), binary_data.size());
        file.close();
        std::cout << std::endl << "Binary file created successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error writing file: " << e.what() << std::endl;
        return false;
    }

    // ============ STATISTICS ============
    printStatistics(inputFile, outputFile, total_size, data);

    // Verify file
    std::ifstream verify(outputFile, std::ios::binary);
    if (verify.is_open()) {
        uint32_t magic_read;
        verify.read(reinterpret_cast<char*>(&magic_read), sizeof(magic_read));
        if (magic_read == MAGIC) {
            std::cout << "File verification: OK (valid GMAP magic number)" << std::endl;
        } else {
            std::cerr << "File verification: FAILED" << std::endl;
            return false;
        }
        verify.close();
    }

    std::cout << std::endl;
    return true;
}

} // namespace ECS
