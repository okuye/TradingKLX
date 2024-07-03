cmake_minimum_required(VERSION 3.8)
project("TradingKLX")

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Use FetchContent to make nlohmann_json available
include(FetchContent)
FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.10.5  # Use the desired version of nlohmann_json
)
FetchContent_MakeAvailable(nlohmann_json)

# Find libcurl
find_package(CURL REQUIRED)
if(NOT CURL_FOUND)
    message(FATAL_ERROR "CURL library not found")
else()
    message(STATUS "Found CURL library: ${CURL_LIBRARIES}")
endif()

# Find and include pkg-config
find_package(PkgConfig REQUIRED)

# Use pkg-config to get the flags for mongocxx and bsoncxx
pkg_check_modules(MONGOCXX REQUIRED libmongocxx)
pkg_check_modules(BSONCXX REQUIRED libbsoncxx)

# Add the include directories for mongocxx and bsoncxx
include_directories(
        ${CURL_INCLUDE_DIRS}
        ${MONGOCXX_INCLUDE_DIRS}
        ${BSONCXX_INCLUDE_DIRS}
)

# Add the library paths for mongocxx and bsoncxx to the linker's search paths
link_directories(
        ${MONGOCXX_LIBRARY_DIRS}
        ${BSONCXX_LIBRARY_DIRS}
)

# Add source to this project's executable
add_executable(TradingKLX
        "TradingKLX.cpp"
        "AlphaVantageAPI.cpp"
        "JSONParser.cpp"
        "DataProcessor.cpp"
        "TechnicalIndicators.cpp"
        "database_utils.cpp"
        "ConfigManager.cpp"
        "OandA_API.cpp"
)

# Link libraries to the executable
target_link_libraries(TradingKLX
        ${CURL_LIBRARIES}
        nlohmann_json::nlohmann_json
        ${MONGOCXX_LIBRARIES}
        ${BSONCXX_LIBRARIES}
)

target_include_directories(TradingKLX
        PRIVATE "/usr/local/include/mongocxx"
)
