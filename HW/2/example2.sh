#!/bin/bash
./sdb ./example/hello <<< "\
break 0x401030
break 0x40103b
cont
cont
si
si
"
