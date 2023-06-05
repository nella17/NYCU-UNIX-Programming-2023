#!/bin/bash
./sdb ./example/loop1 <<< "\
break 0x401024
cont
si
break 0x40102e
cont
si
si
si
si
cont
"
