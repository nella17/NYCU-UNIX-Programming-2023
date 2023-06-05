#!/bin/bash
./sdb ./example/hello <<< "\
break 0x401005
break 0x401004
cont
cont
cont
"
