#include "JSONParser.h"
#include <iostream>  // For debugging purposes

std::optional<double> JSONParser::extractValue(const nlohmann::json& data, const std::string& key) {
    if (data.contains(key) && data[key].is_number()) {
        return data[key].get<double>();
    } else {
        return std::nullopt;
    }
}

bool JSONParser::validateJson(const nlohmann::json& j, const std::string& key, const nlohmann::json::value_t expectedType) {
    return j.contains(key) && j[key].type() == expectedType;
}

std::tuple<std::vector<double>, std::vector<double>, std::vector<double>> JSONParser::parseJsonForPrices(
        const std::string& jsonData,
        const std::string& timeSeriesKey,
        const std::vector<std::string>& fields
) {
    std::vector<double> openPrices;
    std::vector<double> highPrices;
    std::vector<double> lowPrices;

    try {
        auto json = nlohmann::json::parse(jsonData);

        if (validateJson(json, timeSeriesKey, nlohmann::json::value_t::object)) {
            const auto& timeSeries = json[timeSeriesKey];

            for (const auto& [time, data] : timeSeries.items()) {
                if (data.is_object()) {
                    if (fields.size() >= 1 && validateJson(data, fields[0], nlohmann::json::value_t::string)) {
                        openPrices.push_back(std::stod(data[fields[0]].get<std::string>()));
                    }
                    if (fields.size() >= 2 && validateJson(data, fields[1], nlohmann::json::value_t::string)) {
                        highPrices.push_back(std::stod(data[fields[1]].get<std::string>()));
                    }
                    if (fields.size() >= 3 && validateJson(data, fields[2], nlohmann::json::value_t::string)) {
                        lowPrices.push_back(std::stod(data[fields[2]].get<std::string>()));
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Failed to parse JSON: " << e.what() << std::endl;
    }

    return std::make_tuple(openPrices, highPrices, lowPrices);
}
