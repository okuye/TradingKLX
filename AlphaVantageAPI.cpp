#include "AlphaVantageAPI.h"
#include <iostream> // Include necessary headers
#include <curl/curl.h>
#include <chrono>
#include <thread>

// Constructor implementation
AlphaVantageAPI::AlphaVantageAPI(const std::string& apiKey, long timeoutSeconds)
        : apiKey(apiKey), timeoutSeconds(timeoutSeconds) {
}

// Function to build the URL for the API request
std::string AlphaVantageAPI::buildURL(const std::string& function, const std::string& from_symbol, const std::string& to_symbol, const std::string& interval) {
    std::string url = "https://www.alphavantage.co/query?function=" + function +
                      "&from_symbol=" + from_symbol + "&to_symbol=" + to_symbol +
                      "&interval=" + interval + "&apikey=" + apiKey;
    return url;
}

// Function to fetch data from the API
std::string AlphaVantageAPI::fetchData(const std::string& function, const std::string& from_symbol, const std::string& to_symbol, const std::string& interval) {
    std::string url = buildURL(function, from_symbol, to_symbol, interval);
    return fetchWithRetry(url);
}

// Helper function to handle rate-limited requests with exponential backoff
std::string AlphaVantageAPI::fetchWithRetry(const std::string& url, int attempts) {
    CURL* curl;
    CURLcode res;
    std::string response;

    for (int attempt = 1; attempt <= attempts; ++attempt) {
        curl = curl_easy_init();
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeoutSeconds);

            // Lambda function to handle response data
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, [](void* contents, size_t size, size_t nmemb, std::string* s) -> size_t {
                size_t totalSize = size * nmemb;
                s->append(static_cast<char*>(contents), totalSize);
                return totalSize;
            });

            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
            res = curl_easy_perform(curl);

            if (res == CURLE_OK) {
                curl_easy_cleanup(curl);
                return response;
            }

            std::cerr << "Attempt " << attempt << " failed: " << curl_easy_strerror(res) << std::endl;
            curl_easy_cleanup(curl);

            // Exponential backoff
            std::this_thread::sleep_for(std::chrono::seconds(1 << attempt));
        }
    }

    return "Failed to fetch data after " + std::to_string(attempts) + " attempts";
}
