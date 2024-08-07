cmake_minimum_required(VERSION 3.10)
project(TradingKLX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add paths to include directories for the MongoDB C++ driver and nlohmann/json
include_directories(
        ${CMAKE_SOURCE_DIR}/mongo-cxx-driver/src
        ${CMAKE_SOURCE_DIR}/third_party/nlohmann_json
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

# Add executable
add_executable(TradingKLX TradingKLX.cpp OandA_API.cpp ConfigManager.cpp DataProcessor.cpp TechnicalIndicators.cpp database_utils.cpp)

# Link libraries
target_link_libraries(TradingKLX ${MONGOCXX_LIBRARIES} ${BSONCXX_LIBRARIES} CURL::libcurl)