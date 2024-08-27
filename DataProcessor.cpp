#include "DataProcessor.h"
#include "PriceData.h"
#include <iostream>
#include <stdexcept> // For std::invalid_argument and custom exceptions
#include <json/json.h> // Include the jsoncpp header
//#include <spdlog/spdlog.h> // Logging library// Define custom exception types for specific JSON structure issues
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
//double DataProcessor::getFieldValue(const Json::Value& data, const std::string& field) {
//    try {
//        if (data.isMember(field) && data[field].isString()) {
//            return std::stod(data[field].asString());
//        }
//        throw FieldExtractionException("Missing or invalid field: " + field);
//    } catch (const std::exception& e) {
//        std::cerr << "Error extracting field '" << field << "': " << e.what() << std::endl;
//        throw FieldExtractionException("Failed to extract field '" + field + "': " + std::string(e.what()));
//    }
//}

// Static method to safely extract fields from JSON data using jsoncpp
double DataProcessor::getFieldValue(const Json::Value& data, const std::string& field) {
    try {
        if (data.isMember(field) && data[field].isString()) {
            return std::stod(data[field].asString());
        }
        std::cerr << "Missing or invalid field: " << field << std::endl;
        throw FieldExtractionException("Missing or invalid field: " + field);
    } catch (const std::exception& e) {
        std::cerr << "Error extracting field '" << field << "': " << e.what() << std::endl;
        throw FieldExtractionException("Failed to extract field '" + field + "': " + std::string(e.what()));
    }
}

std::vector<PriceData> DataProcessor::processData(const std::string& jsonDataString, const std::string& key) {
    Json::Value root;
    Json::CharReaderBuilder readerBuilder;
    std::string errs;

    std::istringstream stream(jsonDataString);
    if (!Json::parseFromStream(readerBuilder, stream, &root, &errs)) {
        std::cerr << "Error parsing JSON data: " << errs << std::endl;
        throw std::runtime_error("Error parsing JSON data: " + errs);
    }

    std::vector<PriceData> dataList;

    if (!root.isMember(key) || !root[key].isArray()) {
        std::cerr << "Invalid JSON structure: missing or incorrect '" << key << "' key" << std::endl;
        throw std::runtime_error("Invalid JSON structure: missing or incorrect '" + key + "' key");
    }

    const Json::Value& dataArray = root[key];
    for (const auto& item : dataArray) {
        std::cout << "Processing item: " << jsonToString(item) << std::endl;

        // Check for required keys within each candle item
        if (!item.isMember("mid") || !item["mid"].isObject()) {
            std::cerr << "Invalid JSON structure: 'mid' key is missing or incorrect" << std::endl;
            throw std::runtime_error("Invalid JSON structure: 'mid' key is missing or incorrect");
        }

        if (!item["mid"].isMember("c") || !item["mid"]["c"].isString()) {
            std::cerr << "Invalid JSON structure: 'c' key is missing or not a string" << std::endl;
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

    std::cout << "Successfully processed " << dataList.size() << " items" << std::endl;
    return dataList;
}


//std::vector<PriceData> DataProcessor::processData(const std::string& jsonDataString, const std::string& key) {
//    Json::Value root;
//    Json::CharReaderBuilder readerBuilder;
//    std::string errs;
//
//    std::istringstream stream(jsonDataString);
//    if (!Json::parseFromStream(readerBuilder, stream, &root, &errs)) {
//        std::cerr << "Error parsing JSON data: " << errs << std::endl;
//        throw std::runtime_error("Error parsing JSON data: " + errs);
//    }
//
//    std::vector<PriceData> dataList;
//
//    if (!root.isMember(key) || !root[key].isArray()) {
//        std::cerr << "Invalid JSON structure: missing or incorrect '" << key << "' key" << std::endl;
//        throw std::runtime_error("Invalid JSON structure: missing or incorrect '" + key + "' key");
//    }
////    if (!root.isMember(key) || !root[key].isArray()) {
////        throw std::runtime_error("Invalid JSON structure: missing or incorrect '" + key + "' key");
////    }
//
//    const Json::Value& dataArray = root[key];
//    for (const auto& item : dataArray) {
//        // Debug: log each element
//        std::cout << "Processing item: " << jsonToString(item) << std::endl;
//
//        // Check for required keys within each candle item
//        if (!item.isMember("mid") || !item["mid"].isObject()) {
//            throw std::runtime_error("Invalid JSON structure: 'mid' key is missing or incorrect");
//        }
//
//        if (!item["mid"].isMember("c") || !item["mid"]["c"].isString()) {
//            throw std::runtime_error("Invalid JSON structure: 'c' key is missing or not a string");
//        }
//
//        // Assuming PriceData is a structure that holds the parsed data
//        PriceData priceData;
//        priceData.close = std::stod(item["mid"]["c"].asString());
//        priceData.high = std::stod(item["mid"]["h"].asString());
//        priceData.low = std::stod(item["mid"]["l"].asString());
//        priceData.open = std::stod(item["mid"]["o"].asString());
//        priceData.volume = item["volume"].asInt();
//
//        dataList.push_back(priceData);
//    }
//
//    return dataList;
//}