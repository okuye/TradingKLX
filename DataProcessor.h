#pragma once
#include <string>
#include <vector>
#include <json/json.h> // Include the jsoncpp header
#include "PriceData.h"
#include "Utilities.h"

class DataProcessor {
public:
    static double getFieldValue(const Json::Value& data, const std::string& field);
//    static double getFieldValue(const Json::Value& data, const std::string& field);
    static std::vector<PriceData> processData(const std::string& jsonData, const std::string& timeSeriesKey);
//    static std::vector<PriceData> processData(const std::string& jsonData, const std::string& timeSeriesKey);
};