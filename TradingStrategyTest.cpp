#include "gtest/gtest.h"
#include "TradingStrategy.h"
#include "TechnicalIndicators.h"
#include "PriceData.h"
#include <vector>
#include <fstream>
#include <string>

// Define a test fixture class for the TradingStrategy tests
class TradingStrategyTest : public ::testing::Test {
protected:
    TradingStrategy* strategy;  // Use pointer to TradingStrategy
    std::vector<PriceData> priceData;
    std::vector<double> closes;
    std::vector<double> highs;
    std::vector<double> lows;
    std::vector<double> tenkanS;
    std::vector<double> kijunS;
    std::vector<double> senkouA;
    std::vector<double> senkouB;
    std::vector<double> lowerBB;
    std::vector<double> upperBB;

    // Set up the test data
    void SetUp() override {
        // Initialize strategy with required parameters
        strategy = new TradingStrategy(10000.0, 0.0015, 1.25);  // Initial balance, risk per trade, stop-loss multiplier

        // Initialize priceData with proper values
        priceData = std::vector<PriceData>{
                PriceData("2023-09-01", 1.12, 1.13, 1.11, 1.12, 1000.0, 1.11, 1.12, 1.10, 1.11, 900.0),
                PriceData("2023-09-02", 1.13, 1.14, 1.12, 1.13, 1200.0, 1.12, 1.13, 1.11, 1.12, 1000.0),
                PriceData("2023-09-03", 1.15, 1.16, 1.14, 1.15, 1500.0, 1.14, 1.15, 1.13, 1.14, 1100.0),
        };

        // Example close prices
        closes = {1.12, 1.13, 1.15};

        // Example high, low, tenkanS, kijunS, senkouA, and senkouB values
        highs = {1.13, 1.14, 1.16};
        lows = {1.11, 1.12, 1.14};
        tenkanS = {1.12, 1.13, 1.14};
        kijunS = {1.12, 1.13, 1.13};
        senkouA = {1.12, 1.13, 1.14};
        senkouB = {1.11, 1.12, 1.13};
        lowerBB = {1.10, 1.11, 1.12};
        upperBB = {1.14, 1.15, 1.16};
    }

    // Clean up after each test
    void TearDown() override {
        delete strategy;  // Clean up dynamically allocated memory
    }
};

// Test for initialization
TEST_F(TradingStrategyTest, InitializationTest) {
    EXPECT_EQ(strategy->getSignals().size(), 0);  // Initially, no signals should be generated
    EXPECT_EQ(strategy->getAccountBalance(), 10000.0);  // Initial account balance should be correct
}

// Test for insufficient data
TEST_F(TradingStrategyTest, InsufficientDataTest) {
    std::vector<TradingSignal> signals = strategy->evaluateSignals(
            priceData, closes, highs, lows, tenkanS, kijunS, senkouA, senkouB, lowerBB, upperBB);

    EXPECT_EQ(signals.size(), 0);  // No signals should be generated with insufficient data
}

TEST_F(TradingStrategyTest, BuySignalTest) {
    strategy->setAccountBalance(10000.0);
    strategy->setRiskPerTrade(0.01);
    strategy->setStopLossMultiplier(2.0);

    // Initialize vectors with at least 52 elements for senkouSpanPeriod
    std::vector<double> closes(53, 1.5);  // 53 elements
    std::vector<double> highs(53, 1.6);
    std::vector<double> lows(53, 1.4);
    std::vector<double> tenkanS(10, 1.5);  // Tenkan-sen uses 9 elements, so 10 is safe
    std::vector<double> kijunS(27, 1.5);   // Kijun-sen uses 26 elements, so 27 is safe
    std::vector<double> senkouA(53, 1.5);  // At least 52 elements
    std::vector<double> senkouB(53, 1.5);
    std::vector<double> lowerBB(53, 1.2);
    std::vector<double> upperBB(53, 1.8);

    // Create priceData with 53 elements to match the size of the other vectors
    std::vector<PriceData> priceData;
    for (size_t i = 0; i < 53; ++i) {
        priceData.emplace_back("2023-09-" + std::to_string(i + 1), 1.5, 1.6, 1.4, 1.5, 1000.0, 1.5, 1.6, 1.4, 1.5, 900.0);
    }

    // Evaluate signals with sufficiently large vectors
    auto signals = strategy->evaluateSignals(priceData, closes, highs, lows, tenkanS, kijunS, senkouA, senkouB, lowerBB, upperBB);

    EXPECT_GT(signals.size(), 0);  // Ensure at least one signal is generated
}







TEST_F(TradingStrategyTest, SellSignalTest) {
    closes.resize(53, 1.12);  // 53 elements, all initialized to 1.12
    highs.resize(53, 1.13);
    lows.resize(53, 1.11);
    tenkanS.resize(10, 1.12);  // At least 9 elements for Tenkan-sen
    kijunS.resize(27, 1.12);   // At least 26 elements for Kijun-sen
    senkouA.resize(53, 1.12);  // At least 52 elements for Senkou Span
    senkouB.resize(53, 1.11);
    lowerBB.resize(53, 1.10);  // Bollinger Bands lower limit
    upperBB.resize(53, 1.14);  // Bollinger Bands upper limit

    // Resize priceData to 53 elements to match the other vectors
    priceData.resize(53);
    for (size_t i = 0; i < 53; ++i) {
        priceData[i] = PriceData("2023-09-" + std::to_string(i + 1), 1.12, 1.13, 1.11, 1.12, 1000.0, 1.11, 1.12, 1.10, 1.11, 900.0);
    }

    // Mimic a previous buy position (as if one was previously triggered)
    TradingSignal buySignal;
    buySignal.buy = true;
    strategy->addSignal(buySignal);

    std::vector<TradingSignal> signals = strategy->evaluateSignals(
            priceData, closes, highs, lows, tenkanS, kijunS, senkouA, senkouB, lowerBB, upperBB);

    EXPECT_GT(signals.size(), 0);  // Ensure at least one signal is generated
    EXPECT_TRUE(signals[0].sell);  // Ensure the signal is a sell signal
}



// Test for edge case: minimal data required for a valid signal
TEST_F(TradingStrategyTest, MinimalDataEdgeCase) {
    priceData.resize(52);  // Ensure we only have exactly 52 data points (boundary condition)
    closes.resize(52);
    highs.resize(52);
    lows.resize(52);
    tenkanS.resize(9);  // Minimal data for Tenkan
    kijunS.resize(26);  // Minimal data for Kijun
    senkouA.resize(52); // Minimal data for Senkou spans
    senkouB.resize(52);
    lowerBB.resize(52);
    upperBB.resize(52);

    std::vector<TradingSignal> signals = strategy->evaluateSignals(
            priceData, closes, highs, lows, tenkanS, kijunS, senkouA, senkouB, lowerBB, upperBB);

    EXPECT_EQ(signals.size(), 0);  // No signal should be generated with minimal data
}
