#ifndef CONFIG_HPP
    #define CONFIG_HPP
#include <fstream>
#include <string>

namespace ECS {
    class Config final {
        public:
            /**
             * @brief Retrieves a configuration value from a key-value config file
             *
             * @param filePath Path to the configuration file
             * @param searchKey The key to search for in the config file
             * @return The value associated with the key, or empty string if not found
             *
             * This function parses a simple key=value configuration file, ignoring
             * comments (lines starting with #) and trimming whitespace.
             */
            static std::string getConfigValue(const std::string &filePath, const std::string &searchKey) {
                std::ifstream ifs(filePath);
                if (!ifs) return std::string();

                std::string line;
                auto is_space = [](char c) { return std::isspace(static_cast<unsigned char>(c)); };

                while (std::getline(ifs, line)) {
                    auto cpos = line.find('#');
                    if (cpos != std::string::npos) line.erase(cpos);

                    size_t start = 0;
                    while (start < line.size() && is_space(line[start])) ++start;
                    size_t end = line.size();
                    while (end > start && is_space(line[end-1])) --end;
                    if (start >= end) continue;

                    std::string kv = line.substr(start, end-start);
                    auto eq = kv.find('=');
                    if (eq == std::string::npos) continue;

                    std::string key = kv.substr(0, eq);
                    std::string val = kv.substr(eq+1);

                    size_t ks = 0; while (ks < key.size() && is_space(key[ks])) ++ks;
                    size_t ke = key.size(); while (ke > ks && is_space(key[ke-1])) --ke;
                    key = key.substr(ks, ke-ks);

                    size_t vs = 0; while (vs < val.size() && is_space(val[vs])) ++vs;
                    size_t ve = val.size(); while (ve > vs && is_space(val[ve-1])) --ve;
                    val = val.substr(vs, ve-vs);

                    if (key == searchKey) return val;
                }
                return {};
            }
    };
}
#endif