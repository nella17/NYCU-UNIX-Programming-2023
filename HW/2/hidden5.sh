#!/bin/bash
./sdb ./example/deep <<< "\
break 0x401034
cont
anchor
break 0x40114a
cont
timetravel
cont
cont
"
