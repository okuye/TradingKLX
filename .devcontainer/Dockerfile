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
    python3-venv

# Set up Python environment
RUN python3 -m venv /app/venv

# Set the working directory inside the container
WORKDIR /workspace

# Copy the source code into the container
# Note: You can use a Docker volume instead of copying during build
COPY . /workspace

# Set executable permissions for the build script
RUN chmod +x /workspace/build.sh

# Expose any necessary ports (for example, if you’re running a web server, replace 3000)
EXPOSE 3000

# Install MongoDB C++ driver
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
    rm -rf build && \
    mkdir build && \
    cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local -DBUILD_VERSION=3.10.2 -DBUILD_TESTING=OFF -DENABLE_TESTS=OFF .. && \
    make && \
    make install


# Install GoogleTest and copy the built static libraries
RUN apt-get install -y libgtest-dev && \
    cd /usr/src/gtest && \
    cmake . && \
    make && \
    cp lib/*.a /usr/lib


# Expose SSH port for development tools like CLion or VSCode to connect
EXPOSE 22

# Install OpenSSH
RUN apt-get install -y openssh-server
RUN mkdir /var/run/sshd
RUN echo 'root:root' | chpasswd
RUN sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config

# Start SSH service
CMD ["/usr/sbin/sshd", "-D"]

