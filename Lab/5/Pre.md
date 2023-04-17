UP23 Lab05 (Pre-Lab Announcement)
=================================
Date: 2023-04-24

[TOC]

# Implement a Kernel Module

Our next lab is to practice implementing a kernel module. The details will be announced next week. Before you play with our lab next week, here are some hints for you to prepare before our lab officially announced.

## Preparation

You may not have experience in implementing a kernel module. Before you start your implementation, you may read some relevant kernel documents and tutorials.

- Please check the `file+stdio` course slide and read the `hellomod` example to see how a simple kernel module is implemented.
- The [Linux kernel documentation](https://www.kernel.org/doc/html/latest/), including
   - [ioctl based interface](https://www.kernel.org/doc/html/latest/driver-api/ioctl.html) 
   - [Memory allocation guide](https://www.kernel.org/doc/html/latest/core-api/memory-allocation.html)
   - [Memory management APIs](https://www.kernel.org/doc/html/latest/core-api/mm-api.html)
- Linux kernel [memory mapping](https://linux-kernel-labs.github.io/refs/heads/master/labs/memory_mapping.html) labs materials from the OS course offered by the University Politehnica of Bucharest. ***Note:*** this is for kernel 5.10.14 and the introduced functions might be different from our lab experiment runtime.

Our development package (including runtime and development files) can be found [here (dist.tbz)](https://up23.zoolab.org/up23/lab05/dist.tbz). You may download and play with it before our lab officially starts.

You can develop the module on Apple chip macs but note that all the files must be cross-compiled to x86_64 architecture. Please read the rest of the lab instructions carefully before you implement this lab.

:::info
Our course video `file+stdio` has introduced how `ioctl` works with a kernel module. This lab extends it by implementing more features in the kernel module.
:::

