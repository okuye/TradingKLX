#include "DataProcessor.h"
#include "PriceData.h" // Ensure this is included
#include <iostream>

// Helper function to safely extract fields from JSON data
double DataProcessor::getFieldValue(const nlohmann::json& data, const std::string& field) {
    try {
        if (data.contains(field) && data[field].is_string()) {
            return std::stod(data[field].get<std::string>());
        }
        throw std::runtime_error("Missing or invalid field: " + field);
    } catch (const std::exception& e) {
        std::cerr << "Error extracting field '" << field << "': " << e.what() << std::endl;
        throw;
    }
}

// processData implementation
std::vector<PriceData> DataProcessor::processData(const std::string& jsonData, const std::string& timeSeriesKey) {
    std::vector<PriceData> priceDataList;
    auto json = nlohmann::json::parse(jsonData);

    if (!json.contains(timeSeriesKey) || !json[timeSeriesKey].is_object()) {
        std::cerr << "Invalid JSON structure: missing or incorrect '" << timeSeriesKey << "' key" << std::endl;
        throw std::runtime_error("Invalid JSON structure");
    }

    const auto& timeSeries = json[timeSeriesKey];
    for (const auto& [timestamp, data] : timeSeries.items()) {
        try {
            PriceData dataPoint;
            dataPoint.timestamp = timestamp;
            dataPoint.open = getFieldValue(data, "1. open");
            dataPoint.high = getFieldValue(data, "2. high");
            dataPoint.low = getFieldValue(data, "3. low");
            dataPoint.close = getFieldValue(data, "4. close");

            priceDataList.push_back(dataPoint);
        } catch (const std::exception& e) {
            std::cerr << "Skipping data point at '" << timestamp << "' due to error: " << e.what() << std::endl;
            // Continue processing other data points
        }
    }

    return priceDataList;
}
