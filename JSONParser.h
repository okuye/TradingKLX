#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <vector>
#include <tuple>
#include <optional>
#include <json/json.h> // Include jsoncpp header
#include <string>

class JSONParser {
public:
    // Extracts a numeric value from a JSON object for a given key
    static std::optional<double> extractValue(const Json::Value& data, const std::string& key);

    // Function to parse JSON data for prices; made more generic to handle different structures
    static std::tuple<std::vector<double>, std::vector<double>, std::vector<double>> parseJsonForPrices(
            const std::string& jsonData,
            const std::string& timeSeriesKey,  // Key for the time series data
            const std::vector<std::string>& fields  // Keys for the required fields within each time series entry
    );

private:
    // Helper method to validate the presence and type of keys in JSON
    static bool validateJson(const Json::Value& j, const std::string& key, Json::ValueType expectedType);
};

#endif // JSON_PARSER_H