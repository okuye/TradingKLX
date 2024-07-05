FROM ubuntu:latest

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    libcurl4-openssl-dev \
    git \
    cmake \
    pkg-config \
    libssl-dev \
    wget \
    python3

# Set working directory
WORKDIR /app

# Install mongo-c-driver
RUN wget https://github.com/mongodb/mongo-c-driver/releases/download/1.17.4/mongo-c-driver-1.17.4.tar.gz && \
    tar xzf mongo-c-driver-1.17.4.tar.gz && \
    cd mongo-c-driver-1.17.4 && \
    mkdir cmake-build && \
    cd cmake-build && \
    cmake -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF .. && \
    make && \
    make install

# Install mongo-cxx-driver
RUN wget https://github.com/mongodb/mongo-cxx-driver/releases/download/r3.10.2/mongo-cxx-driver-r3.10.2.tar.gz && \
    tar xzf mongo-cxx-driver-r3.10.2.tar.gz && \
    cd mongo-cxx-driver-r3.10.2 && \
    rm -rf src/bsoncxx/test && \
    sed -i 's/static constexpr std::size_t sigStackSize = 32768 >= MINSIGSTKSZ ? 32768 : MINSIGSTKSZ;/static const std::size_t sigStackSize = 32768 >= MINSIGSTKSZ ? 32768 : MINSIGSTKSZ;/' src/third_party/catch/include/catch.hpp && \
    sed -i 's/char FatalConditionHandler::altStackMem[sigStackSize];/char FatalConditionHandler::altStackMem[32768];/' src/third_party/catch/include/catch.hpp && \
    rm -rf build && \
    mkdir build && \
    cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local -DBUILD_VERSION=3.10.2 -DBUILD_TESTING=OFF -DENABLE_TESTS=OFF .. && \
    make && \
    make install
