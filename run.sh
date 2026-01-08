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
stdbuf -oL ./build/app | while read line
do
   echo "$(date '+%y-%m-%d %H:%M:%S')
$line"
done

