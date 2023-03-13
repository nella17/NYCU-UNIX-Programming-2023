#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import base64
import hashlib
import time
from pwn import *
from multiprocessing.pool import Pool

def task(args):
    prefix, i = args
    while True:
        h = hashlib.sha1((prefix + str(i)).encode()).hexdigest()
        if h[:6] == '000000':
            solved = str(i).encode()
            return solved
        i += 1

def solve_pow(r, lower = 0, upper = 1000000000, count = 8):
    prefix = r.recvline().decode().split("'")[1]
    info(f"{time.time():.5f} solving pow ...")
    solved = b''
    with Pool(processes=count) as pool:
        it = pool.imap_unordered(task, [(prefix, lower + x * upper // count) for x in range(count)])
        solved = next(it)
        success(f"solved = {solved}")
        info(f"{time.time():.5f} done.")
        r.sendlineafter(b'string S: ', base64.b64encode(solved))
        return True

def create_connect(*args, **kwargs):
    io = remote(*args, **kwargs)
    solve_pow(io)
    return io

if __name__ == '__main__':
    if 1:
        r = create_connect('up23.zoolab.org', 10330)
    else:
        #r = remote('localhost', 10330);
        r = remote('up23.zoolab.org', 10330)
        solve_pow(r)

    #  r.interactive()
    r.shutdown()
    print(r.recv().decode())
    r.close()

# vim: set tabstop=4 expandtab shiftwidth=4 softtabstop=4 number cindent fileencoding=utf-8 :
