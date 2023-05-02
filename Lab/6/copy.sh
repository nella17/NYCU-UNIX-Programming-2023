#!/bin/bash
set -ex
cat temp.s \
  | sed 's/ .\{4\} <\(.\+\)>/ \1/g' \
  | sed 's/.\+:.\+\t/\t/g' \
  | sed '/:\t/d' \
  | sed 's/.\+<\(.\+\)>/\1/g' \
  | tee sort.s
