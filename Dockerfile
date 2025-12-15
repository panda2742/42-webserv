FROM ubuntu:22.04

RUN apt-get update && \
    apt-get install -y make g++ valgrind curl && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

ENV TERM=xterm

COPY . /src
WORKDIR /src
