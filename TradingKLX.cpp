// TradingKLX.cpp : Defines the entry point for the application.
//
#include <tuple>
using namespace std;
#include "AlphaVantageAPI.h"
#include "JSONParser.h"
#include "TradingKLX.h"
#include <curl/curl.h>
#include <iostream>
#include <vector>
#include <deque>
#include <cmath>
#include <unordered_map>
#include <string>
#include <algorithm> // For std::max
#include <numeric> // For std::accumulate
#include <cmath>   // For std::sqrt
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <nlohmann/json.hpp> // Include JSON library header

bool useMongoDB() {
    // Example of checking an environment variable
    const char* useMongoEnv = std::getenv("USE_MONGO_DB");
    return useMongoEnv && std::string(useMongoEnv) == "true";
}


std::string fetchData(const std::string& apiKey, const std::string& function, const std::string& from_symbol, const std::string& to_symbol) {
    if (useMongoDB()) {
        // Fetch data from MongoDB
        return fetchDataFromMongo("databaseName", "collectionName");
    } else {
        // Fetch live data from Alpha Vantage
        return fetchDataFromAlphaVantage(apiKey, function, from_symbol, to_symbol);
    }
}


void storeDataInMongo(const std::string& jsonData) {
    // Convert jsonData to BSON and store in MongoDB
    saveDataToMongo(client, "klx", "fxTrade", jsonData);
}

// Define a structure to hold the price data for each interval
struct PriceData {
    double open;
    double high;
    double low;
    double close;
    std::string timestamp;
};

void processData(const std::string& jsonData) {
    // Parse the JSON data
    auto j = nlohmann::json::parse(jsonData);

    // Access the "Time Series FX (5min)" part of the JSON
    const auto& timeSeries = j["Time Series FX (5min)"];

    // Container to hold parsed data
    std::vector<PriceData> priceData;

    // Iterate through each time interval in the time series
    for (auto& item : timeSeries.items()) {
        PriceData data;
        data.timestamp = item.key(); // The key is the timestamp
        data.open = std::stod(item.value()["1. open"].get<std::string>());
        data.high = std::stod(item.value()["2. high"].get<std::string>());
        data.low = std::stod(item.value()["3. low"].get<std::string>());
        data.close = std::stod(item.value()["4. close"].get<std::string>());

        // Add the extracted data to the container
        priceData.push_back(data);
    }

    // Example: Print out the parsed data
    for (const auto& data : priceData) {
        std::cout << "Timestamp: " << data.timestamp
                  << ", Open: " << data.open
                  << ", High: " << data.high
                  << ", Low: " << data.low
                  << ", Close: " << data.close << std::endl;
    }

    // Here, you can further process the priceData, such as calculating technical indicators or generating signals
}

// MongoDB Data Storing
void storeDataInMongo(const std::string& jsonData, const std::string& dbName, const std::string& collectionName) {
    mongocxx::client client{mongocxx::uri{"mongodb://localhost:27017"}};
    auto collection = client[dbName][collectionName];

    auto document = bsoncxx::from_json(jsonData);
    collection.insert_one(document.view());
}



// Memoization structure for Ichimoku
struct IchimokuMemo {
    std::unordered_map<int, double> tenkanSenMemo;
    std::unordered_map<int, double> kijunSenMemo;
    std::unordered_map<int, double> high52Memo;
    std::unordered_map<int, double> low52Memo;
};

// Function to calculate Tenkan-sen (considering it's already implemented)
double calculateTenkanSen(const std::vector<double>& highs, const std::vector<double>& lows, int period, int index, IchimokuMemo& memo) {
    if (memo.tenkanSenMemo.find(index) == memo.tenkanSenMemo.end()) {
        // Calculate Tenkan-sen as the average of the highest high and the lowest low over the last 'period' periods
        auto highIt = std::max_element(highs.begin() + index - period + 1, highs.begin() + index + 1);
        auto lowIt = std::min_element(lows.begin() + index - period + 1, lows.begin() + index + 1);
        memo.tenkanSenMemo[index] = (*highIt + *lowIt) / 2.0;
    }
    return memo.tenkanSenMemo[index];
}

