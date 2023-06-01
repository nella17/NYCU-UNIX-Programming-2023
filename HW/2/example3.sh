#!/bin/bash
./sdb ./example/guess <<< "\
break 0x4010bf
break 0x40111e
cont
anchor
cont
haha           
timetravel
cont
42             
cont
"
