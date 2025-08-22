#!/bin/bash

mkdir -p build bin tmp

cmake -B build && \
cmake --build build && \

./build/frontend/frontend && \
./build/backend/backend && \

chmod +x bin/output.elf && \
echo "Executable file: ./bin/output.elf"
