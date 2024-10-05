#ifdef USE_BOOST_FILESYSTEM
    #include <boost/filesystem.hpp>
    namespace fs = boost::filesystem;
#else
    #include <filesystem>
    namespace fs = std::filesystem;
#endif
#pragma once
#include <string>
#include <vector>
#include <json/json.h> // Include the jsoncpp header
#include "PriceData.h"
#include "Utilities.h"

class DataProcessor {
public:
    static double getFieldValue(const Json::Value& data, const std::string& field);
    static std::vector<PriceData> processData(const std::string& jsonData, const std::string& timeSeriesKey);
    static std::string jsonToString(const Json::Value& jsonValue);
    std::vector<PriceData> loadPriceData(const std::string& filename);
};