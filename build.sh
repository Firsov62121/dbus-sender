#! /usr/bin/bash

mkdir bin
cd bin
cmake ..
make
./server 1>/dev/null 2>>./server.log < /dev/null &
echo "Server PID: " $!
