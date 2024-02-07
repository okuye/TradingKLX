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
#include <algorithm> 
#include <deque>    
#include <numeric> 
#include <cmath>  
#include <unordered_map>
#include <string>
#include <algorithm> // For std::max
#include <numeric> // For std::accumulate
#include <cmath>   // For std::sqrt


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

// Helper function to calculate Simple Moving Average (SMA)
std::vector<double> calculateSMA(const std::vector<double>& data, int window) {
	if (data.empty() || window <= 0) return {};

	std::vector<double> sma;
	std::deque<double> windowElements;
	double sum = 0;

	for (size_t i = 0; i < data.size(); ++i) {
		sum += data[i];
		windowElements.push_back(data[i]);

		if (windowElements.size() > window) {
			sum -= windowElements.front();
			windowElements.pop_front();
		}

		if (i >= static_cast<size_t>(window) - 1) {
			sma.push_back(sum / window);
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

// Utility function for calculating Tenkan-sen or Kijun-sen
std::vector<double> calculateSen(const std::vector<double>& high, const std::vector<double>& low, int period) {
	std::vector<double> sen(high.size() - period + 1);
	auto high_ext = rollingExtremum(high, period, true);
	auto low_ext = rollingExtremum(low, period, false);

	std::transform(high_ext.begin(), high_ext.end(), low_ext.begin(), sen.begin(), [](double h, double l) { return (h + l) / 2; });

	return sen;
}

void calculateBollingerBands(const std::vector<double>& sma, const std::vector<double>& rollingStd, std::vector<double>& bb_upper, std::vector<double>& bb_lower) {
	for (size_t i = 0; i < sma.size(); ++i) {
		bb_upper[i] = sma[i] + (rollingStd[i] * 2);
		bb_lower[i] = sma[i] - (rollingStd[i] * 2);
	}
}

void calculateFibLevels(const std::vector<double>& highs, const std::vector<double>& lows, std::vector<double>& fib_23_6, std::vector<double>& fib_38_2, std::vector<double>& fib_61_8) {
	for (size_t i = 0; i < highs.size(); ++i) {
		double range = highs[i] - lows[i];
		fib_23_6[i] = highs[i] - (range * 0.236);
		fib_38_2[i] = highs[i] - (range * 0.382);
		fib_61_8[i] = highs[i] - (range * 0.618);
	}
}

// Corrected calculateATR function
std::vector<double> calculateATR(const std::vector<double>& highs, const std::vector<double>& lows, const std::vector<double>& closes, int window) {
	std::vector<double> atr(highs.size());
	std::deque<double> trQueue;
	double trSum = 0;

	for (size_t i = 0; i < highs.size(); ++i) {
		double tr = highs[i] - lows[i];
		if (i > 0) {
			tr = (std::max)(tr, std::abs(highs[i] - closes[i - 1]));
			tr = (std::max)(tr, std::abs(lows[i] - closes[i - 1]));


		}

		trQueue.push_back(tr);
		trSum += tr;

		if (i >= window) {
			trSum -= trQueue.front();
			trQueue.pop_front();
			atr[i] = trSum / window;
		}
		else if (i == static_cast<size_t>(window) - 1) {
			atr[i] = trSum / window;
		}

	}

	return atr;
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

std::vector<double> calculateSenkouSpanA(const std::vector<double>& tenkanSen, const std::vector<double>& kijunSen) {
	std::vector<double> senkouSpanA(tenkanSen.size(), 0);
	for (size_t i = 0; i < tenkanSen.size(); ++i) {
		senkouSpanA[i] = (tenkanSen[i] + kijunSen[i]) / 2.0;
	}
	// Note: Senkou Span A is plotted 26 periods ahead, so you may need to adjust indices accordingly
	return senkouSpanA;
}

std::vector<double> calculateSenkouSpanB(const std::vector<double>& highs, const std::vector<double>& lows) {
	std::vector<double> senkouSpanB(highs.size(), 0);
	for (size_t i = 51; i < highs.size(); ++i) { // Starting from 52nd period
		auto highIt = std::max_element(highs.begin() + i - 51, highs.begin() + i + 1);
		auto lowIt = std::min_element(lows.begin() + i - 51, lows.begin() + i + 1);
		senkouSpanB[i] = (*highIt + *lowIt) / 2.0;
	}
	// Note: Senkou Span B is plotted 26 periods ahead, so you may need to adjust indices accordingly
	return senkouSpanB;
}

void calculateFibLevels(const std::vector<double>& highs, const std::vector<double>& lows, std::vector<double>& fib_38_2, std::vector<double>& fib_61_8) {
	double high = *std::max_element(highs.begin(), highs.end());
	double low = *std::min_element(lows.begin(), lows.end());
	double range = high - low;

	// Assuming entire price series for simplicity. Adjust as needed.
	for (size_t i = 0; i < highs.size(); ++i) {
		fib_38_2[i] = high - (range * 0.382);
		fib_61_8[i] = high - (range * 0.618);
	}
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


#include <vector>
#include <iostream>

// Assume these functions are defined elsewhere in your code
std::vector<double> calculateATR(const std::vector<double>& highs, const std::vector<double>& lows, const std::vector<double>& closes, int period);
std::vector<double> calculateSMA(const std::vector<double>& data, int period);
std::vector<double> calculateStopLossLevels(const std::vector<double>& entryPrices, const std::vector<double>& atr, double multiplier);
std::vector<double> calculatePositionSizes(const std::vector<double>& atr, double accountBalance, double riskPerTrade);


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
	std::vector<double> atr = calculateATR(highPrices, lowPrices, closePrices, 14);
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
    // Your API key and the function and symbol you want to query
    std::string apiKey = "YOUR_API_KEY"; // Replace with your actual API key
    std::string function = "TIME_SERIES_DAILY";
    std::string symbol = "IBM"; // Example symbol, replace with your desired symbol

    // Fetch the JSON data from Alpha Vantage
    std::string jsonData = fetchDataFromAlphaVantage(apiKey, function, symbol);
    if (jsonData.empty()) {
        std::cerr << "Failed to fetch data from Alpha Vantage." << std::endl;
        return 1;
    }

    // Parse the JSON data to extract high, low, and close prices
    std::vector<double> closePrices, highPrices, lowPrices;
    std::tie(highPrices, lowPrices, closePrices) = parseJsonForPrices(jsonData);

    // Define your trading strategy parameters
    double accountBalance = 10000.0; // Example account balance
    double riskPerTrade = 0.01; // Risk 1% of account balance per trade
    double stopLossMultiplier = 3.0; // Set stop-loss at 3x ATR below the entry price

    // Memoization structures
    std::unordered_map<std::string, double> smaMemo;
    std::unordered_map<std::string, std::vector<double>> atrMemo;
    BollingerBandsMemo bbMemo;
    IchimokuMemo ichimokuMemo;

    // Calculate necessary indicators with memoization
    std::vector<double> sma50 = calculateSMAWithMemoization(closePrices, 50, smaMemo);
    std::vector<double> atr = calculateATRWithMemoization(highPrices, lowPrices, closePrices, 14, atrMemo);
    std::vector<double> bbUpper, bbMiddle, bbLower;
    calculateBollingerBandsWithMemoization(closePrices, 20, 2, bbUpper, bbMiddle, bbLower, bbMemo);

    // Determine entry prices based on your trading signals (implementation depends on your strategy)
    std::vector<double> entryPrices; // Populate this based on your buy signals

    // Call the risk management function with all required parameters
    integrateRiskManagement(
        highPrices,
        lowPrices,
        closePrices,
        entryPrices,
        accountBalance,
        riskPerTrade,
        stopLossMultiplier
    );

    // Based on the output of risk management, make final decisions and execute trades
    // (Your trade execution logic here)

    return 0;
}


// int main() {
// 	// Your API key and the function and symbol you want to query
// 	std::string apiKey = "YOUR_API_KEY"; // Replace with your actual API key
// 	std::string function = "TIME_SERIES_DAILY";
// 	std::string symbol = "IBM"; // Example symbol, replace with your desired symbol

// 	// Fetch the JSON data from Alpha Vantage
// 	std::string jsonData = fetchDataFromAlphaVantage(apiKey, function, symbol);
// 	if (jsonData.empty()) {
// 		std::cerr << "Failed to fetch data from Alpha Vantage." << std::endl;
// 		return 1;
// 	}

// 	// Parse the JSON data to extract high, low, and close prices
// 	std::vector<double> closePrices, highPrices, lowPrices;
// 	std::tie(highPrices, lowPrices, closePrices) = parseJsonForPrices(jsonData);

// 	// Calculate necessary indicators and generate trading signals
// 	// (Your indicator calculations and signal generation logic here)

// 	// Define your trading strategy parameters
// 	double accountBalance = 10000.0; // Example account balance
// 	double riskPerTrade = 0.01; // Risk 1% of account balance per trade
// 	double stopLossMultiplier = 3.0; // Set stop-loss at 3x ATR below the entry price

// 	// Determine entry prices based on your trading signals
// 	// (You need to define how you determine entryPrices based on your signals)
// 	std::vector<double> entryPrices; // Populate this based on your buy signals

// 	// Call the risk management function with all required parameters
// 	integrateRiskManagement(
// 		highPrices,
// 		lowPrices,
// 		closePrices,
// 		entryPrices,
// 		accountBalance,
// 		riskPerTrade,
// 		stopLossMultiplier
// 	);

// 	// Based on the output of risk management, make final decisions and execute trades
// 	// (Your trade execution logic here)

// 	return 0;
// }


