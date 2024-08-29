#include "ConfigManager.h"
#include <fstream>
#include <iostream>
#include <json/json.h>

Json::Value ConfigManager::readConfig(const std::string& configFile) {
    std::ifstream file(configFile, std::ifstream::binary);
    if (!file.is_open()) {
        std::cerr << "Unable to open config file: " << configFile << std::endl;
        throw std::runtime_error("Unable to open config file: " + configFile);
    }

    Json::CharReaderBuilder readerBuilder;
    Json::Value configJson;
    std::string errs;

    if (!Json::parseFromStream(readerBuilder, file, &configJson, &errs)) {
        std::cerr << "Error parsing config file: " << errs << std::endl;
        throw std::runtime_error("Error parsing config file: " + errs);
    }

    // Optionally, adjust the configuration based on the environment
    const char* env = std::getenv("APP_ENVIRONMENT");
    std::string environment = env ? std::string(env) : "development";

    if (configJson.isMember(environment) && configJson[environment].isObject()) {
        // Merge or overwrite global settings with environment-specific settings
        Json::Value mergedConfig = configJson;
        const Json::Value& envConfig = configJson[environment];

        for (Json::ValueConstIterator it = envConfig.begin(); it != envConfig.end(); ++it) {
            mergedConfig[it.key().asString()] = *it;
        }

        std::cout << "Config loaded for environment: " << environment << std::endl;
        return mergedConfig;
    }

    std::cout << "Config loaded with no specific environment." << std::endl;

    return configJson;
}