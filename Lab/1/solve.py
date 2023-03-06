from pwn import *
import re

import os
import sys

sys.path.insert(0, os.path.normpath(os.path.join(__file__, '../../../pow'))) 

from pow import solve_pow

io = remote('up23.zoolab.org', 10363)

solve_pow(io)

text = io.readuntil(b'base64\n\n').decode()
total = int(re.findall('the (\d+) challenges', text)[0])
print(f'{total} challenges')

for _ in range(total):
    line = io.readuntil(b'= ?').decode()
    equation = line.split(': ')[1].split(' =')[0]
    print(equation)
    value = eval(equation)
    out = b64e(value.to_bytes(100, 'little').rstrip(b'\0'))
    print(out)
    io.sendline(out.encode())

io.interactive()
