cmake_minimum_required (VERSION 3.8)

project ("TradingKLX")

# Set C++ standard to C++11. Adjust if your project needs a later standard like C++14, C++17, or C++20.
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Manually set the path for CURL include directory
set(CURL_INCLUDE_DIR "C:/cygwin64/usr/include")

# Attempt to find the CURL library file
find_library(CURL_LIBRARY NAMES libcurl.dll.a PATHS "C:/cygwin64/lib" NO_DEFAULT_PATH)

if(NOT CURL_LIBRARY)
    message(FATAL_ERROR "CURL library not found")
else()
    message(STATUS "Found CURL library: ${CURL_LIBRARY}")
endif()

find_package(CURL REQUIRED)

# Include directories
include_directories(${CURL_INCLUDE_DIR})

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

# Link libraries
target_link_libraries(TradingKLX ${CURL_LIBRARY} nlohmann_json::nlohmann_json)

# TODO: Add tests and install targets if needed.
