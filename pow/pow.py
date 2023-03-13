#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import base64
import hashlib
import time
from pwn import *

def solve_pow(r, lower = 0, upper = 1000000000, debug = True, event = None):
    prefix = r.recvline().decode().split("'")[1]
    if debug: print(time.time(), "solving pow ...")
    solved = b''
    for i in range(lower, upper):
        h = hashlib.sha1((prefix + str(i)).encode()).hexdigest()
        if h[:6] == '000000':
            solved = str(i).encode()
            if 1 or debug: print("solved =", solved)
            if debug: print(time.time(), "done.")
            r.sendlineafter(b'string S: ', base64.b64encode(solved))
            return True
        if event and event.is_set():
            break
    return False

from threading import Event
from concurrent import futures

def create_connect(event, host, port, lower, upper, *args, **kwargs):
    with context.local(log_level='ERROR'):
        io = remote(host, port, *args, **kwargs)
        if solve_pow(io, lower, upper, False, event):
            event.set()
            return io
        io.close()

def wrap_func(event, func = create_connect, *args, **kwargs):
    while not event.is_set():
        result = func(event, *args, **kwargs)
        if result:
            event.set()
            return result

def connect_pow(host, port, count = 10, *args, **kwargs):
    event = Event()
    with futures.ThreadPoolExecutor(max_workers = count) as executor:
        while True:
            conn = [
                executor.submit(
                    create_connect, event,
                    host=host, port=port,
                    lower = 0,
                    upper = 1000000000,
                    *args, **kwargs
                )
            ]
            for _ in range(count):
                time.sleep(1 / count)
                conn.append(
                    executor.submit(
                        wrap_func, event, create_connect,
                        host=host, port=port,
                        lower = 50000,
                        upper = 60000,
                        *args, **kwargs
                    )
                )
            for future in futures.as_completed(conn):
                result = future.result()
                if result:
                    return result

if __name__ == '__main__':
    r = connect_pow('up23.zoolab.org', 10330)
    print(r.readall().decode())
    exit(0)

    #r = remote('localhost', 10330);
    r = remote('up23.zoolab.org', 10330)
    solve_pow(r)
    r.interactive()
    r.close()

# vim: set tabstop=4 expandtab shiftwidth=4 softtabstop=4 number cindent fileencoding=utf-8 :
