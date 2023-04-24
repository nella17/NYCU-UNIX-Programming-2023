#!/bin/bash
set -eux

cd kshram
make
cd ..

cp hellomod/hellomod.ko rootfs/
cp kshram/kshram.ko rootfs/

./rootfs-pack.sh
exec ./qemu.sh
