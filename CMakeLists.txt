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

if(CYGWIN)
  message(STATUS "Configuring on/for Cygwin")
  find_path(CURL_INCLUDE_DIR NAMES curl/curl.h HINTS /usr/include)
  find_library(CURL_LIBRARY NAMES curl HINTS /usr/lib)
endif()

# Platform-specific handling for finding libcurl
if(WIN32)
  message(STATUS "Configuring on/for Windows")

  # Use an environment variable CURL_DIR to specify the root directory of CURL if needed
  if(DEFINED ENV{CURL_DIR})
    set(CURL_INCLUDE_DIR "$ENV{CURL_DIR}/include")
    find_library(CURL_LIBRARY NAMES libcurl libcurl_imp PATHS "$ENV{CURL_DIR}/lib")
  else()
    find_path(CURL_INCLUDE_DIR NAMES curl/curl.h)
    find_library(CURL_LIBRARY NAMES libcurl libcurl_imp)
  endif()

  # Verify the found paths
  if(NOT EXISTS "${CURL_INCLUDE_DIR}")
    message(FATAL_ERROR "CURL include directory not found: ${CURL_INCLUDE_DIR}")
  endif()
  if(NOT EXISTS "${CURL_LIBRARY}")
    message(FATAL_ERROR "CURL library not found: ${CURL_LIBRARY}")
  endif()

elseif(APPLE)
  message(STATUS "Configuring on/for macOS")
  # macOS specific paths or variables can be set here

elseif(UNIX AND NOT APPLE)
  message(STATUS "Configuring on/for Linux")
  # Linux specific paths or variables can be set here

endif()

# Attempt to find the CURL package
find_package(CURL REQUIRED)

if(NOT CURL_FOUND)
    message(FATAL_ERROR "CURL library not found")
else()
    message(STATUS "Found CURL library: ${CURL_LIBRARIES}")
endif()

# Add source to this project's executable
add_executable(TradingKLX "TradingKLX.cpp" "TradingKLX.h" "AlphaVantageAPI.cpp" "JSONParser.cpp" "AlphaVantageAPI.h" "JSONParser.h")

# Include directories and link libraries
include_directories(${CURL_INCLUDE_DIRS})
target_link_libraries(TradingKLX ${CURL_LIBRARIES} nlohmann_json::nlohmann_json)
