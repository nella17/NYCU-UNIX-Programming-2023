#!/bin/bash
rm -fr rootfs
mkdir rootfs
cd rootfs
bzcat ../dist/rootfs.cpio.bz2 | cpio -idv
