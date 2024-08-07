cmake_minimum_required(VERSION 3.10)
project(TradingKLX)

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add paths to include directories for the MongoDB C++ driver
include_directories(
        ${CMAKE_SOURCE_DIR}/mongo-cxx-driver/src
)

# Find packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(MONGOCXX REQUIRED libmongocxx)
pkg_check_modules(BSONCXX REQUIRED libbsoncxx)

# Include directories from pkg-config
include_directories(${MONGOCXX_INCLUDE_DIRS} ${BSONCXX_INCLUDE_DIRS})

# Link directories
link_directories(${MONGOCXX_LIBRARY_DIRS} ${BSONCXX_LIBRARY_DIRS})

# Find the CURL package
find_package(CURL REQUIRED)

# Find the JsonCpp package
find_package(jsoncpp REQUIRED)

# Add executable
add_executable(TradingKLX
        TradingKLX.cpp
        OandA_API.cpp
        ConfigManager.cpp
        DataProcessor.cpp
        TechnicalIndicators.cpp
        AlphaVantageAPI.cpp
        server_error_code.h
        TradingKLX.h
        TradingStrategy.cpp
        JSONParser.h
        JSONParser.cpp
)

# Link libraries
target_link_libraries(TradingKLX ${MONGOCXX_LIBRARIES} ${BSONCXX_LIBRARIES} CURL::libcurl jsoncpp_lib)