#!/bin/bash
set -ex
g++ sort-test.cpp -o sort.test \
  -DDEBUG -g \
  -O2 -masm=intel \
  -fcf-protection=none
time ./sort.test
