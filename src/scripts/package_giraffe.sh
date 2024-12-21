#!/bin/bash
set -e

# !!! Run from CMake !!!

echo "== package_giraffe.sh =="

# Package the ground station
cd ground_station
npm install
cd ..
tar -czf ./bin/ground_station.tar.gz ground_station

# Package Giraffe
echo "Package Giraffe"
cd bin
rm -rf giraffe
mkdir giraffe

# Copy in the files
cp gcli giraffe
cp ground_station.tar.gz giraffe
cp gdl_server giraffe
cp gfs giraffe
cp giraffe_manifest.json giraffe

tar -czf giraffe-update.tar.gz giraffe

echo "==============================="


