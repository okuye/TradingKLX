# Use Ubuntu as the base image
FROM ubuntu:latest

# Install necessary packages
RUN apt-get update && apt-get install -y \
    build-essential \
    libcurl4-openssl-dev \
    git \
    cmake \
    pkg-config \
    libssl-dev \
    libsasl2-dev

# Install MongoDB C Driver (libmongoc)
RUN apt-get install -y libmongoc-1.0-0

# Install MongoDB C++ Driver (libmongocxx)
# Note: The installation steps might vary based on the version and might need adjustment
RUN apt-get install -y wget && \
    wget https://github.com/mongodb/mongo-c-driver/releases/download/1.17.4/mongo-c-driver-1.17.4.tar.gz && \
    tar xzf mongo-c-driver-1.17.4.tar.gz && \
    cd mongo-c-driver-1.17.4 && \
    mkdir cmake-build && \
    cd cmake-build && \
    cmake -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF .. && \
    make && \
    make install && \
    cd ../../ && \
    wget https://github.com/mongodb/mongo-cxx-driver/releases/download/r3.6.5/mongo-cxx-driver-r3.6.5.tar.gz && \
    tar xzf mongo-cxx-driver-r3.6.5.tar.gz && \
    cd mongo-cxx-driver-r3.6.5/build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release -DBSONCXX_POLY_USE_MNMLSTC=1 && \
    make EP_mnmlstc_core && \
    make && \
    make install

# Clone nlohmann_json from the official repository
RUN git clone --branch v3.10.5 https://github.com/nlohmann/json.git /nlohmann_json

# Set the working directory in the container
WORKDIR /app

# Copy your project files into the container
COPY . /app

# Use CMake to build the project
RUN cmake . && make

# Command to run the application
CMD ["./TradingKLX"]
