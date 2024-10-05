# CMakeLists.txt

cmake_minimum_required(VERSION 3.16)
project(TradingKLX)

# Set C++ standard to C++17 and ensure it's required
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Address Sanitizer for memory debugging
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g")

# Suppress CMP0167 warning by setting the policy
if(POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif()

# Python setup
set(Python3_EXECUTABLE "/Users/olakunlekuye/Documents/Dev/KLX/demo/TradingKLX/tradingklx_venv/bin/python")
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

# Find spdlog
find_package(spdlog REQUIRED)

# Boost setup
find_package(Boost 1.70 REQUIRED COMPONENTS filesystem)

# Include Boost directories
include_directories(${Boost_INCLUDE_DIRS})

# Check for std::filesystem support
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
    message(STATUS "std::filesystem is available")
else()
    message(WARNING "std::filesystem is not available, falling back to Boost.Filesystem")
    add_definitions(-DUSE_BOOST_FILESYSTEM)
    message(STATUS "Using Boost.Filesystem")
endif()

# Get NumPy include directory
execute_process(
        COMMAND ${Python3_EXECUTABLE} -c "import numpy; print(numpy.get_include())"
        OUTPUT_VARIABLE NUMPY_INCLUDE_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(NOT NUMPY_INCLUDE_DIR)
    message(FATAL_ERROR "NumPy not found. Please ensure NumPy is installed in the Python environment.")
else()
    message(STATUS "NumPy found at: ${NUMPY_INCLUDE_DIR}")
    include_directories(${NUMPY_INCLUDE_DIR})
endif()

# Platform-specific configuration
if(APPLE)
    message(STATUS "Configuring for macOS")
elseif(UNIX AND NOT APPLE)
    message(STATUS "Configuring for Linux")
else()
    message(FATAL_ERROR "Unsupported operating system")
endif()

# Package configuration
find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP REQUIRED jsoncpp)
find_package(CURL REQUIRED)

# Include directories
include_directories(${JSONCPP_INCLUDE_DIRS})
include_directories(${CURL_INCLUDE_DIRS})
include_directories(${Python3_INCLUDE_DIRS})

# Link directories
link_directories(${JSONCPP_LIBRARY_DIRS})
link_directories(${CURL_LIBRARY_DIRS})
link_directories(${Python3_LIBRARY_DIRS})

# Source files for the main executable
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
        PerformanceAssessor.cpp
)

# Create the main executable
add_executable(TradingKLX ${SOURCE_FILES})

# Include directories in the target
target_include_directories(TradingKLX PRIVATE
        ${JSONCPP_INCLUDE_DIRS}
        ${CURL_INCLUDE_DIRS}
        ${Python3_INCLUDE_DIRS}
        ${NUMPY_INCLUDE_DIR}
        ${Boost_INCLUDE_DIRS}
        include  # Ensure the include directory is added
)

# Link libraries (ensure libraries are linked only once)
target_link_libraries(TradingKLX PRIVATE
        curl
        jsoncpp
        Python3::Python
        Boost::filesystem
        spdlog::spdlog
)

# GoogleTest setup (using precompiled GoogleTest)
find_package(GTest REQUIRED)
include_directories(${GTest_INCLUDE_DIRS})

# Enable testing
enable_testing()

# Test executable for GoogleTest
add_executable(TradingKLX_Test
        TradingStrategyTest.cpp
        TradingStrategy.cpp
        TechnicalIndicators.cpp
)

# Link GoogleTest libraries to the test executable
target_link_libraries(TradingKLX_Test PRIVATE GTest::gtest GTest::gtest_main pthread spdlog::spdlog)

# Add test case for CTest
add_test(NAME TradingKLX_Test COMMAND TradingKLX_Test)

# Copy configuration file to the build directory
configure_file(${CMAKE_SOURCE_DIR}/config.json ${CMAKE_BINARY_DIR}/config.json COPYONLY)

# Create logs directory if it doesn't exist
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/logs)
