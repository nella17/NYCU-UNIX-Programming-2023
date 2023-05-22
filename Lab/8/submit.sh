#!/bin/bash
set -ex
gcc runner.c -o runner.static -static-pie
python ./submit_1310ed1cb40c16067911bdda36189abf.py runner.static
