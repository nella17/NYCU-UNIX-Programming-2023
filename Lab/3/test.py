from pwn import *
elf = ELF('./lab03_pub/chals')
#  print("main =", hex(elf.symbols['main']))
#  print("{:<12s} {:<8s} {:<8s}".format("Func", "GOT", "Address"))

size = 1477
gots = [0] * size

cnt = 0

for g in elf.got:
   if "code_" in g:
       cnt += 1
       nu = int(g[5:])
       gots[nu] = elf.got[g]
      #  print("{:<12s} {:<8x} {:<8x}".format(g, elf.got[g], elf.symbols[g]))

#  print(cnt, size)

print(gots)
