#!/bin/bash
set -ex
g++ sort.cpp -o sort -nostdlib -O1 \
  -fcf-protection=none
rm temp.s
objdump -M intel --disassemble=sort sort | tee -a temp.s
objdump -M intel --disassemble=merge sort | tee -a temp.s
