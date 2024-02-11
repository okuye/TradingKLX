#include "ConfigManager.h"
#include <fstream>
#include <iostream>

nlohmann::json ConfigManager::readConfig(const std::string& configFile) {
    std::ifstream file(configFile);
    nlohmann::json configJson;
    if (file.is_open()) {
        file >> configJson;
    } else {
        std::cerr << "Unable to open config file: " << configFile << std::endl;
        // Handle the error as per your project requirements
    }
    return configJson;
}
