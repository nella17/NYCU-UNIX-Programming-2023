#!/bin/bash
set -eux

cd kshram
make
cd ..
cp kshram/kshram.ko rootfs/

./rootfs-pack.sh
exec ./qemu.sh