// Function to calculate Kijun-sen (considering it's already implemented)
double calculateKijunSen(const std::vector<double>& highs, const std::vector<double>& lows, int period, int index, IchimokuMemo& memo) {
    if (memo.kijunSenMemo.find(index) == memo.kijunSenMemo.end()) {
        // Calculate Kijun-sen as the average of the highest high and the lowest low over the last 'period' periods
        auto highIt = std::max_element(highs.begin() + index - period + 1, highs.begin() + index + 1);
        auto lowIt = std::min_element(lows.begin() + index - period + 1, lows.begin() + index + 1);
        memo.kijunSenMemo[index] = (*highIt + *lowIt) / 2.0;
    }
    return memo.kijunSenMemo[index];
}

// Function to calculate Senkou Span A with memoization
double calculateSenkouSpanA(int index, IchimokuMemo& memo) {
    // Senkou Span A is the average of Tenkan-sen and Kijun-sen, plotted 26 periods ahead
    // Assume that Tenkan-sen and Kijun-sen are already calculated and stored in the memo
    return (memo.tenkanSenMemo[index] + memo.kijunSenMemo[index]) / 2.0;
}

// Function to calculate Senkou Span B with memoization
double calculateSenkouSpanB(const std::vector<double>& highs, const std::vector<double>& lows, int index, IchimokuMemo& memo) {
    if (memo.high52Memo.find(index) == memo.high52Memo.end() || memo.low52Memo.find(index) == memo.low52Memo.end()) {
        // Calculate the highest high and the lowest low over the last 52 periods
        auto highIt = std::max_element(highs.begin() + index - 51, highs.begin() + index + 1); // 52 periods including the current
        auto lowIt = std::min_element(lows.begin() + index - 51, lows.begin() + index + 1);
        memo.high52Memo[index] = *highIt;
        memo.low52Memo[index] = *lowIt;
    }
    // Senkou Span B is the average of the highest high and the lowest low over the last 52 periods, plotted 26 periods ahead
    return (memo.high52Memo[index] + memo.low52Memo[index]) / 2.0;
}

// Function to calculate SMA; can also be memoized if called frequently with the same parameters
double calculateSMA(const std::vector<double>& data, int start, int end) {
    double sum = std::accumulate(data.begin() + start, data.begin() + end, 0.0);
    return sum / (end - start);
}

// Function to calculate standard deviation
double calculateStdDev(const std::vector<double>& data, int start, int end, double mean) {
    double sum = 0.0;
    for (int i = start; i < end; ++i) {
        sum += (data[i] - mean) * (data[i] - mean);
    }
    return std::sqrt(sum / (end - start));
}

// Memoization structure for Bollinger Bands
struct BollingerBandsMemo {
    std::unordered_map<int, double> smaMemo;
    std::unordered_map<int, double> stdDevMemo;
};

// Function to calculate Bollinger Bands with memoization
void calculateBollingerBandsWithMemoization(const std::vector<double>& data, int window, double numStdDev, std::vector<double>& upperBand, std::vector<double>& middleBand, std::vector<double>& lowerBand, BollingerBandsMemo& memo) {
    upperBand.resize(data.size());
    middleBand.resize(data.size());
    lowerBand.resize(data.size());

    for (size_t i = window - 1; i < data.size(); ++i) {
        double sma, stdDev;

        // Check if SMA is already calculated for this window
        if (memo.smaMemo.find(i) == memo.smaMemo.end()) {
            sma = calculateSMA(data, i - window + 1, i + 1);
            memo.smaMemo[i] = sma; // Store the calculated SMA
        } else {
            sma = memo.smaMemo[i];
        }

        // Check if standard deviation is already calculated for this window
        if (memo.stdDevMemo.find(i) == memo.stdDevMemo.end()) {
            stdDev = calculateStdDev(data, i - window + 1, i + 1, sma);
            memo.stdDevMemo[i] = stdDev; // Store the calculated standard deviation
        } else {
            stdDev = memo.stdDevMemo[i];
        }

        // Calculate Bollinger Bands
        middleBand[i] = sma;
        upperBand[i] = sma + numStdDev * stdDev;
        lowerBand[i] = sma - numStdDev * stdDev;
    }
}


