#!/bin/bash

docker stop $(docker ps -qa) || true
docker rm $(docker ps -qa) || true
docker rmi -f $(docker images -qa) || true
docker volume rm $(docker volume ls -q) || true
docker system prune -a --volumes -f || true
docker network rm $(docker network ls -q) || true
