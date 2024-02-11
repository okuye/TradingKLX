#ifndef DATAPROCESSOR_H
#define DATAPROCESSOR_H

#include "PriceData.h" // Include the PriceData header
#include <string>
#include <vector>
#include <nlohmann/json.hpp> // Include this header for JSON parsing

// DataProcessor class declaration
class DataProcessor {
public:
    // Function to process JSON data and return a vector of PriceData
    static std::vector<PriceData> processData(const std::string& jsonData, const std::string& timeSeriesKey = "Time Series (5min)");

private:
    // Helper function to validate and extract a field from JSON
    static double getFieldValue(const nlohmann::json& data, const std::string& field);
};

#endif // DATAPROCESSOR_H
