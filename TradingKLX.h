#ifndef TRADINGKLX_H
#define TRADINGKLX_H

#include <vector>
#include <string>

//// Declares a function to calculate the Simple Moving Average
//std::vector<double> calculateSMA(const std::vector<double>& data, int window);
//
//// Declares a function to calculate rolling maximum or minimum
//std::vector<double> rollingExtremum(const std::vector<double>& data, int window, bool findMax);
//
//// Declares a utility function for calculating Tenkan-sen or Kijun-sen
//std::vector<double> calculateSen(const std::vector<double>& high, const std::vector<double>& low, int period);

#include <memory>
#include <string>

class DataProcessor; // Forward declaration
class ConfigManager; // Forward declaration

class TradingKLX {
public:
    TradingKLX();
    ~TradingKLX();

    void Run(const std::string& configFilePath);

private:
    std::shared_ptr<DataProcessor> dataProcessor;
    std::shared_ptr<ConfigManager> configManager;

    void InitializeAndProcessData(const std::string& configFilePath);
};
#endif // TRADINGKLX_H
