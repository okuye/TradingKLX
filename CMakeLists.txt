# CMakeList.txt : CMake project for TradingKLX, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("TradingKLX")

# Manually set the paths for CURL include directory and library
set(CURL_INCLUDE_DIR "C:/Users/User/Documents/GitHub/vcpkg/installed/x64-windows/include")
set(CURL_LIBRARY "C:/Users/User/Documents/GitHub/vcpkg/installed/x64-windows/lib/libcurl.lib")

# Find the libcurl package
find_package(CURL REQUIRED)

# Include libcurl headers
include_directories(${CURL_INCLUDE_DIRS})

# Add source to this project's executable.
# Make sure TradingKLX.cpp and TradingKLX.h are present in your project directory
add_executable (TradingKLX "TradingKLX.cpp" "TradingKLX.h")

# Link against libcurl
target_link_libraries(TradingKLX ${CURL_LIBRARIES})

# TODO: Add tests and install targets if needed.
