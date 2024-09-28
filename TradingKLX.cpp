#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <json/json.h>
#include "TechnicalIndicators.h"
#include "TradingStrategy.h"
#include "PriceData.h"
#include "PerformanceAssessor.h"
#include "AlphaVantageAPI.h"

// Function to process market data and update the strategy with new data
void processMarketData(const std::vector<PriceData>& priceData, TradingStrategy& strategy) {
    for (size_t i = 0; i < priceData.size(); ++i) {
        const auto& data = priceData[i];
        // Log every data point
//        std::cout << "Processing data point " << i << ": High=" << data.askHigh << ", Low=" << data.askLow << ", Close=" << data.askClose << std::endl;

        // Update strategy with new market data
        strategy.onNewData(data.askHigh, data.askLow, data.askClose);
    }
}

void calculateIchimokuIndicators(const std::vector<double>& highs, const std::vector<double>& lows,
                                 SlidingWindow& tenkanWindow, SlidingWindow& kijunWindow,
                                 std::vector<double>& senkouA, std::vector<double>& senkouB,
                                 SlidingWindow& highsWindow, SlidingWindow& lowsWindow,
                                 TechnicalIndicators& indicators, IchimokuMemo& ichimokuMemo) {
    // Ensure the sliding windows accumulate data properly
    for (size_t i = 0; i < highs.size(); ++i) {
        // Add data to sliding windows
        highsWindow.addDataPoint(highs[i]);
        lowsWindow.addDataPoint(lows[i]);

        if (highsWindow.size() >= 9) {
            std::vector<double> highsVector(highsWindow.getData().begin(), highsWindow.getData().end());
            std::vector<double> lowsVector(lowsWindow.getData().begin(), lowsWindow.getData().end());
            double tenkan = indicators.calculateTenkanSen(highsVector, lowsVector, 9, highsWindow.size() - 1, ichimokuMemo);
            tenkanWindow.addDataPoint(tenkan);
        }
        if (highsWindow.size() >= 26) {
            std::vector<double> highsVector(highsWindow.getData().begin(), highsWindow.getData().end());
            std::vector<double> lowsVector(lowsWindow.getData().begin(), lowsWindow.getData().end());
            double kijun = indicators.calculateKijunSen(highsVector, lowsVector, 26, highsWindow.size() - 1, ichimokuMemo);
            kijunWindow.addDataPoint(kijun);
        }

        if (highsWindow.size() >= 52 && lowsWindow.size() >= 52) {
            // Ensure sliding windows have accumulated enough data
            double senkouAVal = indicators.calculateSenkouSpanA(tenkanWindow, kijunWindow, ichimokuMemo);
            double senkouBVal = indicators.calculateSenkouSpanB(highsWindow, lowsWindow, ichimokuMemo);
            senkouA.push_back(senkouAVal);
            senkouB.push_back(senkouBVal);
        }
    }
}

// Function to calculate Bollinger Bands
void calculateBollingerBands(const std::vector<double>& closes, std::vector<double>& lowerBB,
                             std::vector<double>& upperBB, TechnicalIndicators& indicators,
                             BollingerBandsMemo& bbMemo) {
    if (closes.size() < 20) {
        std::cerr << "Not enough data for Bollinger Bands calculation." << std::endl;
        return;
    }

    for (size_t i = 0; i < closes.size(); ++i) {
        if (i >= 20) {  // Ensure at least 20 data points are available
            auto bands = indicators.calculateBollingerBandsWithMemoization(closes, 20, 2, bbMemo);
            lowerBB.push_back(bands.first);
            upperBB.push_back(bands.second);
        } else {
            lowerBB.push_back(0);  // Placeholder when data is insufficient
            upperBB.push_back(0);  // Placeholder when data is insufficient
        }
    }
}

// Function to log signals
void logSignals(const std::vector<TradingSignal>& signals) {
    for (const auto& signal : signals) {
        if (signal.buy) {
            std::cout << "Buy signal at index: " << signal.index << "\n"
                      << "Position Size: " << signal.positionSize << "\n"
                      << "Stop Loss: " << signal.stopLossLevel << "\n"
                      << "Entry Price: " << signal.entryPrice << "\n" << std::endl;
        } else if (signal.sell) {
            std::cout << "Sell signal at index: " << signal.index << "\n"
                      << "Profit: " << signal.profit << "\n"
                      << "Exit Price: " << signal.exitPrice << "\n" << std::endl;
        }
    }
}

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <json/json.h>
#include <curl/curl.h>
#include "PriceData.h"
#include "AlphaVantageAPI.h"

