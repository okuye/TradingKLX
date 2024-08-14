cmake_minimum_required(VERSION 3.10)
project(TradingKLX)

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Determine the operating system and set the compiler accordingly
if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    # macOS
    message(STATUS "Configuring for macOS")

    # Use AppleClang and enforce libc++ as the standard library
    set(CMAKE_C_COMPILER "/usr/bin/clang")
    set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")

    # Ensure the architecture is correctly set (especially important for ARM64 Macs)
    if (CMAKE_OSX_ARCHITECTURES)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -arch ${CMAKE_OSX_ARCHITECTURES}")
    endif()

elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # Ubuntu or other Linux distributions
    message(STATUS "Configuring for Linux")
    set(CMAKE_C_COMPILER "/usr/bin/gcc")
    set(CMAKE_CXX_COMPILER "/usr/bin/g++")
else()
    message(FATAL_ERROR "Unsupported operating system: ${CMAKE_SYSTEM_NAME}")
endif()

# Verify that the chosen compiler supports C++17 and std::filesystem
include(CheckCXXSourceCompiles)
check_cxx_source_compiles("
       #include <filesystem>
       int main() {
           std::filesystem::path p = \"/\";
           return 0;
       }
   " HAVE_FILESYSTEM)

if (HAVE_FILESYSTEM)
    message(STATUS "std::filesystem is available")
else()
    message(FATAL_ERROR "std::filesystem is not available with the current compiler.")
endif()

# Add paths to include directories for the MongoDB C++ driver
include_directories(
        ${CMAKE_SOURCE_DIR}/mongo-cxx-driver/src
)

# Find packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(MONGOCXX REQUIRED libmongocxx)
pkg_check_modules(BSONCXX REQUIRED libbsoncxx)
pkg_check_modules(JSONCPP REQUIRED jsoncpp)

# Include directories from pkg-config
include_directories(${MONGOCXX_INCLUDE_DIRS} ${BSONCXX_INCLUDE_DIRS} ${JSONCPP_INCLUDE_DIRS})

# Link directories
link_directories(${MONGOCXX_LIBRARY_DIRS} ${BSONCXX_LIBRARY_DIRS} ${JSONCPP_LIBRARY_DIRS})

# Find the CURL package
find_package(CURL REQUIRED)

# Add executable
add_executable(TradingKLX
        TradingKLX.cpp
        OandA_API.cpp
        ConfigManager.cpp
        DataProcessor.cpp
        TechnicalIndicators.cpp
        AlphaVantageAPI.cpp
        TradingStrategy.cpp
        JSONParser.cpp
        exceptions.hpp
        JsonStructureException.hpp
        Utilities.h
        Utilities.cpp
)

# Link libraries
target_link_libraries(TradingKLX ${MONGOCXX_LIBRARIES} ${BSONCXX_LIBRARIES} CURL::libcurl ${JSONCPP_LIBRARIES})

# Copy config.json to the build directory
add_custom_command(TARGET TradingKLX POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/config.json ${CMAKE_BINARY_DIR}/config.json
)