cmake_minimum_required(VERSION 3.8)

project("TradingKLX")

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
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

# Find TA-Lib headers and library
find_path(TA_INCLUDE_DIR NAMES ta_libc.h PATHS /usr/include /usr/local/include)
find_library(TA_LIBRARY NAMES ta_lib PATHS /usr/lib /usr/local/lib)

if(NOT TA_INCLUDE_DIR OR NOT TA_LIBRARY)
    message(FATAL_ERROR "TA-Lib not found")
endif()

# Add source to this project's executable
add_executable(TradingKLX "TradingKLX.cpp" "TradingKLX.h" "AlphaVantageAPI.cpp" "JSONParser.cpp" "AlphaVantageAPI.h" "JSONParser.h")

# Include directories for CURL and TA-Lib
include_directories(${CURL_INCLUDE_DIRS} ${TA_INCLUDE_DIR})

# Link libraries to the executable
target_link_libraries(TradingKLX ${CURL_LIBRARIES} nlohmann_json::nlohmann_json ${TA_LIBRARY})
