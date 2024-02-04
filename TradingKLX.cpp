// TradingKLX.cpp : Defines the entry point for the application.
//
using namespace std;
#include "TradingKLX.h"
#include <curl/curl.h>




#include <iostream>
#include <vector>
#include <algorithm> // For std::max_element, std::min_element, std::transform
#include <deque>     // For std::deque

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

int main() {
	// Example data
	std::vector<double> high = { 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8 };
	std::vector<double> low = { 0.9, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6 };
	std::vector<double> close = { 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7 };

	// Calculate Tenkan-sen (Conversion Line)
	std::vector<double> tenkan_sen = calculateSen(high, low, 9);

	// Calculate Kijun-sen (Base Line)
	std::vector<double> kijun_sen = calculateSen(high, low, 26);

	// Calculate SMA 50 and SMA 200
	std::vector<double> sma_50 = calculateSMA(close, 50);
	std::vector<double> sma_200 = calculateSMA(close, 200);

	// Output some results to verify
	std::cout << "Tenkan-sen: ";
	for (const auto& value : tenkan_sen) {
		std::cout << value << " ";
	}
	std::cout << "\nKijun-sen: ";
	for (const auto& value : kijun_sen) {
		std::cout << value << " ";
	}
	std::cout << std::endl;

	// Continue implementing other indicators as needed...

	return 0;
}
