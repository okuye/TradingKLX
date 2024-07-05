FROM ubuntu:latest

# Install necessary packages
RUN apt-get update && apt-get install -y \
    build-essential \
    libcurl4-openssl-dev \
    git \
    cmake \
    pkg-config \
    libssl-dev \
    libsasl2-dev \
    wget \
    libmongoc-1.0-0 \
    python3

# Install MongoDB C Driver (libmongoc)
RUN wget https://github.com/mongodb/mongo-c-driver/releases/download/1.17.4/mongo-c-driver-1.17.4.tar.gz && \
    tar xzf mongo-c-driver-1.17.4.tar.gz && \
    cd mongo-c-driver-1.17.4 && \
    mkdir -p cmake-build && \
    cd cmake-build && \
    cmake -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF .. && \
    make && \
    make install

# Install MongoDB C++ Driver (libmongocxx)
RUN wget https://github.com/mongodb/mongo-cxx-driver/releases/download/r3.6.6/mongo-cxx-driver-r3.6.6.tar.gz && \
    tar xzf mongo-cxx-driver-r3.6.6.tar.gz && \
    cd mongo-cxx-driver-r3.6.6 && \
    sed -i 's/add_subdirectory(test)//' src/bsoncxx/CMakeLists.txt && \
    sed -i 's/add_subdirectory(test)//' src/mongocxx/CMakeLists.txt && \
    sed -i '2i#include <cstdint>' src/mongocxx/events/heartbeat_failed_event.hpp && \
    sed -i '2i#include <cstdint>' src/mongocxx/events/heartbeat_started_event.hpp && \
    sed -i '2i#include <cstdint>' src/mongocxx/events/server_closed_event.hpp && \
    sed -i '2i#include <cstdint>' src/mongocxx/events/server_opening_event.hpp && \
    sed -i '2i#include <cstdint>' src/mongocxx/exception/error_code.hpp && \
    sed -i '27s/enum class error_code : std::int32_t {/enum error_code : std::int32_t {/' src/mongocxx/exception/error_code.hpp && \
    sed -i '2i#include <cstdint>' src/mongocxx/exception/server_error_code.hpp && \
    sed -i '28s/enum class server_error_code : std::int32_t {/enum server_error_code : std::int32_t {/' src/mongocxx/exception/server_error_code.hpp && \
    mkdir -p cmake-build && \
    cd cmake-build && \
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local -DBUILD_VERSION=3.6.6 .. && \
    make && \
    make install

# Debugging steps to verify the installation of pkg-config files
RUN find /usr/local -name "*.pc"

# Verify pkg-config files
RUN ls /usr/local/lib/pkgconfig/ && \
    find /usr/local -name "libbsoncxx.pc" && \
    if [ ! -f /usr/local/lib/pkgconfig/libbsoncxx.pc ]; then \
        echo "libbsoncxx.pc not found"; \
        exit 1; \
    fi

CMD ["bash"]
