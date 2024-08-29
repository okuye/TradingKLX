#ifndef TRADINGKLX_H
#define TRADINGKLX_H

#include <vector>
#include <string>
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
