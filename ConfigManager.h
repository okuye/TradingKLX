#pragma once
#include "third_party/nlohmann_json/json.hpp"
#include <string>

class ConfigManager {
public:
    static nlohmann::json readConfig(const std::string& configFile);
};
