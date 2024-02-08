#include "DataProcessor.h"

// processData implementation
std::vector<PriceData> DataProcessor::processData(const std::string& jsonData) {
    std::vector<PriceData> priceDataList;
    auto json = nlohmann::json::parse(jsonData);

    // Assuming the JSON structure is something like:
    // {"Time Series (5min)": {"2020-01-01 00:00:00": {"1. open": "1.2345", "2. high": "1.2346", ...}, ...}}
    const auto& timeSeries = json["Time Series (5min)"];

    for (const auto& [timestamp, data] : timeSeries.items()) {
        PriceData dataPoint;
        dataPoint.timestamp = timestamp;
        dataPoint.open = std::stod(data["1. open"].get<std::string>());
        dataPoint.high = std::stod(data["2. high"].get<std::string>());
        dataPoint.low = std::stod(data["3. low"].get<std::string>());
        dataPoint.close = std::stod(data["4. close"].get<std::string>());

        priceDataList.push_back(dataPoint);
    }

    return priceDataList;
}
