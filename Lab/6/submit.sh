#!/bin/bash

TOKEN=""
if [ $# -ge 1 ]; then
  echo "use token"
  TOKEN="$(cat token)"
fi

ASM=sort.s

python ./submit_ea7c7bdbbbf63d90e647337bdf623049.py $ASM $TOKEN
