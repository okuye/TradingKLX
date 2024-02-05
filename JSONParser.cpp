#include "JSONParser.h"
#include <nlohmann/json.hpp>
#include <tuple>


std::tuple<std::vector<double>, std::vector<double>, std::vector<double>> parseJsonForPrices(const std::string& jsonData) {
    std::vector<double> highPrices, lowPrices, closePrices;

    auto j = nlohmann::json::parse(jsonData);
    const auto& timeSeries = j["Time Series (Daily)"];

    for (auto it = timeSeries.begin(); it != timeSeries.end(); ++it) {
        highPrices.push_back(std::stod(it.value()["2. high"].get<std::string>()));
        lowPrices.push_back(std::stod(it.value()["3. low"].get<std::string>()));
        closePrices.push_back(std::stod(it.value()["4. close"].get<std::string>()));
    }

    return std::make_tuple(highPrices, lowPrices, closePrices);
}


// Function to parse JSON and extract close prices
std::vector<double> parseJsonForClosePrices(const std::string& jsonData) {
    std::vector<double> closePrices;

    // Parse the JSON data
    auto j = nlohmann::json::parse(jsonData);

    // Navigate through the JSON structure based on Alpha Vantage's response format
    // Assuming the structure is something like: {"Time Series (Daily)": {"2020-01-01": {"4. close": "123.45"}, ...}}
    const auto& timeSeries = j["Time Series (Daily)"];

    for (auto it = timeSeries.begin(); it != timeSeries.end(); ++it) {
        // Extract the closing price for each day and convert it to double
        double closePrice = std::stod(it.value()["4. close"].get<std::string>());
        closePrices.push_back(closePrice);
    }

    return closePrices;
}
