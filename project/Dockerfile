# Build environment for the project
# joshuajerred/giraffe
# update with:
# cd project/
# docker build -t joshuajerred/giraffe .
# docker push joshuajerred/giraffe

# to run:
# docker run -it --rm --name giraffe joshuajerred/giraffe

FROM debian:bookworm-slim

ARG CMAKE_VERSION=3.30.5

RUN apt-get update

# Basic build tools
RUN apt-get install -y \
   curl \
   git \
   build-essential \
   cppcheck \
   ninja-build

# Install CMake 3.30.5
COPY cmake-${CMAKE_VERSION}-linux-x86_64.sh /tmp/cmake-${CMAKE_VERSION}-Linux-x86_64.sh
RUN chmod +x /tmp/cmake-${CMAKE_VERSION}-Linux-x86_64.sh
RUN /tmp/cmake-${CMAKE_VERSION}-Linux-x86_64.sh --skip-license --prefix=/usr/local

# Libraries/Dependencies
RUN apt-get install -y \
   libgtest-dev \
   libcurl4-gnutls-dev \
   libi2c-dev \
   libncurses-dev \
   libpulse-dev \
   
   # ca-certificates \
   # gnupg \

RUN curl -fsSL https://deb.nodesource.com/setup_20.x | bash - &&\
apt-get install -y nodejs