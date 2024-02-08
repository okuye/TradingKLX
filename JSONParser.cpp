#include "JSONParser.h"
#include <iostream>

std::tuple<std::vector<double>, std::vector<double>, std::vector<double>> JSONParser::parseJsonForPrices(const std::string& jsonData, const std::string& timeSeriesKey) {
    std::vector<double> highPrices, lowPrices, closePrices;
    try {
        auto j = nlohmann::json::parse(jsonData);
        if (!validateJson(j, timeSeriesKey)) {
            std::cerr << "Invalid JSON data or structure." << std::endl;
            throw std::runtime_error("Invalid JSON data or structure.");
        }

        const auto& timeSeries = j[timeSeriesKey];
        for (const auto& [key, value] : timeSeries.items()) {
            highPrices.push_back(value["2. high"].get<double>());
            lowPrices.push_back(value["3. low"].get<double>());
            closePrices.push_back(value["4. close"].get<double>());
        }
    } catch (nlohmann::json::parse_error& e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
        throw;
    } catch (nlohmann::json::type_error& e) {
        std::cerr << "JSON type error: " << e.what() << std::endl;
        throw;
    } catch (std::exception& e) {
        std::cerr << "Error processing JSON data: " << e.what() << std::endl;
        throw;
    }

    return std::make_tuple(highPrices, lowPrices, closePrices);
}

bool JSONParser::validateJson(const nlohmann::json& j, const std::string& timeSeriesKey) {
    if (!j.contains(timeSeriesKey)) return false;
    const auto& timeSeries = j[timeSeriesKey];
    if (!timeSeries.is_object()) return false;

    for (const auto& [key, value] : timeSeries.items()) {
        if (!value.contains("2. high") || !value["2. high"].is_number()) return false;
        if (!value.contains("3. low") || !value["3. low"].is_number()) return false;
        if (!value.contains("4. close") || !value["4. close"].is_number()) return false;
    }
    return true;
}
