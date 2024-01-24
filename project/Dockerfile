# Build environment for the project
# joshuajerred/giraffe
# update with:
# cd project/
# docker build -t joshuajerred/giraffe .
# docker push joshuajerred/giraffe

# to run:
# docker run -it --rm --name giraffe joshuajerred/giraffe

FROM ubuntu

RUN apt-get update && apt-get install -y \
   git \
   build-essential \
   cmake \
   libcurl4-gnutls-dev \
   libi2c-dev \
   libncurses-dev \
   libpulse-dev \
   ca-certificates \
   curl \
   gnupg

RUN curl -fsSL https://deb.nodesource.com/setup_20.x | bash - &&\
apt-get install -y nodejs