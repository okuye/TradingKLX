cmake_minimum_required(VERSION 3.16)
project(TradingKLX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Address Sanitizer for memory debugging
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g")

# Python setup
set(Python3_EXECUTABLE "/app/venv/bin/python")
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

# Find spdlog
find_package(spdlog REQUIRED)

# Find Boost
find_package(Boost 1.70 REQUIRED COMPONENTS filesystem)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    set(BOOST_LIBS ${Boost_LIBRARIES})
else()
    message(FATAL_ERROR "Could not find Boost libraries.")
endif()

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

if(UNIX AND NOT APPLE)
    message(STATUS "Configuring for Ubuntu Linux")
else()
    message(FATAL_ERROR "Unsupported operating system, expected Ubuntu")
endif()

# OpenMP support for parallel processing
find_package(OpenMP REQUIRED)
if(OpenMP_CXX_FOUND)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
else()
    message(WARNING "OpenMP not found. The project will be built without OpenMP support.")
endif()

# Package configuration
find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP REQUIRED jsoncpp)
find_package(CURL REQUIRED)

include_directories(${JSONCPP_INCLUDE_DIRS})
include_directories(${CURL_INCLUDE_DIRS})
include_directories(${Python3_INCLUDE_DIRS})

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
        BacktestManager.cpp
        TradingServerAPI.cpp
        TradingStrategy.cpp
)

add_executable(TradingKLX ${SOURCE_FILES})

target_link_libraries(TradingKLX PRIVATE
        curl
        jsoncpp
        Python3::Python
        ${BOOST_LIBS}
        spdlog::spdlog
        OpenMP::OpenMP_CXX
)

# GoogleTest setup
find_package(GTest REQUIRED)
include_directories(${GTest_INCLUDE_DIRS})

enable_testing()

add_executable(TradingKLX_Test
        TradingStrategyTest.cpp
        TradingStrategy.cpp
        TechnicalIndicators.cpp
)

target_link_libraries(TradingKLX_Test PRIVATE GTest::gtest GTest::gtest_main pthread spdlog::spdlog)

add_test(NAME TradingKLX_Test COMMAND TradingKLX_Test)

configure_file(${CMAKE_SOURCE_DIR}/config.json ${CMAKE_BINARY_DIR}/config.json COPYONLY)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/logs)
