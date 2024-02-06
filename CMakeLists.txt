cmake_minimum_required (VERSION 3.8)

project ("TradingKLX")

# Set C++ standard to C++11. Adjust if your project needs a later standard like C++14, C++17, or C++20.
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# If you are using vcpkg, uncomment and set the path to your vcpkg toolchain file
# set(CMAKE_TOOLCHAIN_FILE "/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake")

# Find required packages
find_package(CURL REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# Include directories
include_directories(${CURL_INCLUDE_DIRS})

# Add source to this project's executable.
add_executable (TradingKLX "TradingKLX.cpp" "TradingKLX.h" "AlphaVantageAPI.cpp" "JSONParser.cpp" "AlphaVantageAPI.h" "JSONParser.h")

# Link libraries
target_link_libraries(TradingKLX ${CURL_LIBRARIES} nlohmann_json::nlohmann_json)

# TODO: Add tests and install targets if needed.
