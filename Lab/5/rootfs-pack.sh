#!/bin/bash
cd rootfs
find . -print0 | cpio --null -ov --format=newc 2>/dev/null | bzip2 -c > ../rootfs.cpio.bz2
