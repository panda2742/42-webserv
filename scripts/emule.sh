#!/bin/bash

docker build --platform=linux/amd64 -t 42-webserv .
docker run --rm \
	--platform=linux/amd64 \
	-it \
	-v "$(pwd)":/src \
	-p 80:8080 \
	--name webserv \
	42-webserv

