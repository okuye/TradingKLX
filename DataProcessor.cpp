#include "DataProcessor.h"
#include "PriceData.h"
#include "TradeData.h"
#include <iostream>
#include <stdexcept>
#include <json/json.h>
#include <sstream>
#include <spdlog/spdlog.h>



std::vector<TradeData> DataProcessor::processTradingServerData(const Json::Value& data) {
    std::vector<TradeData> tradeDataList;
    for (const auto& item : data) {
        TradeData tradeData;
        tradeData.symbol = item["symbol"].asString();
        tradeData.date = item["date"].asString();
        tradeData.hour = item["hour"].asInt();
        tradeData.openBid = item["openBid"].asDouble();
        tradeData.highBid = item["highBid"].asDouble();
        tradeData.lowBid = item["lowBid"].asDouble();
        tradeData.closeBid = item["closeBid"].asDouble();
        tradeData.openAsk = item["openAsk"].asDouble();
        tradeData.highAsk = item["highAsk"].asDouble();
        tradeData.lowAsk = item["lowAsk"].asDouble();
        tradeData.closeAsk = item["closeAsk"].asDouble();
        tradeData.totalTicks = item["totalTicks"].asInt();
        tradeDataList.push_back(tradeData);
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
        tradeData.openBid = item["openBid"].asDouble();
        tradeData.highBid = item["highBid"].asDouble();
        tradeData.lowBid = item["lowBid"].asDouble();
        tradeData.closeBid = item["closeBid"].asDouble();
        tradeData.openAsk = item["openAsk"].asDouble();
        tradeData.highAsk = item["highAsk"].asDouble();
        tradeData.lowAsk = item["lowAsk"].asDouble();
        tradeData.closeAsk = item["closeAsk"].asDouble();
        tradeData.totalTicks = item["totalTicks"].asInt();
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