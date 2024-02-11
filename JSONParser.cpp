#include "JSONParser.h"  // Assuming this includes the necessary nlohmann/json header and declaration of the JSONParser class
#include <iostream>
#include <optional>

class JSONParser {
public:
    // Function to parse JSON data for prices; made more generic to handle different structures
    static std::tuple<std::vector<double>, std::vector<double>, std::vector<double>> parseJsonForPrices(
        const std::string& jsonData,
        const std::string& timeSeriesKey,  // Key for the time series data
        const std::vector<std::string>& fields  // Keys for the required fields within each time series entry
    );

private:
    // Helper method to validate the presence and type of keys in JSON
    static bool validateJson(const nlohmann::json& j, const std::string& key, const nlohmann::json::value_t expectedType);

    // Generic method to extract a value from JSON safely, now returning std::optional<double>
    static std::optional<double> extractValue(const nlohmann::json& data, const std::string& key);
};

bool JSONParser::validateJson(const nlohmann::json& j, const std::string& key, const nlohmann::json::value_t expectedType) {
    return j.contains(key) && j[key].type() == expectedType;
}

std::optional<double> JSONParser::extractValue(const nlohmann::json& data, const std::string& key) {
    try {
        if (data.contains(key)) {
            if (data[key].is_number()) {
                return data[key].get<double>();
            } else if (data[key].is_string()) {
                return std::stod(data[key].get<std::string>());
            }
        }
        // Log the error or handle it as needed
        std::cerr << "Invalid or missing field: " << key << std::endl;
    } catch (const std::exception& e) {
        // Log the detailed error message
        std::cerr << "Error extracting field '" << key << "': " << e.what() << std::endl;
    }
    // Return an empty optional to indicate failure
    return std::nullopt;
}

std::tuple<std::vector<double>, std::vector<double>, std::vector<double>> JSONParser::parseJsonForPrices(
    const std::string& jsonData,
    const std::string& timeSeriesKey,
    const std::vector<std::string>& fields) {
    
    nlohmann::json json = nlohmann::json::parse(jsonData);
    std::vector<double> fieldValues[fields.size()];

    if (!validateJson(json, timeSeriesKey, nlohmann::json::value_t::object)) {
        throw std::runtime_error("Invalid JSON structure for key: " + timeSeriesKey);
    }

    const auto& timeSeries = json[timeSeriesKey];
    for (const auto& [_, entry] : timeSeries.items()) {
        for (size_t i = 0; i < fields.size(); ++i) {
            auto value = extractValue(entry, fields[i]);
            if (value) {
                fieldValues[i].push_back(*value);
            } else {
                std::cerr << "Skipping field '" << fields[i] << "' due to error.\n";
                // Optionally, continue to next field or handle the error as needed
            }
        }
    }

    return std::make_tuple(fieldValues[0], fieldValues[1], fieldValues[2]);
}
