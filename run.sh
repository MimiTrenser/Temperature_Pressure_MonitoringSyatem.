#!/bin/bash

DATE=$(date)
echo "Build time: $DATE"

set -e
echo "Compiling Tempearture & Pressure Monitoring System"
echo "######################################################################"
make clean
make

echo "Running Application"
echo "######################################################################"
gdb --args ./build/app

