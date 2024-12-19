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

mv ground_station.tar.gz giraffe
mv gcli giraffe
mv gdl_server giraffe
mv gfs giraffe

tar -czf giraffe.tar.gz giraffe

echo "==============================="


