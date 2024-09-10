
#!/bin/bash
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

# Navigate to the build directory
cd build

# Run cmake and make
cmake ..
make

# # Set up Python virtual environment
# VENV_DIR="tradingklx_venv"
# PYTHON_VERSION="python3"

# # Remove the existing virtual environment if it exists
# if [ -d "$VENV_DIR" ]; then
#     echo "Removing existing virtual environment..."
#     rm -rf $VENV_DIR
# fi

# # Create a new virtual environment
# echo "Setting up Python virtual environment..."
# $PYTHON_VERSION -m venv $VENV_DIR

# # Activate the virtual environment
# source $VENV_DIR/bin/activate

# # Upgrade pip to the latest version
# pip install --upgrade pip

# # Install the required packages
# pip install numpy==1.24.0 cmake

# # Check if numpy headers are installed
# NUMPY_HEADERS=$(find $VENV_DIR/lib/python3.9/site-packages/numpy/ -name "arrayobject.h")
# if [ -z "$NUMPY_HEADERS" ]; then
#     echo "Error: numpy headers not found. Installation failed."
#     exit 1
# fi

# # Export the Python path for CMake to use
# export PYTHON_EXECUTABLE=$VENV_DIR/bin/python

# # Proceed with the build
# mkdir -p build
# cd build
# cmake -DPYTHON_EXECUTABLE=$PYTHON_EXECUTABLE -DPython3_NumPy_INCLUDE_DIRS=$VENV_DIR/lib/python3.9/site-packages/numpy/core/include/numpy ..
# make -j$(nproc)

# # Deactivate the virtual environment
# deactivate

# echo "Build completed successfully."