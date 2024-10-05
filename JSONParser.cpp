#ifdef USE_BOOST_FILESYSTEM
    #include <boost/filesystem.hpp>
    namespace fs = boost::filesystem;
#else
    #include <filesystem>
    namespace fs = std::filesystem;
#endif

#include "JSONParser.h"
#include <iostream>  // For debugging purposes
#include <sstream>   // For std::istringstream
#include <algorithm>  // For std::remove_if
#include <cctype>     // For std::isspace

// Extracts a numeric value from the given JSON data for a specified key
std::optional<double> JSONParser::extractValue(const Json::Value& data, const std::string& key) {
    if (data.isMember(key) && data[key].isNumeric()) {
        return data[key].asDouble();
    } else {
        std::cerr << "Missing or invalid key: " << key << std::endl;
        return std::nullopt;
    }
}

// Validates if the JSON contains a key and matches the expected type
bool JSONParser::validateJson(const Json::Value& j, const std::string& key, Json::ValueType expectedType) {
    return j.isMember(key) && j[key].type() == expectedType;
}

// Trim function to remove leading/trailing whitespaces or non-visible characters
std::string trim(const std::string& str) {
    std::string result = str;
    result.erase(result.begin(), std::find_if(result.begin(), result.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    result.erase(std::find_if(result.rbegin(), result.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), result.end());
    return result;
}


std::tuple<std::vector<double>, std::vector<double>, std::vector<double>> JSONParser::parseJsonForPrices(
        const std::string& jsonData,
        const std::string& timeSeriesKey,
        const std::vector<std::string>& fields
) {
    std::vector<double> openPrices;
    std::vector<double> highPrices;
    std::vector<double> lowPrices;

    try {
        std::cout << "Parsing JSON data..." << std::endl;
        Json::CharReaderBuilder readerBuilder;
        Json::Value json;
        std::istringstream s(jsonData);
        std::string errs;

        if (Json::parseFromStream(readerBuilder, s, &json, &errs)) {
            std::cout << "JSON parsed successfully." << std::endl;

            // Check for API rate limit response
            if (json.isMember("Information")) {
                std::cerr << "API Error: " << json["Information"].asString() << std::endl;
                return std::make_tuple(openPrices, highPrices, lowPrices);  // Return empty vectors
            }

            // Check if the key for time series exists
            if (json.isMember(timeSeriesKey)) {
                const Json::Value& timeSeries = json[timeSeriesKey];
                for (const auto& time : timeSeries.getMemberNames()) {
                    const Json::Value& data = timeSeries[time];
                    if (data.isObject()) {
                        if (fields.size() >= 1 && validateJson(data, fields[0], Json::stringValue)) {
                            openPrices.push_back(std::stod(data[fields[0]].asString()));
                        }
                        if (fields.size() >= 2 && validateJson(data, fields[1], Json::stringValue)) {
                            highPrices.push_back(std::stod(data[fields[1]].asString()));
                        }
                        if (fields.size() >= 3 && validateJson(data, fields[2], Json::stringValue)) {
                            lowPrices.push_back(std::stod(data[fields[2]].asString()));
                        }
                    }
                }
            } else {
                std::cerr << "Key '" << timeSeriesKey << "' not found in JSON." << std::endl;
            }
        } else {
            std::cerr << "Error parsing JSON data: " << errs << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception while parsing JSON: " << e.what() << std::endl;
    }

    return std::make_tuple(openPrices, highPrices, lowPrices);
}

void JSONParser::printJsonKeys(const Json::Value& json) {
    for (const auto& key : json.getMemberNames()) {
        std::cout << key << ", ";
    }
    std::cout << std::endl;
}