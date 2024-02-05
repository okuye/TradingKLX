#include "AlphaVantageAPI.h"
#include <iostream>
#include <curl/curl.h>
#include <string>
#include <cstdlib> // For std::getenv

// Suppress the specific warning for this section of code
#pragma warning(push)
#pragma warning(disable : 26812)

// Callback function to handle data received by libcurl
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Function to fetch data from Alpha Vantage
std::string fetchDataFromAlphaVantage(const std::string& apiKey, const std::string& function, const std::string& symbol) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        // Construct the Alpha Vantage API request URL with the provided parameters
        std::string url = "https://www.alphavantage.co/query?function=" + function + "&symbol=" + symbol + "&apikey=" + apiKey;

        // Set libcurl options
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // Perform the request and check for errors
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }

        // Clean up
        curl_easy_cleanup(curl);
    }
    return readBuffer; // This string contains the API response
}
// Re-enable the warning after this section
#pragma warning(pop)
