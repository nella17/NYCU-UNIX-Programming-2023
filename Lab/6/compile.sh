#!/bin/bash
set -ex
gcc sort.c -o sort \
  -nostdlib \
  -Os -masm=intel \
  -fcf-protection=none
rm temp.s
objdump -M intel -j .text --disassemble=sort sort  | tail +7 | tee -a temp.s
objdump -M intel -j .text --disassemble=merge sort | tail +7 | tee -a temp.s
