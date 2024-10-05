//
// Created by olakunle kuye on 14/08/2024.
//
#ifdef USE_BOOST_FILESYSTEM
    #include <boost/filesystem.hpp>
    namespace fs = boost::filesystem;
#else
    #include <filesystem>
    namespace fs = std::filesystem;
#endif
#ifndef TRADINGKLX_UTILITIES_H
#define TRADINGKLX_UTILITIES_H
#include <json/json.h>
#include <string>

std::string jsonToString(const Json::Value& jsonValue);
#endif //TRADINGKLX_UTILITIES_H
