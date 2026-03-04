#!/bin/bash

mkdir -p bin tmp && \

./build/frontend/frontend && \
./build/backend/backend && \

chmod +x bin/output.elf && \
echo "Executable file: ./bin/output.elf"
