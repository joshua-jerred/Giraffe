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
   libcurl4-gnutls-dev \
   libi2c-dev \
   libncurses-dev \
   libpulse-dev \
   ca-certificates \
   curl \
   gnupg \
   libgtest-dev

COPY cmake-3.30.5-linux-x86_64.sh /tmp/cmake-3.30.5-Linux-x86_64.sh
RUN chmod +x /tmp/cmake-3.30.5-Linux-x86_64.sh
RUN /tmp/cmake-3.30.5-Linux-x86_64.sh --skip-license --prefix=/usr/local

RUN curl -fsSL https://deb.nodesource.com/setup_20.x | bash - &&\
apt-get install -y nodejs