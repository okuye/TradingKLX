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

double DataProcessor::getFieldValue(const Json::Value& data, const std::string& field) {
    try {
        if (data.isMember(field) && data[field].isString()) {
            return std::stod(data[field].asString());
        }
        throw std::invalid_argument("Missing or invalid field: " + field);
    } catch (const std::exception& e) {
        throw std::invalid_argument("Failed to extract field '" + field + "': " + std::string(e.what()));
    }
}

std::vector<PriceData> DataProcessor::processData(const std::string& jsonDataString, const std::string& key) {
    Json::Value root;
    Json::CharReaderBuilder readerBuilder;
    std::string errs;

    // Parse the JSON data
    std::istringstream stream(jsonDataString);
    if (!Json::parseFromStream(readerBuilder, stream, &root, &errs)) {
        std::cerr << "Error parsing JSON data: " << errs << std::endl;
        throw std::runtime_error("Error parsing JSON data: " + errs);
    }

    // Now you can access 'root'
    if (!root.isMember(key) || !root[key].isArray()) {
        std::cerr << "Invalid JSON structure: missing or incorrect '" << key << "' key" << std::endl;
        throw JsonStructureException("Invalid JSON structure: missing or incorrect '" + key + "' key");
    }

    // Continue with processing the JSON data as before
    std::vector<PriceData> dataList;
    const Json::Value& dataArray = root[key];
    for (const auto& item : dataArray) {
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
    }

    return dataList;
}

// Helper function to convert a Json::Value to string for logging/debugging
std::string DataProcessor::jsonToString(const Json::Value& jsonValue) {
    Json::StreamWriterBuilder writer;
    writer["indentation"] = "";
    return Json::writeString(writer, jsonValue);
}

// Process data from TradingServer API
std::vector<TradeData> DataProcessor::processTradingServerData(const Json::Value& data) {
    std::vector<TradeData> tradeDataList;
    const Json::Value& rows = data["datatable"]["data"];

    for (const auto& row : rows) {
        try {
            TradeData tradeData;

            tradeData.symbol = row["symbol"].isString() ? row["symbol"].asString() : "UNKNOWN_SYMBOL";
            tradeData.date = row["date"].isString() ? row["date"].asString() : "UNKNOWN_DATE";
            tradeData.hour = row["hour"].isInt() ? row["hour"].asInt() : -1;
            tradeData.openBid = row["openbid"].isDouble() ? row["openbid"].asDouble() : 0.0;
            tradeData.highBid = row["highbid"].isDouble() ? row["highbid"].asDouble() : 0.0;
            tradeData.lowBid = row["lowbid"].isDouble() ? row["lowbid"].asDouble() : 0.0;
            tradeData.closeBid = row["closebid"].isDouble() ? row["closebid"].asDouble() : 0.0;
            tradeData.openAsk = row["openask"].isDouble() ? row["openask"].asDouble() : 0.0;
            tradeData.highAsk = row["highask"].isDouble() ? row["highask"].asDouble() : 0.0;
            tradeData.lowAsk = row["lowask"].isDouble() ? row["lowask"].asDouble() : 0.0;
            tradeData.closeAsk = row["closeask"].isDouble() ? row["closeask"].asDouble() : 0.0;
            tradeData.totalTicks = row["totalticks"].isInt() ? row["totalticks"].asInt() : 0;


            tradeDataList.push_back(tradeData);

        } catch (const std::exception& e) {
            std::cerr << "Error processing trade data row: " << e.what() << std::endl;
            std::cerr << "Row data: " << DataProcessor::jsonToString(row) << std::endl;
        }
    }

    return tradeDataList;
}

std::vector<TradeData> DataProcessor::processOandAData(const Json::Value& data) {
    std::vector<TradeData> tradeDataList;
    const Json::Value& candles = data["candles"];

    for (const auto& candle : candles) {
        try {
            TradeData tradeData;
            tradeData.symbol = data["instrument"].asString();
            tradeData.date = candle["time"].asString();
            tradeData.openBid = candle["bid"]["o"].isString() ? std::stod(candle["bid"]["o"].asString()) : 0.0;
            tradeData.highBid = candle["bid"]["h"].isString() ? std::stod(candle["bid"]["h"].asString()) : 0.0;
            tradeData.lowBid = candle["bid"]["l"].isString() ? std::stod(candle["bid"]["l"].asString()) : 0.0;
            tradeData.closeBid = candle["bid"]["c"].isString() ? std::stod(candle["bid"]["c"].asString()) : 0.0;
            tradeData.openAsk = candle["ask"]["o"].isString() ? std::stod(candle["ask"]["o"].asString()) : 0.0;
            tradeData.highAsk = candle["ask"]["h"].isString() ? std::stod(candle["ask"]["h"].asString()) : 0.0;
            tradeData.lowAsk = candle["ask"]["l"].isString() ? std::stod(candle["ask"]["l"].asString()) : 0.0;
            tradeData.closeAsk = candle["ask"]["c"].isString() ? std::stod(candle["ask"]["c"].asString()) : 0.0;
            tradeData.totalTicks = candle["volume"].isInt() ? candle["volume"].asInt() : 0;

            tradeDataList.push_back(tradeData);
        } catch (const std::exception& e) {
            std::cerr << "Error processing OandA data candle: " << e.what() << std::endl;
        }
    }

    return tradeDataList;
}