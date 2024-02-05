#ifndef TRADINGKLX_H
#define TRADINGKLX_H

#include <vector>
#include <string>

// Declares a function to calculate the Simple Moving Average
std::vector<double> calculateSMA(const std::vector<double>& data, int window);

// Declares a function to calculate rolling maximum or minimum
std::vector<double> rollingExtremum(const std::vector<double>& data, int window, bool findMax);

// Declares a utility function for calculating Tenkan-sen or Kijun-sen
std::vector<double> calculateSen(const std::vector<double>& high, const std::vector<double>& low, int period);

#endif // TRADINGKLX_H
