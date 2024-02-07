cmake_minimum_required (VERSION 3.8)

project ("TradingKLX")

# Set C++ standard to C++11. Adjust if your project needs a later standard like C++14, C++17, or C++20.
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find the CURL package
find_package(CURL REQUIRED)

if(NOT CURL_FOUND)
    message(FATAL_ERROR "CURL library not found")
else()
    message(STATUS "Found CURL library: ${CURL_LIBRARIES}")
endif()

# Use FetchContent to make nlohmann_json available
include(FetchContent)
FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.10.5 # Specify the desired version, this is an example
)
FetchContent_MakeAvailable(nlohmann_json)

# Add source to this project's executable.
add_executable (TradingKLX "TradingKLX.cpp" "TradingKLX.h" "AlphaVantageAPI.cpp" "JSONParser.cpp" "AlphaVantageAPI.h" "JSONParser.h")

# Include directories and link libraries
include_directories(${CURL_INCLUDE_DIRS})
target_link_libraries(TradingKLX ${CURL_LIBRARIES} nlohmann_json::nlohmann_json)