// Generate a unique key for the memoization map based on the window size
std::string generateATRKey(int window) {
    return "ATR-" + std::to_string(window);
}

// Function to calculate True Range
double calculateTrueRange(double high, double low, double previousClose) {
    return std::max({high - low, std::abs(high - previousClose), std::abs(low - previousClose)});
}

// Memoized ATR calculation function
std::vector<double> calculateATRWithMemoization(const std::vector<double>& highs, const std::vector<double>& lows, const std::vector<double>& closes, int window, std::unordered_map<std::string, std::vector<double>>& memo) {
    std::string key = generateATRKey(window);
    auto it = memo.find(key);
    if (it != memo.end()) {
        // If ATR for this window size is already calculated, use it
        return it->second;
    }

    std::vector<double> atr(highs.size(), 0.0);
    double trSum = 0;
    for (size_t i = 1; i < highs.size(); ++i) {
        double tr = calculateTrueRange(highs[i], lows[i], closes[i - 1]);
        trSum += tr;
        if (i < window) continue; // Skip until we have enough data
        if (i == window) {
            atr[i] = trSum / window; // First ATR value is an average of the first 'window' TR values
        } else {
            // Subsequent ATR values are calculated using the previous ATR value
            atr[i] = (atr[i - 1] * (window - 1) + tr) / window;
        }
    }

    // Store the calculated ATR values in the memoization map
    memo[key] = atr;
    return atr;
}


// A helper function to generate a unique key for the memoization map
std::string generateKey(int start, int window) {
    return std::to_string(start) + "-" + std::to_string(window);
}

// Modified SMA function with memoization
std::vector<double> calculateSMAWithMemoization(const std::vector<double>& data, int window, std::unordered_map<std::string, double>& memo) {
    std::vector<double> sma;
    if (data.empty() || window <= 0) return sma;

    double sum = 0;
    for (size_t i = 0; i < data.size(); ++i) {
        sum += data[i];
        if (i >= window - 1) {
            std::string key = generateKey(i - window + 1, window);
            auto it = memo.find(key);
            if (it != memo.end()) {
                // If the SMA for this window is already calculated, use it
                sma.push_back(it->second);
            } else {
                // Otherwise, calculate, store in memo, and then use it
                double avg = sum / window;
                memo[key] = avg;
                sma.push_back(avg);
            }
            sum -= data[i - window + 1];
        }
    }

    return sma;
}



// Helper function to calculate rolling maximum or minimum
std::vector<double> rollingExtremum(const std::vector<double>& data, int window, bool findMax) {
    if (data.empty() || window <= 0) return {};

    std::vector<double> extremumValues;
    std::deque<size_t> deq;

    for (size_t i = 0; i < data.size(); ++i) {
        // Remove elements outside of the current window
        while (!deq.empty() && deq.front() <= i - window) {
            deq.pop_front();
        }

        // For maximum, remove all elements smaller than the current
        // For minimum, remove all elements larger than the current
        while (!deq.empty() && ((findMax && data[i] >= data[deq.back()]) || (!findMax && data[i] <= data[deq.back()]))) {
            deq.pop_back();
        }

        deq.push_back(i);

        if (i >= static_cast<size_t>(window) - 1) {
            extremumValues.push_back(data[deq.front()]);
        }

    }

    return extremumValues;
}



