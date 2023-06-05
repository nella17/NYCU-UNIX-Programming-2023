#!/bin/bash
./sdb ./example/deep <<< "\
break 0x4010de
cont
anchor
break 0x401159
cont
timetravel
cont
cont
"
