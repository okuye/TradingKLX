#include "JSONParser.h"
#include <iostream>  // For debugging purposes
#include <sstream>   // For std::istringstream

// Extracts a numeric value from the given JSON data for a specified key
std::optional<double> JSONParser::extractValue(const Json::Value& data, const std::string& key) {
    if (data.isMember(key) && data[key].isNumeric()) {
        return data[key].asDouble();
    } else {
        return std::nullopt;
    }
}

// Validates if the JSON contains a key and matches the expected type
bool JSONParser::validateJson(const Json::Value& j, const std::string& key, Json::ValueType expectedType) {
    return j.isMember(key) && j[key].type() == expectedType;
}

// Parses JSON for price data
std::tuple<std::vector<double>, std::vector<double>, std::vector<double>> JSONParser::parseJsonForPrices(
        const std::string& jsonData,
        const std::string& timeSeriesKey,
        const std::vector<std::string>& fields
) {
    std::vector<double> openPrices;
    std::vector<double> highPrices;
    std::vector<double> lowPrices;

    try {
        Json::CharReaderBuilder readerBuilder;
        Json::Value json;
        std::istringstream s(jsonData);
        std::string errs;

        if (Json::parseFromStream(readerBuilder, s, &json, &errs)) {
            if (validateJson(json, timeSeriesKey, Json::objectValue)) {
                const Json::Value& timeSeries = json[timeSeriesKey];

                for (const auto& time : timeSeries.getMemberNames()) {
                    const Json::Value& data = timeSeries[time];
                    if (data.isObject()) {
                        if (fields.size() >= 1 && validateJson(data, fields[0], Json::stringValue)) {
                            openPrices.push_back(std::stod(data[fields[0]].asString()));
                        }
                        if (fields.size() >= 2 && validateJson(data, fields[1], Json::stringValue)) {
                            highPrices.push_back(std::stod(data[fields[1]].asString()));
                        }
                        if (fields.size() >= 3 && validateJson(data, fields[2], Json::stringValue)) {
                            lowPrices.push_back(std::stod(data[fields[2]].asString()));
                        }
                    }
                }
            }
        } else {
            std::cerr << "Error parsing JSON data: " << errs << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Failed to parse JSON: " << e.what() << std::endl;
    }

    return std::make_tuple(openPrices, highPrices, lowPrices);
}