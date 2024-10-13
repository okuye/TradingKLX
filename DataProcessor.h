#ifndef DATAPROCESSOR_H
#define DATAPROCESSOR_H

#include <string>
#include <vector>
#include <json/json.h>  // Include the jsoncpp header
#include "PriceData.h"
#include "Utilities.h"

class DataProcessor {
public:
    static double getFieldValue(const Json::Value& data, const std::string& field);
    static std::vector<PriceData> processData(const std::string& jsonData, const std::string& timeSeriesKey);
    static std::string jsonToString(const Json::Value& jsonValue);
    std::vector<PriceData> loadPriceData(const std::string& filename);
    static std::vector<TradeData> processTradingServerData(const Json::Value& data);

    // Add this declaration
    static std::vector<TradeData> processOandAData(const Json::Value& data);
};

#endif // DATAPROCESSOR_H
