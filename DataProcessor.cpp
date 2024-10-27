#include "DataProcessor.h"
#include "PriceData.h"
#include "TradeData.h"
#include <iostream>
#include <stdexcept>
#include <json/json.h>
#include <sstream>
#include <spdlog/spdlog.h>
#include <unordered_map>


std::vector<TradeData> DataProcessor::processTradingServerData(const Json::Value& data) {
    std::vector<TradeData> tradeDataList;
    std::unordered_map<std::string, int> invalidEntriesCount;
    int validTradeCount = 0;
    const int minValidTradesThreshold = 52;  // Example threshold for minimum valid trades per date

    for (const auto& item : data) {
        TradeData tradeData;
        tradeData.symbol = item["symbol"].asString();
        tradeData.date = item["date"].asString();
        tradeData.hour = item["hour"].asInt();

        // Log the raw JSON data for debugging
        spdlog::debug("Processing trade: {}", item.toStyledString());

        bool invalidAsk = !item["openask"].isDouble() || !item["highask"].isDouble() ||
                          !item["lowask"].isDouble() || !item["closeask"].isDouble() ||
                          item["openask"].asDouble() == 0 || item["highask"].asDouble() == 0 ||
                          item["lowask"].asDouble() == 0 || item["closeask"].asDouble() == 0;

        if (invalidAsk) {
            invalidEntriesCount[tradeData.date]++;
            spdlog::warn("Invalid ask values for trade on {}: openAsk={}, highAsk={}, lowAsk={}, closeAsk={}",
                         tradeData.date, item["openask"].asDouble(), item["highask"].asDouble(),
                         item["lowask"].asDouble(), item["closeask"].asDouble());
            continue;
        }

        validTradeCount++;  // Increment count for valid trades
        tradeData.openBid = item["openbid"].asDouble();
        tradeData.highBid = item["highbid"].asDouble();
        tradeData.lowBid = item["lowbid"].asDouble();
        tradeData.closeBid = item["closebid"].asDouble();
        tradeData.openAsk = item["openask"].asDouble();
        tradeData.highAsk = item["highask"].asDouble();
        tradeData.lowAsk = item["lowask"].asDouble();
        tradeData.closeAsk = item["closeask"].asDouble();
        tradeData.totalTicks = item["totalticks"].asInt();
        tradeDataList.push_back(tradeData);
    }

    // Log the number of valid trades processed
    spdlog::info("Processed {} valid trades.", validTradeCount);

    // Log insufficient valid data if threshold is not met
    if (validTradeCount < minValidTradesThreshold) {
        spdlog::warn("Insufficient valid trade data: Only {} valid trades available. Minimum required: {}.", validTradeCount, minValidTradesThreshold);
    }

    for (const auto& [date, count] : invalidEntriesCount) {
        spdlog::warn("Date {} has {} trades with invalid ask values for symbol EUR/USD.", date, count);
    }

    return tradeDataList;
}

std::vector<TradeData> DataProcessor::processOandAData(const Json::Value& data) {
    std::vector<TradeData> tradeDataList;
    for (const auto& item : data) {
        TradeData tradeData;

        tradeData.symbol = item["symbol"].asString();
        tradeData.date = item["date"].asString();
        tradeData.hour = item["hour"].asInt();

        // Fill values with default if missing, check for valid asks
        tradeData.openBid = item["openBid"].isDouble() ? item["openBid"].asDouble() : 0.0;
        tradeData.highBid = item["highBid"].isDouble() ? item["highBid"].asDouble() : 0.0;
        tradeData.lowBid = item["lowBid"].isDouble() ? item["lowBid"].asDouble() : 0.0;
        tradeData.closeBid = item["closeBid"].isDouble() ? item["closeBid"].asDouble() : 0.0;
        tradeData.openAsk = item["openAsk"].isDouble() ? item["openAsk"].asDouble() : 0.0;
        tradeData.highAsk = item["highAsk"].isDouble() ? item["highAsk"].asDouble() : 0.0;
        tradeData.lowAsk = item["lowAsk"].isDouble() ? item["lowAsk"].asDouble() : 0.0;
        tradeData.closeAsk = item["closeAsk"].isDouble() ? item["closeAsk"].asDouble() : 0.0;
        tradeData.totalTicks = item["totalTicks"].isInt() ? item["totalTicks"].asInt() : 0;

        // Skip trade if any ask values are zero (invalid)
        if (tradeData.closeAsk == 0 || tradeData.highAsk == 0 || tradeData.lowAsk == 0) {
            spdlog::warn("Skipping trade with invalid ask values. Symbol: {}, Date: {}", tradeData.symbol, tradeData.date);
            continue;
        }

        tradeDataList.push_back(tradeData);
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