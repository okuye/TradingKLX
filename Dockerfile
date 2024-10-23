# Base image
FROM ubuntu:latest

# Install system dependencies
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
    gdb \
    libgtest-dev \
    libjsoncpp-dev \
    libspdlog-dev \
    clang \
    libomp-dev \
    vim \
    nano \
    openssh-server \
    curl \
    python3-venv \
    libboost-all-dev

# Set up Python virtual environment
RUN python3 -m venv /app/venv

# Install NumPy in the virtual environment
RUN /app/venv/bin/pip install numpy

# Set the working directory inside the container
WORKDIR /workspace

# Copy the source code into the container
COPY . /workspace

# Set executable permissions for the build script
RUN chmod +x /workspace/build.sh

# Expose ports
EXPOSE 3000 
EXPOSE 22

# Add this line to install spdlog
RUN apt-get update && apt-get install -y libspdlog-dev

# Install and configure OpenSSH
RUN apt-get install -y openssh-server
RUN mkdir /var/run/sshd
RUN echo 'root:root' | chpasswd
RUN sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config

# Start SSH service
CMD ["/usr/sbin/sshd", "-D"]
