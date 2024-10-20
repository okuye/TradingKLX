#include "DataProcessor.h"
#include "PriceData.h"
#include <iostream>
#include <stdexcept>
#include <json/json.h>
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

// Helper function to convert a Json::Value to string for logging/debugging
std::string DataProcessor::jsonToString(const Json::Value& jsonValue) {
    Json::StreamWriterBuilder writer;
    writer["indentation"] = "";
    return Json::writeString(writer, jsonValue);
}

std::vector<TradeData> DataProcessor::processTradingServerData(const Json::Value& data) {
    std::vector<TradeData> tradeDataList;

    if (!data.isMember("datatable") || !data["datatable"].isMember("data")) {
        spdlog::error("JSON does not contain 'datatable.data' key.");
        throw JsonStructureException("Invalid JSON structure: missing 'datatable.data' key.");
    }

    const Json::Value& rows = data["datatable"]["data"];
    spdlog::info("Number of trade entries found: {}", rows.size());

    for (const auto& row : rows) {
        try {
            TradeData tradeData;
            tradeData.symbol = row["symbol"].asString();
            tradeData.date = row["date"].asString();
            tradeData.hour = row["hour"].asInt();
            tradeData.openBid = row["openbid"].asDouble();
            tradeData.highBid = row["highbid"].asDouble();
            tradeData.lowBid = row["lowbid"].asDouble();
            tradeData.closeBid = row["closebid"].asDouble();
            tradeData.openAsk = row["openask"].asDouble();
            tradeData.highAsk = row["highask"].asDouble();
            tradeData.lowAsk = row["lowask"].asDouble();
            tradeData.closeAsk = row["closeask"].asDouble();
            tradeData.totalTicks = row["totalticks"].asInt();

            tradeDataList.push_back(tradeData);
        } catch (const std::exception& e) {
            spdlog::error("Error processing trade entry: {}", e.what());
        }
    }

    spdlog::info("Processed {} trades.", tradeDataList.size());
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
            spdlog::error("Error processing OandA data candle: {}", e.what());
        }
    }

    return tradeDataList;
}

std::vector<PriceData> DataProcessor::processData(const std::string& jsonDataString, const std::string& key) {
    Json::Value root;
    Json::CharReaderBuilder readerBuilder;
    std::string errs;

    std::istringstream stream(jsonDataString);
    if (!Json::parseFromStream(readerBuilder, stream, &root, &errs)) {
        spdlog::error("Error parsing JSON data: {}", errs);
        throw std::runtime_error("Error parsing JSON data: " + errs);
    }

    if (!root.isMember(key) || !root[key].isArray()) {
        spdlog::error("Invalid JSON structure: missing or incorrect '{}' key", key);
        throw JsonStructureException("Invalid JSON structure: missing or incorrect '" + key + "' key");
    }

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
