#include "AlphaVantageAPI.h"
#include <iostream>
#include <curl/curl.h>
#include <chrono>
#include <thread>
#include <json/json.h>
#include <fstream>
#include <sstream>

// Function to handle received data from CURL
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
    size_t totalSize = size * nmemb;
    if (s == nullptr) return 0;
    s->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

// Constructor
AlphaVantageAPI::AlphaVantageAPI(const std::string& apiKey, long timeoutSeconds)
        : apiKey(apiKey), timeoutSeconds(timeoutSeconds) {}

// Build URL
std::string AlphaVantageAPI::buildURL(const std::string& function, const std::string& from_symbol, const std::string& to_symbol) {
    return "https://www.alphavantage.co/query?function=" + function +
           "&from_symbol=" + from_symbol + "&to_symbol=" + to_symbol +
           "&outputsize=full&apikey=" + apiKey;
}

#include <unistd.h> // for getcwd()

void AlphaVantageAPI::saveDataLocally(const std::string& data) {
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    std::string workingDirectory(cwd);

    std::string filename = workingDirectory + "/advantage-full-json-data.json";
    std::ofstream file(filename);
    if (file.is_open()) {
        file << data;
        file.close();
        std::cout << "Data saved to " << filename << std::endl;
    } else {
        std::cerr << "Unable to open file: " << filename << std::endl;
    }
}

// Load data from the JSON file
std::string AlphaVantageAPI::loadLocalData() {
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    std::string workingDirectory(cwd);

    std::string filename = workingDirectory + "/advantage-full-json-data.json";
    std::ifstream file(filename);
    std::stringstream buffer;
    if (file.is_open()) {
        buffer << file.rdbuf();
        file.close();
        return buffer.str();
    }
    std::cerr << "Unable to open file: " << filename << std::endl;
    return "";
}

// Fetch data with retry
std::string AlphaVantageAPI::fetchWithRetry(const std::string& url, int attempts) {
    CURL* curl;
    CURLcode res;
    std::string response;
    long http_code = 0;

    for (int attempt = 1; attempt <= attempts; ++attempt) {
        curl = curl_easy_init();
        if (curl) {
            std::cout << "Attempting to fetch URL: " << url << " (Attempt " << attempt << ")\n";

            // Set CURL options
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeoutSeconds);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
            curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1); // Enforce HTTP/1.1

            res = curl_easy_perform(curl);

            // Get HTTP response code
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

            if (res == CURLE_OK && http_code == 200) {
                curl_easy_cleanup(curl);
                return response;  // Successful fetch
            } else {
                std::cerr << "Attempt " << attempt << " failed: "
                          << curl_easy_strerror(res)
                          << " (HTTP code: " << http_code << ")" << std::endl;
            }

            curl_easy_cleanup(curl);

            // Exponential backoff before retrying
            std::this_thread::sleep_for(std::chrono::seconds(1 << attempt));
        } else {
            std::cerr << "Failed to initialize CURL." << std::endl;
            break;
        }
    }

    // Return error after all retry attempts failed
    return "Failed to fetch data after " + std::to_string(attempts) + " attempts.";
}

// Fetch data, with a check for local storage
std::string AlphaVantageAPI::fetchData(const std::string& function, const std::string& from_symbol,
                                       const std::string& to_symbol) {
    std::string localData = loadLocalData();

    // If the local data file contains matching data, use it
    if (!localData.empty()) {
        // Optionally, you could parse the local file and check its metadata
        // to ensure it's for the correct symbols (EUR/USD, etc.)
        std::cout << "Using local data from advantage-full-json-data.json" << std::endl;
        return localData;
    }

    // Otherwise, fetch from API
    std::string url = buildURL(function, from_symbol, to_symbol);
    std::string response = fetchWithRetry(url, 5); // Use default retry attempts (5)

    // Save data locally, overwriting existing file
    saveDataLocally(response);

    return response;
}