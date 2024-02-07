#include "AlphaVantageAPI.h"
#include <string>
#include <iostream>
#include <curl/curl.h>
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
std::string fetchDataFromAlphaVantage(const std::string& apiKey, const std::string& function, const std::string& from_symbol, const std::string& to_symbol) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        std::string url = "https://www.alphavantage.co/query?function=" + function + "&from_symbol=" + from_symbol + "&to_symbol=" + to_symbol + "&interval=5min&apikey=" + apiKey;
        
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);
    }
    return readBuffer;
}
// Re-enable the warning after this section
#pragma warning(pop)
