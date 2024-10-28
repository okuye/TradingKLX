#ifdef USE_BOOST_FILESYSTEM
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
#else
#include <filesystem>
namespace fs = std::filesystem;
#endif

#ifndef TRADINGKLX_H
#define TRADINGKLX_H

#include <vector>
#include <string>
#include <memory>
#include <json/json.h>
#include "TradingStrategy.h"
#include "PerformanceAssessor.h"
#include "DataProcessor.h"
#include "ConfigManager.h"

class TradingKLX {
public:
    TradingKLX();  // Constructor
    ~TradingKLX(); // Destructor

    void InitializeAndProcessData(const std::string& configFilePath);
    void Run(const std::string& configFilePath);

private:
    TradingStrategy strategy;
    PerformanceAssessor assessor;
    std::shared_ptr<DataProcessor> dataProcessor;
    std::shared_ptr<ConfigManager> configManager;
};

#endif // TRADINGKLX_H
