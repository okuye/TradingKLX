#!/bin/bash

# Get the current directory name
current_dir=${PWD##*/}

# Check if the current directory is 'build'
if [ "$current_dir" == "build" ]; then
    # Go back one level
    cd ..
fi

# Remove the build directory if it exists
if [ -d "build" ]; then
    rm -rf build
fi

# Create a new build directory
mkdir build

# No need to set specific permissions on the host path in Docker; this line can be removed.
# If permission issues occur, it may be related to Docker volume settings

# Navigate to the build directory
cd build

# Run cmake and make
cmake ..
make
