#!/bin/bash
set -ex
gcc ${1?fn} -o sort \
  -nostdlib \
  -Os -masm=intel \
  -fcf-protection=none
rm temp.s
objdump -M intel -j .text --disassemble=sort sort  | tail +7 | tee -a temp.s
objdump -M intel -j .text --disassemble=impl sort | tail +7 | tee -a temp.s
