#include "DataProcessor.h"
#include "PriceData.h"
#include <iostream>
#include <stdexcept>  
#include <json/json.h>  
#include <sstream>  

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

// Process the JSON data and extract price information
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
        throw JsonStructureException("Invalid JSON structure: missing or incorrect '" + key + "' key");
    }

    const Json::Value& dataArray = root[key];
    for (const auto& item : dataArray) {
        std::cout << "Processing item: " << jsonToString(item) << std::endl;

        if (!item.isMember("ask") || !item["ask"].isObject()) {
            std::cerr << "Invalid JSON structure: 'ask' key is missing or incorrect" << std::endl;
            throw JsonStructureException("Invalid JSON structure: 'ask' key is missing or incorrect");
        }
        if (!item.isMember("bid") || !item["bid"].isObject()) {
            std::cerr << "Invalid JSON structure: 'bid' key is missing or incorrect" << std::endl;
            throw JsonStructureException("Invalid JSON structure: 'bid' key is missing or incorrect");
        }

        try {
            std::string timestamp = item["time"].asString();
            double askOpen = std::stod(item["ask"]["o"].asString());
            double askHigh = std::stod(item["ask"]["h"].asString());
            double askLow = std::stod(item["ask"]["l"].asString());
            double askClose = std::stod(item["ask"]["c"].asString());
            double askVolume = item["ask"]["volume"].asDouble();

            double bidOpen = std::stod(item["bid"]["o"].asString());
            double bidHigh = std::stod(item["bid"]["h"].asString());
            double bidLow = std::stod(item["bid"]["l"].asString());
            double bidClose = std::stod(item["bid"]["c"].asString());
            double bidVolume = item["bid"]["volume"].asDouble();

            PriceData priceData(timestamp, askOpen, askHigh, askLow, askClose, askVolume,
                                bidOpen, bidHigh, bidLow, bidClose, bidVolume);

            dataList.push_back(priceData);
        } catch (const std::exception& e) {
            std::cerr << "Error processing item in JSON: " << e.what() << std::endl;
            throw std::runtime_error("Error processing item in JSON: " + std::string(e.what()));
        }
    }

    std::cout << "Successfully processed " << dataList.size() << " items" << std::endl;
    return dataList;
}

// Helper function to convert a Json::Value to string for logging/debugging
std::string DataProcessor::jsonToString(const Json::Value& jsonValue) {
    Json::StreamWriterBuilder writer;
    writer["indentation"] = "";  
    return Json::writeString(writer, jsonValue);
}
