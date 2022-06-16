#!/bin/bash

set -e
set -x

cd client
g++ main.cpp Client.cpp -o client.out

cd ../server
g++ main.cpp Server.cpp -o server.out