void calculateFibLevels(const std::vector<double>& highs, const std::vector<double>& lows, std::vector<double>& fib_23_6, std::vector<double>& fib_38_2, std::vector<double>& fib_61_8) {
    for (size_t i = 0; i < highs.size(); ++i) {
        double range = highs[i] - lows[i];
        fib_23_6[i] = highs[i] - (range * 0.236);
        fib_38_2[i] = highs[i] - (range * 0.382);
        fib_61_8[i] = highs[i] - (range * 0.618);
    }
}



std::vector<double> calculateRSI(const std::vector<double>& closes, int window) {
    std::vector<double> rsi(closes.size(), 0); // Initialize RSI values to 0

    double gainSum = 0, lossSum = 0;
    // Calculate initial average gain and loss
    for (size_t i = 1; i <= static_cast<size_t>(window); ++i) {
        // Explicitly handle the subtraction to avoid underflow
        double delta = closes[i] - closes[i - 1];
        if (delta > 0) gainSum += delta; // Gain
        else lossSum -= delta; // Loss (as a positive value)
    }

    double avgGain = gainSum / window;
    double avgLoss = lossSum / window;

    // Calculate RSI starting from 'window + 1'
    for (size_t i = static_cast<size_t>(window) + 1; i < closes.size(); ++i) {
        // Safely handle the subtraction to avoid underflow
        double delta = closes[i] - closes[i - 1];
        double gain = delta > 0 ? delta : 0;
        double loss = delta < 0 ? -delta : 0;

        // Ensure the entire operation is cast to size_t to avoid overflow
        avgGain = (avgGain * (static_cast<size_t>(window) - 1) + gain) / window;
        avgLoss = (avgLoss * (static_cast<size_t>(window) - 1) + loss) / window;

        if (avgLoss != 0) { // Avoid division by zero
            double rs = avgGain / avgLoss;
            rsi[i] = 100 - (100 / (1 + rs));
        }
        else {
            rsi[i] = 100; // If avgLoss is 0, RSI is considered 100
        }
    }

    return rsi;
}

std::vector<double> calculateRollingStd(const std::vector<double>& data, int window) {
    std::vector<double> rollingStd;
    std::deque<double> windowElements;

    for (size_t i = 0; i < data.size(); ++i) {
        // Cast window to size_t before performing subtraction to avoid warning
        if (i >= static_cast<size_t>(window) - 1) {
            double sum = std::accumulate(windowElements.begin(), windowElements.end(), 0.0);
            double mean = sum / windowElements.size();

            double sq_sum = std::inner_product(windowElements.begin(), windowElements.end(), windowElements.begin(), 0.0,
                                               std::plus<double>(), [mean](double a, double b) { return (a - mean) * (b - mean); });

            double std_dev = std::sqrt(sq_sum / windowElements.size());
            rollingStd.push_back(std_dev);
        }

        windowElements.push_back(data[i]);
        if (windowElements.size() > static_cast<size_t>(window)) {
            windowElements.pop_front();
        }
    }

    return rollingStd;
}




// Function to calculate stop-loss levels based on ATR
std::vector<double> calculateStopLossLevels(const std::vector<double>& entryPrices, const std::vector<double>& atr, double multiplier) {
    std::vector<double> stopLossLevels(entryPrices.size());
    for (size_t i = 0; i < entryPrices.size(); ++i) {
        // Setting stop-loss a certain multiplier below the entry price
        stopLossLevels[i] = entryPrices[i] - (atr[i] * multiplier);
    }
    return stopLossLevels;
}

// Function to calculate position size based on volatility and a fixed risk per trade
std::vector<double> calculatePositionSizes(const std::vector<double>& atr, double accountBalance, double riskPerTrade) {
    std::vector<double> positionSizes(atr.size());
    for (size_t i = 0; i < atr.size(); ++i) {
        // Calculate position size as a fraction of account balance based on ATR
        // Ensure that the position size never risks more than 'riskPerTrade' percentage of the account balance
        positionSizes[i] = (std::min)((accountBalance * riskPerTrade) / atr[i], accountBalance);

    }
    return positionSizes;
}



