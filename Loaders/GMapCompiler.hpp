/**
 * @file GMapCompiler.hpp
 * @brief GMAP binary compiler - converts .gmap (JSON) to .gmapb (binary)
 *
 * Generates optimized binary format with:
 * - Fixed header (24 bytes)
 * - FNV-1a hashing for strings
 * - Section-based layout (prefabs, entities)
 * - Pre-calculated offsets for fast parsing
 */

#ifndef GMAP_COMPILER_HPP
#define GMAP_COMPILER_HPP

#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <cstdint>

namespace ECS {

using json = nlohmann::json;

/**
 * @brief GMAP Binary Compiler
 * 
 * Converts .gmap (JSON) files to .gmapb (binary) format for optimized loading.
 */
class GMapCompiler {
public:
    /**
     * @brief Compile a .gmap JSON file to .gmapb binary format
     *
     * @param inputFile Path to .gmap file
     * @param outputFile Path to output .gmapb file
     * @param verbose Enable detailed output
     * @return true if compilation succeeded, false otherwise
     */
    static bool compile(const std::string& inputFile, const std::string& outputFile, bool verbose = false);

private:
    // Binary format constants
    static constexpr uint32_t MAGIC = 0x50414D47;          // "GMAP"
    static constexpr uint16_t VERSION_MAJOR = 1;
    static constexpr uint16_t VERSION_MINOR = 0;
    static constexpr uint32_t HEADER_SIZE = 24;

    /**
     * @brief FNV-1a hash algorithm for strings
     * @param str The string to hash
     * @return 32-bit hash value
     */
    static uint32_t fnv1a_hash(const std::string& str);

    /**
     * @brief Write a null-terminated string to binary data
     * @param data The binary buffer
     * @param str The string to write
     */
    static void writeString(std::vector<uint8_t>& data, const std::string& str);

    /**
     * @brief Write a value to binary data
     * @param data The binary buffer
     * @param value The value to write
     */
    template<typename T>
    static void writeValue(std::vector<uint8_t>& data, T value);

    /**
     * @brief Compile prefabs section
     * @param data The binary buffer
     * @param prefabs JSON prefabs object
     * @return Offset where prefabs section starts
     */
    static uint32_t compilePrefabs(std::vector<uint8_t>& data, const json& prefabs, bool verbose);

    /**
     * @brief Compile entities section
     * @param data The binary buffer
     * @param entities JSON entities array
     * @return Offset where entities section starts
     */
    static uint32_t compileEntities(std::vector<uint8_t>& data, const json& entities, bool verbose);

    /**
     * @brief Check if file exists
     */
    static bool fileExists(const std::string& path);

    /**
     * @brief Print compilation header
     */
    static void printHeader();

    /**
     * @brief Print compilation statistics
     */
    static void printStatistics(const std::string& inputFile, const std::string& outputFile,
                               uint32_t totalSize, const json& data);
};

} // namespace ECS

#endif // GMAP_COMPILER_HPP
