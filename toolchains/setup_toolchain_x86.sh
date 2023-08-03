#!/usr/bin/env bash

# Run from the root of the project

SSH_HOST="balloon_zero"
DOWNLOAD_LINK=https://netactuate.dl.sourceforge.net/project/raspberry-pi-cross-compilers/Raspberry%20Pi%20GCC%20Cross-Compiler%20Toolchains/Bullseye/GCC%2010.3.0/Raspberry%20Pi%201%2C%20Zero/cross-gcc-10.3.0-pi_0-1.tar.gz
INSTALL_DIR="${HOME}/pi_x86_toolchain"

################################################################################

# setup the directory structure
download_toolchain() {
  mkdir ${INSTALL_DIR}
  mkdir ${INSTALL_DIR}/root_fs

  # download the toolchain binaries and extract them
  wget $DOWNLOAD_LINK -O ${INSTALL_DIR}/toolchain.tar.gz
  tar -xvf ${INSTALL_DIR}/toolchain.tar.gz -C ${INSTALL_DIR}
  rm ${INSTALL_DIR}/toolchain.tar.gz
}

sync() {
  # sync the root filesystem
  rsync -avz --rsync-path="sudo rsync" --delete $SSH_HOST:/lib ${INSTALL_DIR}/root_fs
  rsync -avz --rsync-path="sudo rsync" --delete $SSH_HOST:/usr/include ${INSTALL_DIR}/root_fs/usr
  rsync -avz --rsync-path="sudo rsync" --delete $SSH_HOST:/usr/lib ${INSTALL_DIR}/root_fs/usr
  rsync -avz --rsync-path="sudo rsync" --delete $SSH_HOST:/opt/vc ${INSTALL_DIR}/root_fs/opt

  # Fix simlinks
  python3 sysroot-relativelinks.py ${INSTALL_DIR}/root_fs
}

install_prereqs_on_remote() {
  ssh $SSH_HOST "sudo apt-get install -y rsync build-essential cmake"
}

REQUEST=$1
if [ "$REQUEST" == "download" ]; then
    download_toolchain
    exit 0
elif [ "$REQUEST" == "sync" ]; then
    sync
    exit 0
elif [ "$REQUEST" == "prereqs" ]; then
    install_prereqs_on_remote
    exit 0
else
    echo "Options: download, sync, prereqs"
    exit 1
fi