#!/bin/bash

## Stop and remove all containers
echo "Stopping and removing all containers..."
docker stop $(docker ps -aq)
docker rm $(docker ps -aq)

## Remove all images
echo "Removing all images..."
docker rmi $(docker images -q) -f

## Remove all volumes
echo "Removing all volumes..."
docker volume rm $(docker volume ls -q)

## Remove any leftover networks
echo "Removing unused networks..."
docker network prune -f

## Final cleanup
echo "Performing final cleanup..."
docker system prune -af --volumes

echo "All Docker containers, images, and volumes have been removed."