#ifndef TRADINGKLX_SLIDINGWINDOW_H
#define TRADINGKLX_SLIDINGWINDOW_H
#include <deque>

class SlidingWindow {
public:
    SlidingWindow(size_t maxSize) : maxSize(maxSize) {}

    void addDataPoint(double newData) {
        if (data.size() >= maxSize) {
            data.pop_front();  // Remove the oldest data point
        }
        data.push_back(newData);  // Add the newest data point
    }

    const std::deque<double>& getData() const {
        return data;
    }

    size_t size() const {
        return data.size();
    }

private:
    std::deque<double> data;  // Store the data points in a deque
    size_t maxSize;           // Maximum number of elements in the sliding window
};
#endif //TRADINGKLX_SLIDINGWINDOW_H