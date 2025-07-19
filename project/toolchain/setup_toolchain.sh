#!/usr/bin/env bash

# Follows this guide: https://github.com/abhiTronix/raspberry-pi-cross-compilers/wiki/Cross-Compiler-CMake-Usage-Guide-with-rsynced-Raspberry-Pi-64-bit-OS#cross-compiler-cmake-usage-guide-with-rsynced-raspberry-pi-64-bit-os

SSH_HOST="balloon"
DOWNLOAD_LINK=https://github.com/joshua-jerred/docker-raspberry-pi-cross-compiler/releases/download/v0.0.1/cross-gcc-12.2.0-pi_64.tar.gz
INSTALL_DIR="${HOME}/pi_64_gcc"

GIRAFFE_DEV_PACKAGES="libgtest-dev libcurl4-gnutls-dev libi2c-dev libncurses5-dev libpulse-dev"

################################################################################

# setup the directory structure
download_toolchain() {
  mkdir -p ${INSTALL_DIR}
  mkdir -p ${INSTALL_DIR}/root_fs

  # download the toolchain binaries and extract them
  wget $DOWNLOAD_LINK -O ${INSTALL_DIR}/toolchain.tar.gz
  tar -xvf ${INSTALL_DIR}/toolchain.tar.gz -C ${INSTALL_DIR}
  rm ${INSTALL_DIR}/toolchain.tar.gz
}

# update the remote system
update_remote() {
  # update the remote system
  ssh $SSH_HOST "sudo apt-get update && sudo apt-get upgrade -y"
  ssh $SSH_HOST "sudo apt-get install -y rsync"
  ssh $SSH_HOST "sudo apt install -y build-essential cmake unzip pkg-config gfortran gcc g++ gperf flex texinfo gawk bison openssl pigz libncurses-dev autoconf automake tar figlet"
  ssh $SSH_HOST "sudo apt install -y ${GIRAFFE_DEV_PACKAGES}"
}

# sync the root filesystem
sync() {
  # run SSymlinker
  scp SSymlinker ${SSH_HOST}:/tmp
  ssh $SSH_HOST "chmod +x /tmp/SSymlinker"
  ssh $SSH_HOST "/tmp/SSymlinker -s /usr/include/aarch64-linux-gnu/asm -d /usr/include"
  ssh $SSH_HOST "/tmp/SSymlinker -s /usr/include/aarch64-linux-gnu/gnu -d /usr/include"
  ssh $SSH_HOST "/tmp/SSymlinker -s /usr/include/aarch64-linux-gnu/bits -d /usr/include"
  ssh $SSH_HOST "/tmp/SSymlinker -s /usr/include/aarch64-linux-gnu/sys -d /usr/include"
  ssh $SSH_HOST "/tmp/SSymlinker -s /usr/include/aarch64-linux-gnu/openssl -d /usr/include"
  ssh $SSH_HOST "/tmp/SSymlinker -s /usr/include/aarch64-linux-gnu/curl -d /usr/include"
  ssh $SSH_HOST "/tmp/SSymlinker -s /usr/lib/aarch64-linux-gnu/crtn.o -d /usr/lib/crtn.o"
  ssh $SSH_HOST "/tmp/SSymlinker -s /usr/lib/aarch64-linux-gnu/crt1.o -d /usr/lib/crt1.o"
  ssh $SSH_HOST "/tmp/SSymlinker -s /usr/lib/aarch64-linux-gnu/crti.o -d /usr/lib/crti.o"

  # sync the root filesystem
  rsync -avz --rsync-path="sudo rsync" --delete $SSH_HOST:/lib ${INSTALL_DIR}/root_fs
  rsync -avz --rsync-path="sudo rsync" --delete $SSH_HOST:/usr/include ${INSTALL_DIR}/root_fs/usr
  rsync -avz --rsync-path="sudo rsync" --delete $SSH_HOST:/usr/lib ${INSTALL_DIR}/root_fs/usr
  rsync -avz --rsync-path="sudo rsync" --delete $SSH_HOST:/opt/vc ${INSTALL_DIR}/root_fs/opt

  # Fix simlinks
  python3 sysroot-relativelinks.py ${INSTALL_DIR}/root_fs
}

toolchain_file() {
  cp pi_toolchain.cmake ${INSTALL_DIR}/toolchain.cmake
}

build_and_push_image() {
  docker build --build-arg HOME_DIR=${HOME} -t joshuajerred/giraffe:pi_64 .
  docker push joshuajerred/giraffe:pi_64
}

REQUEST=$1
if [ "$REQUEST" == "download" ]; then
    download_toolchain
    exit 0
elif [ "$REQUEST" == "update_remote" ]; then
    update_remote
    exit 0
# elif [ "$REQUEST" == "sudo_rsync" ]; then
    # sudo_rsync
    # exit 0
elif [ "$REQUEST" == "sync" ]; then
    sync
    exit 0
elif [ "$REQUEST" == "toolchain" ]; then
    toolchain_file
    exit 0
elif [ "$REQUEST" == "build_and_push_image" ]; then
    build_and_push_image
    exit 0
else
    echo "Options: download, update_remote, sync, toolchain"
    exit 1
fi