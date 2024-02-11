#pragma once
#include <nlohmann/json.hpp>
#include <string>

class ConfigManager {
public:
    static nlohmann::json readConfig(const std::string& configFile);
};
