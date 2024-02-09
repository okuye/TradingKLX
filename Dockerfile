# Use Ubuntu as the base image
FROM ubuntu:latest

# Install necessary packages including MongoDB
RUN apt-get update && apt-get install -y \
    build-essential \
    libcurl4-openssl-dev \
    libta-lib-dev \
    git \
    cmake \
    mongodb

# Start MongoDB service
RUN service mongodb start

# Clone nlohmann_json from the official repository
RUN git clone --branch v3.10.5 https://github.com/nlohmann/json.git /nlohmann_json

# Set the working directory in the container
WORKDIR /app

# Copy your project files into the container
COPY . /app

# Include the nlohmann_json directory
INCLUDE_DIRECTORIES(/nlohmann_json/single_include)

# Use CMake to build the project
RUN cmake . && make

# Command to run the application
CMD ["./TradingKLX"]
