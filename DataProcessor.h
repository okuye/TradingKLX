#ifndef DATAPROCESSOR_H
#define DATAPROCESSOR_H

#include <string>
#include <vector>
#include <nlohmann/json.hpp> // Make sure to include this header for JSON parsing

// Structure to hold price data for each time interval
struct PriceData {
    double open;
    double high;
    double low;
    double close;
    std::string timestamp;
};

// DataProcessor class declaration
class DataProcessor {
public:
    // Function to process JSON data and return a vector of PriceData
    static std::vector<PriceData> processData(const std::string& jsonData);
};

#endif // DATAPROCESSOR_H
