#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <vector>
#include <tuple>
#include <optional>
#include <nlohmann/json.hpp>
#include <string>

class JSONParser {
public:
    static std::optional<double> extractValue(const nlohmann::json& data, const std::string& key);
    // Function to parse JSON data for prices; made more generic to handle different structures
    static std::tuple<std::vector<double>, std::vector<double>, std::vector<double>> parseJsonForPrices(
            const std::string& jsonData,
            const std::string& timeSeriesKey,  // Key for the time series data
            const std::vector<std::string>& fields  // Keys for the required fields within each time series entry
    );

private:
    // Helper method to validate the presence and type of keys in JSON
    static bool validateJson(const nlohmann::json& j, const std::string& key, const nlohmann::json::value_t expectedType);
};

#endif // JSON_PARSER_H
