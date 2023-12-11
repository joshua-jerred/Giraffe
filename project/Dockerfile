# Build environment for the project
# joshuajerred/giraffe
# update with:
# cd project/
# docker build -t joshuajerred/giraffe .
# docker push joshuajerred/giraffe

FROM ubuntu

RUN apt-get update && apt-get install -y \
   git \
   build-essential \
   cmake \
   libgtest-dev \
   libcurl4-gnutls-dev \
   nlohmann-json3-dev \
   libi2c-dev \
   libncurses-dev \
   libpulse-dev