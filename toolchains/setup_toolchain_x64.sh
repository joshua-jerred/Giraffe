#!/usr/bin/env bash

# Run from the root of the project

SSH_HOST="balloon"
DOWNLOAD_LINK=https://cytranet.dl.sourceforge.net/project/raspberry-pi-cross-compilers/Bonus%20Raspberry%20Pi%20GCC%2064-Bit%20Toolchains/Raspberry%20Pi%20GCC%2064-Bit%20Cross-Compiler%20Toolchains/Bullseye/GCC%2010.3.0/cross-gcc-10.3.0-pi_64.tar.gz
INSTALL_DIR="${HOME}/toolchain"

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

toolchain_file() {
  cp pi_toolchain.cmake ${INSTALL_DIR}/toolchain.cmake
}

REQUEST=$1
if [ "$REQUEST" == "download" ]; then
    download_toolchain
    exit 0
elif [ "$REQUEST" == "sync" ]; then
    sync
    exit 0
elif [ "$REQUEST" == "toolchain" ]; then
    toolchain_file
    exit 0
else
    echo "Options: download, sync, toolchain"
    exit 1
fi