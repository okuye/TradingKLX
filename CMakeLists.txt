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

# Add source to this project's executable
add_executable(TradingKLX "TradingKLX.cpp" "TradingKLX.h" "AlphaVantageAPI.cpp" "JSONParser.cpp" "AlphaVantageAPI.h" "JSONParser.h")

# Include directories for CURL and TA-Lib
include_directories(
    ${CURL_INCLUDE_DIRS}
    /usr/local/opt/ta-lib/include
    "/usr/local/include/mongocxx/v_noabi"
    "/usr/local/include/bsoncxx/v_noabi"
    "/usr/local/Cellar/mongo-cxx-driver/3.9.0/include/bsoncxx/v_noabi/bsoncxx/third_party/mnmlstc/core"  # Include the mnmlstc/core directory for the MongoDB C++ driver
)

# Link libraries to the executable
target_link_libraries(TradingKLX
    ${CURL_LIBRARIES}
    nlohmann_json::nlohmann_json
    /usr/local/opt/ta-lib/lib/libta_lib.dylib  # Directly link against the TA-Lib library using the full path
    # Add MongoDB C++ driver libraries here if they are not auto-detected
)
