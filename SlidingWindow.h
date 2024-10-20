#ifndef TRADINGKLX_SLIDINGWINDOW_H
#define TRADINGKLX_SLIDINGWINDOW_H

#include <deque>
#include <vector>
#include <stdexcept>
#include <iostream>

class SlidingWindow {
public:
    // Constructor
    SlidingWindow(size_t size) : maxSize(size) {}

    // Method to add a data point to the sliding window
    void addDataPoint(double value) {
        if (window.size() >= maxSize) {
            window.pop_front();  // Remove the oldest element if the size exceeds maxSize
        }
        window.push_back(value);
    }

    // Method to remove the oldest element
    void removeOldest() {
        if (!window.empty()) {
            window.pop_front();
        }
    }

    // Get the current size of the sliding window
    size_t size() const {
        return window.size();
    }

    // Get the maximum size of the sliding window
    size_t getMaxSize() const {
        return maxSize;
    }

    // Get the data as a deque
    const std::deque<double>& getData() const {
        return window;
    }

    // Convert the window data to a vector
    std::vector<double> toVector() const {
        if (window.empty()) {
            throw std::runtime_error("Sliding window is empty, cannot convert to vector");
        }
        return std::vector<double>(window.begin(), window.end());
    }

    double at(size_t index) const {
        if (index >= window.size()) {
            throw std::out_of_range("Index out of range");
        }
        return window[index];
    }

private:
    size_t maxSize;  // Maximum number of elements in the sliding window
    std::deque<double> window;  // Store the data points in a deque
};

#endif // TRADINGKLX_SLIDINGWINDOW_H
