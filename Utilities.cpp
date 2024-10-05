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
#include "Utilities.h"

std::string jsonToString(const Json::Value& jsonValue) {
    Json::StreamWriterBuilder writer;
    return Json::writeString(writer, jsonValue);
}