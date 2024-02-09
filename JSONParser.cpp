#include "JSONParser.h"
#include <iostream>

bool JSONParser::validateJson(const nlohmann::json& j, const std::string& key, const nlohmann::json::value_t expectedType) {
    return j.contains(key) && j[key].type() == expectedType;
}

double JSONParser::extractValue(const nlohmann::json& data, const std::string& key) {
    try {
        if (data.contains(key) && data[key].is_number()) {
            return data[key].get<double>();
        } else if (data.contains(key) && data[key].is_string()) {
            return std::stod(data[key].get<std::string>());
        }
        throw std::runtime_error("Invalid or missing field: " + key);
    } catch (const std::exception& e) {
        std::cerr << "Error extracting field '" << key << "': " << e.what() << std::endl;
        throw;
    }
}

std::tuple<std::vector<double>, std::vector<double>, std::vector<double>> JSONParser::parseJsonForPrices(
    const std::string& jsonData,
    const std::string& timeSeriesKey,
    const std::vector<std::string>& fields) {
    
    std::vector<double> fieldValues[fields.size()];
    auto json = nlohmann::json::parse(jsonData);

    if (!validateJson(json, timeSeriesKey, nlohmann::json::value_t::object)) {
        throw std::runtime_error("Invalid JSON structure for key: " + timeSeriesKey);
    }

    const auto& timeSeries = json[timeSeriesKey];
    for (const auto& [_, entry] : timeSeries.items()) {
        for (size_t i = 0; i < fields.size(); ++i) {
            try {
                fieldValues[i].push_back(extractValue(entry, fields[i]));
            } catch (const std::exception& e) {
                std::cerr << "Skipping field '" << fields[i] << "' due to error: " << e.what() << std::endl;
                // Optionally, continue to next field or handle the error as needed
            }
        }
    }

    return std::make_tuple(fieldValues[0], fieldValues[1], fieldValues[2]);
}
