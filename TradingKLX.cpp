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
#include <algorithm> // For std::max_element, std::min_element, std::transform
#include <deque>     // For std::deque
#include <numeric>  // Include for std::accumulate and std::inner_product
#include <cmath>    // Include for std::sqrt
#include <algorithm>  // Include for std::max and other algorithm functions


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

//std::vector<double> calculateRollingStd(const std::vector<double>& data, int window) {
//	std::vector<double> rollingStd;
//	std::deque<double> windowElements;
//	for (size_t i = 0; i < data.size(); ++i) {
//		if (i >= window - 1) {
//			// Calculate standard deviation for elements in windowElements
//			double sum = std::accumulate(windowElements.begin(), windowElements.end(), 0.0);
//			double mean = sum / windowElements.size();
//			double sq_sum = std::inner_product(windowElements.begin(), windowElements.end(), windowElements.begin(), 0.0, std::plus<double>(), [mean](double a, double b) { return (a - mean) * (b - mean); });
//			double std = std::sqrt(sq_sum / windowElements.size());
//			rollingStd.push_back(std);
//		}
//		windowElements.push_back(data[i]);
//		if (windowElements.size() > window) {
//			windowElements.pop_front();
//		}
//	}
//	return rollingStd;
//}

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

std::vector<double> calculateATR(const std::vector<double>& highs, const std::vector<double>& lows, const std::vector<double>& closes, int window) {
	std::vector<double> atr(highs.size());
	std::deque<double> trQueue; // Queue to hold the true ranges for the moving average calculation
	double trSum = 0; // Sum of the true ranges in the queue

	for (size_t i = 0; i < highs.size(); ++i) {
		double tr = highs[i] - lows[i]; // Basic true range calculation
		if (i > 0) {
			tr = (std::max)(tr, std::abs(highs[i] - closes[i - 1])); // Compare with the previous close using std::max and std::abs
			tr = (std::max)(tr, std::abs(lows[i] - closes[i - 1]));  // Again, using std::max and std::abs
		}

		trQueue.push_back(tr);
		trSum += tr;

		if (i >= window) {
			trSum -= trQueue.front(); // Remove the oldest true range from the sum
			trQueue.pop_front();
			atr[i] = trSum / window; // Calculate the ATR as the moving average of the true range
		}
		else if (i == window - 1) {
			atr[i] = trSum / window; // First ATR value (simple average of the first 'window' true ranges)
		}
		// For indices less than 'window - 1', ATR is undefined or could be set to a placeholder value
	}

	return atr;
}
std::vector<double> calculateRSI(const std::vector<double>& closes, int window) {
	std::vector<double> rsi(closes.size(), 0); // Initialize RSI values to 0

	double gainSum = 0, lossSum = 0;
	// Calculate initial average gain and loss
	for (int i = 1; i <= window; ++i) {
		double delta = closes[i] - closes[i - 1];
		if (delta > 0) gainSum += delta; // Gain
		else lossSum -= delta; // Loss (as a positive value)
	}

	double avgGain = gainSum / window;
	double avgLoss = lossSum / window;

	// Calculate RSI starting from 'window + 1'
	for (size_t i = window + 1; i < closes.size(); ++i) {
		double delta = closes[i] - closes[i - 1];
		double gain = delta > 0 ? delta : 0;
		double loss = delta < 0 ? -delta : 0;

		// Apply smoothing factor
		avgGain = (avgGain * (window - 1) + gain) / window;
		avgLoss = (avgLoss * (window - 1) + loss) / window;

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
		if (i >= window - 1) {
			double sum = accumulate(windowElements.begin(), windowElements.end(), 0.0);
			double mean = sum / windowElements.size();
			double sq_sum = inner_product(windowElements.begin(), windowElements.end(), windowElements.begin(), 0.0, plus<double>(), [mean](double a, double b) { return (a - mean) * (b - mean); });
			double std = sqrt(sq_sum / windowElements.size());
			rollingStd.push_back(std);
		}
		windowElements.push_back(data[i]);
		if (windowElements.size() > window) {
			windowElements.pop_front();
		}
	}
	return rollingStd;
}



int main() {
	// Your API key and the function and symbol you want to query
	std::string apiKey = "YOUR_API_KEY"; // Make sure to replace this with your actual API key
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
	std::tie(highPrices, lowPrices, closePrices) = parseJsonForPrices(jsonData); // Ensure this function is implemented to return these vectors

	// Calculate necessary indicators
	std::vector<double> tenkan_sen = calculateSen(highPrices, lowPrices, 9);
	std::vector<double> kijun_sen = calculateSen(highPrices, lowPrices, 26);
	std::vector<double> sma_50 = calculateSMA(closePrices, 50);
	std::vector<double> sma_200 = calculateSMA(closePrices, 200);
	std::vector<double> rsi = calculateRSI(closePrices, 14); // Assuming a 14-day period for RSI
	// Assume functions for Senkou Span A, Senkou Span B, Bollinger Bands, and Fibonacci Levels are also called here

	// Calculate Buy and Sell Signals
	std::vector<bool> buySignals;
	std::vector<bool> sellSignals;
	// Implement the logic for buy and sell signals here as shown in the previous response

	// Output the results and signals
	for (size_t i = 0; i < closePrices.size(); ++i) {
		if (buySignals[i]) {
			std::cout << "Buy Signal at index " << i << " - Price: " << closePrices[i] << std::endl;
		}
		if (sellSignals[i]) {
			std::cout << "Sell Signal at index " << i << " - Price: " << closePrices[i] << std::endl;
		}
	}

	return 0;
}

//
//int main() {
//	// Your API key and the function and symbol you want to query
//	std::string apiKey = "YOUR_API_KEY"; // Make sure to replace this with your actual API key
//	std::string function = "TIME_SERIES_DAILY";
//	std::string symbol = "IBM"; // Example symbol, replace with your desired symbol
//
//	// Fetch the JSON data from Alpha Vantage
//	std::string jsonData = fetchDataFromAlphaVantage(apiKey, function, symbol);
//
//	if (jsonData.empty()) {
//		std::cerr << "Failed to fetch data from Alpha Vantage." << std::endl;
//		return 1;
//	}
//
//	// Assuming parseJsonForClosePrices is modified to also return high and low prices
//	std::vector<double> closePrices, highPrices, lowPrices;
//	std::tie(highPrices, lowPrices, closePrices) = parseJsonForPrices(jsonData); // Modify your JSON parser accordingly
//
//	// Calculate Tenkan-sen (Conversion Line) using high and low prices
//	std::vector<double> tenkan_sen = calculateSen(highPrices, lowPrices, 9);
//
//	// Calculate Kijun-sen (Base Line) using high and low prices
//	std::vector<double> kijun_sen = calculateSen(highPrices, lowPrices, 26);
//
//	// Calculate SMA 50 and SMA 200 using close prices
//	std::vector<double> sma_50 = calculateSMA(closePrices, 50);
//	std::vector<double> sma_200 = calculateSMA(closePrices, 200);
//
//	// Output some results to verify
//	std::cout << "Tenkan-sen: ";
//	for (const auto& value : tenkan_sen) {
//		std::cout << value << " ";
//	}
//	std::cout << "\nKijun-sen: ";
//	for (const auto& value : kijun_sen) {
//		std::cout << value << " ";
//	}
//	std::cout << std::endl;
//
//	// Continue implementing other indicators as needed...
//
//	return 0;
//}