// Function to load historical data from AlphaVantage API
std::vector<PriceData> loadHistoricalDataFromAPI(const std::string& from_symbol, const std::string& to_symbol) {
    AlphaVantageAPI alphaVantage("J3N168JI7LFNWHU1");
    std::vector<PriceData> priceData;

    // Fetch data from AlphaVantage API
    std::string jsonData = alphaVantage.fetchData("FX_DAILY", from_symbol, to_symbol);
    if (jsonData.empty()) {
        std::cerr << "Error: Failed to fetch data from AlphaVantage API." << std::endl;
        return priceData;  // Return empty if fetching failed
    }

    // Initialize JSON parsing
    Json::CharReaderBuilder reader;
    Json::Value jsonRoot;
    std::string errs;
    std::istringstream jsonStream(jsonData);

    // Attempt to parse the JSON response
    if (!Json::parseFromStream(reader, jsonStream, &jsonRoot, &errs)) {
        std::cerr << "Error: Failed to parse JSON data. " << errs << std::endl;
        return priceData;  // Return empty if parsing failed
    }

    // Check for "Time Series FX (Daily)" in the parsed JSON
    const Json::Value& timeSeries = jsonRoot["Time Series FX (Daily)"];
    if (timeSeries.isNull()) {
        std::cerr << "Error: No 'Time Series FX (Daily)' data found in JSON response." << std::endl;
        return priceData;  // Return empty if no time series data is found
    }

    // Iterate over the time series data
    for (auto itr = timeSeries.begin(); itr != timeSeries.end(); ++itr) {
        std::string date = itr.key().asString();
        const Json::Value& dailyData = *itr;

        // Ensure all required fields (open, high, low, close) are present
        if (dailyData.isMember("1. open") && dailyData.isMember("2. high") &&
            dailyData.isMember("3. low") && dailyData.isMember("4. close")) {

            // Parse ASK prices
            double askOpen = std::stod(dailyData["1. open"].asString());
            double askHigh = std::stod(dailyData["2. high"].asString());
            double askLow = std::stod(dailyData["3. low"].asString());
            double askClose = std::stod(dailyData["4. close"].asString());

            // Compute BID prices (slightly adjusted from ASK prices)
            double bidOpen = askOpen * 0.9995;
            double bidHigh = askHigh * 0.9995;
            double bidLow = askLow * 0.9995;
            double bidClose = askClose * 0.9995;

            // Create a PriceData object and add it to the priceData vector
            priceData.emplace_back(date, askOpen, askHigh, askLow, askClose, 0.0, bidOpen, bidHigh, bidLow, bidClose, 0.0);
        } else {
            std::cerr << "Warning: Missing required data fields for date " << date << std::endl;
        }
    }

    // Return the populated priceData vector
    return priceData;
}

void walkForwardOptimization(TradingStrategy& strategy, const std::vector<PriceData>& priceData, size_t walkForwardPeriod) {
    // Sliding windows for Ichimoku calculations
    SlidingWindow highsWindow(52);
    SlidingWindow lowsWindow(52);
    SlidingWindow tenkanWindow(9);
    SlidingWindow kijunWindow(26);

    for (size_t start = 0; start < priceData.size(); start += walkForwardPeriod) {
        std::vector<PriceData> walkForwardData(priceData.begin() + start, priceData.begin() + std::min(start + walkForwardPeriod, priceData.size()));

        std::cout << "Processing walk-forward period starting at index " << start << std::endl;
        processMarketData(walkForwardData, strategy);

        TechnicalIndicators indicators;
        std::vector<double> askHighs, askLows, askCloses;

        // Extract market data from walkForwardData
        for (const auto& data : walkForwardData) {
            askHighs.push_back(data.askHigh);
            askLows.push_back(data.askLow);
            askCloses.push_back(data.askClose);
        }

        // Perform Ichimoku Indicator calculations
        std::vector<double> senkouA, senkouB;
        IchimokuMemo ichimokuMemo;
        calculateIchimokuIndicators(askHighs, askLows, tenkanWindow, kijunWindow, senkouA, senkouB, highsWindow, lowsWindow, indicators, ichimokuMemo);

        // Perform Bollinger Bands calculations
        std::vector<double> lowerBB, upperBB;
        BollingerBandsMemo bbMemo;
        calculateBollingerBands(askCloses, lowerBB, upperBB, indicators, bbMemo);

        // Calculate valid start index for signal evaluation (Senkou Span A/B requires 52 points)
        size_t validStartIndex = 52;

        // Ensure sufficient data before evaluating signals
        for (size_t i = validStartIndex; i < askCloses.size(); ++i) {
            // Convert sliding windows to vectors for signal evaluation
            std::vector<double> tenkanVector(tenkanWindow.getData().begin(), tenkanWindow.getData().end());
            std::vector<double> kijunVector(kijunWindow.getData().begin(), kijunWindow.getData().end());

            // Use the converted vectors for signal evaluation
            auto signals = strategy.evaluateSignals(walkForwardData, askCloses, askHighs, askLows, tenkanVector, kijunVector, senkouA, senkouB, lowerBB, upperBB);
            logSignals(signals);
        }
    }
}

int main() {
    std::vector<PriceData> priceData = loadHistoricalDataFromAPI("EUR", "USD");

    if (priceData.empty()) {
        std::cerr << "Error: Failed to load price data from API." << std::endl;
        return -1;
    }

    TradingStrategy strategy(10000.0, 0.0015, 1.25);
    size_t walkForwardPeriod = 1000;
    walkForwardOptimization(strategy, priceData, walkForwardPeriod);

    return 0;
}