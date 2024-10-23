#ifndef DATAPROCESSOR_H
#define DATAPROCESSOR_H

#include <string>
#include <vector>
#include <json/json.h>
#include "PriceData.h"
#include "TradeData.h"
#include "Utilities.h"
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <spdlog/spdlog.h>

// Exception for JSON structure issues
class JsonStructureException : public std::invalid_argument {
public:
    explicit JsonStructureException(const std::string& message)
            : std::invalid_argument(message) {}
};

// Exception for field extraction issues
class FieldExtractionException : public std::invalid_argument {
public:
    explicit FieldExtractionException(const std::string& message)
            : std::invalid_argument(message) {}
};

class DataProcessor {
public:
    static double getFieldValue(const Json::Value& data, const std::string& field);
    static std::vector<PriceData> processData(const std::string& jsonData, const std::string& timeSeriesKey);
    std::vector<PriceData> loadPriceData(const std::string& filename);
    static std::vector<TradeData> processTradingServerData(const Json::Value& data);
    static std::vector<TradeData> processOandAData(const Json::Value& data);
};

#endif // DATAPROCESSOR_H