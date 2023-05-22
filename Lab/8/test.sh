#!/bin/bash
set -ex
gcc runner.c -o runner -g
./runner ${1-./sample/sample1}
