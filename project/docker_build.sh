# Run this inside of the docker container to build the project
# with: docker compose run --rm build_giraffe

REPO_PATH="/project"
DOCKER_CMAKE_TOOLCHAIN_FILE="/toolchain/toolchain.cmake"
BUILD_DIR="${REPO_PATH}/build"

# Fix the symlink issue
cd /toolchain && python3 sysroot-relativelinks.py root_fs

# Take ownership
git config --global --add safe.directory '*'

# Build the project
cd /project
# rm -rf build
mkdir -p build
cd build && cmake .. --preset=gcc_docker_pi_zero
cd gcc_docker_pi_zero && make -j$(nproc) $1

chown -R 1000:1000 ${BUILD_DIR}