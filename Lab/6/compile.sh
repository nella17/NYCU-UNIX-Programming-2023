#!/bin/bash
g++ sort.cpp -o sort -nostdlib -Os
objdump -M intel --disassemble=sort sort
objdump -M intel --disassemble=merge sort
