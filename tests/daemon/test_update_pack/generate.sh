#!/bin/bash
set -e

# Generates a demo giraffe package for unit testing.
# Run this script from its directory.

TAR_SRC="tar_src"
WORK_DIR="giraffe-0.8.0-beta-dev"
OUTPUT_FILE="$WORK_DIR.tar.gz"

# Create the package directory
mkdir -p $WORK_DIR

# Tar the ground station directory
cd $TAR_SRC && tar -czf ground_station.tar.gz ground_station && cd ..
mv $TAR_SRC/ground_station.tar.gz $WORK_DIR

# Copy the binaries into the work directory
cp $TAR_SRC/binaries/* $WORK_DIR






# Clean up
# rm -rf ground_station.tar.gz