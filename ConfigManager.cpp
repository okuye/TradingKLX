#include "ConfigManager.h"
#include <fstream>
#include <iostream>

nlohmann::json ConfigManager::readConfig(const std::string& configFile) {
    std::ifstream file(configFile);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open config file: " + configFile);
    }

    nlohmann::json configJson;
    try {
        file >> configJson;
    } catch (const nlohmann::json::parse_error& e) {
        throw std::runtime_error("Error parsing config file: " + std::string(e.what()));
    }

    // Optionally, adjust the configuration based on the environment
    const char* env = std::getenv("APP_ENVIRONMENT");
    std::string environment = env ? std::string(env) : "development";

    if (configJson.contains(environment) && configJson[environment].is_object()) {
        // Merge or overwrite global settings with environment-specific settings
        nlohmann::json mergedConfig = configJson;
        for (auto& el : configJson[environment].items()) {
            mergedConfig[el.key()] = el.value();
        }
        return mergedConfig;
    }

    return configJson;
}
