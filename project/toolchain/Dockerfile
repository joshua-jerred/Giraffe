FROM debian:bookworm

ARG CMAKE_VERSION=3.30.5

# Basic build tools
RUN apt-get update && apt-get install -y \
   curl \
   git \
   build-essential \
   cppcheck \
   ninja-build

# Install CMake 3.30.5
COPY cmake-${CMAKE_VERSION}-linux-x86_64.sh /tmp/cmake-${CMAKE_VERSION}-Linux-x86_64.sh
RUN chmod +x /tmp/cmake-${CMAKE_VERSION}-Linux-x86_64.sh
RUN /tmp/cmake-${CMAKE_VERSION}-Linux-x86_64.sh --skip-license --prefix=/usr/local

RUN mkdir -p /home/root
RUN usermod -d /home/root root

# Copy over the toolchain
ARG HOME_DIR
COPY ${HOME_DIR}/pi_64_gcc /home/root/pi_64_gcc