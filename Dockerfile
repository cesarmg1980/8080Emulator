FROM gcc:latest

RUN apt-get update && \ 
    apt-get -y install libcmocka-dev && \ 
    apt-get -y install gdb

COPY . /app
WORKDIR /app