void integrateRiskManagement(
    const std::vector<double>& highPrices,
    const std::vector<double>& lowPrices,
    const std::vector<double>& closePrices,
    const std::vector<double>& entryPrices,
    double& accountBalance,  // Note the use of reference to modify accountBalance
    double riskPerTrade,
    double stopLossMultiplier
    ) {
    // Calculate ATR, stop-loss levels, and position sizes
    std::vector<double> stopLossLevels = calculateStopLossLevels(entryPrices, atr, stopLossMultiplier);
    std::vector<double> positionSizes = calculatePositionSizes(atr, accountBalance, riskPerTrade);

    // Calculate sma50 here
    std::vector<double> sma50 = calculateSMA(closePrices, 50);

    for (size_t i = 1; i < closePrices.size(); ++i) {  // Start from 1 to avoid underflow in i-1
        // Check condition to enter a trade
        if (closePrices[i] > sma50[i] && closePrices[i - 1] <= sma50[i - 1]) {
            double positionSize = positionSizes[i];  // Get the position size for the current trade
            // Logic to execute the trade, including setting a stop-loss level
            std::cout << "Trade executed at index " << i << std::endl;
            std::cout << "Position size: " << positionSize << std::endl;
            std::cout << "Stop-loss level: " << stopLossLevels[i] << std::endl;

            // Update the account balance based on the outcome of the trade
            accountBalance -= positionSize;  // Deduct the position size to simulate the trade
        }
    }
}




int main() {
    mongocxx::instance instance{};

    std::string apiKey = "42XW1WPYAK910VR7";
    std::string function = "FX_INTRADAY";
    std::string from_symbol = "EUR";
    std::string to_symbol = "USD";
    std::string dbName = "klx";
    std::string collectionName = "fxTrade";

    std::string data;
    if (useMongoDB()) {
        mongocxx::client client{mongocxx::uri{"mongodb://localhost:27017"}};
        data = fetchDataFromMongo(client, dbName, collectionName);
    } else {
        data = fetchDataFromAlphaVantage(apiKey, function, from_symbol, to_symbol);
        if (!data.empty()) {
            mongocxx::client client{mongocxx::uri{"mongodb://localhost:27017"}};
            storeDataInMongo(client, data, dbName, collectionName);
        }
    }

    if (data.empty()) {
        std::cerr << "Failed to fetch or retrieve data." << std::endl;
        return 1;
    }

    std::vector<double> highPrices, lowPrices, closePrices;
    std::tie(highPrices, lowPrices, closePrices) = parseJsonForPrices(data);

    if (closePrices.empty() || highPrices.empty() || lowPrices.empty()) {
        std::cerr << "Failed to parse data." << std::endl;
        return 1;
    }

    double accountBalance = 10000.0;
    double riskPerTrade = 0.01;
    double stopLossMultiplier = 3.0;

    // Memoization structures
    std::unordered_map<std::string, double> smaMemo;
    BollingerBandsMemo bbMemo;
    std::unordered_map<std::string, std::vector<double>> atrMemo;

    // Calculate SMA with memoization
    std::vector<double> sma50 = calculateSMAWithMemoization(closePrices, 50, smaMemo);

    // Calculate ATR with memoization
    std::vector<double> atr = calculateATRWithMemoization(highPrices, lowPrices, closePrices, 14, atrMemo);

    // Calculate Bollinger Bands with memoization
    std::vector<double> bbUpper(closePrices.size()), bbMiddle(closePrices.size()), bbLower(closePrices.size());
    calculateBollingerBandsWithMemoization(closePrices, 20, 2, bbUpper, bbMiddle, bbLower, bbMemo);

    // Generate buy and sell signals based on your strategy
    std::vector<bool> buySignals(closePrices.size(), false);
    std::vector<bool> sellSignals(closePrices.size(), false);
    // Example strategy: Buy when close is above SMA and price is at the lower Bollinger Band
    for (size_t i = 0; i < closePrices.size(); ++i) {
        buySignals[i] = closePrices[i] > sma50[i] && closePrices[i] <= bbLower[i];
        // Define sellSignals based on your strategy
    }

    // Determine entry prices for buy signals
    std::vector<double> entryPrices;
    for (size_t i = 0; i < buySignals.size(); ++i) {
        if (buySignals[i]) {
            entryPrices.push_back(closePrices[i]);
            // Log or execute buy orders here
            std::cout << "Buy signal at index " << i << " with entry price: " << entryPrices.back() << std::endl;
        }
    }

    // Apply risk management before executing trades
    if (!entryPrices.empty()) {
        integrateRiskManagement(
            highPrices, lowPrices, closePrices, entryPrices,
            accountBalance, riskPerTrade, stopLossMultiplier
            );
    }

    // Example: Log trade execution (replace with actual trade execution logic)
    for (size_t i = 0; i < entryPrices.size(); ++i) {
        std::cout << "Executing trade at index " << i << " with entry price: " << entryPrices[i] << std::endl;
        // Example: Deduct position size from account balance
        accountBalance -= 100; // Assuming a fixed position size for illustration
    }

    return 0;
}



