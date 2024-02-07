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

	// Calculate necessary indicators and generate trading signals
	// (Your indicator calculations and signal generation logic here)

	// Define your trading strategy parameters
	double accountBalance = 10000.0; // Example account balance
	double riskPerTrade = 0.01; // Risk 1% of account balance per trade
	double stopLossMultiplier = 3.0; // Set stop-loss at 3x ATR below the entry price

	// Determine entry prices based on your trading signals
	// (You need to define how you determine entryPrices based on your signals)
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


