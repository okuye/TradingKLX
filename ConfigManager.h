#pragma once

#include <json/json.h>
#include <string>

class ConfigManager {
public:
    static Json::Value readConfig(const std::string& configFile);
};