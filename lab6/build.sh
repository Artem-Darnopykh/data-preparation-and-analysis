#!/bin/bash
echo "=== Збірка проекту ==="
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
echo "=== Збірка завершена ==="
