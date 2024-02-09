#!/bin/bash

# Define your application port
APP_PORT=3000  # Change this to your application's actual port

# Step 1: Build your application's Docker image using the Dockerfile in the current directory
echo "Building the Docker image for the application..."
docker-compose build app

# Step 2: Start the services defined in docker-compose.yml, MongoDB will start before the app due to 'depends_on'
echo "Starting the services with Docker Compose..."
docker-compose up -d

# Step 3: The 'volumes' and 'ports' configurations are already defined in docker-compose.yml,
# so they will be automatically applied when the services start.

# Additional step: Check if the services are up and running
echo "Checking the status of the services..."
docker-compose ps

# Display the application's access URL
echo "Your application should now be accessible at http://localhost:$APP_PORT"