//https://www.alphavantage.co/query?function=FX_DAILY&from_symbol=EUR&to_symbol=USD&outputsize=full&apikey=demo
// int main() {


//     mongocxx::instance instance{}; // This should be done only once.

//     std::string apiKey = "42XW1WPYAK910VR7"; // Your API key
//     std::string function = "FX_INTRADAY";
//     std::string from_symbol = "EUR";
//     std::string to_symbol = "USD";
//     std::string dbName = "klx";
//     std::string collectionName = "fxTrade";

//     std::string data;
//     if (useMongoDB()) {
//         data = fetchDataFromMongo(dbName, collectionName);
//     } else {
//         data = fetchDataFromAlphaVantage(apiKey, function, from_symbol, to_symbol);
//         storeDataInMongo(data, dbName, collectionName); // Store fetched data for future use
//     }

//     processData(data);

//     // Fetch the JSON data from Alpha Vantage
//     std::string jsonData = fetchDataFromAlphaVantage(apiKey, function, from_symbol, to_symbol);
//     if (jsonData.empty()) {
//         std::cerr << "Failed to fetch data from Alpha Vantage." << std::endl;
//         return 1;
//     }

//     // Parse the JSON data to extract high, low, and close prices
//     std::vector<double> closePrices, highPrices, lowPrices;
//     std::tie(highPrices, lowPrices, closePrices) = parseJsonForPrices(jsonData);

//     // Define your trading strategy parameters
//     double accountBalance = 10000.0; // Example account balance
//     double riskPerTrade = 0.01; // Risk 1% of account balance per trade
//     double stopLossMultiplier = 3.0; // Set stop-loss at 3x ATR below the entry price

//     // Memoization structures
//     std::unordered_map<std::string, double> smaMemo;
//     std::unordered_map<std::string, std::vector<double>> atrMemo;
//     BollingerBandsMemo bbMemo;
//     IchimokuMemo ichimokuMemo;

//     // Calculate necessary indicators with memoization
//     std::vector<double> sma50 = calculateSMAWithMemoization(closePrices, 50, smaMemo);
//     std::vector<double> atr = calculateATRWithMemoization(highPrices, lowPrices, closePrices, 14, atrMemo);
//     std::vector<double> bbUpper, bbMiddle, bbLower;
//     calculateBollingerBandsWithMemoization(closePrices, 20, 2, bbUpper, bbMiddle, bbLower, bbMemo);

//     // Determine entry prices based on your trading signals (implementation depends on your strategy)
//     std::vector<double> entryPrices; // Populate this based on your buy signals

//     // Call the risk management function with all required parameters
//     integrateRiskManagement(
//         highPrices,
//         lowPrices,
//         closePrices,
//         entryPrices,
//         accountBalance,
//         riskPerTrade,
//         stopLossMultiplier
//         );

//     // Based on the output of risk management, make final decisions and execute trades
//     // (Your trade execution logic here)

//     return 0;
// }

