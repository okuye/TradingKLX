#!/bin/bash

# Check if the build directory exists
if [ -d "build" ]; then
    echo "Build directory exists. Cleaning..."
    # Add a prompt for confirmation to avoid accidental deletion
    read -p "Are you sure you want to remove the existing build directory? (y/N): " confirm
    if [[ $confirm == [yY] || $confirm == [yY][eE][sS] ]]; then
        rm -rf build
    else
        echo "Exiting without cleaning build directory."
        exit
    fi
fi

# Create the build directory
mkdir build
cd build

# Run CMake based on the platform
if [[ "$OSTYPE" == "cygwin" || "$OSTYPE" == "msys" || "$OSTYPE" == "win32" ]]; then
    # Adjust the paths to your compilers if necessary
    cmake .. -G "Ninja" -DCMAKE_C_COMPILER=C:/cygwin64/bin/gcc.exe -DCMAKE_CXX_COMPILER=C:/cygwin64/bin/g++.exe
else
    cmake .. -G "Ninja"
fi
