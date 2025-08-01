#!/bin/bash

mkdir -p build bin tmp

cmake -B build && cmake --build build

./build/frontend/frontend
./build/backend/backend

chmod +x bin/output.elf
./bin/output.elf
