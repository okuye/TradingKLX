cmake_minimum_required(VERSION 3.10)
project(TradingKLX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(APPLE)
    message("Configuring for macOS")
elseif(UNIX AND NOT APPLE)
    message("Configuring for Linux")
else()
    message(FATAL_ERROR "Unsupported operating system")
endif()

include(CheckCXXSourceCompiles)
check_cxx_source_compiles("
    #include <filesystem>
    int main() {
        std::filesystem::path p;
        return 0;
    }"
        HAVE_FILESYSTEM
)
if(HAVE_FILESYSTEM)
    message("std::filesystem is available")
else()
    message(FATAL_ERROR "std::filesystem is not available")
endif()

find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP REQUIRED jsoncpp)
find_package(CURL REQUIRED)
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

# Get the numpy include directory
execute_process(
        COMMAND ${Python3_EXECUTABLE} -c "import numpy; print(numpy.get_include())"
        OUTPUT_VARIABLE NUMPY_INCLUDE_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Set paths correctly
include_directories(${JSONCPP_INCLUDE_DIRS})
include_directories(${CURL_INCLUDE_DIRS})
include_directories(${Python3_INCLUDE_DIRS})
include_directories(${NUMPY_INCLUDE_DIR})

link_directories(${JSONCPP_LIBRARY_DIRS})
link_directories(${CURL_LIBRARY_DIRS})
link_directories(${Python3_LIBRARY_DIRS})

set(SOURCE_FILES
        TradingKLX.cpp
        OandA_API.cpp
        ConfigManager.cpp
        DataProcessor.cpp
        TechnicalIndicators.cpp
        AlphaVantageAPI.cpp
        TradingStrategy.cpp
        JSONParser.cpp
        Utilities.cpp
)

add_executable(TradingKLX ${SOURCE_FILES})

target_include_directories(TradingKLX PRIVATE
        ${JSONCPP_INCLUDE_DIRS}
        ${CURL_INCLUDE_DIRS}
        ${Python3_INCLUDE_DIRS}
        ${NUMPY_INCLUDE_DIR}
)

target_link_libraries(TradingKLX PRIVATE
        jsoncpp
        curl
        Python3::Python
)

configure_file(${CMAKE_SOURCE_DIR}/config.json ${CMAKE_BINARY_DIR}/config.json COPYONLY)