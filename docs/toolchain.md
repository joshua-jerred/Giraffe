# Toolchain

Building GFS on a Raspberry Pi Zero 2 takes at least 30 minutes. To speed up the
process, there is a toolchain available that allows you to cross-compile GFS.

This toolchain is based off of the wonderful work done by [abhiTronix/raspberry-pi-cross-compilers](https://github.com/abhiTronix/raspberry-pi-cross-compilers).

You can build the toolchain by running the following commands in `project/toolchain`:
```bash
# Download the toolchain binaries from *https://github.com/joshua-jerred/docker-raspberry-pi-cross-compiler*.
# Made with the *abhiTronix/raspberry-pi-cross-compilers* project.
# Download URL is at the top of the file and can be changed.
bash setup_toolchain.sh download

# (optional) Enable rsync with elevated privileges
bash setup_toolchain.sh sudo_rsync

# Update the remote system
# You can configure SSH at the top of this script
bash setup_toolchain.sh update_remote

# Sync sysroot with a Raspberry Pi Zero 2
bash setup_toolchain.sh sync
```

Now, change the CMake preset to `gcc_x86_pi_64_cross_compile` and build the project as usual.