#include "DataProcessor.h"
#include "PriceData.h"
#include <iostream>
#include <stdexcept> // For std::invalid_argument and custom exceptions
#include <json/json.h> // Include the jsoncpp header

// Define custom exception types for specific JSON structure issues
class JsonStructureException : public std::invalid_argument {
public:
    explicit JsonStructureException(const std::string& message)
            : std::invalid_argument(message) {}
};

class FieldExtractionException : public std::invalid_argument {
public:
    explicit FieldExtractionException(const std::string& message)
            : std::invalid_argument(message) {}
};

// Static method to safely extract fields from JSON data using jsoncpp
double DataProcessor::getFieldValue(const Json::Value& data, const std::string& field) {
    try {
        if (data.isMember(field) && data[field].isString()) {
            return std::stod(data[field].asString());
        }
        throw FieldExtractionException("Missing or invalid field: " + field);
    } catch (const std::exception& e) {
        std::cerr << "Error extracting field '" << field << "': " << e.what() << std::endl;
        throw FieldExtractionException("Failed to extract field '" + field + "': " + std::string(e.what()));
    }
}

// Static method to process data using jsoncpp
//std::vector<PriceData> DataProcessor::processData(const std::string& jsonData, const std::string& timeSeriesKey) {
//    std::vector<PriceData> priceDataList;
//
//    // Parse the JSON data
//    Json::CharReaderBuilder readerBuilder;
//    Json::Value json;
//    std::string errs;
//    std::istringstream s(jsonData);
//
//    if (!Json::parseFromStream(readerBuilder, s, &json, &errs)) {
//        std::cerr << "Error parsing JSON data: " << errs << std::endl;
//        throw JsonStructureException("Invalid JSON structure");
//    }
//
//    if (!json.isMember(timeSeriesKey) || !json[timeSeriesKey].isObject()) {
//        std::cerr << "Invalid JSON structure: missing or incorrect '" << timeSeriesKey << "' key" << std::endl;
//        throw JsonStructureException("Invalid JSON structure");
//    }
//
//    const Json::Value& timeSeries = json[timeSeriesKey];
//    for (const auto& key : timeSeries.getMemberNames()) {
//        const Json::Value& data = timeSeries[key];
//        try {
//            PriceData dataPoint;
//            dataPoint.timestamp = key;
//            dataPoint.open = getFieldValue(data, "1. open");
//            dataPoint.high = getFieldValue(data, "2. high");
//            dataPoint.low = getFieldValue(data, "3. low");
//            dataPoint.close = getFieldValue(data, "4. close");
//
//            priceDataList.push_back(dataPoint);
//        } catch (const std::exception& e) {
//            std::cerr << "Skipping data point at '" << key << "' due to error: " << e.what() << std::endl;
//            // Continue processing other data points
//        }
//    }
//
//    return priceDataList;
//}

std::vector<PriceData> DataProcessor::processData(const std::string& jsonDataString, const std::string& key) {
    Json::Value root;
    Json::CharReaderBuilder readerBuilder;
    std::string errs;

    std::istringstream stream(jsonDataString);
    if (!Json::parseFromStream(readerBuilder, stream, &root, &errs)) {
        throw std::runtime_error("Error parsing JSON data: " + errs);
    }

    std::vector<PriceData> dataList;

    if (!root.isMember(key) || !root[key].isArray()) {
        throw std::runtime_error("Invalid JSON structure: missing or incorrect '" + key + "' key");
    }

    const Json::Value& dataArray = root[key];
    for (const auto& item : dataArray) {
        // Debug: log each element
        std::cout << "Processing item: " << jsonToString(item) << std::endl;

        // Check for required keys within each candle item
        if (!item.isMember("mid") || !item["mid"].isObject()) {
            throw std::runtime_error("Invalid JSON structure: 'mid' key is missing or incorrect");
        }

        if (!item["mid"].isMember("c") || !item["mid"]["c"].isString()) {
            throw std::runtime_error("Invalid JSON structure: 'c' key is missing or not a string");
        }

        // Assuming PriceData is a structure that holds the parsed data
        PriceData priceData;
        priceData.close = std::stod(item["mid"]["c"].asString());
        priceData.high = std::stod(item["mid"]["h"].asString());
        priceData.low = std::stod(item["mid"]["l"].asString());
        priceData.open = std::stod(item["mid"]["o"].asString());
        priceData.volume = item["volume"].asInt();

        dataList.push_back(priceData);
    }

    return dataList;
}