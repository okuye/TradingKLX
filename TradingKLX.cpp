#include <iostream>
#include <vector>
#include <fstream>
#include "TechnicalIndicators.h"
#include "TradingStrategy.h"
#include "PriceData.h"
#include "PerformanceAssessor.h"
#include <sstream>
#include <algorithm> // For std::min

// Function to load historical data
std::vector<PriceData> loadHistoricalData(const std::string& askFile, const std::string& bidFile) {
    std::vector<PriceData> priceData;
    std::ifstream askFileStream(askFile);
    std::ifstream bidFileStream(bidFile);

    if (!askFileStream.is_open()) {
        throw std::runtime_error("Could not open Ask file: " + askFile);
    }
    if (!bidFileStream.is_open()) {
        throw std::runtime_error("Could not open Bid file: " + bidFile);
    }

    std::string lineAsk, lineBid;
    std::getline(askFileStream, lineAsk);  // Skip header
    std::getline(bidFileStream, lineBid);  // Skip header

    bool askHasNext = (bool)std::getline(askFileStream, lineAsk);
    bool bidHasNext = (bool)std::getline(bidFileStream, lineBid);

    while (askHasNext && bidHasNext) {
        std::stringstream ssAsk(lineAsk);
        std::stringstream ssBid(lineBid);
        std::string tempAsk, tempBid, timestampAsk, timestampBid;

        std::getline(ssAsk, timestampAsk, ',');
        std::getline(ssBid, timestampBid, ',');

        if (timestampAsk < timestampBid) {
            askHasNext = (bool)std::getline(askFileStream, lineAsk);
            continue;
        } else if (timestampAsk > timestampBid) {
            bidHasNext = (bool)std::getline(bidFileStream, lineBid);
            continue;
        }

        try {
            std::getline(ssAsk, tempAsk, ',');
            double askOpen = std::stod(tempAsk);
            std::getline(ssAsk, tempAsk, ',');
            double askHigh = std::stod(tempAsk);
            std::getline(ssAsk, tempAsk, ',');
            double askLow = std::stod(tempAsk);
            std::getline(ssAsk, tempAsk, ',');
            double askClose = std::stod(tempAsk);
            std::getline(ssAsk, tempAsk, ',');
            double askVolume = std::stod(tempAsk);

            std::getline(ssBid, tempBid, ',');
            double bidOpen = std::stod(tempBid);
            std::getline(ssBid, tempBid, ',');
            double bidHigh = std::stod(tempBid);
            std::getline(ssBid, tempBid, ',');
            double bidLow = std::stod(tempBid);
            std::getline(ssBid, tempBid, ',');
            double bidClose = std::stod(tempBid);
            std::getline(ssBid, tempBid, ',');
            double bidVolume = std::stod(tempBid);

            priceData.emplace_back(timestampAsk, askOpen, askHigh, askLow, askClose, askVolume,
                                   bidOpen, bidHigh, bidLow, bidClose, bidVolume);

        } catch (const std::invalid_argument& e) {
            std::cerr << "Invalid data at row (Ask or Bid): " << lineAsk << " or " << lineBid << "\nError: " << e.what() << std::endl;
        }

        askHasNext = (bool)std::getline(askFileStream, lineAsk);
        bidHasNext = (bool)std::getline(bidFileStream, lineBid);
    }

    askFileStream.close();
    bidFileStream.close();
    return priceData;
}

// Function to process market data and extract highs, lows, and closes
void processMarketData(const std::vector<PriceData> &priceData,
                       std::vector<double> &askHighs,
                       std::vector<double> &askLows,
                       std::vector<double> &askCloses,
                       std::vector<double> &bidHighs,
                       std::vector<double> &bidLows,
                       std::vector<double> &bidCloses) {
    for (const auto &data : priceData) {
        askHighs.push_back(data.askHigh);
        askLows.push_back(data.askLow);
        askCloses.push_back(data.askClose);

        bidHighs.push_back(data.bidHigh);
        bidLows.push_back(data.bidLow);
        bidCloses.push_back(data.bidClose);
    }
}

// Function to log signals
void logSignals(const std::vector<TradingSignal> &signals) {
    for (const auto &signal : signals) {
        if (signal.buy) {
            std::cout << "Buy signal at index: " << signal.index << ", Position Size: " << signal.positionSize
                      << ", Stop Loss: " << signal.stopLossLevel << std::endl;
        } else if (signal.sell) {
            std::cout << "Sell signal at index: " << signal.index << ", Profit: " << signal.profit << std::endl;
        }
    }
}

