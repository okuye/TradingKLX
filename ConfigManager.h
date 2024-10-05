#ifdef USE_BOOST_FILESYSTEM
    #include <boost/filesystem.hpp>
    namespace fs = boost::filesystem;
#else
    #include <filesystem>
    namespace fs = std::filesystem;
#endif
#pragma once

#include <json/json.h>
#include <string>

class ConfigManager {
public:
    static Json::Value readConfig(const std::string& configFile);
};