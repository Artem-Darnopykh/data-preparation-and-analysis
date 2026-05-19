#!/bin/bash
echo "=== Встановлення залежностей ==="
sudo apt update
sudo apt install -y libopencv-dev cmake gcc g++ make
echo "=== Залежності встановлено ==="