// Function to calculate Ichimoku indicators
void calculateIchimokuIndicators(const std::vector<double>& highs, const std::vector<double>& lows,
                                 std::vector<double>& tenkanS, std::vector<double>& kijunS,
                                 std::vector<double>& senkouA, std::vector<double>& senkouB,
                                 TechnicalIndicators& indicators, IchimokuMemo& ichimokuMemo) {
    for (size_t i = 0; i < highs.size(); ++i) {
        tenkanS.push_back(indicators.calculateTenkanSen(highs, lows, 9, i, ichimokuMemo));
        kijunS.push_back(indicators.calculateKijunSen(highs, lows, 26, i, ichimokuMemo));
        senkouA.push_back(indicators.calculateSenkouSpanA(i, ichimokuMemo));

        if (i >= 51) {
            senkouB.push_back(indicators.calculateSenkouSpanB(highs, lows, i, ichimokuMemo));
        }
    }
}

// Function to calculate Bollinger Bands
void calculateBollingerBands(const std::vector<double>& closes, std::vector<double>& lowerBB,
                             std::vector<double>& upperBB, TechnicalIndicators& indicators,
                             BollingerBandsMemo& bbMemo) {
    for (size_t i = 0; i < closes.size(); ++i) {
        if (i >= 20) {
            auto bands = indicators.calculateBollingerBandsWithMemoization(closes, 20, 2, bbMemo);
            lowerBB.push_back(bands.first);
            upperBB.push_back(bands.second);
        } else {
            lowerBB.push_back(0.0);
            upperBB.push_back(0.0);
        }
    }
}

// Walk-forward optimization function
void walkForwardOptimization(TradingStrategy& strategy, const std::vector<PriceData>& priceData, size_t walkForwardPeriod) {
    for (size_t start = 0; start < priceData.size(); start += walkForwardPeriod) {
        size_t end = std::min(start + walkForwardPeriod, priceData.size());
        std::vector<PriceData> walkForwardData(priceData.begin() + start, priceData.begin() + end);

        std::vector<double> askHighs, askLows, askCloses, bidHighs, bidLows, bidCloses;
        processMarketData(walkForwardData, askHighs, askLows, askCloses, bidHighs, bidLows, bidCloses);

        IchimokuMemo ichimokuMemo;
        BollingerBandsMemo bbMemo;

        std::vector<double> tenkanS, kijunS, senkouA, senkouB, lowerBB, upperBB;

        TechnicalIndicators indicators;
        calculateIchimokuIndicators(askHighs, askLows, tenkanS, kijunS, senkouA, senkouB, indicators, ichimokuMemo);
        calculateBollingerBands(bidCloses, lowerBB, upperBB, indicators, bbMemo);

        std::vector<TradingSignal> signals = strategy.evaluateSignals(walkForwardData, askCloses, askHighs, askLows,
                                                                      tenkanS, kijunS, senkouA, senkouB, lowerBB, upperBB);

        logSignals(signals);
    }
}

int main() {
    std::vector<PriceData> priceData = loadHistoricalData("/Users/olakunlekuye/Documents/Dev/KLX/demo/TradingKLX/EURUSD_Candlestick_1_Hour_ASK_01.09.2023-07.09.2024.csv",
                                                          "/Users/olakunlekuye/Documents/Dev/KLX/demo/TradingKLX/EURUSD_Candlestick_1_Hour_BID_01.01.2023-07.09.2024.csv");

    TradingStrategy strategy(10000.0, 0.0015, 1.25); // $10,000 starting balance, 0.15% risk, stop-loss multiplier 1.25

    size_t walkForwardPeriod = 1000;  // Define the walk-forward optimization period
    walkForwardOptimization(strategy, priceData, walkForwardPeriod);

    std::vector<double> portfolioValues;
    double currentBalance = strategy.getAccountBalance();
    portfolioValues.push_back(currentBalance);

     // Performance assessment calculations
    double totalReturn = PerformanceAssessor::calculateTotalReturn(portfolioValues, 10000.0);
    double maxDrawdown = PerformanceAssessor::calculateMaxDrawdown(portfolioValues);
    double winLossRatio = PerformanceAssessor::calculateWinLossRatio(strategy.getSignals());
    double avgProfit = PerformanceAssessor::calculateAverageProfit(strategy.getSignals());

    // Generate portfolio returns for Sharpe Ratio calculation
    std::vector<double> portfolioReturns;
    for (size_t i = 1; i < portfolioValues.size(); ++i) {
        double returnRate = (portfolioValues[i] - portfolioValues[i - 1]) / portfolioValues[i - 1];
        portfolioReturns.push_back(returnRate);
    }

    double sharpeRatio = PerformanceAssessor::calculateSharpeRatio(portfolioReturns);

    // Output the performance results
    std::cout << "Total Return: " << totalReturn << "%\n";
    std::cout << "Max Drawdown: " << maxDrawdown << "%\n";
    std::cout << "Win/Loss Ratio: " << winLossRatio << "\n";
    std::cout << "Average Profit per Trade: " << avgProfit << "\n";
    std::cout << "Sharpe Ratio: " << sharpeRatio << "\n";

    return 0;
}
