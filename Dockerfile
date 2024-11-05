# Base image
FROM ubuntu:latest

# Update and install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    libcurl4-openssl-dev \
    git \
    cmake \
    pkg-config \
    libssl-dev \
    wget \
    python3 \
    python3-pip \
    python3-venv \
    gdb \
    libspdlog-dev \
    openssh-server \
    libboost-all-dev \
    libjsoncpp-dev \
    libgtest-dev  # Install GTest

# Compile GTest (as it doesn't come precompiled on Ubuntu)
RUN cd /usr/src/gtest && \
    cmake . && \
    make && \
    mv lib/*.a /usr/lib

# Python virtual environment setup
RUN python3 -m venv /app/venv
RUN /app/venv/bin/pip install numpy

# Set up working directory
WORKDIR /workspace

# Copy the entire project to the workspace
COPY . /workspace

# Copy ForexConnect.framework into the container
COPY libs/ /workspace/libs/

COPY libs/ForexConnect.framework /workspace/libs/ForexConnect.framework

# Set permissions for build script
RUN chmod +x /workspace/build.sh

# SSH setup
RUN mkdir /var/run/sshd
RUN echo 'root:root' | chpasswd
RUN sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config

# Build the project and run commands
CMD ["bash", "-c", "./build.sh && ./RunTradingKLX.sh"]