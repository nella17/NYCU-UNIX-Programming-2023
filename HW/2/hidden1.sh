#!/bin/bash
./sdb ./example/hello <<< "\
break 0x401004
break 0x401005
cont
cont
cont
"
